#include <EasyBMP.h>
#include <fractalcompressor.h>
#include <fractal.h>

#include <stdio.h>

#include <ar_encoder.h>
#include <ar_decoder.h>

int compress(char *input, char *output, int q, bool force) {
    BMP image;
    image.ReadFromFile(input);

    FractalCompressor c;
    c.compress(&image, output, q, force);

    AR_Encoder e(0, 0, 0);
    FILE *fin = fopen(output, "r");
    unsigned char symb;
    while (fread(&symb, 1, 1, fin))
        e.putSymbol(symb);

    fclose(fin);
    std::vector<char> res = e.getEncodedResult();
    FILE *fout = fopen(output, "w");
    fwrite(res.data(), res.size(), 1, fout);
    fclose(fout);

    return 0;
}

int decompress(char *input, char *output) {
    FILE *fin = fopen(input, "r");
    char symb;
    std::vector<char> data;
    while (fread(&symb, 1, 1, fin))
        data.push_back(symb);

    AR_Decoder d(data);
    data = d.getDecoded();
    fclose(fin);

    FractalCompressor c;
    c.decompress(data.data(), output);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argv[1][1] == 'c') {
        int quality;
        sscanf(argv[5], "%d", &quality);
        return compress(argv[2], argv[3], quality, argv[4][1] == 's');
    } else
        return decompress(argv[2], argv[3]);

    return 0;
}
