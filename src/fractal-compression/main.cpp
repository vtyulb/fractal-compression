#include <EasyBMP.h>
#include <fractalcompressor.h>
#include <fractal.h>

int compress(char *input, char *output, int s) {
    BMP image;
    image.ReadFromFile(input);

    FractalCompressor c(&image, output);
    c.compress();

    return 0;
}

int decompress(char *input, char *output) {

}

int main(int argc, char *argv[])
{
    if (argv[1][1] == 'c')
        return compress(argv[2], argv[3], 16);
    else
        return decompress(argv[2], argv[3]);

    return 0;
}