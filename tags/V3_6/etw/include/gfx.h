/* Gli AnimObject hanno TUTTI maschera e bitmap su cui salvare lo sfondo,
    i gfx NO, usare i gfx_t solo su oggetti che NON cambiano mai
    e stanno sempre dietro ai personaggi 
 */

/* Tipi di oggetti grafici, vengono tutti liberati da FreeGraphics */

#include "mytypes.h"
#include "lists.h"
#include "structs.h"
#include "mydebug.h"

#ifndef ETW_GFX_SYSTEM_H

#if defined(__PPC__) && defined(__SASC)
#pragma options align=mac68k
#endif

// Questi li metto se non voglio il debug!

#define ETW_GFX_SYSTEM_H

#define TYPE_GFXOBJ  1
#define TYPE_ANIMOBJ 2  
#define TYPE_RASTPORT   3
#define TYPE_DBUFINFO   4
#define TYPE_BITMAP  5
#define TYPE_SCREENBUFFER 6
#define TYPE_PORT 7
#define TYPE_MEMORY  8

struct anim
{
    struct MyMinNode node;    /* Per attaccarlo alla drawlist */
    struct MChunky **Frames;    /* Le bitmap dei vari frames */
    unsigned char *Palette;         /* Puntatore alla palette, organizzata come RGB */
    int *Widths, *Heights;        /* Dimensioni dei singoli frames */
    int32_t *pens;       /* Nel caso di remapping conservo le penne che alloco */
    uint8_t *bg;           /* Sfondo, grande max_width*max_height */
    uint16_t max_width, max_height; /* Dimensioni massime dei frames */
    uint16_t nframes, current_frame;  /* Frame corrente e numero di frames */
    uint16_t real_depth;
    BOOL moved;
    int x_pos, y_pos, x_back, y_back; /* Coordinate dell'angolo sinistro dell'immagine
                        visualizzata e del "cut" dello sfondo */
    int bottom;         /* Coordinata y del pixel piu' basso dell'immagine,
                        usata per fare il sorting */
    int Flags;
    int ScaledX,ScaledY;
    uint8_t *sb;  /* Buffers per lo scaling */
};

/* Per flags */

#define AOBJ_CLONED 1
#define AOBJ_SHAREPENS 2
#define AOBJ_COPIED 4
#define AOBJ_INTERLEAVED 8
#define AOBJ_OVER 16
#define AOBJ_BEHIND 32
#define AOBJ_NOMASK 64

/* Oggetti semplici, sfondi per esempio */

struct gfx
{
    uint8_t *bmap;
    unsigned char *Palette;
    int32_t *pens;
    int width,height,realdepth;
};

#define BITRASSIZE(w, h) ((uint32_t)(h) * ( ((uint32_t)(w) + 15) >> 3 & 0xFFFE))

typedef struct anim anim_t;
typedef struct gfx gfx_t;

// Macros

#define RemAnimObj(a) MyRemove((struct MyNode *)a);
#define BltGfxObj(s,xs,ys,d,xd,yd,w,h,dm) bltchunkybitmap(s->bmap,xs,ys,d,xd,yd,w,h,s->width,dm)
#define BltAnimObj(obj,dest,frame,x,y,dmod) bltanimobj(obj->Frames[frame],dest,x,y,dmod)

/* Funzioni trasformate in macro! */

#define MoveAnimObj(obj, x, y) {obj->x_pos=(x);obj->y_pos=(y);obj->moved=TRUE;}
#define ChangeAnimObj(obj, f)  {obj->current_frame=(f); obj->bottom=obj->y_pos+obj->Heights[f];}


extern BOOL use_remapping; /* Questa variabile gestisce il remapping, deve
                    essere TRUE solo se si lavora sullo schermo
                    del WB, in caso si usi il remapping gli oggetti
                    allocati con i datatypes non vanno liberati e
                    le bitmap non vengono copiate, quindi l'occupazione
                    di memoria e' MOLTO maggiore
                 */

extern BOOL save_back;  /* Questa variabile regola la gestione degli anim_t, di
                default e' TRUE, se e' false non funziona la ClearAnimObj(),
                ovviamente il redrawing e' piu' veloce pero'.
             */

extern BOOL public_screen; /* Viene attivato se si sta lavorando sul WB */

extern BOOL use_window; /* Da settare nell'applicazione se si intende usare SEMPRE
                                    il single buffer (su una finestra) */

extern BOOL use_scaling; /* Abilita il supporto dello scaling */

extern BOOL use_clipping; /* Abilita il clipping, disabilitato per default */

extern BOOL use_template; /* Usa BltTemplate+ BltBitMap invece di BltMaskBitMapRastPort */

extern BOOL double_buffering,triple_buffering;

extern int ClipX,ClipY;

extern BOOL force_single; /* Forzo il single buffering! */

// extern struct MyList GfxList,DrawList;

extern uint8_t *main_bitmap;
extern int bitmap_width,bitmap_height;

/* Da use_dt.c */

extern void FreeGfxObj(gfx_t *);
extern void FreeGraphics(void);
extern void DrawObject(gfx_t *, WORD, WORD);
extern gfx_t *LoadGfxObject(char *, int32_t *, uint8_t *);
extern anim_t *LoadAnimObject(char *, int32_t *);
extern void RemapAnimObjColor(anim_t *, UBYTE, UBYTE);
extern void ClearAnimObj(void);
extern void SortDrawList(void);
extern void DrawAnimObj(void);
void DrawAnimObjScale(void); // Disegna i bob tenendo conto dello scaling
extern void ScreenSwap(void);
extern BOOL InitAnimSystem(void);
extern BOOL InAnimList(anim_t *);
extern void AddAnimObj(anim_t *,WORD, WORD, WORD);
extern void FreeAnimObj(anim_t *);
extern BOOL LoadIFFPalette(char *);
extern void RemapMColor(struct MChunky *,UBYTE, UBYTE);
extern void RemapMChunkyColors(struct MChunky *, uint8_t *);
extern void RemapColor(uint8_t *, uint8_t, uint8_t, int);
extern void RemapColors(uint8_t *, int32_t *, int);
extern anim_t *CloneAnimObj(anim_t *);
extern anim_t *CopyAnimObj(anim_t *);
extern int RemapIFFPalette(char *, int32_t *);
extern void FreeIFFPalette(void);
extern void LoadGfxObjPalette(char *);
#endif

