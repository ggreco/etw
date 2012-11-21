#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct gfx
{
    uint8_t *bmap;
    unsigned char *Palette;
    int width,height,depth;
} gfx_t;

#define BITRASSIZE(w, h) ((uint32_t)(h) * ( ((uint32_t)(w) + 15) >> 3 & 0xFFFE))

typedef struct                       /**** BMP file header structure ****/
{
    unsigned short bfType;           /* Magic number for file */
    unsigned int   bfSize;           /* Size of file */
    unsigned short bfReserved1;      /* Reserved */
    unsigned short bfReserved2;      /* ... */
    unsigned int   bfOffBits;        /* Offset to bitmap data */
} __attribute__((__packed__))  BitmapHeader;

#  define BF_TYPE 0x4D42             /* "MB" */

typedef struct                       /**** BMP file info structure ****/
{
    unsigned int   biSize;           /* Size of info header */
    int            biWidth;          /* Width of image */
    int            biHeight;         /* Height of image */
    unsigned short biPlanes;         /* Number of color planes */
    unsigned short biBitCount;       /* Number of bits per pixel */
    unsigned int   biCompression;    /* Type of compression to use */
    unsigned int   biSizeImage;      /* Size of image data */
    int            biXPelsPerMeter;  /* X pixels per meter */
    int            biYPelsPerMeter;  /* Y pixels per meter */
    unsigned int   biClrUsed;        /* Number of colors used */
    unsigned int   biClrImportant;   /* Number of important colors */
} __attribute__((__packed__))  BitmapInfo;

/*
 * Constants for the biCompression field...
 */

#  define BI_RGB       0             /* No compression - straight BGR data */
#  define BI_RLE8      1             /* 8-bit run-length compression */
#  define BI_RLE4      2             /* 4-bit run-length compression */
#  define BI_BITFIELDS 3             /* RGB bitmap with RGB masks */

typedef struct                       /**** Colormap entry structure ****/
{
    unsigned char  b;          /* Blue value */
    unsigned char  g;         /* Green value */
    unsigned char  r;           /* Red value */
    unsigned char  a;      /* Reserved */
}__attribute__((__packed__)) RGBQUAD;

typedef struct                       /**** Bitmap information structure ****/
{
    BitmapHeader     head;      /* Image header */
    BitmapInfo       info;      /* Image header */
    RGBQUAD          colors[256]; /* Image colormap */
} __attribute__((__packed__)) BMPheader;

void do_p2c(unsigned char **p, uint8_t * b, int width, int height, int depth)
{
    register int k, i, source_color, current_bit;
    int planesize = BITRASSIZE(width, height);

    for(k = 0; k < planesize; k++)
    {
        for(current_bit = 128; current_bit > 0; current_bit >>= 1)
        {
            source_color = 0;

            for(i = 0; i < depth; i++)
            {
                if(p[i][k] & current_bit)
                    source_color |= (1 << i); 
            }

            *b++ = source_color;
        }
    }
}

void fill_plane(uint8_t *src, uint8_t *dst, int plane, int size) {
    int i, j;

    for (i = 0; i < size; i++) {
        uint8_t p = 0;
        for (j = 0; j < 8; ++j)
            if (src[i * 8 + j] & plane)
                p |= (1 << (7 - j));

        dst[i] = p; 
    }
}

int SaveGfxObject(BMPheader *h, uint8_t *pixels, char *name)
{
    FILE *fh;
    int i;
    uint16_t temp;
    uint8_t *plane;
    char buffer[4];
    int planesize = BITRASSIZE(h->info.biWidth, h->info.biHeight);
    
    if (!(plane = malloc(planesize)))
        return 0;

    if (!(fh = fopen(name, "wb"))) {
        free(plane);
        return 0;
    }

    fwrite("GGFX", 4, 1, fh);

    temp = htons(h->info.biWidth);
    fwrite(&temp, sizeof(uint16_t), 1, fh);
    temp = htons(h->info.biHeight);
    fwrite(&temp, sizeof(uint16_t), 1, fh);
    temp = htons(h->info.biBitCount);
    fwrite(&temp, sizeof(uint16_t), 1, fh);

    for (i = 0; i < (1 << h->info.biBitCount); ++i) {
        fwrite(&h->colors[i].r, 1, 1, fh);
        fwrite(&h->colors[i].g, 1, 1, fh);
        fwrite(&h->colors[i].b, 1, 1, fh);
    }

    for (i = 0; i < h->info.biBitCount; ++i) {
        fill_plane(pixels, plane, 1 << i, planesize);
        if (fwrite(plane, 1, planesize, fh) != planesize) {
            fprintf(stderr, "Error writing plane datas!\n");
            fclose(fh);
            free(plane);
            return 0;
        }
    }

    free(plane); 
    fclose(fh);

    fprintf(stderr, "Saved GfxObject %s: %d x %d x %d\n", name,
           h->info.biWidth, h->info.biHeight, h->info.biBitCount);
    return 1;    
}

BMPheader *ReadBMP(char *src, uint8_t **pixels)
{
    FILE *fh;

    if ((fh = fopen(src, "rb"))) {
        BMPheader *h;
        if ((h = malloc(sizeof(BMPheader)))) {
            if (fread(h, 1, sizeof(BMPheader), fh) == sizeof(BMPheader)) {
                if (h->info.biBitCount == 8) {
                    if ((*pixels = malloc(h->info.biWidth * h->info.biHeight))) {
                        int i;

                        for (i = 1; i <= h->info.biHeight; ++i) 
                            fread((*pixels) + (h->info.biHeight - i) * h->info.biWidth, 1, h->info.biWidth, fh);

                        fprintf(stderr, "Loaded GIF %dx%d/%d\n", h->info.biWidth, h->info.biHeight, h->info.biBitCount);
                        fclose(fh);
                        return h;
                    }
                    free(pixels);
                }
                else
                    fprintf(stderr, "Unsupported bit depth %d\n", h->info.biBitCount);
            }
            free(h);
        }
        fclose(fh);
    }

    return NULL;
}

void remove_unused_planes(BMPheader *source, uint8_t *pixels)
{
    int i, j, size = source->info.biWidth * source->info.biHeight;
    fprintf(stderr, "Removing unused planes:");
    for (j = 7; j > 0; j--) {
        for (i = 0; i < size; ++i) 
            if (pixels[i] & (1 << j))
                break;

        if (i == size) {
            fprintf(stderr, " %d", j);
            source->info.biBitCount--;
        }
    }
    fprintf(stderr, "\n");
}

int main(int argc, char *argv[])
{
    BMPheader *source;
    uint8_t *pixels;

    if (argc != 3) {
        fprintf(stderr, "Usage:\n%s infile outfile\n", argv[0]);
        return 0;
    }

    if (!(source = ReadBMP(argv[1], &pixels))) {
        fprintf(stderr, "Unable to load <%s>\n", argv[1]);
        return 0;
    }

    remove_unused_planes(source, pixels);

    if (!(SaveGfxObject(source, pixels, argv[2]))) {
        fprintf(stderr, "Unable to save <%s>\n", argv[2]);
        return 0;
    }
}

