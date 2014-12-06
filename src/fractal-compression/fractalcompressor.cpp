#include "fractalcompressor.h"
#include "fractal.h"
#include <vector>

using std::min;
using std::max;

FractalCompressor::FractalCompressor() { }

void FractalCompressor::compress(BMP *im, char *output, int q) {
    out = fopen(output, "wb");
    image = im;
//    quality = 1050 - q * 10;
    quality = 100;

//    pre(im);
//    post(im);
//    im->WriteToFile("testfile.bmp");

    Header h;
    h.size = im->TellHeight();
    fwrite(&h, sizeof(h), 1, out);

    pre(im);
    for (int i = 2; i >= 0; i--) {
        main.initFromImage(im, i);
        nativeCompress(0, 0, image->TellWidth(), image->TellHeight());
    }

    fclose(out);
}

void FractalCompressor::post(BMP *src) {
    for (int i = 0; i < src->TellHeight(); i++)
        for (int j = 0; j < src->TellWidth(); j++) {
            RGBApixel pix = src->GetPixel(i, j);
            int r = pix.Red + (pix.Blue - 128) * 1.14075 + 10;
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

    char dt;
    while (fread(&dt, 1, 1, fin) == 1)
        data.push_back(dt);


    Transform *next;
    for (int i = 0; i < ITERATIONS; i++) {
        t = (Transform*)data.data();
        deQuadro(0, 0, h.size, h.size);

        next = t;
    }
    BMP *redChannel = image;
    fastClear(redChannel);
    redChannel->WriteToFile("red.bmp");

    Transform *last;

    image = generateSRC(h.size);
    for (int i = 0; i < ITERATIONS; i++) {
        t = next;
        deQuadro(0, 0, h.size, h.size);

        last = t;
    }
    BMP *greenChannel = image;
    fastClear(greenChannel);
    greenChannel->WriteToFile("green.bmp");


    image = generateSRC(h.size);
    for (int i = 0; i < ITERATIONS; i++) {
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

    post(image);
    image->WriteToFile(output);
}

void FractalCompressor::deQuadro(int x1, int y1, int x2, int y2) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
            int nx1 = x1 + (x2 - x1) / 2 * i;
            int ny1 = y1 + (y2 - y1) / 2 * j;
            int nx2 = x1 + (x2 - x1) / 2 * (i + 1);
            int ny2 = y1 + (y2 - y1) / 2 * (j + 1);
            if (nx2 - nx1 == MIN_BLOCK) {
                unsigned char *px = (unsigned char*)t;
                for (int i = nx1; i < nx2; i++)
                    for (int j = ny1; j < ny2; j++) {
                        RGBApixel pix;
                        pix.Red = *px;
                        px++;
                        image->SetPixel(i, j, pix);
                    }

                t = (Transform*)px;
            } else if (t->quad) {
                t = (Transform*)(((char*)t) + 1);
                deQuadro(nx1, ny1, nx2, ny2);
            } else {
                int size = nx2 - nx1;
                BMP *im = selectImage(t->x, t->y, (x2 - x1));
//                im->WriteToFile("tmp.bmp");
                applyBright(im, t->p, t->q);
//                im->WriteToFile("tmp-ap.bmp");

                for (int i = 0; i < size; i++)
                    for (int j = 0; j < size; j++) {
                        int red = 0;
                        for (int q = 0; q < 2; q++)
                            for (int w = 0; w < 2; w++)
                                red += im->GetPixel(i * 2 + w, j * 2 + q).Red / 4.0;

                        RGBApixel pix;
                        pix.Red = red;
                        image->SetPixel(i + nx1, j + ny1, pix);
                    }

                t++;
            }
        }
}

bool FractalCompressor::tryWin(int x1, int y1, int x2, int y2) {
    if (x2 - x1 == MIN_BLOCK) {
        for (int i = x1; i < x2; i++)
            for (int j = y1; j < y2; j++)
                fwrite(&(main[i][j]), 1, 1, out);

        return true;
    }

    if (x1 == 64 && x2 == 80 && y1 == 64 && y2 == 80)
        main[0][0] = -64;

    double best = 1e+100;
    int step = (x2 - x1) * 2;
    Transform res;
    res.quad = 1;

    long long firstSum = 0, sqSum = 0;
    for (int i = x1; i < x2; i++)
        for (int j = y1; j < y2; j++) {
            first[i - x1][j - y1] = main[i][j];
            firstSum += main[i][j];
            sqSum += main[i][j] * main[i][j];
        }

    long long firstDivision = (step / 2) * (step / 2) * sqSum - firstSum * firstSum;

//    for (int mir = 0; mir < 2; mir++)
//        for (int ang = 0; ang < 4; ang++)
            for (int i = 0; i < image->TellHeight() - step + 1; i += step)
                for (int j = 0; j < image->TellWidth() - step + 1; j += step) {

                    for (int k = 0; k < step / 2; k++)
                        for (int u = 0; u < step / 2; u++)
                            second[k][u] = 0;

                    for (int k = i; k < i + step; k++)
                        for (int u = j; u < j + step; u++)
                            second[(k - i) / 2][(u - j) / 2] += main[k][u] / 4;

                    long long secondSum = 0;
                    for (int i = 0; i < step / 2; i++)
                        for (int j = 0; j < step / 2; j++)
                            secondSum += second[i][j];

                    long long ab = 0;
                    for (int i = 0; i < step / 2; i++)
                        for (int j = 0; j < step / 2; j++)
                            ab += first[i][j] * second[i][j];

                    double u = (firstDivision) ? ((step / 2) * (step / 2) * double(ab) - secondSum * firstSum) / firstDivision : 0;
                    signed char v = (secondSum - u * firstSum) / (step / 2) / (step / 2);

                    for (int i = 0; i < step / 2; i++)
                        for (int j = 0; j < step / 2; j++)
                            second[i][j] = second[i][j] * u + v;

                    double d = 0;
                    for (int i = 0; i < step / 2; i++)
                        for (int j = 0; j < step / 2; j++)
                            d += (first[i][j] - second[i][j]) * (first[i][j] - second[i][j]);

                    d /= (step * step / 4);

                    if (d < best)
                        if (d < quality || (x2 - x1 == MIN_BLOCK)) {
//                            if (first[0][0] != -64)
//                                u = u + 0.0001;

                            best = d;
                            res.angle = 0;// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                            res.mirror = 0;// !!!!!!!!!!!!!!!1111111111111111111!!!!!!!!!!!111111111111111111
                            res.p = u;
                            res.q = v;
                            res.x = i;
                            res.y = j;

                            if (d <= 1)
                                goto l1;// |
                        }               // |
                }                       // |
                                        // |
    l1://<---------------------------------/

    if (x2 - x1 == MIN_BLOCK || best < quality) {
        res.quad = 0;
        fwrite(&res, sizeof(res), 1, out);
        printf("Dumped: (%d %d) (%d %d) : %d %d, step: %d, diff: %.2g\n", x1, y1, x2, y2, res.x, res.y, x2 - x1, best);
        return true;
    } else {
        fwrite(&res, 1, 1, out);
        return false;
    }
}

void FractalCompressor::applyBright(BMP *n, double k, int offset) {
    for (int i = 0; i < n->TellHeight(); i++)
        for (int j = 0; j < n->TellWidth(); j++) {
            RGBApixel pix;
            pix.Red = nrm(n->GetPixel(i, j).Red * k + offset);
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

void FractalCompressor::fastClear(BMP *src) {
    for (int i = 0; i < src->TellHeight(); i++)
        for (int j = 0; j < src->TellWidth(); j++) {
            RGBApixel pix = src->GetPixel(i, j);
            pix.Green = pix.Red;
            pix.Blue = pix.Red;
            src->SetPixel(i, j, pix);
        }
}

int FractalCompressor::nrm(int a) {
    return char(a);
//    return min(max(a, 0), 255);
}
