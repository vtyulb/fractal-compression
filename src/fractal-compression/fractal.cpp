#include <fractal.h>

BMP *generateSRC(int size) {
    BMP *res = new BMP;
    res->SetSize(size, size);
    res->SetBitDepth(24);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            RGBApixel pix;
            pix.Blue = 50;
            pix.Green = 50;
            pix.Red = 50;
            res->SetPixel(i, j, pix);
        }

    return res;
}
