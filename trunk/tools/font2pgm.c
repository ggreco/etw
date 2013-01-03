#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *in, *out;

    if (argc != 3) {
        fprintf(stderr, "Usage:\n%s [input ETW font] [output pgm file]\n\n", argv[0]);
        return 0;
    }

    if ((in = fopen(argv[1], "rb"))) {
        unsigned char w, h;
        int len;
        fread(&w, 1, 1, in);
        fread(&h, 1, 1, in);
        len = (int)w * (int)h * 96;

        unsigned char dest[len];

        if (fread(dest, 1, len, in) == len) {
            int i;

            for (i = 0; i < len; ++i)
                dest[i] = dest[i] ? 0 : 1;

            if ((out = fopen(argv[2], "wb"))) {
                fprintf(out, "P5 %d %d 1\n", (int)w * 96, (int)h);
                fwrite(dest, 1, len, out);
                fclose(out);
            }            
        }
        else
            fprintf(stderr, "Bitmap len mismatch, expecting %dx%d\n", (int)w, (int)h);

        fclose(in);
    }
}
