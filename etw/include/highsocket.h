#if !defined(HIGHSOCKET_H)
	#define HIGHSOCKET_H


#if defined(WIN32)
	#include <winsock.h>
	#define EWOULDBLOCK 10000
	extern struct Library *SockBase;

#elif defined(__SASC)

#include <ios1.h>
#include <ss/socket.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <netdb.h>
#include <proto/exec.h>
#include <dos/dos.h>

extern struct Library *SockBase;

#elif defined(AMIGA) || defined(MORPHOS)

#define BSDSOCKET_H

#include <sys/types.h>

#ifndef CROSSAMIGA
  #include <sc:netinclude/sys/socket.h>
#else
  #include <netinclude/sys/socket.h>
#endif

#include <netinet/in.h>
#include <inline/socket.h>


#define	IOCPARM_MASK	0x1fff		/* parameter length, at most 13 bits */
#define	IOC_IN		0x80000000	               /* copy in parameters */

#define _IOC(inout,group,num,len) \
	(inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num))
#define	_IOW(g,n,t)	_IOC(IOC_IN,	(g), (n), sizeof(t))

#define	FIOASYNC	_IOW('f', 125, long)
#define	FIONBIO		_IOW('f', 126, long)

extern struct Library *SocketBase;

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif /* WIN32 */

int SocketSystemBoot(void);
void SocketSystemShutDown(void);
int SockRead(int Socket,void *Buffer,long Size);
int SockWrite(int Socket,void *Buffer,long Size);
void SockNonBlock(int Socket);

#if defined(FAKENET)

#define SockClose(s)

#elif defined(__SASC)

#define SockClose(s) s_close(s)

#elif defined(AMIGA) || defined(MORPHOS)

#define SockClose(s) CloseSocket(s)

#elif defined(WIN32)

#define SockClose(s) closesocket(s)

#else

#define SockClose(s) close(s)

#endif /* Socket functions */

#endif /* HIGHSOCKET_H */

