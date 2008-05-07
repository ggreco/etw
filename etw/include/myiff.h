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

#include "mytypes.h"
#include "lists.h"

/* Structs ... */

struct ContextNode
{
    struct MyMinNode cn_Node;
    long cn_ID;
    long cn_Type;
    long cn_Size;    /*  Size of this chunk           */
    long cn_Scan;    /*  # of bytes read/written so far */
};

struct IFFHandle
{
    FILE *iff_Stream;
    unsigned long iff_Flags, iff_Stops;
    int32_t *stops;
    struct ContextNode Current;
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
long OpenIFF(struct IFFHandle *iff, long rwMode);
long ParseIFF(struct IFFHandle *iff, long control);
void CloseIFF(struct IFFHandle *iff);
void FreeIFF(struct IFFHandle *iff);

/* Read/Write functions */

size_t ReadChunkBytes(struct IFFHandle *iff, APTR buf, long numBytes);
size_t ReadChunkRecords(struct IFFHandle *iff, APTR buf, long bytesPerRecord,
                         long numRecords);

/* Built-in chunk/property handlers */

size_t StopChunks(struct IFFHandle *iff, int32_t *propArray, long numPairs);
struct ContextNode *CurrentChunk(struct IFFHandle *iff);

/* IFFHandle initialization */

void InitIFFasDOS(struct IFFHandle *iff);

#endif
