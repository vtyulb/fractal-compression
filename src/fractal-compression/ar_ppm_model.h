#ifndef AR_PPM_MODEL_H
#define AR_PPM_MODEL_H

#include "ar_model.h"
#include <list>
#include <map>

const int AR_PPM_MODEL_ORDER = 5;
const int AR_PPM_MODEL_AGRO = 300;

const long long AR_MAX_MEMORY_USAGE = 1000; // in megabytes

struct index {
    int r[AR_PPM_MODEL_ORDER];
    int order;

    bool operator < (const index &i) const {
        if (order < i.order)
            return true;
        else if (order > i.order)
            return false;

        for (int j = AR_PPM_MODEL_ORDER - 1; j > AR_PPM_MODEL_ORDER - order - 1; j--)
            if (r[j] < i.r[j])
                return true;
            else if (r[j] > i.r[j])
                return false;

        return false;
    }

    index() {
        order = AR_PPM_MODEL_ORDER;
    }
};

class AR_PPM_Model : public AR_Model {
    public:
        AR_PPM_Model(bool txt);

        int freq(AR_symbol);
        int totalFreq();
        void update(AR_symbol);
        void resetModel();

    private:
        std::list<AR_symbol> lastSymbols;
        std::map<index, long long* > table;

        //cache block
        int cumFreq[AR_TOTAL_SYMBOLS];
        bool cached;
        int _totalFreq;
        //end of cache block

        long long ticks;
        int memoryUsage;
        int ML;


        void cacheIt();
        long long *getCurrentBlock(int order);

};

#endif // AR_PPM_MODEL_H
