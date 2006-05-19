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

#elif defined HAVE_STDINT_H
#   include <stdint.h>
#   include <stdio.h>
typedef void *          APTR;
typedef int32_t         LONG;
typedef uint32_t        ULONG;
typedef int16_t         WORD;
typedef int8_t          BYTE;
typedef unsigned int    BOOL;
typedef uint16_t        UWORD;
typedef uint8_t         UBYTE;
typedef uint8_t         BYTEBITS;
typedef uint16_t        WORDBITS;
typedef uint32_t        LONGBITS;
typedef unsigned char * STRPTR;

#else
typedef void            *APTR;      /* 32-bit untyped pointer */
#   if !defined(_WINNT_H) && !defined(_WINNT_)
typedef signed long     LONG;       /* signed 32-bit quantity */
typedef unsigned long   ULONG;      /* unsigned 32-bit quantity */
#   endif
#   if !defined( _WINDEF_H) && !defined(_WINDEF_)
#       ifndef WORD
typedef signed short    WORD;       /* signed 16-bit quantity */
#       endif
#       ifndef BYTE
typedef signed char     BYTE;
#       endif
#       ifndef BOOL
typedef unsigned int    BOOL;
#       endif
#   endif /* _WINDEF */
typedef unsigned short  UWORD;      /* unsigned 16-bit quantity */
typedef unsigned char   UBYTE;      /* unsigned 8-bit quantity */
typedef unsigned char   BYTEBITS;   /* 8 bits manipulated individually */
typedef unsigned short  WORDBITS;   /* 16 bits manipulated individually */
typedef unsigned long   LONGBITS;   /* 32 bits manipulated individually */
// typedef void VOID;
typedef unsigned char  *STRPTR;

typedef signed char     int8_t;
typedef unsigned char   uint8_t;
typedef signed short    int16_t;
typedef unsigned short  uint16_t;
typedef signed int      int32_t;
typedef unsigned int    uint32_t;
#endif

/* Get TRUE, FALSE and NULL if not available */
#ifndef TRUE
#   define TRUE 1
#endif
#ifndef FALSE
#   define FALSE 0
#endif
#ifndef NULL
#   define NULL 0L
#endif

#endif
