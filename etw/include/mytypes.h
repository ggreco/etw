#ifndef MY_TYPES_H
#define MY_TYPES_H

/* Get the Amiga types if available */
#if defined AMIGA
#   include <exec/types.h>
typedef BYTE            int8_t;
typedef UBYTE           uint8_t;
typedef WORD            int16_t;
typedef UWORD           uint16_t;
typedef LONG            int32_t;
typedef ULONG           uint32_t;

#else
#   include <stdint.h>
#   include <stdio.h>
typedef void *          APTR;
typedef /*unsigned*/ int    BOOL;
typedef uint8_t         BYTEBITS;
typedef uint16_t        WORDBITS;
typedef uint32_t        LONGBITS;
typedef char * STRPTR;
#endif

/* Get TRUE, FALSE and NULL if not available */
#ifndef TRUE
#   define TRUE 1
#endif
#ifndef FALSE
#   define FALSE 0
#endif
#ifndef NULL
#   define NULL ((void *)0L)
#endif

#ifdef _MSC_VER
#if _MSC_VER <= 1600
#define inline __inline
#endif
#define snprintf _snprintf

typedef signed int ssize_t;
#else
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#endif

#endif
