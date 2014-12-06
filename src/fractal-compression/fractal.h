#ifndef FRACTAL_H
#define FRACTAL_H

#include <EasyBMP.h>

struct Matrix {
    unsigned char r[512][512];

    inline unsigned char* operator [](int i) {
        return r[i];
    }

    void initFromImage(BMP *src, int channel) {
        for (int i = 0; i < src->TellHeight(); i++)
            for (int j = 0; j < src->TellWidth(); j++) {
                RGBApixel pix = src->GetPixel(i, j);
                r[i][j] = *(((unsigned char*)&pix) + channel);
            }
    }
};

struct Transform {
    unsigned int mirror:1;
    unsigned int angle:2;
    unsigned int quad:1;
    unsigned int x:8;
    unsigned int y:8;
    unsigned int p:12;
    signed char q;
};

struct Header {
    unsigned int size:16;
    unsigned int n:16;
};

const int MIN_BLOCK = 2;
const int ITERATIONS = 50;
const double DIV = 32.0;

BMP *generateSRC(int size);

#endif // FRACTAL_H
