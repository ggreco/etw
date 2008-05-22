#ifndef ETW_STRUCTS_H

#if defined(__PPC__) && defined(__SASC)
#pragma options align=mac68k
#endif

#define ETW_STRUCTS_H

#define CHAR_ARRAY_LEN 96

struct Rect
{
    int MinX, MinY, MaxX, MaxY;
};

struct myfont
{
    uint8_t *bm;
    int width, height;
};

struct MyScaleArgs
{
    int SrcX, SrcY;
    int SrcSpan, DestSpan;
    int DestX, DestY;
    int SrcWidth, SrcHeight, DestWidth, DestHeight;
    uint8_t *Src, *Dest;
};

struct MyFastScaleArgs
{
    int SrcSpan, DestSpan;
    int SrcWidth, SrcHeight, DestWidth, DestHeight;
    uint8_t *XRef, *YRef;
    uint8_t *Src, *Dest;
};

// Da generic_video.c

int drawtext(char *, int, int, int, int);
void setfont(struct myfont *);
struct myfont *openfont(char *);
void closefont(struct myfont *);
void rectfill_pattern(uint8_t * b, int x1, int y1, int x2, int y2, uint8_t color, int width);
void rectfill(uint8_t * b, int x1, int y1, int x2, int y2, uint8_t color, int width);
void bitmapScale(struct MyScaleArgs *);
void freedraw(long, int, int, int, int);
void freepolydraw(long, int, int *);
void draw(long, int, int, int, int);
void polydraw(long, int, int, int, int *);
void bitmapFastScale(struct MyFastScaleArgs *);

#endif

