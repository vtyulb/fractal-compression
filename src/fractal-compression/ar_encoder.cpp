#include "ar_encoder.h"
#include "ar_decoder.h"

#include "ar_model.h"
#include "ar_normal_model.h"
#include "ar_ppm_model.h"

AR_Encoder::AR_Encoder(bool usePPM, bool isTXT, bool BWTused) {
    packed = false;
    low = 0;
    high = AR_MAX_VALUE;
    reverseBits = 0;
    size = 0;
    ppm = usePPM;
    txt = isTXT;
    bwt = BWTused;

    if (usePPM)
        model = new AR_PPM_Model(txt);
    else
        model = new AR_Normal_Model(bwt);
}

void AR_Encoder::putSymbol(AR_symbol s) {
    if (packed)
        return;

    long long range = high - low + 1;
    high = low + range * model->freq(s) / model->totalFreq() - 1;
    if (s != 0)
        low = low + range * model->freq(s - 1) / model->totalFreq();

    while (true) {
        if (high < AR_HALF)
            writeBit(0);
        else if (low >= AR_HALF) {
            writeBit(1);
            low -= AR_HALF;
            high -= AR_HALF;
        } else if (low >= AR_FIRST_QRT && high < AR_THIRD_QRT) {
            reverseBits++;
            low -= AR_FIRST_QRT;
            high -= AR_FIRST_QRT;
        } else
            break;

        low *= 2;
        high = high * 2 + 1;
    }

    if (s == 256)
        model->resetModel();
    else {
        model->update(s);
        size++;
    }
}

void AR_Encoder::putVector(std::vector<AR_symbol> s) {
    for (unsigned i = 0; i < s.size(); i++)
        putSymbol(s[i]);
}

void AR_Encoder::writeBit(int bit) {
    res.push_back(bit);
    while (reverseBits) {
        reverseBits--;
        res.push_back(!bit);
    }
}

std::vector<char> AR_Encoder::getEncodedResult() {
     if (packed) {
         return convert(res);
     } else {
         reverseBits++;
         if (low < AR_FIRST_QRT)
             writeBit(0);
         else
             writeBit(1);

         while (res.size() % 8)
             res.push_back(0);

         packed = true;

         model->resetModel();
         return convert(res);
     }
}

std::vector<char> AR_Encoder::convert(const std::vector<bool> &data) {
    std::vector<char> res;
    header h;
    h.ppm = ppm;
    h.txt = txt;
    h.bwt = bwt;
    h.size = size;
    res.resize(data.size() / 8 + sizeof(header), 0);
    *(header*)res.data() = h;
    for (unsigned i = 0; i < res.size(); i++)
        for (int j = 0; j < 8; j++)
            if (data[i * 8 + j])
                res[i + sizeof(header)] |= (char)(1 << j);

    return res;
}
