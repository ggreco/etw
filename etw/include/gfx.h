/* Gli AnimObject hanno TUTTI maschera e bitmap su cui salvare lo sfondo,
	i GfxObject NO, usare i GfxObj solo su oggetti che NON cambiano mai
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

struct AnimObject
{
	struct MyMinNode node;    /* Per attaccarlo alla drawlist */
	struct MChunky **Frames;    /* Le bitmap dei vari frames */
	unsigned char *Palette;         /* Puntatore alla palette, organizzata come RGB */
	long *Widths;        /* Dimensioni dei singoli frames */
	long *Heights;
	long *Pens;       /* Nel caso di remapping conservo le penne che alloco */
	bitmap bg;           /* Sfondo, grande max_width*max_height */
	WORD max_width,max_height; /* Dimensioni massime dei frames */
	WORD num_frames,current_frame;  /* Frame corrente e numero di frames */
	WORD RealDepth;
	BOOL moved;
	int x_pos,y_pos,x_back,y_back; /* Coordinate dell'angolo sinistro dell'immagine
						visualizzata e del "cut" dello sfondo */
	int bottom;         /* Coordinata y del pixel piu' basso dell'immagine,
						usata per fare il sorting */
	int Flags;
	int ScaledX,ScaledY;
	bitmap sb;  /* Buffers per lo scaling */
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

struct GfxObject
{
	bitmap bmap;
	unsigned char *Palette;
	long *Pens;
	int width,height,realdepth;
};

#define BITRASSIZE(w,h)	((ULONG)(h)*( ((ULONG)(w)+15)>>3&0xFFFE))

typedef struct AnimObject AnimObj;
typedef struct GfxObject GfxObj;

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

extern BOOL save_back;  /* Questa variabile regola la gestione degli AnimObj, di
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

extern bitmap main_bitmap;
extern int bitmap_width,bitmap_height;

/* Da use_dt.c */

extern void FreeGfxObj(GfxObj *);
extern void FreeGraphics(void);
extern void DrawObject(GfxObj *, WORD , WORD );
extern GfxObj *LoadGfxObject(char *,long *,bitmap );
extern AnimObj *LoadAnimObject(char * ,LONG *);
extern void RemapAnimObjColor(AnimObj *,UBYTE, UBYTE);
extern void ClearAnimObj(void);
extern void SortDrawList(void);
extern void DrawAnimObj(void);
void DrawAnimObjScale(void); // Disegna i bob tenendo conto dello scaling
extern void ScreenSwap(void);
extern BOOL InitAnimSystem(void);
extern void AddAnimObj(AnimObj *,WORD, WORD, WORD);
extern void FreeAnimObj(AnimObj *);
extern BOOL LoadIFFPalette(char *);
extern void RemapMColor(struct MChunky *,UBYTE ,UBYTE );
extern void RemapMChunkyColors(struct MChunky *,UBYTE *);
extern void RemapColor(bitmap,UBYTE,UBYTE, int );
extern void RemapColors(bitmap,long *, int );
extern AnimObj *CloneAnimObj(AnimObj *);
extern AnimObj *CopyAnimObj(AnimObj *);
extern LONG RemapIFFPalette(char *,LONG *);
extern void FreeIFFPalette(void);
extern void LoadGfxObjPalette(char *);
#endif

