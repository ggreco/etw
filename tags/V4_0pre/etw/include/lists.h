#include "mytypes.h"

#if defined(__PPC__) && defined(__SASC)
#pragma options align=mac68k
#endif

#ifndef MY_LIST_H

struct MyNode 
{
    struct  MyNode *pNext;    /* Pointer to next (successor) */
    struct  MyNode *pPrev;    /* Pointer to previous (predecessor) */
    unsigned char ln_Type;
    char    ln_Pri;        /* Priority, for sorting */
    char    *ln_Name;        /* ID string, null terminated */
};    /* Note: word aligned */

struct MyMinNode
{
    struct MyMinNode *mpNext;
    struct MyMinNode *mpPrev;
};

struct MyList
{
   struct  MyNode *pHead;
   struct  MyNode *pTail;
   struct  MyNode *pTailPred;
   UBYTE   lh_Type;
   UBYTE   l_pad;
};    /* word aligned */

#define MY_LIST_H

void MyNewList(struct MyList *);
void MyAddHead(struct MyList *,struct MyNode *);
void MyAddTail(struct MyList *,struct MyNode *);
void MyRemove(struct MyNode *);
#endif

