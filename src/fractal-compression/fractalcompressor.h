#ifndef FRACTALCOMPRESSOR_H
#define FRACTALCOMPRESSOR_H

#include <EasyBMP.h>
#include <stdio.h>
#include <fractal.h>

class FractalCompressor {
    public:
        FractalCompressor();
        void compress(BMP *image, char *output);
        void decompress(char *input, char *output);

    private:
        FILE *out;
        BMP *image;
        int quality;

        Transform *t;

        void nativeCompress(int x1, int y1, int x2, int y2);
        bool tryWin(int x1, int y1, int x2, int y2);
        int select(RGBApixel);
        BMP *selectImage(int x, int y, int size);

        BMP *transform(BMP *im, int mir, int ang);
        void applyBright(BMP *n, int bright);

        void deQuadro(int x1, int y1, int x2, int y2);

        double diff(BMP *src, BMP *dst);
};

#endif // FRACTALCOMPRESSOR_H
