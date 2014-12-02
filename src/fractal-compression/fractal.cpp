#include <fractal.h>

BMP *generateSRC(int size) {
    BMP *res = new BMP;
    res->SetSize(size, size);
    res->SetBitDepth(24);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            RGBApixel pix;
            pix.Blue = 128;
            pix.Green = 128;
            pix.Red = 128;
            res->SetPixel(i, j, pix);
        }

    return res;
}
