#include <stdlib.h>
#include "mytypes.h"
#include "os_defs.h"
#include "mydebug.h"
#include "myiff.h"

/* Basic functions */

struct IFFHandle *AllocIFF(void)
{
	struct IFFHandle *h;

	if ((h = malloc(sizeof(struct IFFHandle)))) {
		h->iff_Stream = NULL;
		h->stops = NULL;
		h->iff_Stops = 0;
	}

	return h;
}

LONG OpenIFF(struct IFFHandle * iff, long rwMode)
{
	if (iff->iff_Stream && rwMode == IFFF_READ) {
		ULONG temp;

		fseek(iff->iff_Stream, 0, SEEK_SET);

		fread(&temp, sizeof(temp), 1, iff->iff_Stream);

		SWAP_LONG(temp);

		if (temp == ID_FORM) {
			fseek(iff->iff_Stream, 4, SEEK_CUR);
			fread(&temp, sizeof(temp), 1, iff->iff_Stream);
			SWAP_LONG(temp);
			iff->Current.cn_Type = temp;
			return 0L;
		} else
			return 1L;
	} else {
		D(bug("Stream !=IFFF_READ non implementato!\n"));
		return 1;
	}
}

LONG ParseIFF(struct IFFHandle * iff, long control)
{
	unsigned long temp, i;

	if (control != IFFPARSE_SCAN) {
		D(bug("ParseIFF!=IFFPARSE_SCAN non implementato!\n"));
		return 1L;
	}

	for (;;) {
		if (fread(&temp, sizeof(temp), 1, iff->iff_Stream) != 1)
			return 1L;

		SWAP_LONG(temp);

		for (i = 0; i < iff->iff_Stops; i++) {
			if (temp == iff->stops[i * 2 + 1]) {
				iff->Current.cn_ID = temp;
				fread(&temp, sizeof(temp), 1, iff->iff_Stream);
				SWAP_LONG(temp);
				iff->Current.cn_Size = temp;
				return 0L;
			}
		}
		fseek(iff->iff_Stream, -2, SEEK_CUR);
	}
}

void CloseIFF(struct IFFHandle *iff)
{
}

void FreeIFF(struct IFFHandle *iff)
{
	free(iff);
}

/* Read/Write functions */

LONG ReadChunkBytes(struct IFFHandle *iff, APTR buf, long numBytes)
{
	return (LONG) fread(buf, 1, numBytes, iff->iff_Stream);
}

LONG ReadChunkRecords(struct IFFHandle * iff, APTR buf,
					  long bytesPerRecord, long numRecords)
{
	return (LONG) fread(buf, bytesPerRecord, numRecords, iff->iff_Stream);
}

/* Built-in chunk/property handlers */

LONG StopChunks(struct IFFHandle * iff, LONG * propArray, long numPairs)
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
