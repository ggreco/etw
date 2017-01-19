#ifndef MYIFF_H
#define MYIFF_H
#define IFF_IFFPARSE_H

#include <stdio.h>

/* Questa va cambiata per i little endian */

#define MAKE_ID(a,b,c,d) \
    ((uint32_t)(a) << 24 | (uint32_t)(b) << 16 | \
     (uint32_t)(c) << 8 | (uint32_t)(d))

#define ID_FORM    MAKE_ID('F','O','R','M')
#define ID_LIST    MAKE_ID('L','I','S','T')
#define ID_CAT     MAKE_ID('C','A','T',' ')
#define ID_PROP    MAKE_ID('P','R','O','P')
#define ID_ILBM         MAKE_ID('I','L','B','M')
#define ID_BMHD         MAKE_ID('B','M','H','D')
#define ID_CMAP         MAKE_ID('C','M','A','P')
#define ID_CAMG         MAKE_ID('C','A','M','G')
#define ID_DLTA    	MAKE_ID('D','L','T','A')
#define ID_BODY     MAKE_ID('B','O','D','Y')
#define ID_GRAB     MAKE_ID('G','R','A','B')
#define ID_ANHD     MAKE_ID('A','N','H','D')

#define IFFERR_EOF        -1L
#define IFFERR_EOC        -2L
#define IFFERR_NOSCOPE    -3L
#define IFFERR_NOMEM      -4L
#define IFFERR_READ       -5L
#define IFFERR_WRITE      -6L
#define IFFERR_SEEK       -7L
#define IFFERR_MANGLED    -8L
#define IFFERR_SYNTAX     -9L
#define IFFERR_NOTIFF     -10L
#define IFFERR_NOHOOK     -11L

#define INVALID_ID ~0

#include "mytypes.h"
#include "lists.h"

/* Structs ... */

struct StoredProperty
{
    int32_t sp_Size;
    APTR sp_Data;
};

struct ContextNode
{
    struct MyMinNode cn_Node;
    int32_t cn_ID;
    int32_t cn_Type;
    int32_t cn_Size;    /*  Size of this chunk           */
    int32_t cn_Scan;    /*  # of bytes read/written so far */
};

struct PropNode
{
    struct PropNode *next;
    int32_t cn_ID;
    int32_t cn_Scan;
    struct StoredProperty sp;
};

struct IFFHandle
{
    FILE *iff_Stream;
    uint32_t iff_Flags, iff_Stops, iff_props;
    int32_t *stops, *props;
    struct ContextNode Current;
    struct PropNode *Prop;
};

/* Control modes for ParseIFF() function */
#define IFFPARSE_SCAN     0L
#define IFFPARSE_STEP     1L
#define IFFPARSE_RAWSTEP 2L

#define IFFF_READ    0L             /* read mode - default    */
#define IFFF_WRITE    1L             /* write mode           */
#define IFFF_RWBITS    (IFFF_READ | IFFF_WRITE) /* read/write bits       */
#define IFFF_FSEEK    (1L<<1)         /* forward seek only       */
#define IFFF_RSEEK    (1L<<2)         /* random seek       */
#define IFFF_RESERVED    0xFFFF0000L         /* Don't touch these bits */

/* Basic functions */

struct IFFHandle *AllocIFF(void);
int32_t OpenIFF(struct IFFHandle *iff, int32_t rwMode);
int32_t ParseIFF(struct IFFHandle *iff, int32_t control);
void CloseIFF(struct IFFHandle *iff);
void FreeIFF(struct IFFHandle *iff);

/* additional anim parsing functions */
struct StoredProperty *FindProp(struct IFFHandle *, int32_t type, int32_t id);
int32_t PropChunks(struct IFFHandle * iff, int32_t *chunks, int num_chunks);

/* Read/Write functions */

int32_t ReadChunkBytes(struct IFFHandle *iff, APTR buf, int32_t numBytes);
int32_t ReadChunkRecords(struct IFFHandle *iff, APTR buf, int32_t bytesPerRecord,
                         int32_t numRecords);

/* Built-in chunk/property handlers */

int32_t StopChunks(struct IFFHandle *iff, int32_t *propArray, int32_t numPairs);
struct ContextNode *CurrentChunk(struct IFFHandle *iff);

/* IFFHandle initialization */

void InitIFFasDOS(struct IFFHandle *iff);

#endif
