#include "ar_decoder.h"
#include <stdio.h>

#include "ar_model.h"
#include "ar_ppm_model.h"
#include "ar_normal_model.h"
#include "bwt.h"

AR_Decoder::AR_Decoder(const std::vector<char> data):
    data(data)
{
    value = 0;
    low = 0;
    high = AR_MAX_VALUE;

    header h = *(header*)data.data();
    size = h.size;
    bwt = h.bwt;

    if (h.ppm)
        model = new AR_PPM_Model(h.txt);
    else
        model = new AR_Normal_Model(bwt);

    position = 8 * sizeof(header);
    for (int i = 0; i < AR_CODE_VALUE_BITS; i++)
        value = value * 2 + getNextBit();
}

std::vector<char> AR_Decoder::getDecoded() {
    std::vector<char> res;
    res.resize(size);
    for (int i = 0; i < size; i++)
        res[i] = getNextSymbol();

    model->resetModel();

    if (bwt)
        return BWT_Backward(res);

    return res;
}

AR_symbol AR_Decoder::getNextSymbol() {
    long long range = high - low + 1;
    int cum = ((value - low + 1) * (long long)model->totalFreq() - 1) / range;
    AR_symbol symbol = findSymbol(cum);

    high = low + (range * model->freq(symbol)) / model->totalFreq() - 1;
    if (symbol != 0)
        low = low + (range * model->freq(symbol - 1)) / model->totalFreq();

    while (1) {
        if (high < AR_HALF) {

        } else if (low >= AR_HALF) {
            value -= AR_HALF;
            low -= AR_HALF;
            high -= AR_HALF;
        } else if (low >= AR_FIRST_QRT && high < AR_THIRD_QRT) {
            value -= AR_FIRST_QRT;
            low -= AR_FIRST_QRT;
            high -= AR_FIRST_QRT;
        } else
            break;

        low *= 2;
        high = high * 2 + 1;
        value = 2 * value + getNextBit();
    }

    if (symbol == 256) {
        model->resetModel();
        return getNextSymbol();
    } else
        model->update(symbol);

    return symbol;
}

int AR_Decoder::getNextBit() {
    if (position >= data.size() * 8)
        return 0;

    int res = data[position / 8] == (data[position / 8] | (1 << (position % 8)));
    position++;
    return res;
}

AR_symbol AR_Decoder::findSymbol(int cum) {
    int left = -1;
    int right = AR_TOTAL_SYMBOLS - 1;
    while (left < right - 1)
        if (model->freq(AR_symbol((left + right) / 2)) <= cum)
            left = (left + right) / 2;
        else
            right = (left + right) / 2;

    return AR_symbol(right);
}
