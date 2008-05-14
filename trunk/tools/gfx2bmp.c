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

gfx_t *LoadGfxObject(char *name)
{
    int planesize;
    gfx_t *obj;
    FILE *fh;
    int i;
    uint16_t temp;
    uint8_t *planes[8];
    char buffer[4];

    if (!(obj = calloc(1, sizeof(gfx_t))))
        return NULL;

    if (!(fh = fopen(name, "rb"))) {
        free(obj);
        return NULL;
    }

    fread(buffer, 4, 1, fh);

    if (strncmp(buffer, "GGFX" /*-*/ , 4)) {
        fclose(fh);
        free(obj);
        return NULL;
    }

    fread(&temp, sizeof(uint16_t), 1, fh);
    obj->width = ntohs(temp);
    fread(&temp, sizeof(uint16_t), 1, fh);
    obj->height = ntohs(temp);
    fread(&temp, sizeof(uint16_t), 1, fh);
    obj->depth = ntohs(temp);

    if (!(obj->Palette = malloc((1 << obj->depth) * 3))) {
        fclose(fh);
        free(obj);
        return NULL;
    }

    fread(obj->Palette, sizeof(char) * 3, (1 << obj->depth), fh);

    if (!(obj->bmap = malloc(obj->width * obj->height))) {
        fclose(fh);
        free(obj->Palette);
        free(obj);
        return NULL;
    }
    planesize = BITRASSIZE(obj->width, obj->height);

    if (!(planes[0] = malloc(planesize * obj->depth))) {
        fclose(fh);
        free(obj->bmap);
        free(obj->Palette);
        free(obj);
        return NULL;
    }

    for (i = 1; i < obj->depth; i++) 
        planes[i] = (unsigned char *)planes[0] + i * planesize;


    for (i = 0; i < obj->depth; i++)
        if (fread(planes[i], 1,
                    planesize, fh) != planesize) {
            fclose(fh);
            free(planes[0]);
            free(obj->bmap);
            free(obj->Palette);
            free(obj);
            return NULL;
        }

    do_p2c(planes, obj->bmap, obj->width, obj->height,
            obj->depth);

    free(planes[0]);

    fclose(fh);

    fprintf(stderr, "Loaded GfxObject %s: %ld x %ld x %ld\n", name,
            obj->width, obj->height, obj->depth);

    return obj;
}

int WriteBMP(gfx_t *source, char *dest)
{
    FILE *fh;

    if (source->width != ( ( source->width + 3 ) / 4 ) * 4) {
        fprintf(stderr, "BMP images should have width multiple of 8!\n");
        return 1;
    }

    if ((fh = fopen(dest, "wb"))) {
        int i;
        BMPheader info;

        info.head.bfType = BF_TYPE; /* Non-portable... sigh */
        info.head.bfSize = sizeof(info) + source->width * source->height;
        info.head.bfReserved1 = 0;
        info.head.bfReserved2 = 0;
        info.head.bfOffBits   = sizeof(info);
        info.info.biSize = sizeof(info.info);
        info.info.biWidth = source->width;
        info.info.biHeight = source->height;
        info.info.biSizeImage = source->width * source->height;
        info.info.biBitCount = 8;
        info.info.biCompression = 0;
        info.info.biPlanes = 1;
        info.info.biClrUsed = 256;
        info.info.biXPelsPerMeter = 0;
        info.info.biYPelsPerMeter = 0;
        info.info.biClrImportant = 1 << source->depth;

        for (i = 0; i < (1 << source->depth); ++i) {
            info.colors[i].r = source->Palette[i * 3];
            info.colors[i].g = source->Palette[i * 3 + 1];
            info.colors[i].b = source->Palette[i * 3 + 2];
            info.colors[i].a = 0;
        }
        fwrite(&info, 1, sizeof(info), fh);

        for (i = 1; i <= source->height; i++) 
            fwrite(source->bmap + (source->height - i) * source->width, 1, source->width, fh);

        fclose(fh);
        return 0;
    }

    return 1;
}

int main(int argc, char *argv[])
{
    gfx_t *source;

    if (argc != 3) {
        fprintf(stderr, "Usage:\n%s infile outfile\n", argv[0]);
        return 0;
    }

    if (!(source = LoadGfxObject(argv[1]))) {
        fprintf(stderr, "Unable to load <%s>\n", argv[1]);
        return 0;
    }

    if (WriteBMP(source, argv[2])) {
        fprintf(stderr, "Unable to save <%s>\n", argv[2]);
        return 0;
    }
}

