#include "ar_ppm_model.h"

#include <stdlib.h>
#include <memory.h>

AR_PPM_Model::AR_PPM_Model(bool txt) {
    for (int i = 0; i < AR_PPM_MODEL_ORDER; i++)
        lastSymbols.push_back(32);

    cached = false;
    ticks = 0;
    memoryUsage = 0;
    ML = 0;
    if (!txt)
        ML = 2;
}

void AR_PPM_Model::update(AR_symbol s) {
    for (int i = 0; i < AR_PPM_MODEL_ORDER - ML; i++) {
        long long *block = getCurrentBlock(i);
        block[s] += AR_PPM_MODEL_AGRO;
    }

    lastSymbols.push_back(s);
    lastSymbols.pop_front();
    cached = false;
    ticks++;

    if (ML == 0 && memoryUsage > AR_MAX_MEMORY_USAGE * 1024 * 1024)
        ML = 2;
}

int AR_PPM_Model::freq(AR_symbol s) {
    if (!cached)
        cacheIt();

    return cumFreq[s];
}

int AR_PPM_Model::totalFreq() {
    if (!cached)
        cacheIt();

    return _totalFreq;
}

void AR_PPM_Model::cacheIt() {
    long long *block;
    if (ticks < 256)
        block = getCurrentBlock(0);
    else if (ticks < 3000)
        block = getCurrentBlock(1);
    else if (ticks < 70000)
        block = getCurrentBlock(2);
    else
        block = getCurrentBlock(AR_PPM_MODEL_ORDER - 1 - ML);


    cumFreq[0] = block[0];
    for (int i = 1; i < AR_TOTAL_SYMBOLS; i++)
        cumFreq[i] = cumFreq[i - 1] + block[i];

    _totalFreq = cumFreq[AR_TOTAL_SYMBOLS - 1];

    if (_totalFreq >= AR_FIRST_QRT) {
        for (int i = 0; i < AR_TOTAL_SYMBOLS; i++)
            block[i] = block[i] / 2 + 1;

        cacheIt();
    } else
        cached = true;
}

long long *AR_PPM_Model::getCurrentBlock(int order) {
    std::list<AR_symbol>::iterator it = lastSymbols.begin();

    struct index i;
    i.order = order;
    for (int j = 0; j < AR_PPM_MODEL_ORDER; j++)
        i.r[j] = *(it++);

    if (table.find(i) == table.end()) {
        long long *block = new long long[AR_TOTAL_SYMBOLS];
        memoryUsage += AR_TOTAL_SYMBOLS * sizeof(*block);
        for (int j = 0; j < AR_TOTAL_SYMBOLS; j++)
            block[j] = 1;

        table[i] = block;

        return block;
    }

    return table[i];
}

void AR_PPM_Model::resetModel() {
    std::map<struct index, long long* >::iterator i = table.begin();
    while (i != table.end())
        delete (*(i++)).second;

    table.clear();
    cached = false;
    ticks = 0;
}
