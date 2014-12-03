#include "fractalcompressor.h"
#include "fractal.h"
#include <vector>

using std::min;
using std::max;

FractalCompressor::FractalCompressor() { }

void FractalCompressor::compress(BMP *im, char *output) {
    out = fopen(output, "wb");
    image = im;
    quality = 10;

    pre(im);
    post(im);
    im->WriteToFile("testfile.bmp");

    Header h;
    h.size = im->TellHeight();
    fwrite(&h, sizeof(h), 1, out);

    pre(im);
    nativeCompress(0, 0, image->TellWidth(), image->TellHeight());
    for (int i = 0; i < image->TellWidth(); i++)
        for (int j = 0; j < image->TellHeight(); j++) {
            RGBApixel pix = image->GetPixel(i, j);
            pix.Red = pix.Green;
            image->SetPixel(i, j, pix);
        }

    nativeCompress(0, 0, image->TellWidth(), image->TellHeight());
    for (int i = 0; i < image->TellWidth(); i++)
        for (int j = 0; j < image->TellHeight(); j++) {
            RGBApixel pix = image->GetPixel(i, j);
            pix.Red = pix.Blue;
            image->SetPixel(i, j, pix);
        }

    nativeCompress(0, 0, image->TellWidth(), image->TellHeight());

    fclose(out);
}

void FractalCompressor::post(BMP *src) {
    for (int i = 0; i < src->TellHeight(); i++)
        for (int j = 0; j < src->TellWidth(); j++) {
            RGBApixel pix = src->GetPixel(i, j);
            int r = pix.Red + (pix.Blue - 128) * 1.14075;
            int g = pix.Red - (pix.Green - 128) * 0.3455 - 0.7169 * (pix.Blue - 128);
            int b = pix.Red + 1.779 * (pix.Green - 128);
            pix.Red = nrm(r);
            pix.Green = nrm(g);
            pix.Blue = nrm(b);
            src->SetPixel(i, j, pix);
        }
}

void FractalCompressor::pre(BMP *src) {
    for (int i = 0; i < src->TellHeight(); i++)
        for (int j = 0; j < src->TellWidth(); j++) {
            RGBApixel pix = src->GetPixel(i, j);
            int y = pix.Red * 0.299 + pix.Green * 0.587 + pix.Blue * 0.114;
            int u = pix.Red * (-0.169) - pix.Green * 0.332 + pix.Blue * 0.5 + 128;
            int v = pix.Red * 0.5 - 0.419 * pix.Green - pix.Blue * 0.0813 + 128;
            pix.Red = nrm(y);
            pix.Green = nrm(u);
            pix.Blue = nrm(v);
            src->SetPixel(i, j, pix);
        }
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

void FractalCompressor::decompress(char *input, char *output) {
    FILE *fin = fopen(input, "rb");

    Header h;
    fread(&h, sizeof(h), 1, fin);

    image = generateSRC(h.size);

    std::vector<Transform> transforms;
    Transform tr;
    while (fread(&tr, sizeof(tr), 1, fin) > 0)
        transforms.push_back(tr);

    Transform *next;
    for (int i = 0; i < 50; i++) {
        t = transforms.data();
        deQuadro(0, 0, h.size, h.size);

        next = t;
    }
    BMP *redChannel = image;
    fastClear(redChannel);
    redChannel->WriteToFile("red.bmp");

    Transform *last;

    image = generateSRC(h.size);
    for (int i = 0; i < 50; i++) {
        t = next;
        deQuadro(0, 0, h.size, h.size);

        last = t;
    }
    BMP *greenChannel = image;
    fastClear(greenChannel);
    greenChannel->WriteToFile("green.bmp");


    image = generateSRC(h.size);
    for (int i = 0; i < 50; i++) {
        t = last;
        deQuadro(0, 0, h.size, h.size);
    }
    fastClear(image);
    image->WriteToFile("blue.bmp");


    for (int i = 0; i < image->TellHeight(); i++)
        for (int j = 0; j < image->TellWidth(); j++) {
            RGBApixel pix = image->GetPixel(i, j);
            pix.Blue = pix.Red;
            pix.Red = redChannel->GetPixel(i, j).Red;
            pix.Green = greenChannel->GetPixel(i, j).Red;
            image->SetPixel(i, j, pix);
        }

    //don't ask me why
    for (int i = 0; i < image->TellHeight(); i++)
        for (int j = 0; j < image->TellWidth(); j++) {
            RGBApixel pix = image->GetPixel(i, j);
            redChannel->SetPixel(j, i, pix);
        }

    post(redChannel);
    redChannel->WriteToFile(output);
}

void FractalCompressor::deQuadro(int x1, int y1, int x2, int y2) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
            int nx1 = x1 + (x2 - x1) / 2 * i;
            int ny1 = y1 + (y2 - y1) / 2 * j;
            int nx2 = x1 + (x2 - x1) / 2 * (i + 1);
            int ny2 = y1 + (y2 - y1) / 2 * (j + 1);
            if (t->quad) {
                t++;
                deQuadro(nx1, ny1, nx2, ny2);
            } else {
                int size = nx2 - nx1;
                BMP *im = selectImage(t->y, t->x, (x2 - x1));
                applyBright(im, t->bright);

                for (int i = 0; i < size; i++)
                    for (int j = 0; j < size; j++) {
                        int red = 0;
                        for (int q = 0; q < 2; q++)
                            for (int w = 0; w < 2; w++)
                                red += im->GetPixel(i * 2 + w, j * 2 + q).Red / 4.0;

                        RGBApixel pix;
                        pix.Red = red;
                        image->SetPixel(i + ny1, j + nx1, pix);
                    }

                t++;
            }
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

    for (int mir = 0; mir < 2; mir++)
        for (int ang = 0; ang < 4; ang++)
            for (int i = 0; i < image->TellHeight() - step + 1; i += step / 2)
                for (int j = 0; j < image->TellWidth() - step + 1; j += step / 2) {
                    BMP *n = selectImage(j, i, step);
                    n = transform(n, mir, ang);
                    double nBright = 0;
                    for (int i = 0; i < n->TellHeight(); i++)
                        for (int j = 0; j < n->TellWidth(); j++)
                            nBright += select(n->GetPixel(i, j));

                    nBright /= (n->TellHeight() * n->TellWidth());

                    int k = origBright - nBright * BRIGHT_COMPRESS;
                    applyBright(n, k);
//                    n->WriteToFile("/home/vlad/cur.bmp");

                    double d = diff(orig, n);
                    delete n;

                    if (d < best)
                        if (d < quality || (x2 - x1 == 2)) {
                            best = d;
                            res.angle = ang;
                            res.mirror = mir;
                            res.bright = k;
                            res.x = j;
                            res.y = i;

                            if (d < 1)
                                goto l1;// |
                        }               // |
                }                       // |
                                        // |
    l1://<---------------------------------/

    if (x2 - x1 == 2 || best < quality) {
        res.quad = 0;
        fwrite(&res, sizeof(res), 1, out);
        printf("Dumped: (%d %d) (%d %d) : %d %d, step: %d, diff: %.2g\n", x1, y1, x2, y2, res.x, res.y, x2 - x1, best);
        return true;
    } else {
        fwrite(&res, sizeof(res), 1, out);
        return false;
    }
}

void FractalCompressor::applyBright(BMP *n, int k) {
    for (int i = 0; i < n->TellHeight(); i++)
        for (int j = 0; j < n->TellWidth(); j++) {
            RGBApixel pix;
            pix.Red = nrm(n->GetPixel(i, j).Red * BRIGHT_COMPRESS + k);
            n->SetPixel(i, j, pix);
        }
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

void FractalCompressor::fastClear(BMP *src) {
    for (int i = 0; i < src->TellHeight(); i++)
        for (int j = 0; j < src->TellWidth(); j++) {
            RGBApixel pix = src->GetPixel(i, j);
            pix.Green = 0;
            pix.Blue = 0;
            src->SetPixel(i, j, pix);
        }
}

int FractalCompressor::nrm(int a) {
    return min(max(a, 0), 255);
}
