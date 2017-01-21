#include "eat.h"

// Corretti tick, counter e waitfor...

#include <math.h>

#ifndef PID2
    #define PID2    1.57079632679489661923       /*  PI/2  */
#endif

player_t *has_player_injuried()
{
    return p->player_injuried;
}
// char Table[]={4,1,5,0,3,2,5,2,1,4,0,3};

void SetShotSpeed(player_t *g, int16_t distance)
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

BOOL InArea(int8_t area, int16_t x, int16_t y)
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

BOOL InAnyArea(int16_t x,int16_t y)
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

void SetComando(player_t *g, int8_t cmd, int8_t dopo, int8_t arg)
{
    switch(cmd)
    {
        case ESEGUI_ROTAZIONE:
            {
                int8_t NewDir=(g->dir-arg);

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
        case ENTRA_CAMPO:
            if (g->Comando == STAI_FERMO) {
                g->CA[3] = STAI_FERMO;
            }
            else
                g->CA[3] = 0;

            g->Comando=ENTRA_CAMPO;
            g->CA[0] = ENTRA_CAMPO;
            g->WaitForControl = arg;

            if (g->Special) {
                g->CA[2] = g->AnimType;
                g->Special = FALSE;
            }
            else
                g->CA[2] = 0;
            break;
        default:
            D(bug("Comando non implementato!\n"));
    }
}

int16_t CanScore(player_t *g)
{
    register int16_t temp_x,temp_y;

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
/*                int8_t temp;

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

static void save_temp_back()
{
    extern uint8_t *back;
    if (back)
        free(back);
    if ((back = malloc(bitmap_height * bitmap_width)))
        memcpy(back, main_bitmap, bitmap_width * bitmap_height);

}


team_t *find_controlled_team()
{
    int i;
    for (i = 0; i < 2; ++i) 
        if (p->team[i]->Joystick > 0)
            return p->team[i];
    return NULL;
}


BOOL using_tactic(team_t *t, const char *tct)
{
    return !strcmp(t->tactic->Name, tct);
}

struct team_disk *find_disk_team(team_t *s)
{
    extern struct team_disk leftteam_dk, rightteam_dk;
    
    int snum = s->players[0].SNum;
    if ((snum && !(swaps & 1)) ||
        (!snum && (swaps & 1)) )
        return &rightteam_dk;
    else
        return &leftteam_dk;
}
BOOL handle_pause(int16_t button)
{
    team_t *cnt;
    D(bug("Clicked button %d in handle_pause\n", button));
    cnt = find_controlled_team();

    switch(button) {
            // continue playing
        case 0:
            pause_mode = FALSE;
            break;
            // replay
        case 1:
            if (!replay_mode) 
                start_replay = TRUE;
            else
                SaveReplay();

            pause_mode = FALSE;
            break;
        case 2:
            if (cnt) {
                extern void graphic_frame();
                extern BOOL HandleMenuIDCMP();

                graphic_frame();
                os_delay(1);
                rectfill_pattern(main_bitmap, 0, 0, WINDOW_WIDTH - 1,
                             WINDOW_HEIGHT - 1, 0, bitmap_width);
                save_temp_back();
                ScreenSwap();
                os_delay(1);
                SetTeamSubstitutions(cnt);
                draw_substitutions_menu();
                ScreenSwap();

                while(HandleMenuIDCMP());

                graphic_frame();
                os_delay(1);
                DrawPause();
            }
            break;
            // change tactic
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            if (cnt && change_tactic(cnt, get_tactic_name(button))) {
                set_pause_tactic(button);
                ScreenSwap();
            }
            break;
            // abandon game
        case 9:
            return FALSE;
    }
    return TRUE;
}

BOOL was_stopped=TRUE;

void DoPause(void)
{
    if(!time_stopped) {
        StopTime();
        was_stopped = FALSE;
    }
    else
        was_stopped = TRUE;
    
    hide_vjoy();
    
    pause_mode=TRUE;

    os_stop_audio();

    if (!replay_mode) {
        hide_ads();
        DrawPause();

        save_temp_back();
        ScreenSwap();
    }
    D(bug("Pause mode\n"));
}

player_t *FindNearestDirPlayer(player_t *g)
{
    register player_t *g2,*g_min=NULL;
    int16_t d_min=31000,t;
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
    int16_t d_min=31000,t;
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

int16_t FindDirection(int16_t x1, int16_t y1, int16_t x2,int16_t y2)
{
    register int16_t xd,xr,yd,yr;

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

void MoveTo(player_t *g,int16_t xd,int16_t yd)
{
    register int16_t NewDir=FindDirection(g->world_x,g->world_y,xd,yd);

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

int16_t FindDirection32(int16_t xfrom, int16_t yfrom,int16_t xto,int16_t yto)
{
    register int16_t x,y,x2,y2,x8,y8;

    x=xto-xfrom;
    y=yfrom-yto;

    y2=(y>>1);
    x2=(x>>1);
     x8=(x>>3);
    y8=(y>>3);

    if(x>y8)
    {
        if(y>(-x8)) /*1st sector */
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
        else /* 2nd sector */
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
        if(y < (-x8)) /* 3rd sector */
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
        else /* 4th sector */
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


uint8_t FindDirection256(int16_t xfrom, int16_t yfrom,int16_t xto,int16_t yto)
{
    register int16_t xda=abs(xto-xfrom)>>4,yda=abs(yfrom-yto)>>4;
    register uint8_t angle;

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
            angle=(uint8_t)((PID2-temp)*40.74);
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

BOOL AllowRotation(player_t *g,int16_t NewDir)
{
    register int16_t result=abs(g->dir-NewDir);

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

player_t *FindNearest(team_t *s,int16_t x,int16_t y)
{
    register int i;
    register int16_t t,minv=32767;
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

    if(!use_touch && i>=0 && joyonly && pl->gioc_palla&&(r_controls[i][counter]&JP_DIRECTION_MASK))
        return;

    if(     g->world_x<(pl->world_x-500) || // tolti 100
                g->world_x>(pl->world_x+580) ||
                g->world_y<(pl->world_y-400 ) || // tolti 100
                g->world_y>(pl->world_y+530 ) )
    {
        player_t *g2;

        if(i>=0 && (r_controls[i][counter]&JP_DIRECTION_MASK) )
        {
            int16_t Dir=GetJoyDirection(r_controls[i][counter]);

            /**
             * AC: Codeguard signals here a memory access overrun of 2 bytes
             * in a memory block of 480 bytes.
             */
            g2=FindNearest(s,G2P_X(pl->world_x)+(velocita_x[2][9][Dir]<<3),G2P_Y(pl->world_y)+(velocita_y[2][9][Dir]<<3));
        }
        else
            g2=FindNearest(s,G2P_X(pl->world_x),G2P_Y(pl->world_y));

        if(g2 != g && g2 != NULL)
            ChangeControlled(s, g2->GNum);
    }
}

void RimuoviComandoSquadra(uint8_t sq, int8_t cmd)
{
    team_t *s = p->team[sq];
    int i;

    for(i = 0; i < 10; i++)
    {
        if(s->players[i].Comando == cmd)
            s->players[i].Comando = NESSUN_COMANDO;
    }
}

void EseguiDopoComando(player_t *g)
{
    switch(g->CA[0])
    {
        case ENTRA_CAMPO:
            g->Comando = NESSUN_COMANDO;
            if (g->CA[3]) 
                g->Comando = g->CA[3];

            if (g->CA[2]) {
                g->Special = TRUE;
                g->AnimType = g->CA[2];
            }
            break;
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

int16_t IndirizzaTiro(player_t *g, uint32_t joystate)
{
    register int16_t dest_y, dest_x;

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

    return (int16_t)FindDistance(g->world_x,g->world_y,dest_x,dest_y,pl->dir);
}
