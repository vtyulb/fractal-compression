#ifndef FRACTALCOMPRESSOR_H
#define FRACTALCOMPRESSOR_H

#include <EasyBMP.h>
#include <stdio.h>
#include <fractal.h>
#include <vector>

class FractalCompressor {
    public:
        FractalCompressor();
        void compress(BMP *image, char *output, int q, int force);
        void decompress(char *input, char *output);

    private:
        FILE *out;

        BMP *image;
        Matrix main;
        Matrix first, second;

        int quality;
        int forceLevel;

        Transform *t;

        void nativeCompress(int x1, int y1, int x2, int y2);
        bool tryWin(int x1, int y1, int x2, int y2);
        inline int select(RGBApixel);
        BMP *selectImage(int x, int y, int size);

        void selectMatrix(Matrix &m, int x1, int y1, int x2, int y2);

        BMP *transform(BMP *im, int mir, int ang);
        void applyBright(BMP *n, double bright, int offset);

        void deQuadro(int x1, int y1, int x2, int y2);

        double diff(BMP *src, BMP *dst);

        void fastClear(BMP *src);

        void pre(BMP *src);
        void post(BMP *src);

        inline unsigned char nrm(int);
        inline double conf(int);
};

#endif // FRACTALCOMPRESSOR_H
