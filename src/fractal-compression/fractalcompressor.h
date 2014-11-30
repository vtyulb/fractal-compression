#ifndef FRACTALCOMPRESSOR_H
#define FRACTALCOMPRESSOR_H

#include <EasyBMP.h>
#include <stdio.h>

class FractalCompressor {
    public:
        FractalCompressor(BMP *image, char *output);
        void compress();

    private:
        FILE *out;
        BMP *image;
        int quality;

        void nativeCompress(int x1, int y1, int x2, int y2);
        bool tryWin(int x1, int y1, int x2, int y2);
        int select(RGBApixel);
        BMP *selectImage(int x, int y, int size);
        BMP *transform(BMP *im, int mir, int ang);

        double diff(BMP *src, BMP *dst);
};

#endif // FRACTALCOMPRESSOR_H
