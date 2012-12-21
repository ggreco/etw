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
    uint8_t *Buffer;
    int16_t Length;
};

struct MChunky
{
    struct ALine *FirstLine;
    uint16_t lines, blocks;
    uint32_t buffers;
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
struct MChunky *convert_mchunky(FILE *, FILE *, int, int, int, int32_t *);
struct MChunky *load_mchunky(FILE *, int, int32_t *);
void do_p2c(unsigned char **, uint8_t *, int, int, int, int32_t *);
void free_mchunky(struct MChunky *);
struct MChunky *CloneMChunky(struct MChunky *);
void bltanimobjscale(struct scaleAnimObjArgs *args);
