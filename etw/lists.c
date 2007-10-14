#define NULL 0L
#include "lists.h"

void MyNewList(struct MyList *l)
{
    l->lh_TailPred=(struct MyNode *)l;
    l->lh_Head=(struct MyNode *)&l->lh_Tail;
    l->lh_Tail=NULL;

}

void MyAddTail(struct MyList *l,struct MyNode *n)
{
    n->ln_Pred=l->lh_TailPred;
    n->ln_Succ=l->lh_TailPred->ln_Succ;
    n->ln_Succ->ln_Pred=n->ln_Pred->ln_Succ=n;
}


void MyRemove(struct MyNode *n)
{
    n->ln_Succ->ln_Pred=n->ln_Pred;
    n->ln_Pred->ln_Succ=n->ln_Succ;
}

void MyAddHead(struct MyList *l,struct MyNode *n)
{
    n->ln_Succ=l->lh_Head;
    n->ln_Pred=l->lh_Head->ln_Pred;
    n->ln_Pred->ln_Succ=n->ln_Succ->ln_Pred=n;
}
