#ifndef MYFREQ

#ifndef MACOSX
#include "mytypes.h"
#endif

#define MYFREQ



struct MyFileRequest

{

    char *Title;

    char *Dir;

    char *Filter;

    char *File;
#ifdef MACOSX
// OSX used a signed char for bool, we use int, so we have to change this
    int Save;
#else
    BOOL Save;
#endif
};



#endif
