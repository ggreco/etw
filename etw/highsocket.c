#if defined(WIN32)
#include <winsock.h>
#endif /* WIN32 */

#include "mydebug.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#if defined(LINUX) || defined(FAKENET) || defined(MACOSX) || defined(SOLARIS_X86)
#include <unistd.h>
#if defined(LINUX) || defined(FAKENET) || defined(MACOSX)
#include <fcntl.h>
#elif defined(SOLARIS_X86)
#define FNDELAY O_NDELAY 
#include <sys/fcntl.h>
#endif
#include <assert.h>
#endif /* !WIN32 */

#if defined(AROS)
// per ora non definisco nulla
#warning "Socket implementation not yet available on AROS!"
#elif defined(AMIGA) || defined(MORPHOS)

#ifdef __SASC
#include <ios1.h>
#include <ss/socket.h>
#include <sys/ioctl.h>
/*#include <sys/fcntl.h>*/
#include <sys/time.h>
#include <unistd.h>
#include <exec/nodes.h>
#include <devices/timer.h>
#include <proto/exec.h>
#include <proto/dos.h>
#define MY_MAX_CONNECT 12

struct Library *SockBase;

#define SOCKETLIB

#else 

#include <inline/exec.h>
extern struct ExecBase *SysBase;

struct Library *SocketBase;

#define BSDSOCKETLIB

#endif

#endif

#include "highsocket.h"

int SocketSystemBoot(void)
{

/* GG: Codice di gestione dei socket su Amiga :) */
#if defined(SOCKETLIB)
    D(bug("Apro socket.library\n"));

    if(!(SockBase = OpenLibrary("socket.library"/*-*/,0L)))
        if (!(SockBase=OpenLibrary("amitcp:libs/socket.library"/*-*/,0L)))
        if (!(SockBase=OpenLibrary("inet:libs/socket.library"/*-*/,0L)))
        {
            D(bug("Errore! Non riesco ad aprire la socket.library\n"));
            return 0;
        }
#elif defined(BSDSOCKETLIB)
    D(bug("Apro bsdsocket.library\n"));
    if(!(SocketBase = OpenLibrary("bsdsocket.library",3L))) {
        D(bug("Non posso aprire la bsdsocket.library"));
        return 0;
    }

#elif defined(WIN32)

/* Inizializzazione dei socket sotto Win95/NT; richiediamo almeno
la versione 1.1 */

WORD wVersionRequested; 
WSADATA wsaData; 
int err; 
wVersionRequested = MAKEWORD(1, 1); 
 
err = WSAStartup(wVersionRequested, &wsaData); 
 
if (err != 0) {
    D(bug("Winsock.dll non trovata.\n"));
    return 0;
}

if ( LOBYTE( wsaData.wVersion ) != 1 || 
    HIBYTE( wsaData.wVersion ) != 1 ) { 
    WSACleanup(); 
    D(bug("Winsock.dll 1.1 o superiore non trovata.\n"));
    return 0;
} 

#endif

    return 1;
}

void SocketSystemShutDown(void)
{
#if defined(SOCKETLIB)
  cleanup_sockets();
  CloseLibrary(SockBase);
#elif defined(BSDSOCKETLIB)
  CloseLibrary(SocketBase);
#elif defined(WIN32)
  WSACleanup();
#endif
}

int SockWrite(int Socket,void *Buffer,long Size)
{
#if defined(WIN32)

    int n=send(Socket,Buffer,Size,0);

    if (n==SOCKET_ERROR) {
        n=-1;
        if (WSAGetLastError()==WSAEWOULDBLOCK)
            errno=EWOULDBLOCK;
    }
    return(n);

#elif defined(FAKENET)
    return -1;
#else

    return(send(Socket,Buffer,Size,0));

#endif
}

int SockRead(int Socket,void *Buffer,long Size)
{
#if defined(WIN32)

    int n=recv(Socket,Buffer,Size,0);

    if (n==SOCKET_ERROR) {
        n=-1;
        if (WSAGetLastError()==WSAEWOULDBLOCK)
            errno=EWOULDBLOCK;
    }
    return(n);

#elif defined(FAKENET)
    return -1;
#else

    return(recv(Socket,Buffer,Size,0));

#endif
}

void SockNonBlock(int Socket)
{
#if defined(WIN32)

    unsigned long onoff=1;

    if (ioctlsocket(Socket, FIONBIO, &onoff)==SOCKET_ERROR)
        perror("ioctlsocket() fallita.");

#elif defined(SOCKETLIB)
    int onoff=1;

    s_ioctl(Socket, FIONBIO, (char *)&onoff);
    s_ioctl(Socket, FIOASYNC, (char *)&onoff);

#elif defined(BSDSOCKETLIB)
    int onoff=1;

    IoctlSocket(Socket, FIONBIO, (char *)&onoff);
    IoctlSocket(Socket, FIOASYNC, (char *)&onoff);

#elif defined(LSCO)

  int nFlags;
  
  nFlags = fcntl( Socket, F_GETFL );
  nFlags |= O_NONBLOCK;
  if( fcntl( Socket, F_SETFL, nFlags ) < 0 )
  {
    perror( "Fatal error executing nonblock" );
    exit( 1 );
  }
#elif defined(AROS)

#else

  if (fcntl(Socket, F_SETFL, FNDELAY) == -1)    {
    perror("Noblock");
    exit (1);
  }

#endif
 
}

