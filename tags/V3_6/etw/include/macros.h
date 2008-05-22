// Macro usate in Eat The Whistle

#ifndef EAT_MACROS_H

#define EAT_MACROS_H


#define IsBlack(g) ((g)->Posizioni&P_PELLE_NERA)
#define MyRangeRand(v) (rand()%v)

#define GetTable() Table[p->TabCounter++]

#define CheckTrain(x) if(training){ int i; for(i=0;i<10;i++) if(!p->team[1]->players[i].Special){ D(bug("Err%ld: G%ld!\n",x,i)); break;} }

#define G2P_X(x) ((x)-48)
#define G2P_Y(x) ((x)-80)

// Era 100

/* TRUE orario, FALSE antiorario */

#define SimpleDistance(x1,y1,x2,y2) (abs((x1)-(x2))+abs((y1)-(y2)))
#define BestRotation(x,y) (CambioDirezione[(x)][(y)]>0)

#ifdef DEMOVERSION
    #define Progress() os_delay(4)
#else
void Progress();
#endif

// Macro sperimentale, vediamo se va bene :)

#define IsNear(x1,y1,x2,y2) (SimpleDistance(x1,y1,x2,y2)<250)
#define IsVeryNear(x1,y1,x2,y2) (SimpleDistance(x1,y1,x2,y2)<130)
#define IsVeryVeryNear(x1,y1,x2,y2) (SimpleDistance(x1,y1,x2,y2)<60)

#define InAreaPalla( g )    ( (g)->world_x>(pl->world_x-400) && \
                    (g)->world_x<(pl->world_x+480) && \
                    (g)->world_y>(pl->world_y-400 ) && \
                    (g)->world_y<(pl->world_y+330 ) )

#define InAreaGrandePalla( g )    ( (g)->world_x>(pl->world_x-600) && \
                    (g)->world_x<(pl->world_x+680) && \
                    (g)->world_y>(pl->world_y-500 ) && \
                    (g)->world_y<(pl->world_y+450 ) )

#define InAreaPortierePalla( g )    ( (g)->world_x>(pl->world_x-700) && \
                                (g)->world_x<(pl->world_x+780) && \
                            (g)->world_y>(pl->world_y-600 ) && \
                            (g)->world_y<(pl->world_y+550 ) )

#define NeiPressiPalla( g )    ( pl->world_x>(g->world_x-110)     && \
                                pl->world_x<(g->world_x+190)    && \
                                pl->world_y>(g->world_y+50)     && \
                                pl->world_y<(g->world_y+160) )

#define NeiPressiPallaPortiere( g )    ( pl->world_x>(g->world_x-80)     && \
                                pl->world_x<(g->world_x+150)    && \
                                pl->world_y>(g->world_y+15)     && \
                                pl->world_y<(g->world_y+190) )

// Ristretta di poco l'area del portiere

#define P2G_X(x) ((x)+48)
#define P2G_Y(x) ((x)+90)

// Era 100


#ifndef CD_VERSION

#define UrgentSpeaker(x)

#endif

#define PrintResult() bltchunkybitmap(p->result,0,0,main_bitmap,0,4,p->result_len*VS_CHAR_X,VS_CHAR_Y+1,result_width,bitmap_width)


#define DoSpecialAnim(g,x) g->AnimType=(x);g->FrameLen=0;g->AnimFrame=-1;g->Special=TRUE
#define DoAnim(g,x) g->AnimType=x;g->FrameLen=0;g->AnimFrame=-1

#define TogliPalla() if(pl->gioc_palla){if(pl->gioc_palla->speed<7) pl->gioc_palla->speed+=3;    \
                    pl->gioc_palla->Comando=NESSUN_COMANDO; p->last_touch=pl->gioc_palla->number+(pl->gioc_palla->SNum<<5); \
                    if(pl->gioc_palla->ArcadeEffect) RemoveArcadeEffect(pl->gioc_palla,pl->gioc_palla->ArcadeEffect); \
                    pl->gioc_palla=NULL;pl->sq_palla=NULL;  }
                    

#define TogliPallaMod() if(pl->gioc_palla){  \
                    int i;  p->last_touch=pl->gioc_palla->number+(pl->gioc_palla->SNum<<5); \
                    pl->velocita=(pl->gioc_palla->ActualSpeed<<2);    \
                    pl->dir=(pl->gioc_palla->dir<<5);if(pl->gioc_palla->speed<7) pl->gioc_palla->speed+=3;\
                    if(pl->gioc_palla->ArcadeEffect) RemoveArcadeEffect(pl->gioc_palla,pl->gioc_palla->ArcadeEffect); \
                    pl->gioc_palla->Comando=NESSUN_COMANDO; pl->gioc_palla=NULL;pl->sq_palla=NULL;    \
                    pl->MaxQuota=0;for(i=1;i<SHOT_LENGTH;i++) p->shotheight[i]=0;UpdateBallSpeed();}

#define DaiPalla(g) pl->gioc_palla=g;if(g->speed>2)g->speed-=3;pl->sq_palla=g->team; \
            pl->sq_palla->Possesso=1;p->team[g->SNum^1]->Possesso=0;

#define HideBall()    if(!pl->Hide) \
            {    \
                pl->Hide=TRUE;    \
                        RemAnimObj(pl->anim); \
            }

#define ShowBall()      if(pl->Hide) \
            {    \
                AddAnimObj(pl->anim,(pl->world_x>>3)-field_x,(pl->world_y>>3)-field_y,pl->ActualFrame);    \
                pl->Hide=FALSE;    \
            }

#define FindDistance(xs,ys,xd,yd,angle) (xd!=xs&&sin_table[angle]!=0 ? ((WORD)(abs( (abs(xd-xs)<<7)/sin_table[angle]) )) : abs(yd-ys) )

#define FermaPalla() pl->velocita=0;pl->delta_x=0;pl->delta_y=0;pl->quota=0
#endif
