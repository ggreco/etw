#include "eat.h"

void (*LongPass)(player_t *, uint32_t);

uint32_t mov_inc_angolo[8][2]=
{
    {JPF_JOY_RIGHT,JPF_JOY_LEFT},
    {JPF_JOY_RIGHT|JPF_JOY_DOWN,JPF_JOY_UP|JPF_JOY_LEFT},
    {JPF_JOY_DOWN,JPF_JOY_UP},
    {JPF_JOY_DOWN|JPF_JOY_LEFT,JPF_JOY_RIGHT|JPF_JOY_UP},
    {JPF_JOY_LEFT,JPF_JOY_RIGHT},
    {JPF_JOY_LEFT|JPF_JOY_UP,JPF_JOY_DOWN|JPF_JOY_RIGHT},
    {JPF_JOY_UP,JPF_JOY_DOWN},
    {JPF_JOY_UP|JPF_JOY_RIGHT,JPF_JOY_LEFT|JPF_JOY_DOWN}
};

uint32_t mov_dec_angolo[8][2]=
{
    {JPF_JOY_LEFT,JPF_JOY_RIGHT},
    {JPF_JOY_LEFT|JPF_JOY_UP,JPF_JOY_RIGHT|JPF_JOY_DOWN},
    {JPF_JOY_UP,JPF_JOY_DOWN},
    {JPF_JOY_UP|JPF_JOY_RIGHT,JPF_JOY_LEFT|JPF_JOY_DOWN},
    {JPF_JOY_RIGHT,JPF_JOY_LEFT},
    {JPF_JOY_RIGHT|JPF_JOY_DOWN,JPF_JOY_UP|JPF_JOY_LEFT},
    {JPF_JOY_DOWN,JPF_JOY_UP},
    {JPF_JOY_DOWN|JPF_JOY_LEFT,JPF_JOY_RIGHT|JPF_JOY_UP}
};

uint32_t joy_opposto_neg[8]=
{
    JPF_JOY_UP,
    JPF_JOY_RIGHT|JPF_JOY_UP,
    JPF_JOY_RIGHT,
    JPF_JOY_RIGHT|JPF_JOY_DOWN,
    JPF_JOY_DOWN,
    JPF_JOY_DOWN|JPF_JOY_LEFT,
    JPF_JOY_LEFT,
    JPF_JOY_LEFT|JPF_JOY_UP,
};


void CPUShot(player_t *g)
{
    uint32_t state;

    switch(GetTable()>>1)
    {
        case 1:
            state=JPF_JOY_UP;
            break;
        case 2:
            state=JPF_JOY_DOWN;
            break;
        default:
            state=0L;
    }

    SetShotSpeed(g,IndirizzaTiro(g,state));

    g->dir=(pl->dir+8)>>5;

    if(g->dir>7)
        g->dir-=8;

// Gestione della quota

    switch(GetTable())
    {
        case 5:
            pl->TipoTiro=TIRO_PALLONETTO;
            break;
        case 4:
        case 3:
            pl->TipoTiro=TIRO_RASOTERRA;
            break;
        default:
            pl->TipoTiro=TIRO_ALTO;
    }

    Tira(g);
}

void FreePass(player_t *g, uint32_t joystate)
{
    pl->velocita=23;

    pl->dir=(g->dir<<5);

    if(    joystate&mov_dec_angolo[g->dir][0] &&
        !(joystate&mov_dec_angolo[g->dir][1])
        )
    {
        pl->dir-=(10+g->creativity-GetTable());
    }
    else if(joystate&mov_inc_angolo[g->dir][0] &&
        !(joystate&mov_inc_angolo[g->dir][1])
        )
    {
        pl->dir+=(10+g->creativity-GetTable());
    }

    if( joystate&joy_opposto[g->dir] &&
        !(joystate&joy_opposto_neg[g->dir]) 
    )
    {
// Pallonetto
        pl->TipoTiro=TIRO_PALLONETTO;
    }
    else if( joystate&joy_opposto[opposto[g->dir]] &&
        !(joystate&joy_opposto_neg[opposto[g->dir]] )
    )
    {
// Tiro rasoterra
        pl->velocita>>=1;

        pl->TipoTiro=TIRO_RASOTERRA;
    }
    else    pl->TipoTiro=TIRO_ALTO;

    CheckPortiere(g->SNum^1);

}

void TargetedPass(player_t *g, uint32_t joystate)
{
    WORD d,old_x=g->world_x,old_y=g->world_y;
    player_t *g2;

    if(joystate&JP_DIRECTION_MASK)
        d=GetJoyDirection(joystate);
    else
        d=g->dir;

    g->world_x+=velocita_x[2][9][d]*80;
    g->world_y+=velocita_y[2][9][d]*80;

    g2=FindNearestPlayer(g);
    pl->dir=FindDirection256(old_x,old_y,g2->world_x,g2->world_y);
    d=FindDistance(old_x,old_y,g2->world_x,g2->world_y,pl->dir);
    
    d=min(d,2500); // poco meno di 350 pixel come lunghezza massima

    if(d<300)
    {
        d=300;

        if(g2->ActualSpeed)
        {
            pl->dir=FindDirection256(old_x,old_y,
                g2->world_x+(velocita_x[g->ActualSpeed-1][g->speed][g->dir]<<3),
                g2->world_y+(velocita_y[g->ActualSpeed-1][g->speed][g->dir]<<3));
        }
        else
        {
            if(g2->SNum)
            {
                if(pl->dir>196||pl->dir<64)
                    pl->dir+=10;
                else
                    pl->dir-=10;
            }
            else
            {
                if(pl->dir>196||pl->dir<64)
                    pl->dir-=10;
                else
                    pl->dir+=10;
            }
        }
    }

    g->world_x=old_x;
    g->world_y=old_y;

    pl->velocita=(d>>7)+1;
    pl->TipoTiro=TIRO_PALLONETTO;
    g->SpecialData=g2->GNum;
}

void EseguiCross(player_t *g)
{
    WORD old_x=g->world_x,old_y=g->world_y,distance;
    player_t *g2;

    g->world_y=RIGORE_Y;

// L'omino ideale e' 20 pixel prima del dischetto del rigore

    if(g->SNum)
        g->world_x=(RIGORE_X_E+160);
    else
        g->world_x=(RIGORE_X_O-160);

    g2=FindNearestPlayer(g);

    pl->dir=FindDirection256(old_x,old_y,g2->world_x,g2->world_y);
    distance=FindDistance(old_x,old_y,g2->world_x,g2->world_y,pl->dir);

// Do un bonus qui, senno' mi vengono troppo corti i cross.

    distance=min(2200,distance);

    g->world_x=old_x;
    g->world_y=old_y;

    pl->velocita=((distance+32)>>7); // Non e' piu' dimezzata, ci tolgo il +2 pero'

    pl->MaxQuota=9+GetTable()+g->creativity;
    pl->TipoTiro=TIRO_CORNER;
    g->SpecialData=g2->GNum;
}

void PreparaBarriera(char sq)
{
    team_t *s=p->team[sq];
    player_t *g;
// La barriera la metto sul primo o sul secondo palo a seconda della posizione del pallone
    UBYTE dir=FindDirection256(pl->world_x,pl->world_y,
        (sq ? PORTA_O_X : PORTA_E_X),pl->world_y>CENTROCAMPO_Y ? GOAL_Y_S : GOAL_Y_N);
    WORD fuoco_x=pl->world_x,fuoco_y=pl->world_y,get_position,i;

// 128 pixel di distanza

    fuoco_x+=G2P_X((sin_table[dir]<<3));
    fuoco_y+=G2P_Y(-(cos_table[dir]<<3));

    if(sq)
    {
        dir+=64;
        get_position=CENTROCAMPO_X+2000;
    }
    else
    {
        dir-=64;
        get_position=CENTROCAMPO_X-2000;
    }

// Se e' sotto metto un omino sotto e tre sopra, altrimenti tre sotto e uno sopra.

    if(pl->world_y>CENTROCAMPO_Y)
    {
        fuoco_x-=sin_table[dir];
        fuoco_y+=cos_table[dir];
    }
    else
    {
        fuoco_x-=(sin_table[dir]*3);
        fuoco_y+=(cos_table[dir]*3);
    }

    for(i=0;i<4;i++)
    {
        g=FindNearest(s,get_position,CENTROCAMPO_Y);

        DoAnim(g,GIOCATORE_RESPIRA);

        g->world_x=fuoco_x;
        g->world_y=fuoco_y;

        g->dir=FindDirection(g->world_x,g->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
        g->Comando=STAI_BARRIERA;

// 16 pixel di distanza l'uno dall'altro.

        fuoco_x+=sin_table[dir];
        fuoco_y-=cos_table[dir];
    }
}

void UpdateCornerLine(void)
{
    int cornerline[34];
    register int i;
    register WORD vx,vy;
    WORD line_length;
    long a,b;

/* Sarebbe b=quota/8 e a=-b/32, io pero' ho bisogno di numeri grandi per mantenere
 * la precisione allora moltiplico tutto *256;
 */

    if(!pl->gioc_palla || !pl->gioc_palla->OnScreen)
        return;

    b=(pl->MaxQuota<<6);
    a=-(pl->MaxQuota<<1);

// 80 era velocita_palla[20]...

    pl->delta_y=-(cos_table[pl->dir]*(80))>>7;
    pl->delta_x=(sin_table[pl->dir]*(80))>>7;

    vx=pl->delta_x>>3;
    vy=pl->delta_y>>3;

    cornerline[0]=(pl->world_x>>3)-field_x+3;
    cornerline[1]=(pl->world_y>>3)-field_y+3;

    for(i=1;i<5;i++)
    {
        cornerline[i*2]=cornerline[0]+vx*i;
        
        cornerline[i*2+1]=cornerline[1]+vy*i-((a*i*i+b*i)>>7);

// Divido "solo" per 128 perche' devo avere quota *2

// Stabilisco dei limiti.

        if(    cornerline[i*2+1]<0    || 
            cornerline[i*2+1]>=WINDOW_HEIGHT ||
            cornerline[i*2]<0    ||
            cornerline[i*2]>=WINDOW_WIDTH
        )
        {
            break;
        }
    }

    line_length=i-1;

    freedraw(Pens[P_NERO],
        cornerline[0],cornerline[1],
        min(max(cornerline[0]+(pl->delta_x>>1),0),WINDOW_WIDTH-1),
        min(max(cornerline[1]+(pl->delta_y>>1),0),WINDOW_HEIGHT-1));

    freepolydraw(Pens[P_BIANCO],line_length,cornerline);

    if(pl->gioc_palla->FirePressed&&pl->gioc_palla->TimePress>5)
    {
// Barra di potenza, solo dopo il check del passaggio

        if(pl->gioc_palla->SNum==0)
        {
            rectfill(main_bitmap,WINDOW_WIDTH-36,WINDOW_HEIGHT-((CHAR_Y+2)*2),
                WINDOW_WIDTH-2,WINDOW_HEIGHT-(CHAR_Y+2),Pens[P_VERDE2],bitmap_width);

            rectfill(main_bitmap,
                WINDOW_WIDTH-37,WINDOW_HEIGHT-((CHAR_Y+2)*2+1),
                WINDOW_WIDTH-3,WINDOW_HEIGHT-(CHAR_Y+3),
                p->team[0]->MarkerRed==Pens[RADAR_TEAM_B] ? Pens[P_ROSSO0] : Pens[P_BLU1],
                bitmap_width);

            rectfill(main_bitmap,
                WINDOW_WIDTH-36,WINDOW_HEIGHT-((CHAR_Y+2)*2),
                WINDOW_WIDTH-36+(pl->gioc_palla->TimePress),
                WINDOW_HEIGHT-(CHAR_Y+4),
                p->team[0]->MarkerRed==Pens[RADAR_TEAM_B] ? Pens[P_ROSSO1] : Pens[P_BLU2],
                bitmap_width);
        }
        else
        {
            rectfill(main_bitmap,
                2,WINDOW_HEIGHT-((CHAR_Y+2)*2),
                36,WINDOW_HEIGHT-(CHAR_Y+2),Pens[P_VERDE2],bitmap_width);

            rectfill(main_bitmap,
                1,WINDOW_HEIGHT-((CHAR_Y+2)*2+1),
                35,WINDOW_HEIGHT-(CHAR_Y+3),
                p->team[1]->MarkerRed==Pens[RADAR_TEAM_B] ? Pens[P_ROSSO0] : Pens[P_BLU1],
                bitmap_width);

            rectfill(main_bitmap,
                2,WINDOW_HEIGHT-((CHAR_Y+2)*2),
                2+(pl->gioc_palla->TimePress),
                WINDOW_HEIGHT-(CHAR_Y+4),
                p->team[1]->MarkerRed==Pens[RADAR_TEAM_B] ? Pens[P_ROSSO1] : Pens[P_BLU2],
                bitmap_width);
        }
    }
}

void DoSpecials(player_t *g)
{
// Zona speciale della palla

        if( InAreaPalla(g) && pl->quota<2 )
        {

                    g->TimePress=0;
                    g->FirePressed=FALSE;
                    g->ActualSpeed=0;

                    DoSpecialAnim(g,GIOCATORE_SCIVOLATA);

// Ora le scivolate sono calibrate anche secondo la velocita'

// Non usava G2P ma un +128 su g->world_x

                    if(!pl->gioc_palla)
                            g->SpecialData=FindDirection32(g->world_x,g->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
                    else
                    {
                            register player_t *g2=pl->gioc_palla;

                            if(g2->ActualSpeed>0)
                            {
                                    g->SpecialData=FindDirection32(g->world_x,g->world_y+128,
                                            pl->world_x+(velocita_x[g2->ActualSpeed-1][g2->speed][g2->dir]<<3),
                                            pl->world_y+(velocita_y[g2->ActualSpeed-1][g2->speed][g2->dir]<<3) );
                            }
                            else
                                    g->SpecialData=FindDirection32(g->world_x,g->world_y+128,pl->world_x,pl->world_y);

                    }
                    g->dir=g->SpecialData>>2;

        }
    else if ( pl->quota>3 && InAreaGrandePalla(g) )
    {
            WORD dest_x,dest_y,distance;
            WORD PortaDir=FindDirection(g->world_x,g->world_y,G2P_X( ( g->SNum ? PORTA_E_X : PORTA_O_X) ),G2P_Y(PORTA_O_Y));

// modifica necessaria * i 50fps!

            dest_x=G2P_X(pl->world_x+pl->delta_x*12 );
            dest_y=G2P_Y(pl->world_y+pl->delta_y*12 );

            g->SpecialData=FindDirection256(g->world_x,g->world_y,dest_x,dest_y);

            distance=FindDistance(g->world_x,g->world_y,dest_x,dest_y,g->SpecialData);

// Adesso si fanno in 12 passi, non piu' in 6

            g->CA[0]=min(distance/12,20);

            g->dir=g->SpecialData>>5;
            g->ActualSpeed=0;

            if(pl->quota<8)
            {
                if(CambioDirezione[g->dir][PortaDir]<2 &&
                    CambioDirezione[g->dir][PortaDir]>-2)
                {
                    DoSpecialAnim(g,GIOCATORE_PRETUFFO);
                    g->CA[0]=max(g->CA[0],3);
                }
                else
                {
                    DoSpecialAnim(g,GIOCATORE_PREGIRATA);
                }
            }
            else
            {
                if(CambioDirezione[g->dir][PortaDir]!=4)
                {
                    DoSpecialAnim(g,GIOCATORE_STACCO);
                }
                else
                {
                    DoSpecialAnim(g,GIOCATORE_PREGIRATA);
                }
            }
    }
    else if(g->team->Joystick>=0)
    {
        if(pl->quota>2)
        {
            DoSpecialAnim(g,GIOCATORE_NONPOSSO);
            g->ActualSpeed=0;
        }
    }
}


/* Velocita' e direzione li determiniamo fuori, variano perche' se controllate
 * dal giocatore o dal computer.
 */

void Tira(player_t *g)
{
        pl->Rimbalzi=0;

// Per rimesse e tiri al volo la palla non parte da quota 0!

    if(pl->TipoTiro<TIRO_RIMESSA)
        pl->Stage=0;


    if(pl->TipoTiro<TIRO_CORNER)
    {
            pl->MaxQuota=(pl->velocita>>2);
        
        if(pl->TipoTiro==TIRO_ALTO)
            pl->MaxQuota+=2+GetTable();

        if(pl->TipoTiro==TIRO_PALLONETTO)
        {
            pl->MaxQuota+=10+GetTable();

            if(CanScore(g)==CS_SI)
                pl->velocita=pl->velocita*5/7;
        }

//        g->SpecialData=-1;
    }

    if(pl->MaxQuota>=MAX_QUOTA)
        pl->MaxQuota=MAX_QUOTA-1;

    if(g->ArcadeEffect==ARCADE_CANNON)
    {
        pl->velocita+=8;
        pl->MaxQuota=5+GetTable();

        PlayBackSound(sound[AS_CANNON]);
    }
        else PlayBackSound(sound[SHOT]);

    UpdateShotHeight();
    UpdateBallSpeed();

        g->ActualSpeed=0;

// Questi servono solo per i giocatori controllati, ma non fanno male...
        g->FirePressed=FALSE;
        g->TimePress=0;

        DoSpecialAnim(g,GIOCATORE_TIRO);

    if(g->ArcadeEffect)    
        RemoveArcadeEffect(g,g->ArcadeEffect);
}

void PreparaPassaggio(player_t *g,player_t *g_min)
{
    register WORD distance;
    register WORD ExpectedX=g_min->world_x,ExpectedY=g_min->world_y;


    if(g_min->ActualSpeed>0)
    {
        if(pl->TipoTiro==TIRO_PASSAGGIO_SMARCANTE)
        {
        ExpectedX+=(velocita_x[g_min->ActualSpeed-1][g_min->speed][g_min->dir]<<5);
        ExpectedY+=(velocita_y[g_min->ActualSpeed-1][g_min->speed][g_min->dir]<<5);
        }
        else
        {
        ExpectedX+=(velocita_x[g_min->ActualSpeed-1][g_min->speed][g_min->dir]<<2);
        ExpectedY+=(velocita_y[g_min->ActualSpeed-1][g_min->speed][g_min->dir]<<2);
        }
    }
        pl->TipoTiro=TIRO_RASOTERRA;

        pl->dir=FindDirection256(g->world_x,g->world_y,ExpectedX,ExpectedY);
    distance=FindDistance(g->world_x,g->world_y,ExpectedX,ExpectedY,pl->dir);

    g->SpecialData=g_min->GNum;

    distance=min(distance,2200); // Limito la lunghezza massima del passaggio corto.

// La velocita' e' distance /128 (32 per i passi per toccare terra e 4 per gli scatti della velocita' della palla)

    pl->velocita=distance>>7;

    if(distance<700)
        pl->velocita+=2;

// Maxxo la velocita' a 32.

    if(pl->velocita>32)
        pl->velocita=32;

        if(distance>1500)
    {
        pl->TipoTiro=TIRO_ALTO;
/*
        g_min->dir=FindDirection(g_min->world_x,g_min->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
        DoSpecialAnim(g_min,GIOCATORE_RESPIRA);
*/
    }

        if(g_min->SpecialData>3000)
    {
        pl->TipoTiro=TIRO_PALLONETTO;
/*
        g_min->dir=FindDirection(g_min->world_x,g_min->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
        DoSpecialAnim(g_min,GIOCATORE_RESPIRA);
*/
    }

// Se non c'e' il joystick fermo il giocatore che riceve il passaggio.

    if(g->team->Joystick<0)
    {
        g_min->dir=FindDirection(g_min->world_x,g_min->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
        g_min->ActualSpeed=0;
        g_min->WaitForControl=24;
        DoAnim(g_min,GIOCATORE_RESPIRA);
    }

    if(pl->TipoTiro==TIRO_RASOTERRA)
        pl->velocita+=3;

    g_min->SpecialData=-1;
}

void Passaggio(player_t *g)
{
    player_t *g_min;
    register WORD Dir=g->dir,pos=0;

    while(pos < 5)  {
        if ((g_min=FindNearestDirPlayer(g))) {
            PreparaPassaggio(g,g_min);
            break;
        }

        pos=-pos;

        if(pos>=0)
            pos++;

        g->dir=Dir+pos;

        if(g->dir<0)
            g->dir+=8;
        else if(g->dir>7)
            g->dir-=8;
    }

    if(pos == 5) {
        D(bug("Non dovrebbe succedere! Pos==5, fallito passaggio2!\n"));

        pl->velocita=10;
        g->dir=Dir;
                pl->dir=g->dir<<5;
        g->SpecialData=-1;
    }
}

void PassaggioB(player_t *g)
{
        pl->Rimbalzi=0;
    pl->Stage=0;

        pl->MaxQuota=(pl->velocita>>2);
        
    if(pl->TipoTiro==TIRO_ALTO)
        pl->MaxQuota+=7;

    if(pl->TipoTiro==TIRO_PALLONETTO)
        pl->MaxQuota+=15;

    UpdateShotHeight();
    UpdateBallSpeed();

        g->ActualSpeed=0;

// Questi servono solo per i giocatori controllati, ma non fanno male...
        g->FirePressed=FALSE;
        g->TimePress=0;

        DoSpecialAnim(g,GIOCATORE_PASSAGGIO);

        TogliPalla();

        if(g->SpecialData>-1&&g->SpecialData<10)
        {
           ChangeControlled(g->team,g->SpecialData);
           g->SpecialData=-1;
       g->WaitForControl=40;
        }

        PlayBackSound(sound[PASS]);
}

void Passaggio2(player_t *g,char Dir)
{
    WORD olddir=g->dir;

    g->dir=Dir;

    Passaggio(g);

    if(g->dir!=olddir)
    {
        BYTE NewDir=g->dir;
        
        g->dir=olddir;

        SetComando(g,ESEGUI_ROTAZIONE,ESEGUI_PASSAGGIO,NewDir);
    }
    else
        PassaggioB(g);
}

void RimessaLaterale(player_t *g)
{
/* Uso questo invece di pl->InGioco per evitare che la palla
   esca prima di entrare!
 */

// Serve per evitare che durante la rimessa venga mostrato il pannello

    p->team[0]->ArcadeCounter=0;
    p->team[1]->ArcadeCounter=0;

    if(((pl->dir+1)>>1)==32)
    {
        if(pl->world_y<CENTROCAMPO_Y)
            pl->dir+=4;
        else
            pl->dir-=4;
    }
    else if( ((pl->dir+1)>>1)==96)
    {
        if(pl->world_y>CENTROCAMPO_Y)
            pl->dir+=4;
        else
            pl->dir-=4;
    }

    p->referee.Comando=METTI_IN_GIOCO;
    p->referee.Tick=8;

        pl->world_x=g->world_x+50;

    if(g->world_y<CENTROCAMPO_Y)
            pl->world_y=g->world_y+120;
    else
            pl->world_y=g->world_y+80;

    pl->velocita=g->SpecialData;

//    D(bug("%ld velocita' %ld direzione\n",pl->velocita,pl->dir));

    g->SpecialData=-g->TimePress;
    g->TimePress=0;

    ShowBall();

    pl->Rimbalzi=0;
    pl->MaxQuota=12;
    pl->Stage=0;

    UpdateShotHeight();
    UpdateBallSpeed();

    while(p->shotheight[pl->Stage]<4)
        pl->Stage++;

        pl->TipoTiro=TIRO_RIMESSA;
        g->AnimType=GIOCATORE_DOPORIMESSA;

    RestartTime();

    if(g->ArcadeEffect)
        RemoveArcadeEffect(g,g->ArcadeEffect);

// Serve per evitare che durante la rimessa venga mostrato il pannello

    p->team[0]->ArcadeCounter=0;
    p->team[1]->ArcadeCounter=0;

}

void HandleRimessa(player_t *g)
{
    if(p->show_panel&0xff00)
        return;

    if(g->team->Joystick>=0)
    {
        uint32_t l;

        g->WaitForControl--;

        if(g->WaitForControl<0)
            goto rimessacomputer;

        l=r_controls[g->team->Joystick][counter];

        if(!g->Controlled)
            ChangeControlled(g->team,g->GNum);

        if(l&MYBUTTONMASK)
        {
            p->team[0]->ArcadeCounter=0;
            p->team[1]->ArcadeCounter=0;

            g->FirePressed=TRUE;
            g->TimePress++;

            if(g->TimePress>20)
            {
                TogliPalla();
                DaiPalla(g);
                g->FirePressed=FALSE;
                Passaggio(g);
                g->TimePress=-g->SpecialData;
                g->SpecialData=15;
                DoSpecialAnim(g,GIOCATORE_RIMESSA);
            }
            return;
        }
        else if (g->FirePressed)
        {
            WORD temp;

            p->team[0]->ArcadeCounter=0;
            p->team[1]->ArcadeCounter=0;

            TogliPalla();
            DaiPalla(g);
            g->FirePressed=FALSE;
            Passaggio(g);
            temp=-g->SpecialData;
            g->SpecialData=5+(g->TimePress>>1);
            g->TimePress=temp;
            DoSpecialAnim(g,GIOCATORE_RIMESSA);
            return;
        }
        else g->TimePress=0;

        if(!(l&JP_DIRECTION_MASK))
        {
            if(g->world_y<CENTROCAMPO_Y)
                g->dir=D_SUD;
            else g->dir=D_NORD;

            return;
        }

        g->dir=GetJoyDirection(l);

        if(g->world_y<CENTROCAMPO_Y)
        {
            switch(g->dir)
            {
                case D_NORD:
                    g->dir=D_SUD;
                    break;
                case D_NORD_EST:
                    g->dir=D_SUD_EST;
                    break;
                case D_NORD_OVEST:
                    g->dir=D_SUD_OVEST;
                    break;
            }
        }
        else
        {
            switch(g->dir)
            {
                case D_SUD:
                    g->dir=D_NORD;
                    break;
                case D_SUD_EST:
                    g->dir=D_NORD_EST;
                    break;
                case D_SUD_OVEST:
                    g->dir=D_NORD_OVEST;
                    break;
            }
        }
    }
    else
    {
        g->WaitForControl--;

        if(g->WaitForControl<0)
        {
rimessacomputer:
            TogliPalla();
            DaiPalla(g);
            g->FirePressed=FALSE;
            Passaggio(g);
            g->TimePress=-g->SpecialData;
            g->SpecialData=4+g->creativity+GetTable();
            DoSpecialAnim(g,GIOCATORE_RIMESSA);
        }

/*
        else
        {
            if(g->world_y>CENTROCAMPO_Y)
            {
                g->dir=0;
            }
            else
            {
                g->dir=4;
            }
        }
*/
    }
}

void Rigore(player_t *g)
{
    if(!p->penalty_onscreen)
    {
        if(p->penalty_counter<80)
        {
            p->penalty_counter++;
            return;
        }

        PlayBackSound(sound[FISCHIO_CORTO]);

        g->FirePressed=FALSE;
        g->TimePress=0;

        if(g->team->Joystick<0)
            g->WaitForControl=(GetTable()+1)<<4;

        StopTime();

        p->marker_y=(RIGORE_Y/8);
        p->adder=9-(g->Tiro>>1);

        AddAnimObj(p->extras,0,0,g->SNum);
        p->penalty_onscreen=TRUE;

        p->penalty_counter=0;
/*
    Qui non dovrebbe piu' servire

        g->world_x=pl->world_x-avanzamento_x[g->dir];
        g->world_y=pl->world_y-avanzamento_y[g->dir];
*/
        if(g->SNum)
            DisponiPortiere(p->team[0],PENALTY,0);
        else
            DisponiPortiere(p->team[1],PENALTY,0);
    }

    p->marker_y+=p->adder;

    p->marker_x=(1530-p->marker_y)/13;

    if(g->SNum)
        p->marker_x=1270-p->marker_x;

    if(p->marker_y>(GOAL_Y_S/8)||
        p->marker_y<(GOAL_Y_N/8) )
        p->adder=-p->adder;

    MoveAnimObj(p->extras,p->marker_x-field_x,p->marker_y-field_y);

    if(g->team->Joystick>=0)
    {
        uint32_t l;

        g->WaitForControl--;

        l=r_controls[g->team->Joystick][counter];

        if(!g->Controlled)
            ChangeControlled(g->team,g->GNum);


                if(l&MYBUTTONMASK)
                {
                        g->FirePressed=TRUE;
                        g->TimePress++;

                        if(g->TimePress>16)
                goto tirarigore;
        }
        else if (g->FirePressed)
        {
tirarigore:
                        g->FirePressed=FALSE;
                        pl->MaxQuota=g->TimePress;
            pl->velocita=12+g->Tiro+(g->TimePress>>1);

            goto finetiro;
        }
    }
    else
    {
        g->WaitForControl--;

        if(!g->Controlled)
            ChangeControlled(g->team,g->GNum);

        if(g->WaitForControl<0)
        {
            UBYTE tt=GetTable(); 

// Occhio questa parentesi potrebbe causare dei bei casini!

            if ( (abs(p->marker_y-(pl->world_y>>3)-((tt-3)<<1)))<(18-g->Tiro) ) 
            {
// rigorecomputer: come sopra, non viene usato...
                        pl->MaxQuota=(GetTable()+1)<<1;

            if(pl->MaxQuota>8)
                pl->MaxQuota-=(g->Tiro>>1);

            pl->velocita=12+g->Tiro+GetTable();

//            pl->dir=FindDirection256(pl->world_x+24,pl->world_y+24,(p->marker_x+9)<<3,(p->marker_y+4)<<3);

finetiro:
            pl->dir=FindDirection256(pl->world_x+24,
                pl->world_y+24,((p->marker_x+9)<<3)+(g->SNum ? (200) : (-200) ) ,
                (p->marker_y+4)<<3);

            D(bug("direction rigore: %ld (wx: %ld, mx: %ld)\n",pl->dir,pl->world_x,p->marker_x));

            TogliPalla();
            DaiPalla(g);

            g->SpecialData=-1;
            pl->TipoTiro=TIRO_CORNER;

                        Tira(g);

            if(GetTable()<4)
            {
                keeper_t *po=&p->team[g->SNum^1]->keepers;

                DoSpecialAnim(po,PORTIERE_PRERIGORE);
            }

            pl->InGioco=TRUE;
                        RemAnimObj(p->extras);
            p->penalty_onscreen=FALSE;
            p->penalty_counter=0;
            RimuoviComandoSquadra( g->SNum^1 ,MANTIENI_DISTANZA);
            p->mantieni_distanza=FALSE;

            RestartTime();

            if(penalties)
            {
                p->referee.Comando=FISCHIA_PREPUNIZIONE;
                p->referee.Tick=90;
            }
           }
        }
    }
}

void PunizioneCorner(player_t *g)
{
    if(p->show_panel&0xff00)
        return;

    if(pl->settore==PENALTY)
    {
        Rigore(g);
    }
    else if(g->team->Joystick>=0)
    {
        uint32_t l;

        l=r_controls[g->team->Joystick][counter];

        g->WaitForControl--;

/*
    Non va!

        if(g->WaitForControl<0&&!free_kicks)
            goto punizionecomputer;
*/

                if(!g->Controlled)
                        ChangeControlled(g->team,g->GNum);

        if(pl->gioc_palla!=g)
        {
            TogliPalla();
            DaiPalla(g);
        }

                if(l&MYBUTTONMASK)
                {
            g->team->ArcadeCounter=0;

                        g->FirePressed=TRUE;
                        g->TimePress++;

                        if(g->TimePress>32)
                goto tirapunizione;
                }
                else if (g->FirePressed)
                {
tirapunizione:
                        g->FirePressed=FALSE;
            g->team->ArcadeCounter=0;

                        pl->velocita=(g->TimePress>>1)+3;

            TogliPalla();
            DaiPalla(g);

            g->SpecialData=-1;

            if(g->TimePress>6)
                pl->TipoTiro=TIRO_CORNER;
            else {
// Barra di potenza, solo dopo il check del passaggio

                if(l&JP_DIRECTION_MASK)
                    g->dir=GetJoyDirection(l);

                Passaggio(g);
            }

            Tira(g);
            RimuoviComandoSquadra( g->SNum^1 ,STAI_BARRIERA);
            RimuoviComandoSquadra( g->SNum^1 ,MANTIENI_DISTANZA);

            
            {
                player_t *g2;
                WORD xs=pl->world_x+(pl->delta_x<<7);
                WORD ys=pl->world_y+(pl->delta_y<<7);

                if((g2=FindNearest(g->team,xs,ys))) {
                    g->SpecialData=g2->GNum;
                }
            }

            RestartTime();

            pl->InGioco=TRUE;
            p->mantieni_distanza=FALSE;
            p->doing_shot=FALSE;

            if(free_kicks) {
                p->referee.Comando=FISCHIA_PREPUNIZIONE;
                p->referee.Tick=150;
            }
            return;
        }

        if(l&JPF_JOY_UP) {
            if(pl->MaxQuota<(MAX_QUOTA-1))
                pl->MaxQuota++;

        }
        else if(l&JPF_JOY_DOWN)    {
            if(pl->MaxQuota>0)
                pl->MaxQuota--;

        }

        if(l&JPF_JOY_LEFT) {
            if(g->SpecialData>0)
                g->SpecialData=-1;
            else if(g->SpecialData>-16)
                g->SpecialData--;

            pl->dir+=g->SpecialData;
        }
        else if(l&JPF_JOY_RIGHT) {
            if(g->SpecialData<0)
                g->SpecialData=1;
            else if(g->SpecialData<16)
                g->SpecialData++;

            pl->dir+=g->SpecialData;
        }
        else
            g->SpecialData=1;

        switch(pl->world_x)    {
            case CORNER_X_NO:
                if(pl->dir<64) {
                    pl->dir=64;
                }
                else if(pl->dir>135) {
                    pl->dir=136;
                }
                break;
            case CORNER_X_NE:
                if(pl->dir<120) {
                    pl->dir=120;
                }
                else if(pl->dir>192) {
                    pl->dir=192;
                }
                break;
            case CORNER_X_SO:
                if(pl->dir<8)    {
                    pl->dir=8;
                }
                else if(pl->dir>64) {
                    pl->dir=64;
                }
                break;

            case CORNER_X_SE:
                if(pl->dir<192) {
                    pl->dir=192;
                }
                else if(pl->dir>248) {
                    pl->dir=248;
                }
                break;

// Punizioni:
            default:
// Non dovrebbe piu' servire...
/*
                if(pl->dir>31)
                {
                    pl->dir=0;
                }
                else if(pl->dir<0)
                {
                    pl->dir=31;
                }
*/
                break;
        }
        
        g->dir=((pl->dir+4)>>5);

        if(g->dir>7)
            g->dir-=8;

        g->world_x=pl->world_x-avanzamento_x[g->dir];
        g->world_y=pl->world_y-avanzamento_y[g->dir];

        if(nosync)
            UpdateCornerLine();
        else
            p->doing_shot=TRUE;
    }
    else
    {
        g->WaitForControl--;
        p->doing_shot=FALSE;

        if(g->WaitForControl<0)
        {
            UBYTE c;

// punizionecomputer: non usata per ora
            TogliPalla();
            DaiPalla(g);

            c=CanScore(g);

            if(c==CS_NO&&g!=g->team->attivo)
            {
                        g->dir=FindDirection(g->world_x,g->world_y,g->team->attivo->world_x,g->team->attivo->world_y);

                if(!g->Controlled)
                    ChangeControlled(g->team,g->GNum);

                        Passaggio2(g,g->dir);
            }
            else
            {
                if(!g->Controlled)
                    ChangeControlled(g->team,g->GNum);

                if(c==CS_SI)
                {
/*
                    WORD dest_x;

                    if(g->SNum==0)
                        dest_x=FONDO_O_X;
                    else
                        dest_x=FONDO_E_X;
*/
                    CPUShot(g);
                }
                else
                {
                    WORD dest_x;

                    if(g->SNum==0)
                        dest_x=RIGORE_X_O;
                    else
                        dest_x=RIGORE_X_E;

                    pl->dir=FindDirection256(g->world_x,g->world_y,dest_x+((GetTable()-3)<<4),CENTROCAMPO_Y);

                    pl->MaxQuota=GetTable()+g->creativity+6;

                    pl->velocita=18-GetTable();

                    pl->TipoTiro=TIRO_CORNER;
                    Tira(g);
                }
            }

            g->dir=pl->dir>>5;
            RimuoviComandoSquadra( g->SNum^1 ,STAI_BARRIERA);
            RimuoviComandoSquadra( g->SNum^1 ,MANTIENI_DISTANZA);
            p->mantieni_distanza=FALSE;
            p->doing_shot=FALSE;

            if(free_kicks)
            {
                p->referee.Comando=FISCHIA_PREPUNIZIONE;
                p->referee.Tick=100;
            }

            pl->InGioco=TRUE;

            RestartTime();
        }
        else
        {
            g->world_x=pl->world_x-avanzamento_x[g->dir];
            g->world_y=pl->world_y-avanzamento_y[g->dir];
        }
    }
}

void ColpoDiTesta(player_t *g)
{
    if(pl->gioc_palla||!pl->InGioco)
        return;

    if(CanScore(g)==CS_SI)
    {
        WORD PortaDir, temp;
        WORD Dir = g->dir;
        uint32_t state;

        PortaDir=FindDirection(g->world_x,g->world_y,G2P_X( ( g->SNum ? PORTA_E_X : PORTA_O_X) ),G2P_Y(PORTA_O_Y));

        if(g->team->Joystick>=0)
        {
            state=r_controls[g->team->Joystick][counter];
        }
        else
        {
            switch(GetTable()>>1)
            {
                case 1:
                    state=JPF_JOY_UP;
                    break;
                case 2:
                    state=JPF_JOY_DOWN;
                    break;
                default:
                    state=0L;
            }
        }

        temp=abs(CambioDirezione[pl->dir>>5][PortaDir]);
    
        IndirizzaTiro(g,state);

        if(temp>3)
            pl->velocita+=3;
        else if(temp>1)
            pl->velocita+=2;

        pl->Stage=0;
        pl->Rimbalzi=0;

        pl->MaxQuota=pl->quota+((GetTable()+1)>>1)+1;

        UpdateShotHeight();
        UpdateBallSpeed();

        temp=abs(CambioDirezione[Dir][PortaDir]);

        while(p->shotheight[pl->Stage]<pl->quota)
            pl->Stage++;

        if(temp<2)
        {
            if(g->AnimType==GIOCATORE_PRETUFFO)
                g->AnimType=GIOCATORE_TUFFO_TESTA;
            else
                g->AnimType=GIOCATORE_TESTA_AVANTI;
        }
        else if(temp<4)
        {
            if(g->AnimType!=GIOCATORE_PREGIRATA)
                g->AnimType= ( CambioDirezione[Dir][PortaDir]>0 ? GIOCATORE_TESTA_SINISTRA : GIOCATORE_TESTA_DESTRA);
            else
                g->AnimType= ( CambioDirezione[Dir][PortaDir]>0 ? GIOCATORE_GIRATA_DESTRA : GIOCATORE_GIRATA_SINISTRA);
        }
        else
            g->AnimType=GIOCATORE_ROVESCIATA;

        if(g->AnimType!=GIOCATORE_ROVESCIATA &&
            g->AnimType!=GIOCATORE_GIRATA_DESTRA &&
            g->AnimType!=GIOCATORE_GIRATA_SINISTRA )
        {
// Schiacciata di testa!
            if(GetTable()>2)
            {
                while(p->shotheight[pl->Stage]<=p->shotheight[pl->Stage+1])
                    pl->Stage++;

                while(p->shotheight[pl->Stage]>pl->quota)
                    pl->Stage++;
            }
        }

            PlayBackSound(sound[PASS]);
    }
    else
    {
        WORD olddir=g->dir,temp;
// passaggio
        TogliPalla()
        DaiPalla(g);

        Passaggio(g);

        pl->MaxQuota=5+(GetTable()<<1);
        pl->MaxQuota=max(pl->MaxQuota,pl->quota+1);

        pl->Stage=0;
        pl->TipoTiro=TIRO_CORNER;
        pl->Rimbalzi=0;

        TogliPalla();

        PlayBackSound(sound[PASS]);

        temp=CambioDirezione[olddir][g->dir];

        if(temp>3)
        {
            g->dir=olddir;

            if(g->AnimType==GIOCATORE_PREGIRATA)
                g->AnimType=GIOCATORE_ROVESCIATA;
            else if(g->AnimType==GIOCATORE_PRETUFFO)
            {
                g->AnimType=GIOCATORE_TUFFO_TESTA;
                pl->MaxQuota=10+GetTable();
                pl->velocita=10;
                pl->dir=(g->dir<<5)+(GetTable()<<1)-5;
            }
            else 
            {
                pl->velocita=6;
                g->AnimType=GIOCATORE_TESTA_DESTRA;
                pl->MaxQuota=10+GetTable();
            }
        }
        else if(temp>1)
        {
            g->dir-=2;

            if(g->AnimType==GIOCATORE_PRETUFFO)
            {
                g->dir=olddir-1;

                g->AnimType=GIOCATORE_TUFFO_TESTA;
                pl->velocita-=3;
                pl->dir=(g->dir<<5)+(GetTable()<<1)-5;
            }                
            else if(g->AnimType!=GIOCATORE_PREGIRATA)
                g->AnimType=GIOCATORE_TESTA_SINISTRA;
            else
                g->AnimType=GIOCATORE_GIRATA_DESTRA;

            if(g->dir<0)
                g->dir+=8;
        }
        else if(temp<-1)
        {
            g->dir+=2;

            if(g->AnimType==GIOCATORE_PRETUFFO)
            {
                g->dir=olddir+1;

                g->AnimType=GIOCATORE_TUFFO_TESTA;
                pl->velocita-=3;
                pl->dir=(g->dir<<5)+(GetTable()<<1)-5;
            }                                
            else if(g->AnimType!=GIOCATORE_PREGIRATA)
                g->AnimType=GIOCATORE_TESTA_DESTRA;
            else
                g->AnimType=GIOCATORE_GIRATA_SINISTRA;

            if(g->dir>7)
                g->dir-=8;
        }
        else
        {
            if (g->AnimType==GIOCATORE_PREGIRATA)
            {
                g->AnimType=GIOCATORE_ALZATI;
                pl->velocita-=1;
            }
            else if(g->AnimType!=GIOCATORE_PRETUFFO)
                g->AnimType=GIOCATORE_TESTA_AVANTI;
            else
                g->AnimType=GIOCATORE_TUFFO_TESTA;
        }

        UpdateBallSpeed();
        UpdateShotHeight();

        while(p->shotheight[pl->Stage]<(pl->quota-1))
            pl->Stage++;

        pl->quota=p->shotheight[pl->Stage];

        if(pl->quota>10)
            pl->quota=10;
    }

    g->FrameLen=Animation[g->AnimType].FrameLen;
}

void EseguiEsultanza(int tn)
{
    player_t *g=NULL;
    team_t *sq=p->team[tn];
    int s;

    for(s=9;s>-1;s--)
    {
        if(    sq->players[s].OnScreen && 
            sq->players[s].AnimType!=GIOCATORE_ESPULSO &&
            sq->players[s].AnimType<GIOCATORE_PRIMA_ESULTANZA )
            g=&(sq->players[s]);
    }

    if(!g)
    {
        D(bug("Non trovo un giocatore per l'esultanza!\n"));
        return;
    }

    s=GetTable();

    if(s>0)
    {
        s++;
    }
    else if(GetTable()>2)
    {
        s=1;
    }

    g->ActualSpeed=0;

    switch(s)
    {
        case 0:
            g->dir=D_SUD_EST;
            break;
        case 1:
        case 6:
            g->dir=D_SUD_OVEST;
            break;
    }

    s+=(GIOCATORE_PRIMA_ESULTANZA);

    DoSpecialAnim(g,s);

    g->AnimFrame=0;
}

BOOL IsOffside(player_t *g)
{
    player_t *g1=&g->team->players[g->SpecialData];

    if(g1->SNum)
    {
        if(g1->world_x>CENTROCAMPO_X&&g1->world_x>(g->world_x+150))
        {
            team_t *sa=p->team[0];
            int i;
            WORD xpos=g1->world_x+100;
            BOOL offs=TRUE;

            for(i=0;i<10;i++)
            {
                if(sa->players[i].world_x>xpos)
                {
                    offs=FALSE;
                    break;
                }
            }

            return offs;
        }
    }
    else
    {
        if(g1->world_x<CENTROCAMPO_X&&g1->world_x<(g->world_x-150))
        {
            team_t *sa=p->team[1];
            int i;
            WORD xpos=g1->world_x-100;
            BOOL offs=TRUE;

            for(i=0;i<10;i++)
            {
                if(sa->players[i].world_x<xpos)
                {
                    offs=FALSE;
                    break;
                }
            }

            return offs;
        }
    }

    return FALSE;    
}
