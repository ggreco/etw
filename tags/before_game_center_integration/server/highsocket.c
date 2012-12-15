#if defined(WIN32)

#include <winsock.h>

#endif /* WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#if !defined(WIN32)

#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#endif /* !WIN32 */

#ifdef __SASC
#include <ios1.h>
#include <ss/socket.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <exec/nodes.h>
#include <devices/timer.h>
#include <proto/exec.h>
#include <proto/dos.h>
#define MY_MAX_CONNECT 12

struct Library *SockBase;

void SocketSystemBoot(void);
void SocketSystemShutDown(void);
#endif

#include "highsocket.h"

void SocketSystemBoot(void)
{

/* GG: Codice di gestione dei socket su Amiga :) */

#if defined(__SASC)

	if(!(SockBase = OpenLibrary("socket.library"/*-*/,0L)))
	    if (!(SockBase=OpenLibrary("amitcp:libs/socket.library"/*-*/,0L)))
		if (!(SockBase=OpenLibrary("inet:libs/socket.library"/*-*/,0L)))
		{
			printf("Errore! Non riesco ad aprire la socket.library\n");
			exit (8);
		}

	setup_sockets(MY_MAX_CONNECT,&errno);

#elif defined(WIN32)

/* Inizializzazione dei socket sotto Win95/NT; richiediamo almeno
la versione 1.1 */

WORD wVersionRequested; 
WSADATA wsaData; 
int err; 
wVersionRequested = MAKEWORD(1, 1); 
 
err = WSAStartup(wVersionRequested, &wsaData); 
 
if (err != 0) {
	printf("Winsock.dll non trovata.\n");
	exit(8);
}

if ( LOBYTE( wsaData.wVersion ) != 1 || 
    HIBYTE( wsaData.wVersion ) != 1 ) { 
    WSACleanup(); 
	printf("Winsock.dll 1.1 o superiore non trovata.\n");
	exit(9);
} 

#endif

}

void SocketSystemShutDown(void)
{
  printf("Closing socket system...\n");
#if defined(__SASC)

  cleanup_sockets();

  CloseLibrary(SockBase);

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

#elif defined(__SASC)
    int onoff=1;

    s_ioctl(Socket, FIONBIO, (char *)&onoff);
    s_ioctl(Socket, FIOASYNC, (char *)&onoff);

#elif defined(LSCO)

  int nFlags;
  
  nFlags = fcntl( Socket, F_GETFL );
  nFlags |= O_NONBLOCK;
  if( fcntl( Socket, F_SETFL, nFlags ) < 0 )
  {
    perror( "Fatal error executing nonblock" );
    exit( 1 );
  }

#else

  if (fcntl(Socket, F_SETFL, FNDELAY) == -1)    {
    perror("Noblock");
	exit (1);
  }

#endif
 
}

