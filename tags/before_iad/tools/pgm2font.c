#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *in, *out;

    if (argc != 3) {
        fprintf(stderr, "Usage:\n%s [input ETW font] [output pgm file]\n\n", argv[0]);
        return 0;
    }

    if ((in = fopen(argv[1], "rb"))) {
        int w, h, depth;
        if (fscanf(in, "P5 %d %d %d\n", &w, &h, &depth) == 3) {
            int len = (int)w * (int)h;
            unsigned char dest[len];

            if (fread(dest, 1, len, in) == len) {
                int i;

                if (depth == 1) {
                    for (i = 0; i < len; ++i)
                        dest[i] = dest[i] ? 0 : 1;
                }
                else {
                    depth /= 2;
                    for (i = 0; i < len; ++i) {
                        if (dest[i] > depth)
                            dest[i] = 0;
                        else
                            dest[i] = 1;
                    }

                }

                if ((out = fopen(argv[2], "wb"))) {
                    unsigned char c;
                    c = w/96;
                    fwrite(&c, 1, 1, out);
                    c = h;
                    fwrite(&c, 1, 1, out);
                    fwrite(dest, 1, len, out);
                    fclose(out);
                }            
                else
                    fprintf(stderr, "Unable to open output file: %s\n", argv[2]);
            }
            else
                fprintf(stderr, "Unable to find needed bytes for font size %dx%d", w/96, h);
        }
        else
            fprintf(stderr, "Error parsing source file header!\n");

        fclose(in);
    }
    else
        fprintf(stderr, "Unable to open source file: %s\n", argv[1]);
}
