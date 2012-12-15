#include <stdlib.h>
#include <string.h>
#if defined(WIN32)
#include <winsock.h>
#else
/* Sotto Linux non so, sotto MacOS X includere netient/in.h provoca problemi
 * (manca qualche include prima).
 */
#include <time.h>
//#include <netinet/in.h>
#endif
#include "server.h"
#include "utility.h"

#ifdef LINUX

#include <sys/time.h>

clock_t gettime(void)
{
    static unsigned int start_secs = 0;
    struct timeval tv;
    
    gettimeofday(&tv, NULL);

    if(start_secs == 0)
        start_secs = tv.tv_sec;

    tv.tv_sec -= start_secs;

    return (clock_t)((tv.tv_sec*1000) + tv.tv_usec/1000);
}

#endif

int find_header(char *buffer,int len)
{
	int i;

	len-=3; // non ci interessa controllare gli ultimi 3 byte...

	for(i=0; i<len; i++)
		if(!memcmp(buffer+i,SERVER_HEADER,4))
			return i;

	return -1;
}

simplemsg *getmsg(player *p,int length)
{
	int pos;

//	printf("Ricevuto un msg di %d bytes\n",length);

	if((pos=find_header(p->packetbuffer,length))>=0) {
		short packetlen=ntohs(((simplemsg *)(&p->packetbuffer[pos]))->size);
			
		if( (packetlen+pos+sizeof(simplemsg)) <= length) {
// metto il size nel formato giusto dell'hardware
			((simplemsg *)(p->packetbuffer+pos))->size=packetlen;
			return (simplemsg *)(p->packetbuffer+pos);
		}
	}
	
	return NULL;
}

void stripmsg(player *p, simplemsg *msg, int *length)
{
	int k=msg->size+sizeof(simplemsg)+(int)(((char *)msg)-p->packetbuffer);

	*length = (*length)-k;

	if(*length>0)
		memcpy(p->packetbuffer,p->packetbuffer+k,*length);
}

