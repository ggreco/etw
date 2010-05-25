#include <stddef.h>

#include "lists.h"

void MyNewList(struct MyList *l)
{
    l->pTailPred = (struct MyNode *)l;
    l->pHead = (struct MyNode *)&l->pTail;
    l->pTail = NULL;
}

void MyAddTail(struct MyList *l, struct MyNode *n)
{
    n->pPrev = l->pTailPred;
    n->pNext = l->pTailPred->pNext;
    n->pNext->pPrev = n->pPrev->pNext = n;
}

void MyRemove(struct MyNode *n)
{
    n->pNext->pPrev = n->pPrev;
    n->pPrev->pNext = n->pNext;
}

void MyAddHead(struct MyList *l, struct MyNode *n)
{
    n->pNext = l->pHead;
    n->pPrev = l->pHead->pPrev;
    n->pPrev->pNext = n->pNext->pPrev = n;
}

