#ifndef ETWUTILS_H

#define ETWUTILS_H

#include <time.h>

#if !defined(max)
    #define max(a,b) ((a)>(b) ? (a) : (b))
#endif

#ifdef LINUX
#define MY_CLOCKS_PER_SEC 1000
#else
#define gettime() clock();
#define MY_CLOCKS_PER_SEC CLOCKS_PER_SEC
#endif

extern clock_t gettime(void);
extern int find_header(char *buffer,int len);
extern simplemsg *getmsg(player *p,int length);
extern void stripmsg(player *p, simplemsg *msg, int *length);
#endif

