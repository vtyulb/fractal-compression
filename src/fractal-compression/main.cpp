#include <EasyBMP.h>
#include <fractalcompressor.h>
#include <fractal.h>

int compress(char *input, char *output, int q) {
    BMP image;
    image.ReadFromFile(input);

    FractalCompressor c;
    c.compress(&image, output, q);

    return 0;
}

int decompress(char *input, char *output) {
    FractalCompressor c;
    c.decompress(input, output);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argv[1][1] == 'c') {
        int quality;
        sscanf(argv[5], "%d", &quality);
        return compress(argv[2], argv[3], quality);
    } else
        return decompress(argv[2], argv[3]);

    return 0;
}
