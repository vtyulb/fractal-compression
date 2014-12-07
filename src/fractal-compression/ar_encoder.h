#ifndef ARENCODER_H
#define ARENCODER_H

#include <vector>
#include "ar_model.h"

class AR_Encoder {
    public:
        AR_Encoder(bool usePPM, bool isTXT, bool BWTused);

        void putSymbol(AR_symbol);
        void putVector(std::vector<AR_symbol>);
        std::vector<char> getEncodedResult();

    private:
        std::vector<bool> res;
        bool packed;

        unsigned int size;
        unsigned int low;
        unsigned int high;
        int reverseBits;
        AR_Model *model;
        bool ppm;
        bool txt;
        bool bwt;

        void writeBit(int bit);
        std::vector<char> convert(const std::vector<bool> &data);
};

#endif // ARENCODER_H
