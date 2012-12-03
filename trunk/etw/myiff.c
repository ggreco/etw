#include <stdlib.h>
#include "mytypes.h"
#include "os_defs.h"
#include "mydebug.h"
#include "myiff.h"
#include "files.h"

/* Basic functions */

struct IFFHandle *AllocIFF(void)
{
    struct IFFHandle *h;

    if ((h = malloc(sizeof(struct IFFHandle)))) {
        h->iff_Stream = NULL;
        h->stops = NULL;
        h->props = NULL;
        h->iff_Stops = 0;
        h->iff_props = 0;
        h->Prop = NULL;
    }

    return h;
}

long PropChunks(struct IFFHandle * iff, int32_t *propArray, int numPairs)
{
    iff->props = propArray;
    iff->iff_props = numPairs;
    return 0L;
}

long OpenIFF(struct IFFHandle * iff, long rwMode)
{
    if (iff->iff_Stream && rwMode == IFFF_READ)
    {
        uint32_t temp;

        fseek(iff->iff_Stream, 0, SEEK_SET);
        temp = fread_u32(iff->iff_Stream);

        if (temp == ID_FORM)
        {
            fseek(iff->iff_Stream, 4, SEEK_CUR);
            temp = fread_u32(iff->iff_Stream);
            iff->Current.cn_Type = temp;
            return 0L;
        } else
            return 1L;
    } else {
        D(bug("Stream !=IFFF_READ non implementato!\n"));
        return 1;
    }
}

long ParseIFF(struct IFFHandle * iff, long control)
{
    unsigned long i;
    uint32_t temp;

    if (control != IFFPARSE_SCAN) {
        D(bug("ParseIFF!=IFFPARSE_SCAN not implemented!\n"));
        return 1L;
    }

    for (;;)
    {
        temp = fread_u32(iff->iff_Stream);

        if (feof(iff->iff_Stream))
            return IFFERR_EOF;

        if (ferror(iff->iff_Stream))
            return IFFERR_READ;

#if 0
        if (temp == ID_FORM) {
            int32_t len = fread_u32(iff->iff_Stream);
            temp = fread_u32(iff->iff_Stream);
            D(bug("Found %c%c%c%c chunk of %d bytes\n", temp >> 24, temp >> 16 & 0xff, temp >> 8 & 0xff, temp & 0xff));
            iff->Current.cn_Type = temp;
        }
        else 
#endif     
       for (i = 0; i < iff->iff_Stops; i++)  {
            if (temp == (uint32_t)iff->stops[i * 2 + 1]) {
                D(bug("Stopping at stop %c%c%c%c\n", temp >> 24, temp >> 16 & 0xff, temp >> 8 & 0xff, temp & 0xff));
                iff->Current.cn_ID = temp;
                temp = fread_u32(iff->iff_Stream);
                iff->Current.cn_Size = temp;
                iff->Current.cn_Scan = ftell(iff->iff_Stream);
                return 0L;
            }
        }
        for (i = 0; i < iff->iff_props; i++) {
            if (temp == (uint32_t)iff->props[i * 2 + 1]) {
                D(bug("Inserted prop %c%c%c%c\n", temp >> 24, temp >> 16 & 0xff, temp >> 8 & 0xff, temp & 0xff));
                struct PropNode *p = (struct PropNode *)malloc(sizeof(struct PropNode));
                p->next = iff->Prop;
                p->cn_ID = temp;
                temp = fread_u32(iff->iff_Stream);
                p->sp.sp_Size = temp;
                p->sp.sp_Data = malloc(temp);
                fread(p->sp.sp_Data, 1, temp, iff->iff_Stream);
                p->cn_Scan = ftell(iff->iff_Stream);

                iff->Prop = p;
                continue; // do not seek backward
            }
        }

        fseek(iff->iff_Stream, -2, SEEK_CUR);
    }
}

struct StoredProperty *FindProp(struct IFFHandle *iff, LONG type, LONG id)
{
    struct PropNode *p = iff->Prop;
    while (p) {
        if (p->cn_ID == id)
            return &p->sp;
        p = (struct PropNode *) p->next;
    }
    return NULL;
}

void CloseIFF(struct IFFHandle *iff)
{
    ;
}

void FreeIFF(struct IFFHandle *iff)
{
    while (iff->Prop) {
        struct PropNode *todel = iff->Prop;
        free(todel->sp.sp_Data);        
        iff->Prop = iff->Prop->next;
        free(todel);
    }
    free(iff);
}

/* Read/Write functions */

size_t ReadChunkBytes(struct IFFHandle *iff, void *buf, long numBytes)
{
    return fread(buf, 1, numBytes, iff->iff_Stream);
}

size_t ReadChunkRecords(struct IFFHandle * iff, void *buf,
                         long bytesPerRecord, long numRecords)
{
    return fread(buf, bytesPerRecord, numRecords, iff->iff_Stream);
}

/* Built-in chunk/property handlers */

size_t StopChunks(struct IFFHandle * iff, int32_t * propArray, long numPairs)
{
    iff->stops = propArray;
    iff->iff_Stops = numPairs;
    return 0L;
}

struct ContextNode *CurrentChunk(struct IFFHandle *iff)
{
    return &iff->Current;
}

/* IFFHandle initialization */

void InitIFFasDOS(struct IFFHandle *iff)
{
}

