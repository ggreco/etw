#ifndef MY_TYPES_H

#define MY_TYPES_H

#ifdef AMIGA
#include <exec/types.h>
#else

typedef void	       *APTR;	    /* 32-bit untyped pointer */
#if !defined(_WINNT_H) && !defined(_WINNT_)
typedef long		LONG;	    /* signed 32-bit quantity */
typedef unsigned long	ULONG;	    /* unsigned 32-bit quantity */
#endif

#if !defined( _WINDEF_H) && !defined(_WINDEF_)
#ifndef WORD
typedef short		WORD;	    /* signed 16-bit quantity */
#endif
#ifndef BYTE
typedef char		BYTE;
#endif
typedef unsigned short	RPTR;	    /* signed relative pointer */
#ifndef BOOL
typedef int		BOOL;
#endif
#endif /* _WINDEF */

typedef unsigned short	UWORD;	    /* unsigned 16-bit quantity */
typedef unsigned char	UBYTE;	    /* unsigned 8-bit quantity */
typedef unsigned char	BYTEBITS;   /* 8 bits manipulated individually */
typedef unsigned short	WORDBITS;   /* 16 bits manipulated individually */
typedef unsigned long	LONGBITS;   /* 32 bits manipulated individually */
// typedef void		VOID;

typedef unsigned char  *STRPTR;

#ifndef TRUE
	#define TRUE		1
#endif
#ifndef FALSE
	#define FALSE		0
#endif

#ifndef NULL
	#define NULL 0L
#endif

#endif

#endif

