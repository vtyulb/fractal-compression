#ifndef AR_DECODER_H
#define AR_DECODER_H

#include <vector>
#include "ar_model.h"

class AR_Decoder {
    public:
        AR_Decoder(const std::vector<char> data);
        std::vector<char> getDecoded();

    private:
        unsigned int low;
        unsigned int high;
        unsigned int value;
        int size;
        unsigned long long position;
        AR_Model *model;

        bool bwt;

        std::vector<char> data;

        AR_symbol getNextSymbol();
        int getNextBit();
        AR_symbol findSymbol(int cum);
};\

#endif // AR_DECODER_H
