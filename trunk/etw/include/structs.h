#ifndef ETW_STRUCTS_H

#if defined(__PPC__) && defined(__SASC)
#pragma options align=mac68k
#endif

#define ETW_STRUCTS_H

#define CHAR_ARRAY_LEN 96

typedef uint8_t *bitmap;

struct Rect
{
	WORD MinX,MinY,MaxX,MaxY;
};

struct myfont
{
	uint8_t * bm;
	int width,height;
};

struct MyScaleArgs
{
	int SrcX,SrcY;
	int SrcSpan,DestSpan;
	int DestX,DestY;
	int SrcWidth,SrcHeight,DestWidth,DestHeight;
	uint8_t *Src, *Dest;
};

struct MyFastScaleArgs
{
	int SrcSpan,DestSpan;
	int SrcWidth,SrcHeight,DestWidth,DestHeight;
	UBYTE *XRef,*YRef;
	uint8_t *Src, *Dest;
};

// Da generic_video.c

int drawtext(char *,int,int,int,int);
void setfont(struct myfont *);
struct myfont *openfont(char *);
void closefont(struct myfont *);
void rectfill_pattern(uint8_t * b,int x1,int y1,int x2,int y2,unsigned char color,int width);
void rectfill(uint8_t * b,int x1,int y1,int x2,int y2,unsigned char color,int width);
void bitmapScale(struct MyScaleArgs *);
void freedraw(long , WORD, WORD , WORD , WORD);
void freepolydraw(long , int , WORD *);
void draw(long , WORD, WORD , WORD , WORD);
void polydraw(long , WORD, WORD, int , WORD *);
void bitmapFastScale(struct MyFastScaleArgs *);

#endif

