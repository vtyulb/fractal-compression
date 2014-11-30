#ifndef FRACTAL_H
#define FRACTAL_H

#include <EasyBMP.h>

struct Transform {
    unsigned int mirror:1;
    unsigned int angle:2;
    unsigned int quad:1;
    unsigned int x:8;
    unsigned int y:8;
    unsigned int bright:8;
    unsigned int nothing:4;
};

struct header {
    unsigned int size:10;
    unsigned int n:22;
};

BMP *generateSRC(int size);

#endif // FRACTAL_H
