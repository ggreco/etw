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
    unsigned short lines, blocks;
    unsigned long buffers;
};

struct scaleAnimObjArgs
{
    struct MChunky *src;
    uint8_t * dest;
    int destmod;
    int xs, ys, ws, hs;
    int xd, yd, wd, hd;
};
  

// Da chunkyblitting.c

void bltchunkybitmap(uint8_t *, int, int, uint8_t *, int, int, int, int, int, int);
void bltbitmap_x(uint8_t *, int, int, uint8_t *, int, int, int, int, int, int, unsigned char);
void bltanimobjclipped(struct MChunky *, int, int, uint8_t *, int, int, int, int, int);
void bltanimobj(struct MChunky *, uint8_t *, int, int, int);
struct MChunky *convert_mchunky(FILE *, FILE *, int, int, int, LONG *);
struct MChunky *load_mchunky(FILE *, int, LONG *);
void do_p2c(unsigned char **, uint8_t *, int, int, int, LONG *);
void free_mchunky(struct MChunky *);
struct MChunky *CloneMChunky(struct MChunky *);
void bltanimobjscale(struct scaleAnimObjArgs *args);
