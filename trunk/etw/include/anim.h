// #include <datatypes/animationclass.h>

#define ALIGN_LONG( mem ) ((APTR)((((ULONG)(mem)) + 3UL) & ~3UL))
/* Align memory on 16 byte boundary */
#define ALIGN_QUADLONG( mem ) ((APTR)((((ULONG)(mem)) + 15UL) & ~15UL))

/*
**
**  $VER: classdata.h 1.11 (24.9.97)
**  anim.datatype 1.11
**
**  anim class data
**
**  Written 1996/1997 by Roland 'Gizzy' Mainz
**  Original example source from David N. Junod
**
*/

/* Animation compression modes  */
#define acmpILBM            (0U)
#define acmpXORILBM         (1U)
#define acmpLongDelta       (2U)
#define acmpShortDelta      (3U)
#define acmpDelta           (4U)
#define acmpByteDelta       (5U)
#define acmpStereoByteDelta (6U)
#define acmpAnim7           (7U)
#define acmpAnim8           (8U)
#define acmpAnimJ          (74U) /* ascii 'J', Bill Grahams compression format */

/* Animation header flags */
#define ahfLongData         (1U << 0U)
#define ahfXOR              (1U << 1U)
#define ahfOneInfoList      (1U << 2U)
#define ahfRLC              (1U << 3U)
#define ahfVertical         (1U << 4U)
#define ahfLongInfoOffsets  (1U << 5U)

/* Generic IFF Chunk ID's we may encounter */
#ifndef ID_ANNO
#define ID_ANNO         MAKE_ID('A','N','N','O')
#endif /* !ID_ANNO */

#ifndef ID_AUTH
#define ID_AUTH         MAKE_ID('A','U','T','H')
#endif /* !ID_AUTH */

#ifndef ID_Copyright
#define ID_Copyright    MAKE_ID('(','c',')',' ')
#endif /* !ID_Copyright */

#ifndef ID_FVER
#define ID_FVER         MAKE_ID('F','V','E','R')
#endif /* !ID_FVER */

#ifndef ID_NAME
#define ID_NAME         MAKE_ID('N','A','M','E')
#endif /* !ID_NAME */

/* IFF ANIM related IDs */
#ifndef ID_DPAN
#define ID_DPAN         MAKE_ID('D','P','A','N')
#endif /* !ID_DPAN */

struct DPAnimChunk
{
    UWORD dpan_Version;
    UWORD dpan_nframes;
    ULONG dpan_FPS;
};


struct BitMapHeader
{
    UWORD        bmh_Width;
    UWORD        bmh_Height;
    WORD         bmh_Left;
    WORD         bmh_Top;
    UBYTE        bmh_Depth;
    UBYTE        bmh_Masking;
    UBYTE        bmh_Compression;
    UBYTE        bmh_Pad;
    UWORD        bmh_Transparent;
    UBYTE        bmh_XAspect;
    UBYTE        bmh_YAspect;
    WORD         bmh_PageWidth;
    WORD         bmh_PageHeight;
};

typedef void * PLANEPTR;

struct BitMap
{
    UWORD    BytesPerRow;
    UWORD    Rows;
    UBYTE    Flags;
    UBYTE    Depth;
    UWORD    pad;
    PLANEPTR Planes[8];
};

struct AnimHeader
{
    UBYTE	 ah_Operation;
    UBYTE	 ah_Mask;
    UWORD	 ah_Width;
    UWORD	 ah_Height;
    WORD	 ah_Left;
    WORD	 ah_Top;
    ULONG	 ah_AbsTime;
    ULONG	 ah_RelTime;
    UBYTE	 ah_Interleave;
    UBYTE	 ah_Pad0;
    ULONG	 ah_Flags;
    UBYTE	 ah_Pad[16];
};

/*****************************************************************************/

/* anim.datatype class instance data */
struct AnimInstData
{
    /* Misc */
//    struct SignalSemaphore  aid_SigSem;             /* Instance data lock                      */
//    UWORD                   aid_Pad0;
//    APTR                    aid_Pool;               /* pool for misc things */
//    APTR                    aid_FramePool;          /* pool for animation bitmaps */
    struct BitMapHeader    *aid_BMH;                /* Short cut to animation.datatype's bitmapheader */
    struct MyList          aid_FrameList;          /* List of frames                          */
    struct MyList          aid_PostedFreeList;     /* List of frames which should be freed */
    struct FrameNode       *aid_CurrFN;             /* Last framenode obtained using ADTM_LOADFRAME */
    ULONG                   aid_ModeID;
    BOOL                    aid_NoCMAPs;            /* Don't create colormaps                  */
    BOOL                    aid_LoadAll;            /* Load all frames of the animation        */
    BOOL                    aid_NoDynamicTiming;    /* No dynamic timing ?                     */
    ULONG                   aid_FPS;                /* fps of stream (maybe modified by prefs) */

    /* Disk-loading section */
    FILE                    *aid_FH;
    LONG                    aid_CurrFilePos;
    struct BitMap           *bm; // dest bitmap to be used in planar2chunky
};


/* node which holds information about a single animation frame */
struct FrameNode
{
    struct MyMinNode     fn_Node;           /* Node in aid_FrameList      */
    struct MyMinNode     fn_PostedFreeNode; /* Node in aid_PostedFreeList */

/* Get beginning of struct FrameNode from fn_PostedFreeNode */
#define POSTEDFREENODE2FN( pfn ) ((struct FrameNode *)(((struct MyMinNode *)(pfn)) - 1))

    struct FrameNode  *fn_PrevFrame;

/* Misc */
    UWORD              fn_PostedFree;
    WORD               fn_UseCount;       /* In-Use counter */

/* Timing section */
    ULONG              fn_TimeStamp;
    ULONG              fn_Frame;
    ULONG              fn_Duration;
    ULONG           Clock;

/* Animation info */
    struct AnimHeader  fn_AH;

/* Bitmap/ColorMap section */
    struct BitMap     *fn_BitMap;

/* BitMap loading section */
    UBYTE          *delta;

// Per la gestione via disco...

    LONG               fn_BMOffset; /* File offset (0 is begin of file) */
    ULONG              fn_BMSize;   /* Chunk size  */
    ULONG              fn_LoadSize; /* temporary variable used by ADTM_LOADFRAME */

/* Sample section */
    struct SoundInfo  *fn_Sample;
    ULONG fn_Rate,fn_Volume,fn_Loops;
};

#define ERROR_NOT_IMPLEMENTED 6
#define ERROR_NO_FREE_STORE 1
#define ERROR_REQUIRED_ARG_MISSING 2
#define ERROR_NOT_ENOUGH_DATA 3

LONG MergeAnim(struct AnimInstData *,FILE *);
struct FrameNode *GetFrameNode(struct AnimInstData *,int);
void DisplayAnim(struct AnimInstData *);
void FreeFrames(struct AnimInstData *);
struct AnimInstData *LoadFrames( FILE * );
/*****************************************************************************/

