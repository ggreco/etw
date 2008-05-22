#include "eat.h"

// Corretti tick, counter e waitfor...

#include <math.h>

#ifndef PID2
    #define PID2    1.57079632679489661923       /*  PI/2  */
#endif

// char Table[]={4,1,5,0,3,2,5,2,1,4,0,3};

void SetShotSpeed(player_t *g, WORD distance)
{
// Suppongo che queste saranno da modificare!

    if(pl->TipoTiro==TIRO_PALLONETTO)
    {
        if(distance>2100||g->Tiro<4)
            pl->velocita=10+(g->Tiro<<1);
        else if(distance>1300||g->Tiro<5)
            pl->velocita=9+g->Tiro;
        else
            pl->velocita=6+g->Tiro;
    }
    else if(pl->TipoTiro==TIRO_ALTO)
    {
        if(distance>1500||g->Tiro<4)
            pl->velocita=10+(g->Tiro<<1);
        else
            pl->velocita=11+g->Tiro;
    }
    else if(pl->TipoTiro==TIRO_RASOTERRA)
    {
        if(distance>1500||g->Tiro<5)
            pl->velocita=6+(g->Tiro<<1);
        else
            pl->velocita=4+(g->Tiro<<1);
    }
    else
    {
        pl->velocita=10+(g->Tiro<<1);
    }
}

BOOL InArea(BYTE area, WORD x, WORD y)
{
    if(y <= AREA_RIGORE_Y_N || y >= AREA_RIGORE_Y_S)
        return FALSE;

    if(area && x < CENTROCAMPO_X &&
       ((long int)(y>>3)) < (-((long int)x>>3) * 31 + 6197L)) // era 6157
        return TRUE;

    if(!area && x > CENTROCAMPO_X &&
       ((long int)(y>>3)) < (((long int)x>>3) * 31 - 32963L)) // era 33003
        return TRUE;

    return FALSE;
}

BOOL InAnyArea(WORD x,WORD y)
{
    if(y <= AREA_RIGORE_Y_N || y >= AREA_RIGORE_Y_S)
        return FALSE;

    if(x < CENTROCAMPO_X &&
       ((long int)(y>>3)) < (-((long int)x>>3) * 31 + 6197L))
        return TRUE;

    if(x > CENTROCAMPO_X &&
       ((long int)(y>>3)) < (((long int)x>>3) * 31 - 32963L))
        return TRUE;

    return FALSE;
}

void SetComando(player_t *g, BYTE cmd, BYTE dopo, BYTE arg)
{
    switch(cmd)
    {
        case ESEGUI_ROTAZIONE:
            {
                BYTE NewDir=(g->dir-arg);

                if(NewDir>4)
                {
                    NewDir-=8;
                }
                else if(NewDir<-4)
                {
                    NewDir+=8;
                }

//                D(bug("g->Dir: %ld NewDir: %ld Spost: %ld\n",g->dir,arg,NewDir));

                g->Comando=ESEGUI_ROTAZIONE;
                g->Argomento=NewDir;
                g->WaitForControl=0;
                g->CA[0]=dopo;
                g->CA[1]=pl->TipoTiro;
                g->CA[2]=pl->velocita;
                g->CA[3]=pl->dir;
                g->CA[4]=g->SpecialData;
                g->CA[5]=pl->MaxQuota;
            }
            break;
        case VAI_PALLA:
            g->WaitForControl=4;
            g->Comando=VAI_PALLA;
            g->CA[0]=dopo;
            break;
        default:
            D(bug("Comando non implementato!\n"));
    }
}

WORD CanScore(player_t *g)
{
    register WORD temp_x,temp_y;

    temp_y=(g->world_y>>3);

    if(g->SNum)
    {
        temp_x=1270-(g->world_x>>3);
    }
    else temp_x=(g->world_x>>3);

    if(temp_x<360)
    {
        if(    temp_y>(-temp_x+260) &&
               temp_y<(temp_x+300) )
        {
            if(temp_y>(temp_x-180) &&
               temp_y<(-temp_x+740)    )
            {
/*                BYTE temp;

Si tira anche girati di schiena!

                temp=FindDirection(g->world_x+40,g->world_y+120,(g->SNum ? PORTA_E_X : PORTA_O_X), CENTROCAMPO_Y);
                
                if(    temp==g->dir ||
                    temp==dir_next[g->dir] ||
                    temp==dir_pred[g->dir] )

                else
                    return CS_NO;
*/
                return CS_SI;
            }
        }
        else
            return CS_CROSS;
    }
    
    return CS_NO;
}

void DoPause(void)
{
    BOOL ok=FALSE,was_stopped=TRUE;
    extern BOOL time_stopped;

    if(!time_stopped)
    {
        StopTime();
        was_stopped=FALSE;
    }

    pause_mode=TRUE;

//    SetCrowd(-1);

    os_stop_audio();

    DrawPause();
    ScreenSwap();

    if(triple_buffering)
        ScreenSwap();

    while(!ok)
    {
        if(control[0]!=CTRL_JOYPAD && control[1]!=CTRL_JOYPAD)
        {
            os_wait();
        }
        else
        {
            os_delay(10);
            UpdatePortStatus();

            if( (MyReadPort0(0)&MYBUTTONMASK) ||
                (MyReadPort1(1)&MYBUTTONMASK) )
                ok=TRUE;
            else
                os_getevent();
        }

        ok=os_wait_end_pause()|ok;

/* Quit forzato */
        if(ok==2)
        {
            quit_game=TRUE;
            SetResult("break");

            if(p->team[0]->Joystick<0 ||
                p->team[1]->Joystick<0)
            {
                if(p->team[0]->Joystick>=0)
                {
                    p->team[0]->Reti=0;
                    p->team[1]->Reti=5;
                }
                else
                {
                    p->team[0]->Reti=5;
                    p->team[1]->Reti=0;
                }
            }
        }
    }

    os_start_audio();

//    SetCrowd(FONDO);

    pause_mode=FALSE;

    if(!was_stopped)
        RestartTime();
    
    ideal=Timer()-1;
}

player_t *FindNearestDirPlayer(player_t *g)
{
    register player_t *g2,*g_min=NULL;
    WORD d_min=31000,t;
    register int i;

    for(i=0;i<10;i++)
    {
        if((g2=&g->team->players[i])!=g)
        {
            if(FindDirection(g->world_x,g->world_y,g2->world_x,g2->world_y)==g->dir)
            {
                t=abs(g->world_x-g2->world_x)+abs(g->world_y-g2->world_y);

                if(t<d_min&&g2->Comando==NESSUN_COMANDO)
                {
                    g_min=g2;
                    d_min=t;
                }
            }
        }
    }

    if(g_min)
        g->SpecialData=d_min;

    return g_min;
}

player_t *FindNearestPlayer(player_t *g)
{
    register player_t *g2,*g_min=NULL;
    WORD d_min=31000,t;
    register int i;

    for(i=0;i<10;i++)
    {
        if((g2=&g->team->players[i])!=g)
        {
            t=abs(g->world_x-g2->world_x)+abs(g->world_y-g2->world_y);

            if(t<d_min&&g2->Comando==NESSUN_COMANDO)
            {
                g_min=g2;
                d_min=t;
            }
        }
    }

    if(g_min)
        g->SpecialData=d_min;

    return g_min;
}

// Aggiornata ai 50fps!

void UpdateShotHeight(void)
{
    register int i;
    register int ai,bi,a,b;

/* Sarebbe b=quota/16 e a=-b/64, io pero' ho bisogno di numeri grandi per mantenere
 * la precisione allora moltiplico tutto *1024;
 */
    ai=a=-pl->MaxQuota;
    bi=b=(pl->MaxQuota<<6);
    
    for(i=1;i<SHOT_LENGTH;i++)
    {
        p->shotheight[i]=((ai*i+bi)>>10);

/* Era b*i ma aggiungo b a b ad ogni ciclo ed ottengo la stessa
   cosa, idem per a (era a*i*i) */

/* Il risultato e' diviso per 1024 per ovvi motivi */

        ai+=a;
        bi+=b;
    }

    if(p->shotheight[SHOT_LENGTH/2]>p->shotheight[(SHOT_LENGTH-2)/2])
        p->shotheight[SHOT_LENGTH/2]--;
}

WORD FindDirection(WORD x1, WORD y1, WORD x2,WORD y2)
{
    register WORD xd,xr,yd,yr;

    xd=(x1-x2); 
    yd=(y1-y2);

    yr=( yd >> 1);
    xr=( xd >> 1); // Mi serve il modulo / 2

    if(xd >= yr ) /* NO O, SO o S */
    {

        if( yd > -xr ) /* NO o O */
        {
            if(yd<xr)
                return D_OVEST;

            return D_NORD_OVEST;
        }

        /* SO o S */

        if( xd < -yr)
            return D_SUD;

        return D_SUD_OVEST;
    }

/* N E NE o SE */

    if(yd < -xr) /* E o SE */
    {
        if(yd<xr)
            return D_SUD_EST;
    
        return D_EST;
    }

/* N o NE */

    if(xd>-yr)
        return D_NORD;

    return D_NORD_EST;
}

void MoveTo(player_t *g,WORD xd,WORD yd)
{
    register WORD NewDir=FindDirection(g->world_x,g->world_y,xd,yd);

    if(!g->ActualSpeed)
    {
        g->FrameLen=0;
        g->AnimFrame=-1;
        g->ActualSpeed=1;
    }
    else
    {
        if(NewDir!=g->dir)
        {
            if(NewDir==opposto[g->dir])
            {
                if(g==pl->gioc_palla)
                {
                    DoSpecialAnim(g,GIOCATORE_INVERSIONE_MARCIA_PALLA);
                }
                else
                {
                    DoSpecialAnim(g,GIOCATORE_INVERSIONE_MARCIA);
                }

                g->ActualSpeed=0;
            }
            else g->dir=NewDir;

/*
            if(BestRotation(g->dir,NewDir))
            {
                g->AnimType=GIOCATORE_CAMBIO_ORARIO;
                g->dir++;

                if(g->dir>7)
                    g->dir=0;
            }
            else
            {
                g->AnimType=GIOCATORE_CAMBIO_ANTI;
                g->dir--;

                if(g->dir<0)
                    g->dir=7;
            }
*/
        }
        else if(g->ActualSpeed<3)
        {
            g->ActualSpeed++;
        }
    }
}

WORD FindDirection32(WORD xfrom, WORD yfrom,WORD xto,WORD yto)
{
    register WORD x,y,x2,y2,x8,y8;

    x=xto-xfrom;
    y=yfrom-yto;

    y2=(y>>1);
    x2=(x>>1);
     x8=(x>>3);
    y8=(y>>3);

    if(x>y8)
    {
        if(y>(-x8)) /*1o settore */
        {
            if(y>(x-x8))
            {
                if(x<(y2+y8))
                {
                    if(x<(y2-y8))
                        return 1;

                    return 2;
                }
                else
                {

                    if(x<(y-y8))
                        return 3;

                    return 4;
                }
            }
            else
            {
                if(y>(x2-x8))
                {
                    if(y>(x2+x8))
                        return 5;

                    return 6;
                }
                else
                {
                    if(y>x8)
                        return 7;

                    return 8;
                }
            }
        }
        else /* 2o settore */
        {
            if(x>(-y+y8))
            {
                if(y>(-x2-x8))
                {
                    if(y>(-x2+x8))
                        return 9;

                    return 10;
                }
                else
                {
                    if(y>(-x+x8))
                        return 11;
    
                    return 12;
                }
            }
            else
            {
                if(x>(-y2+y8))
                {
                    if(x>(-y2-y8))
                        return 13;

                    return 14;
                }
                else
                {
                    if(x>(-y8))
                        return 15;

                    return 16;
                }
            }
        }
    }
    else
    {
        if(y < (-x8)) /* 3o settore */
        {
            if(y<(x-x8))
            {
                if(x>(y2+y8))
                {
                    if(x>(y2-y8))
                        return 17;

                    return 18;
                }
                else
                {
                    if(x>(y-y8))
                        return 19;

                    return 20;
                }
            }
            else
            {
                if(y<(x2-x8))
                {
                    if(y<(x2+x8))
                        return 21;

                    return 22;
                }
                else
                {
                    if(y<(x8))
                        return 23;

                    return 24;
                }
            }
        }
        else /* 4o settore */
        {
            if(x<(-y+y8))
            {
                if(y<(-x2-x8))
                {
                    if(y<(-x2+x8))
                        return 25;

                    return 26;
                }
                else
                {
                    if(y<-x+x8)
                        return 27;

                    return 28;
                }
            }
            else
            {
                if(x<(-y2+y8))
                {
                    if(x<(-y2-y8))
                        return 29;
                        
                    return 30;
                }
                else
                {
                    if(x<(-y8))
                        return 31;

                    return 0;
                }
            }
        }
    }
}


UBYTE FindDirection256(WORD xfrom, WORD yfrom,WORD xto,WORD yto)
{
    register WORD xda=abs(xto-xfrom)>>4,yda=abs(yfrom-yto)>>4;
    register UBYTE angle;

    if(yda!=xda)
    {
        if(xda==0)
        {
// Se xda e' 0 sono 0 gradi...
            angle=0;
        }
        else if(yda==0)
        {
// Se yda e' 0 invece sono 90.

            angle=64;
        }
        else
        {
            double temp;

            temp=atan2((double)yda,(double)xda);
            angle=(UBYTE)((PID2-temp)*40.74);
        }
    }
    else angle=32; // 45 gradi in 256ali

    if(xto>=xfrom)
    {
// semispazio destro
        if(yfrom<=yto)
        {
// 2o quadrante 
//            angle+=64;
            angle=128-angle;
        }
/*
        else
        {
// 1o quadrante
            angle=64-angle;
        }
*/
    }
    else
    {
        if(yfrom>=yto)
        {
// 4o quadrante
//            angle+=192;
            angle=255-angle;
            angle++;
        }
        else
        {
// 3o quadrante
//            angle=192-angle;
            angle+=128;
        }
    }

    return angle;
}

void UpdateBallSpeed(void)
{
// Scifto di 6 e non di 7 perche' la velocita' della palla e' in realta' (era 5 con i vecchi limiti)
// = pl->velocita*2

    pl->delta_y=-(cos_table[pl->dir]*pl->velocita)>>6;
    pl->delta_x=(sin_table[pl->dir]*pl->velocita)>>6;
}

void LiberaListe(void)
{
    totale_lista = 0;
    totale_lista_c = 0;
}

BOOL AllowRotation(player_t *g,WORD NewDir)
{
    register WORD result=abs(g->dir-NewDir);

    if(result!=1&&result!=7)
        return FALSE;

    if(BestRotation(g->dir,NewDir))
    {
        DoSpecialAnim(g,GIOCATORE_CAMBIO_ORARIO);
    }
    else
    {
        DoSpecialAnim(g,GIOCATORE_CAMBIO_ANTI);
    }

    return TRUE;
}

BOOL AggiungiLista(object_t *o)
{
    if(totale_lista<DIMENSIONI_LISTA_OGGETTI)
    {
        register int i=0;

        while(object_list[i]&&i<DIMENSIONI_LISTA_OGGETTI)
            i++;

        if(i<DIMENSIONI_LISTA_OGGETTI)
        {
            object_list[i]=o;
            totale_lista++;
            return TRUE;
        }
    }    
    
    D(bug("Attenzione, lista degli oggetti piena!\n"));

    return FALSE;
}

void RimuoviLista(object_t *o)
{
    if(totale_lista>0)
    {
        register int i=0;

        while(object_list[i]!=o&&i<DIMENSIONI_LISTA_OGGETTI)
            i++;

        if(i<DIMENSIONI_LISTA_OGGETTI)
        {
            totale_lista--;
            object_list[i]=object_list[totale_lista];
            object_list[totale_lista]=NULL;
        }
    }
}

// we have to free the object we add to the list
BOOL AggiungiCLista(struct DOggetto *o)
{
    if(totale_lista_c<NUMERO_OGGETTI)
    {
        register int i=0;

        while(c_list[i]&&i<NUMERO_OGGETTI)
            i++;

        if(i<NUMERO_OGGETTI)
        {
            c_list[i]=o;
            totale_lista_c++;
            return TRUE;
        }
    }    
    
    D(bug("Attenzione, lista degli oggetti piena!\n"));

    return FALSE;
}

void VuotaCLista(void)
{
    while (totale_lista_c--) {
        if (c_list[totale_lista_c]) {
            free(c_list[totale_lista_c]);
            c_list[totale_lista_c] = NULL;
        }
    }
}

void RimuoviCLista(struct DOggetto *o)
{
    if (totale_lista_c > 0) {
        register int i = 0;

        while (c_list[i] != o && i < NUMERO_OGGETTI)
            i++;

        if ( i < NUMERO_OGGETTI) {
            totale_lista_c--;
            free(o);
            c_list[i] = c_list[totale_lista_c];
            c_list[totale_lista_c] = NULL;
        }
    }
}

player_t *FindNearest(team_t *s,WORD x,WORD y)
{
    register int i;
    register WORD t,minv=32767;
    register player_t *n=&s->players[0],*g;

    for(i=0;i<10;i++)
    {
        g=&s->players[i];

        t=abs(g->world_x-x)+abs(g->world_y-y);

        if(t<minv&&g->Comando==NESSUN_COMANDO)
        {
            n=g;
            minv=t;
        }
    }

    return n;
}

void DoFlash(void)
{
    memset(main_bitmap,Pens[P_NERO],bitmap_width*bitmap_height);

    if(p->flash_pos<5)
    {
        soft_scroll=FALSE;
    }
    else
    {
        p->flash_mode=FALSE;
        p->flash_pos=0;
    }
}

void CheckActive(void)
{
    register team_t *s=p->team[0];
    register player_t *g;
    static BOOL second_call=FALSE;
    int i;

    if(s->Possesso)
    {
        if(!pl->gioc_palla&&(pl->velocita<8||pl->Rimbalzi>0)&&!second_call)
        {
            second_call=TRUE;
            s->Possesso=0;
            CheckActive();
            s->Possesso=1;
        }

        s=p->team[1];
    }
    else if(!pl->gioc_palla&&(pl->velocita<8||pl->Rimbalzi>0)&&!second_call)
    {
        second_call=TRUE;
        s->Possesso=1;
        CheckActive();
        s->Possesso=0;
    }

    second_call=FALSE;

    g=s->attivo;

// Se la leva e' tirata NON cambio l'uomo!

    i=s->Joystick;

// Modifica, ora lo faccio SOLO se la palla NON e' libera (quindi e' dell'altra squadra)...

    if(i>=0 && joyonly && pl->gioc_palla&&(r_controls[i][counter]&JP_DIRECTION_MASK))
        return;

    if(     g->world_x<(pl->world_x-500) || // tolti 100
                g->world_x>(pl->world_x+580) ||
                g->world_y<(pl->world_y-400 ) || // tolti 100
                g->world_y>(pl->world_y+530 ) )
    {
        player_t *g2;

        if(i>=0 && (r_controls[i][counter]&JP_DIRECTION_MASK) )
        {
            WORD Dir=GetJoyDirection(r_controls[i][counter]);

            /**
             * AC: Codeguard signals here a memory access overrun of 2 bytes
             * in a memory block of 480 bytes.
             */
            g2=FindNearest(s,G2P_X(pl->world_x)+(velocita_x[3][9][Dir]<<3),G2P_Y(pl->world_y)+(velocita_y[3][9][Dir]<<3));
        }
        else
            g2=FindNearest(s,G2P_X(pl->world_x),G2P_Y(pl->world_y));


        if(g2!=g&&g2!=NULL)
            ChangeControlled(s,g2->GNum);
    }
}

void RimuoviComandoSquadra(char sq, BYTE cmd)
{
    register team_t *s=p->team[sq];
    register int i;

    for(i=0;i<10;i++)
    {
        if(s->players[i].Comando==cmd)
            s->players[i].Comando=NESSUN_COMANDO;
    }
}

void EseguiDopoComando(player_t *g)
{
    switch(g->CA[0])
    {
        case ESEGUI_TIRO:
            g->Comando=NESSUN_COMANDO;
            pl->TipoTiro=g->CA[1];
            pl->velocita=g->CA[2];
            pl->dir=g->CA[3];
            g->SpecialData=g->CA[4];
            pl->MaxQuota=g->CA[5];
            Tira(g);
            break;
        case ESEGUI_PASSAGGIO:
            g->Comando=NESSUN_COMANDO;
            pl->TipoTiro=g->CA[1];
            pl->velocita=g->CA[2];
            pl->dir=g->CA[3];
            g->SpecialData=g->CA[4];
            PassaggioB(g);
            break;
        case ESEGUI_RIMESSA:
            g->dir=FindDirection(g->world_x,g->world_y,g->team->attivo->world_x,g->team->attivo->world_y);

            HideBall();

            if(pl->world_y<=(RIMESSA_Y_N+1))
                g->world_y=RIMESSA_Y_N-120;
            else
                g->world_y=RIMESSA_Y_S-90;

                DoSpecialAnim(g,GIOCATORE_RECUPERA_PALLA);


            if(g->team->Joystick<0)
                g->WaitForControl=(GetTable()+2)<<3;
            else
                g->WaitForControl=120; // 4 secondi max per le rimesse umane.

            g->TimePress=0;
            g->ActualSpeed=0;
            g->Comando=NESSUN_COMANDO;
            break;
        case ESEGUI_BATTUTA:
            g->dir=FindDirection(g->world_x,g->world_y,g->SNum ? PORTA_E_X : PORTA_O_X,PORTA_O_Y);

            pl->dir=g->dir<<5;

            DoSpecialAnim(g,GIOCATORE_BATTUTA);

            if(g->team->Joystick<0)
                g->WaitForControl=(GetTable()+5)<<4;

            StopTime(); // Fermo il tempo per corner/punizioni/rigori

            g->world_x=pl->world_x-avanzamento_x[g->dir];
            g->world_y=pl->world_y-avanzamento_y[g->dir];

            TogliPalla();
            DaiPalla(g);

            g->TimePress=0;
            g->ActualSpeed=0;
            g->Comando=NESSUN_COMANDO;
            break;
        default:
            g->Comando=NESSUN_COMANDO;
            D(bug("DopoComando indefinito!!!\n"));
            break;
    }
}

WORD IndirizzaTiro(player_t *g, uint32_t joystate)
{
    register WORD dest_y, dest_x;

    pl->velocita = 8 + GetTable() + g->Tiro;

    if(g->SNum)
        dest_x = G2P_X(PORTA_E_X);
    else
        dest_x = G2P_X(PORTA_O_X);

    if(joystate&JPF_JOY_UP)
    {
        dest_y=G2P_Y(GOAL_Y_N)+20+(g->Tiro<<3)-(GetTable()<<1);
    }
    else if(joystate&JPF_JOY_DOWN)
    {
        dest_y=G2P_Y(GOAL_Y_S)-20-(g->Tiro<<3)+(GetTable()<<1);
    }    
    else
    {
        dest_y=G2P_Y((GOAL_Y_N+GOAL_Y_S)/2)-40+(GetTable()<<4);
    }

    pl->dir=FindDirection256(g->world_x,g->world_y,dest_x,dest_y);

    return (WORD)FindDistance(g->world_x,g->world_y,dest_x,dest_y,pl->dir);
}
