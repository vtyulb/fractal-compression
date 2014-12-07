#ifndef ARMODEL_H
#define ARMODEL_H

typedef unsigned short AR_symbol;

const int AR_TOTAL_SYMBOLS = 257; // 256 == reset model
const int AR_CODE_VALUE_BITS = 30;
const int AR_MAX_VALUE = (1 << AR_CODE_VALUE_BITS) - 1;
const int AR_FIRST_QRT = AR_MAX_VALUE / 4 + 1;
const int AR_HALF = AR_FIRST_QRT * 2;
const int AR_THIRD_QRT = AR_FIRST_QRT * 3;

struct header {
    unsigned int ppm:1;
    unsigned int txt:1;
    unsigned int bwt:1;
    unsigned int size:29;
};

class AR_Model
{
    public:
        AR_Model() {}

        virtual int freq(AR_symbol) {return 0;}
        virtual int totalFreq() {return 0;}
        virtual void update(AR_symbol) {}
        virtual void resetModel() {}
};

#endif // MODEL_H
