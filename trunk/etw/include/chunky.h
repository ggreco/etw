#if defined(__PPC__) && defined(__SASC)
#pragma options align=mac68k
#endif

struct ALine
{
	struct ALine *Next;
	struct ABlock *FirstBlock;
};

struct ABlock
{
	struct ABlock *Next;
	unsigned char *Buffer;
	short Length;
};

struct MChunky
{
	struct ALine *FirstLine;
	unsigned short lines,blocks;
	unsigned long buffers;
};

struct scaleAnimObjArgs
{
    struct MChunky *src;
    bitmap dest;
    int destmod;
    int xs, ys, ws, hs;
    int xd, yd, wd, hd;
};
  

// Da chunkyblitting.c

void bltchunkybitmap(bitmap,int,int,bitmap,int,int,int,int,int,int);
void bltbitmap_x(bitmap,int,int,bitmap,int,int,int,int,int,int,unsigned char);
void bltanimobjclipped(struct MChunky *,int, int, bitmap, int, int,int,int,int);
void bltanimobj(struct MChunky *,bitmap,int,int,int);
struct MChunky *convert_mchunky(FILE *,FILE *,int,int,int,long *);
struct MChunky *load_mchunky(FILE *,int, long *);
void do_p2c(unsigned char **,bitmap ,int ,int ,int ,long *);
void free_mchunky(struct MChunky *);
struct MChunky *CloneMChunky(struct MChunky *);
void bltanimobjscale(struct scaleAnimObjArgs *args);
void do_p2c(unsigned char **p,bitmap b,int width,int height,int depth,long *pens);
