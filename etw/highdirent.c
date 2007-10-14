#if defined(WIN) && !defined(WINCE) && !defined(CROSSCOMPILER)

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "highdirent.h"


DIR *opendir(char *DirName)
{
    DIR *p=malloc(sizeof(*p));
    int l;

    if (!p) {
        return(NULL);
    }
    p->Handle=-1;
    strcpy(p->DirName,DirName);
    if ((l=strlen(p->DirName)) && p->DirName[l-1]!='/')
        strcat(p->DirName,"/");
    strcat(p->DirName,"*.*");
    return(p);
}

dirent_t *readdir(DIR *Directory)
{
    LONG n;

    if (Directory->Handle==-1)
        n=Directory->Handle=_findfirst(Directory->DirName,&Directory->FindBuf);
    else n=_findnext(Directory->Handle,&Directory->FindBuf);
    if (n==-1)
        return(NULL);
    strcpy(Directory->DirentBuf.d_name,Directory->FindBuf.name);
    return(&Directory->DirentBuf);
}



void closedir(DIR *Directory)

{
    if(Directory->Handle != -1)
        _findclose(Directory->Handle);
    
    free(Directory);

}

#endif /* WIN */
