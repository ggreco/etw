#if defined( __PPC__) && !defined(MORPHOS)
#include <devices/ahi.h>
#include <powerup/ppclib/interface.h>

#define	MyQueryKeys(queryArray, arraySize)	NCQueryKeys(LOWLEVEL_BASE_NAME, queryArray, arraySize)

static __inline void
NCQueryKeys(void *LowLevelBase, struct KeyQuery *queryArray, unsigned long arraySize)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHAREA;
	MyCaos.M68kStart	=	queryArray;
	MyCaos.M68kLength		=	sizeof(struct KeyQuery)*arraySize;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHAREA;
	MyCaos.PPCStart		=	queryArray;
	MyCaos.PPCLength		=	sizeof(struct KeyQuery)*arraySize;
	MyCaos.a0		=(ULONG) queryArray;
	MyCaos.d1		=(ULONG) arraySize;
	MyCaos.caos_Un.Offset	=	(-54);
	MyCaos.a6		=(ULONG) LowLevelBase;	
	PPCCallOS(&MyCaos);
}

#define	MyChangeScreenBuffer(sc, sb)	NCChangeScreenBuffer(INTUITION_BASE_NAME, sc, sb)

static __inline ULONG
NCChangeScreenBuffer(void *IntuitionBase, struct Screen *sc, struct ScreenBuffer *sb)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a0		=(ULONG) sc;
	MyCaos.a1		=(ULONG) sb;
	MyCaos.caos_Un.Offset	=	(-780);
	MyCaos.a6		=(ULONG) IntuitionBase;	
	return((ULONG)PPCCallOS(&MyCaos));
}

#define	MyUnLockBitMap(Handle)	NCUnLockBitMap(CYBERGRAPHICS_BASE_NAME, Handle)

static __inline void
NCUnLockBitMap(void *CyberGfxBase, APTR Handle)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHNO /*|IF_ASYNC */;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a0		=(ULONG) Handle;
	MyCaos.caos_Un.Offset	=	(-174);
	MyCaos.a6		=(ULONG) CyberGfxBase;	
	PPCCallOS(&MyCaos);
}

#define	MyReplyMsg(message)	NCReplyMsg(EXEC_BASE_NAME, message)

static __inline void
NCReplyMsg(void *SysBase, struct Message *message)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHNO /*|IF_ASYNC */;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a1		=(ULONG) message;
	MyCaos.caos_Un.Offset	=	(-378);
	MyCaos.a6		=(ULONG) SysBase;	
	PPCCallOS(&MyCaos);
}

#define MyGetMsg(port) NCGetMsg(EXEC_BASE_NAME,port)

static __inline struct Message *
NCGetMsg(void *SysBase, struct MsgPort *port)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	/*IF_CACHEFLUSHNO */IF_CACHEFLUSHALL ;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a0		=(ULONG) port;
	MyCaos.caos_Un.Offset	=	(-372);
	MyCaos.a6		=(ULONG) SysBase;
	return((struct Message *)PPCCallOS(&MyCaos));
}

#define	MySendIO(ioRequest)	NCSendIO(EXEC_BASE_NAME, ioRequest)

static __inline void
NCSendIO(void *SysBase, struct IORequest *ioRequest)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHALL /*|IF_ASYNC */;
//	MyCaos.M68kStart	=	ioRequest;
//	MyCaos.M68kLength	=	sizeof(struct AHIRequest);
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart		=	ioRequest;
//	MyCaos.PPCLength	=	sizeof(struct AHIRequest);
	MyCaos.a1		=(ULONG) ioRequest;
	MyCaos.caos_Un.Offset	=	(-462);
	MyCaos.a6		=(ULONG) SysBase;	
	PPCCallOS(&MyCaos);
}

#define	MyAbortIO(ioRequest)	NCAbortIO(EXEC_BASE_NAME, ioRequest)

static __inline void
NCAbortIO(void *SysBase, struct IORequest *ioRequest)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a1		=(ULONG) ioRequest;
	MyCaos.caos_Un.Offset	=	(-480);
	MyCaos.a6		=(ULONG) SysBase;	
	PPCCallOS(&MyCaos);
}

#define	MyCheckIO(ioRequest)	NCCheckIO(EXEC_BASE_NAME, ioRequest)

static __inline struct IORequest *
NCCheckIO(void *SysBase, struct IORequest *ioRequest)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a1		=(ULONG) ioRequest;
	MyCaos.caos_Un.Offset	=	(-468);
	MyCaos.a6		=(ULONG) SysBase;	
	return((struct IORequest *)PPCCallOS(&MyCaos));
}

#define	MyWaitIO(ioRequest)	NCWaitIO(EXEC_BASE_NAME, ioRequest)

static __inline BYTE
NCWaitIO(void *SysBase, struct IORequest *ioRequest)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.a1		=(ULONG) ioRequest;
	MyCaos.caos_Un.Offset	=	(-474);
	MyCaos.a6		=(ULONG) SysBase;
	return((BYTE)PPCCallOS(&MyCaos));
}

#define	MyDoIO(ioRequest)	NCDoIO(EXEC_BASE_NAME, ioRequest)

static __inline BYTE
NCDoIO(void *SysBase, struct IORequest *ioRequest)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHAREA;
	MyCaos.M68kStart	=	ioRequest;
	MyCaos.M68kLength	=	sizeof(struct AHIRequest);
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHAREA;
	MyCaos.PPCStart		=	ioRequest;
	MyCaos.PPCLength	=	sizeof(struct AHIRequest);
	MyCaos.a1		=(ULONG) ioRequest;
	MyCaos.caos_Un.Offset	=	(-456);
	MyCaos.a6		=(ULONG) SysBase;	
	return((BYTE)PPCCallOS(&MyCaos));
}

#define	MyWaitTOF()	NCWaitTOF(GRAPHICS_BASE_NAME)

static __inline void
NCWaitTOF(void *GfxBase)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.caos_Un.Offset	=	(-270);
	MyCaos.a6		=(ULONG) GfxBase;
	PPCCallOS(&MyCaos);
}

#define	MyExecWait(signalSet)	NCWait(EXEC_BASE_NAME, signalSet)

static __inline ULONG
NCWait(void *SysBase, unsigned long signalSet)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.M68kStart	=	NULL;
//	MyCaos.M68kSize		=	0;
	MyCaos.PPCCacheMode	=	IF_CACHEFLUSHNO;
//	MyCaos.PPCStart		=	NULL;
//	MyCaos.PPCSize		=	0;
	MyCaos.d0		=(ULONG) signalSet;
	MyCaos.caos_Un.Offset	=	(-318);
	MyCaos.a6		=(ULONG) SysBase;	
	return((ULONG)PPCCallOS(&MyCaos));
}

#else
	#define MyQueryKeys QueryKeys
	#define MyChangeScreenBuffer ChangeScreenBuffer
	#define MyUnLockBitMap UnLockBitMap
	#define MyReplyMsg ReplyMsg
	#define MySendIO SendIO
	#define MyAbortIO AbortIO
	#define MyWaitIO WaitIO
	#define MyCheckIO CheckIO
	#define MyDoIO DoIO
	#define MyWaitTOF WaitTOF
	#define MyExecWait Wait
#endif


