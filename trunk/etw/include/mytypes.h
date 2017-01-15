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
typedef unsigned int    BOOL;
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
#define inline __inline
#define snprintf _snprintf

typedef signed int ssize_t;
#else
static inline int min(int a, int b) {
    return a < b ? a : b;
}
static inline int max(int a, int b) {
    return a > b ? a : b;
}
#endif

#endif
