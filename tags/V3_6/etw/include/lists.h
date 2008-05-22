#include "mytypes.h"

#if defined(__PPC__) && defined(__SASC)
#pragma options align=mac68k
#endif

#ifndef MY_LIST_H

struct MyNode 
{
    struct  MyNode *ln_Succ;    /* Pointer to next (successor) */
    struct  MyNode *ln_Pred;    /* Pointer to previous (predecessor) */
    unsigned char ln_Type;
    char    ln_Pri;        /* Priority, for sorting */
    char    *ln_Name;        /* ID string, null terminated */
};    /* Note: word aligned */

struct MyMinNode
{
    struct MyMinNode *mln_Succ;
    struct MyMinNode *mln_Pred;
};

struct MyList
{
   struct  MyNode *lh_Head;
   struct  MyNode *lh_Tail;
   struct  MyNode *lh_TailPred;
   UBYTE   lh_Type;
   UBYTE   l_pad;
};    /* word aligned */

#define MY_LIST_H

void MyNewList(struct MyList *);
void MyAddHead(struct MyList *,struct MyNode *);
void MyAddTail(struct MyList *,struct MyNode *);
void MyRemove(struct MyNode *);
#endif

