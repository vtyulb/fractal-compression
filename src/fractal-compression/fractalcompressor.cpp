#include "fractalcompressor.h"
#include "fractal.h"

using std::min;

FractalCompressor::FractalCompressor(BMP *image, char *output):
    image(image)
{
    out = fopen(output, "w");
    quality = 10;
}

void FractalCompressor::compress() {
    nativeCompress(0, 0, image->TellWidth(), image->TellHeight());
    fflush(out);
}

void FractalCompressor::nativeCompress(int x1, int y1, int x2, int y2) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
            int nx1 = x1 + (x2 - x1) / 2 * i;
            int ny1 = y1 + (y2 - y1) / 2 * j;
            int nx2 = x1 + (x2 - x1) / 2 * (i + 1);
            int ny2 = y1 + (y2 - y1) / 2 * (j + 1);
            if (!tryWin(nx1, ny1, nx2, ny2))
                nativeCompress(nx1, ny1, nx2, ny2);
        }
}

bool FractalCompressor::tryWin(int x1, int y1, int x2, int y2) {
    double best = 1e+100;
    int step = (x2 - x1) * 2;
    Transform res;
    res.quad = 1;
    BMP *orig = selectImage(x1, y1, x2 - x1);
    double origBright = 0;
    for (int i = 0; i < orig->TellHeight(); i++)
        for (int j = 0; j < orig->TellWidth(); j++)
            origBright += select(orig->GetPixel(i, j));

    origBright /= (orig->TellHeight() * orig->TellWidth());

//    orig->WriteToFile("/home/vlad/orig.bmp");

    for (int mir = 0; mir < 2; mir++)
        for (int ang = 0; ang < 4; ang++)
            for (int i = 0; i < image->TellHeight(); i += step)
                for (int j = 0; j < image->TellWidth(); j += step) {
                    BMP *n = selectImage(j, i, step);
                    n = transform(n, mir, ang);
                    double nBright = 0;
                    for (int i = 0; i < n->TellHeight(); i++)
                        for (int j = 0; j < n->TellWidth(); j++)
                            nBright += select(n->GetPixel(i, j));

                    nBright /= (n->TellHeight() * n->TellWidth());

                    double k = nBright / origBright;
                    for (int i = 0; i < n->TellHeight(); i++)
                        for (int j = 0; j < n->TellWidth(); j++) {
                            RGBApixel pix = n->GetPixel(i, j);
                            pix.Blue = min(pix.Blue / k, 255.0);
                            pix.Red = min(pix.Red / k, 255.0);
                            pix.Green = min(pix.Green / k, 255.0);
                            n->SetPixel(i, j, pix);
                        }

//                    n->WriteToFile("/home/vlad/cur.bmp");

                    double d = diff(orig, n);
                    delete n;

                    if (d < best)
                        if (d < quality || (x2 - x1 == 2)) {
                            best = d;
                            res.angle = ang;
                            res.mirror = mir;
                            res.bright = k * 255;
                            res.x = j;
                            res.y = i;
                        }
                }

    fwrite(&res, sizeof(res), 1, out);
    if (x2 - x1 == 2 || best < quality) {
        printf("Dumped: (%d %d) (%d %d) : %d %d, step: %d, diff: %.2g\n", x1, y1, x2, y2, res.x, res.y, x2 - x1, best);
        return true;
    } else
        return false;
}

BMP *FractalCompressor::selectImage(int x, int y, int size) {
    BMP *res = new BMP;
    res->SetSize(size, size);
    res->SetBitDepth(24);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            res->SetPixel(i, j, image->GetPixel(x + i, y + j));

    return res;
}

int FractalCompressor::select(RGBApixel pixel) {
    return pixel.Red;
}

BMP *FractalCompressor::transform(BMP *im, int mir, int ang) {
    BMP *res = new BMP;
    res->SetSize(im->TellWidth(), im->TellHeight());
    if (mir) {
        for (int i = 0; i < im->TellHeight(); i++)
            for (int j = 0; j < im->TellWidth(); j++)
                res->SetPixel(i, j, im->GetPixel(im->TellHeight() - 1 - i, j));
    } else
        for (int i = 0; i < im->TellHeight(); i++)
            for (int j = 0; j < im->TellWidth(); j++)
                res->SetPixel(i, j, im->GetPixel(i, j));

    for (int k = 0; k < ang; k++) {
        BMP *tmp = im;
        im = res;
        res = tmp;
        for (int i = 0; i < res->TellWidth(); i++)
            for (int j = 0; j < res->TellHeight(); j++)
                res->SetPixel(i, j, im->GetPixel(im->TellHeight() - j - 1, i));
    }

    delete im;
    return res;
}

double FractalCompressor::diff(BMP *src, BMP *dst) {
    double res = 0;
    for (int i = 0; i < src->TellHeight(); i++)
        for (int j = 0; j < src->TellWidth(); j++) {
            double a = 0;
            a += select(src->GetPixel(i, j));
            for (int q = 0; q < 2; q++)
                for (int w = 0; w < 2; w++)
                    a -= select(dst->GetPixel(i * 2 + q, j * 2 + w)) / 4.0;

            if (a != 0)
                res += a * a;
        }

    res /= (src->TellHeight() * src->TellWidth());
    res = sqrt(res);

    return res;
}
