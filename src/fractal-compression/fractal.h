#ifndef FRACTAL_H
#define FRACTAL_H

#include <EasyBMP.h>

struct Transform {
    unsigned int mirror:1;
    unsigned int angle:2;
    unsigned int quad:1;
    unsigned int x:8;
    unsigned int y:8;
    int bright:9;
    unsigned int nothing:3;
};

struct Header {
    unsigned int size:16;
    unsigned int n:16;
};

const double BRIGHT_COMPRESS = 0.75;
const int MIN_BLOCK = 2;

BMP *generateSRC(int size);

#endif // FRACTAL_H
