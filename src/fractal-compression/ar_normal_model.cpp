#include "ar_normal_model.h"
#include <assert.h>

AR_Normal_Model::AR_Normal_Model(bool BWT) {
    bwt = BWT;
    resetModel();
}

int AR_Normal_Model::freq(AR_symbol s) {
    return _cumFreq[s];
}

int AR_Normal_Model::totalFreq() {
    return _cumFreq[AR_TOTAL_SYMBOLS - 1];
}

void AR_Normal_Model::update(AR_symbol s) {
    static int count = 0;
    const int diff = 6 + bwt * 40;
    count++;

    if (qu.size() > 3000 / (bwt * 23 + 1)) {
        _freq[qu.front()] -= diff;
        assert(_freq[qu.front()] > 0);
        qu.pop();
    }

    qu.push(s);

    _freq[s] += diff;

    _cumFreq[0] = _freq[0];
    for (int i = 1; i < AR_TOTAL_SYMBOLS; i++)
        _cumFreq[i] = _cumFreq[i - 1] + _freq[i];

    _cumFreq[AR_TOTAL_SYMBOLS - 1] = _cumFreq[AR_TOTAL_SYMBOLS - 2];

    if (_cumFreq[AR_TOTAL_SYMBOLS - 1] + diff * 10000 >= AR_FIRST_QRT) {
        for (int i = 0; i < AR_TOTAL_SYMBOLS; i++) {
            _freq[i] /= 2;
            if (_freq[i] == 0)
                _freq[i] = 1;
        }

        while (qu.size())
            qu.pop();

        update(s);
    }
}

void AR_Normal_Model::resetModel() {
    for (int i = 0; i < AR_TOTAL_SYMBOLS; i++)
        _freq[i] = 1;

    while (qu.size())
        qu.pop();
    update(32);
}
