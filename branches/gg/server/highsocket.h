#if !defined(HIGHSOCKET_H)

#define HIGHSOCKET_H

#if defined(WIN32)

#include <winsock.h>

#define EWOULDBLOCK 10000

#elif defined(__SASC)

#include <ss/socket.h>
extern struct Library *SockBase;

#else

#include <sys/socket.h>

#endif /* WIN32 */

void SocketSystemBoot(void);
void SocketSystemShutDown(void);
int SockRead(int Socket,void *Buffer,long Size);
int SockWrite(int Socket,void *Buffer,long Size);
void SockNonBlock(int Socket);

#if defined(__SASC)

#define SockClose(s) s_close(s)

#elif defined(WIN32)

#define SockClose(s) closesocket(s)

#else

#define SockClose(s) close(s)

#endif /* Socket functions */

#endif /* HIGHSOCKET_H */

