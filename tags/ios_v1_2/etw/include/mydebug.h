#ifndef MYDEBUG_H

#define MYDEBUG_H

#ifdef DEBUG_DISABLED
    #define ETW_RELEASE

    #define bug
    #define D(x)
#endif

#ifndef bug
    #define bug kprintf 
    void kprintf(const char *fmt,...);
    #define D(x) x
#endif

#endif

