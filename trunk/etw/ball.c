#include "eat.h"

uint8_t tipo_porta;

void RimbalzoCasuale(void)
{
    pl->dir-=80;

    pl->dir+=(GetTable()<<5);

    UpdateBallSpeed();
}

void RimbalzoVerticale(void)
{
    if(pl->dir!=0)
    {
        pl->dir=255-pl->dir;
        pl->dir++;
    }
    UpdateBallSpeed();
}

BOOL CheckBall(player_t *plr)
{
    return pl->gioc_palla == plr;
}
void RimbalzoOrizzontale(void)
{
    pl->dir-=128;

//    D(bug("Chiamato Rimbalzo orizzontale!\n"));

    RimbalzoVerticale();
}

void SmorzaPalla(void)
{
    if(pl->velocita<16)
        pl->velocita>>=1;
    else
        pl->velocita>>=2;

    pl->velocita-=(GetTable()<<1);

    if(pl->velocita<=0)
    {
        FermaPalla();
    }
    else
        UpdateBallSpeed();
}

void GL_Fuori(int team)
{
    if(detail_level&USA_GUARDALINEE)
    {
        int k;

        for(k=0;k<2;k++)
        {
            DoSpecialAnim((&linesman[k]),(GL_FUORI_0-team));

// Setto la direzione in modo corretto!
            linesman[k].dir=2+k*4;
        }
    }                        
}

void HandleGol(void)
{
    if(pl->world_y<=GOAL_Y_N || pl->world_y>=GOAL_Y_S )
    {
// Palo!!!
        if(!replay_mode)
            UrgentSpeaker(S_PALO_COLPITO);

        D(bug("Pole!\n"));

        if(pl->Stage<(SHOT_LENGTH/2))
            pl->Stage=(SHOT_LENGTH-1)-pl->Stage;

        if(pl->world_y<=GOAL_Y_N)
        {
            pl->world_y=(GOAL_Y_N-8);

            if(pl->world_x<CENTROCAMPO_X)
                pl->world_x=(37*8);
            else
                pl->world_x=(1230*8);
        }
        else
        {
            pl->world_y=(GOAL_Y_S+8);

            if(pl->world_x<CENTROCAMPO_X)
                pl->world_x=(44*8);
            else
                pl->world_x=(1238*8);
        }
        pl->velocita>>=1;

        PlayIfNotPlaying(PALO);
        RimbalzoCasuale();
    }
    else if(pl->quota<8)
    {
        p->referee.Comando=FISCHIA_GOL;
        p->referee.Tick=0;
        pl->InGioco=FALSE;
        D(bug("Goal!!!\n"));
        p->goal=TRUE;

        if(arcade)
            HideBall();

        TogliPallaMod();
    }
    else if(pl->quota<9)
    {
// Traversa con rimbalzo basso
        if(pl->Stage<(SHOT_LENGTH/2))
            pl->Stage=(SHOT_LENGTH-2)-pl->Stage;

// Puo' anche andare in gol!

        pl->velocita>>=1;

        PlayIfNotPlaying(PALO);

        if(GetTable()>1)
        {
            RimbalzoVerticale();

            if(pl->world_x<CENTROCAMPO_X)
                pl->world_x=((640-(pl->world_y>>3))<<2)/5;
            else
                pl->world_x=((12141+(pl->world_y>>3))/5)<<2;
                
            if(!replay_mode&&urgent_status!=S_TRAVERSA_COLPITA)
                UrgentSpeaker(S_TRAVERSA_COLPITA);
        }
        else
        {
            while(p->shotheight[pl->Stage]>7)
                pl->Stage++;
        }
    }
    else
    {
// Traversa con rimbalzo alto
        if(pl->Stage>(SHOT_LENGTH/2))
            pl->Stage=(SHOT_LENGTH-1)-pl->Stage;

// Non torna sempre in campo
        pl->velocita>>=1;

        if(!replay_mode)
            UrgentSpeaker(S_TRAVERSA_COLPITA);

        PlayIfNotPlaying(PALO);

        if(arcade||GetTable()>2)
        {
            RimbalzoVerticale();

            if(pl->world_x<CENTROCAMPO_X)
                pl->world_x=((640-(pl->world_y>>3))<<2)/5;
            else
                pl->world_x=((12141+(pl->world_y>>3))/5)<<2;
        }
        else
        {
            int i;

            for(i=pl->Stage+1;i<(pl->Stage+4) && i<SHOT_LENGTH;i++)
            {
                p->shotheight[i]=11;
            }
            p->shotheight[pl->Stage+1]=10;
        }
    }

    if(pl->world_x>CENTROCAMPO_X)
        p->team[1]->Tiri++;
    else
        p->team[0]->Tiri++;

}

void HandleBall(void)
{
    if(pl->Hide)
        return;

    if(pl->gioc_palla)
    {
        pl->world_x=avanzamento_x[pl->gioc_palla->dir]+pl->gioc_palla->world_x;
        pl->world_y=avanzamento_y[pl->gioc_palla->dir]+pl->gioc_palla->world_y;

        if(pl->gioc_palla->ActualSpeed)
            pl->ActualFrame++;
    }
    else
    {
        if(pl->velocita)
        {
            pl->world_x+=pl->delta_x;
            pl->world_y+=pl->delta_y;

            pl->quota=p->shotheight[pl->Stage];
            pl->Stage++;

            if(pl->Stage>(SHOT_LENGTH-1))
            {
                pl->Stage=0;
                pl->Rimbalzi++;

                PlayBackSound(sound[PALLEGGIO]);

                if(pl->Rimbalzi>4)
                {
                    FermaPalla();
                }
                else
                {
                    switch(current_field)
                    {
                        case 1: // Dry
                            pl->MaxQuota*=2;
                            pl->MaxQuota/=3;
                            break;
                        case 2: // Hard
                            pl->MaxQuota*=4;
                            pl->MaxQuota/=5;
                            break;
                        case 3: // Soft
                            pl->MaxQuota*=3;
                            pl->MaxQuota/=5;
                            break;
                        case 4: // Frozen
                            pl->velocita+=2;
                            pl->MaxQuota/=2;
                            break;
                        case 5: // Muddy
                            pl->velocita-=2;
                            pl->MaxQuota/=3;    
                            break;
                        case 6: // Wet
                            pl->velocita+=1;
                            pl->MaxQuota/=3;    
                            break;
                        case 7: // Snow
                            pl->MaxQuota/=2;
                            break;
                        default: // Normal
                            pl->MaxQuota*=3;
                            pl->MaxQuota/=5;
                    }

                    pl->MaxQuota-=pl->Rimbalzi;

                    if(pl->MaxQuota<0)
                        pl->MaxQuota=0;

                    pl->velocita-=(pl->velocita>>2);

                    if(pl->velocita<0)
                    {
                        FermaPalla();
                    }
                    else
                        UpdateShotHeight();
                }
                UpdateBallSpeed();
            }

            if(pl->quota==0)
            {
                if(pl->TipoTiro!=TIRO_RASOTERRA&&pl->Rimbalzi>0)
                {
                    pl->velocita-=slowdown[current_field];

                    if(pl->velocita<=0)
                    {
                        FermaPalla();
                    }
                    else UpdateBallSpeed();
                }
                else
                {
// Dimezzo i rallentamenti della palla dopo il 1o rimbalzo (abbiamo il doppio di stage!

                    if(pl->Stage>0&&((pl->Stage>>5)<<5)==pl->Stage)
                    {
                        pl->velocita-=slowdown[current_field];

                        if(pl->velocita<=0)
                        {
                            FermaPalla();
                        }
                        else UpdateBallSpeed();
                    }
                }
            }
            pl->ActualFrame++;
        }
        else if (!pl->special)
        {
            pl->Stage=0;
            pl->quota=0;
        }
    }


    // was 10, still there from 25fps logic?
    if((p->check_sector % 20) == 0 && pl->InGioco)
        pl->sector = (pl->world_y/1450 << 2) + (pl->world_x/2560);

    // was checked every 10 loops, I'm changing it to 50 (one sec) to see if there is an improvement
    // we never change active player by hand in touch enviroment
    if ((p->check_sector % 50) == 0 && !use_touch)
        CheckActive();
    
    p->check_sector++;

    if(pl->ActualFrame>=4)
        pl->ActualFrame=0;

    if(
        pl->world_y<(46*8) ||
        pl->world_y>(501*8) ||
        pl->world_x<( 61*8 ) ||
        pl->world_x>( 1219 *8 ) 
     )
    {
        pezzi_porte[0]->world_x=-1000;
        pezzi_porte[1]->world_x=-1000;
        pezzi_porte[2]->world_x=-1000;
        pezzi_porte[3]->world_x=-1000;

        if(pl->InGioco)
        {
            p->sopra_rete=FALSE;

// la Sq 0 attacca SEMPRE verso sinistra

// Fuori dal lato sinistro
            if( (pl->world_y>>3)<(-( pl->world_x + (pl->world_x>>2) )+640) ) /* era 675, poi 650*/
            {
/* Inserire controllo del gol per la squadra 0 */

                p->team[0]->ArcadeCounter=0;
                p->team[1]->ArcadeCounter=0;

                if(    pl->world_y< (GOAL_Y_S+16) &&
                    pl->world_y> (GOAL_Y_N-16) &&
                    pl->quota<10)
                {
                    HandleGol();
                    return;
                }

                TogliPallaMod();

                if(arcade)
                {
                    RimbalzoVerticale();

                    pl->world_x=512-pl->world_y/10;

                    if(pl->velocita>6)
                    {
                        pl->velocita-=4;
                        UpdateBallSpeed();
                    }
                }
                else
                {
                    p->referee.Comando=FISCHIA_FUORI;
                    p->referee.Tick=0;

                    pl->InGioco=FALSE;

// Rinvio
                    if(p->team[0]->Possesso&&!training)
                    {
                        if(!replay_mode)
                            game_status=S_RIMESSA_DAL_FONDO;
                        pl->sector=GOALKICK;
                        pl->sq_palla=p->team[1];
                        p->team[0]->Possesso=0;
                        p->team[1]->Possesso=1;


                        GL_Fuori(1);
                    }
// Corner
                    else
                    {
                        if(!replay_mode)
                            game_status=S_CALCIO_D_ANGOLO;

                        if(pl->world_y>(270*8) )
                        {
                            pl->sector=CORNER_S;
                        }
                        else
                        {
                            pl->sector=CORNER_N;
                        }

                        pl->sq_palla=p->team[0];
                        p->team[0]->Possesso=1;
                        p->team[1]->Possesso=0;

                        GL_Fuori(0);
                    }
                }
            }

            /* Sono *10 scomposti... */

// Fuori dal lato destro

            else if ( (pl->world_y>>3)<( pl->world_x + (pl->world_x>>2) - 12141) ) /* era 12116 , poi 12131 */
            {
/* controllo del gol per la squadra 1 */
                p->team[0]->ArcadeCounter=0;
                p->team[1]->ArcadeCounter=0;

                if(    pl->world_y< (GOAL_Y_S+16)    &&
                    pl->world_y> (GOAL_Y_N-16)    &&
                    pl->quota<11    )
                {
                    HandleGol();
                    return;
                }

                TogliPallaMod();

                if(arcade)
                {
                    RimbalzoVerticale();

                    pl->world_x=pl->world_y/10+9712;

                    if(pl->velocita>6)
                    {
                        pl->velocita-=4;
                        UpdateBallSpeed();
                    }
                }
                else
                {
                    p->referee.Comando=FISCHIA_FUORI;
                    p->referee.Tick=0;
                    pl->InGioco=FALSE;
// Rinvio
                    if(p->team[1]->Possesso||training)
                    {
                        if(!replay_mode)
                            game_status=S_RIMESSA_DAL_FONDO;

                        pl->sector=GOALKICK;
                        pl->sq_palla=p->team[0];
                        p->team[1]->Possesso=0;
                        p->team[0]->Possesso=1;
                        GL_Fuori(0);

                    }
// Corner
                    else
                    {
                        if(!replay_mode)
                            game_status=S_CALCIO_D_ANGOLO;

                        if(pl->world_y>(270*8) )
                        {
                            pl->sector=CORNER_S;
                        }    
                        else
                        {
                            pl->sector=CORNER_N;
                        }

                        pl->sq_palla=p->team[1];
                        p->team[1]->Possesso=1;
                        p->team[0]->Possesso=0;
                        GL_Fuori(1);
                    }
                }
            }
// Laterale di sotto o di sopra
            else if (pl->world_y>(501*8)|| pl->world_y<(46*8))
            {
                p->team[0]->ArcadeCounter=0;
                p->team[1]->ArcadeCounter=0;

                if(!replay_mode)
                    game_status=S_RIMESSA_LATERALE;

                if(arcade)
                {
                    TogliPallaMod();

                    RimbalzoOrizzontale();

                    if(pl->world_y<(46*8))
                        pl->world_y=(46*8);

                    if(pl->velocita>6)
                    {
                        pl->velocita-=4;
                        UpdateBallSpeed();
                    }
                }
                else
                {
                    p->referee.Comando=FISCHIA_FUORI;
                    p->referee.Tick=0;
                    p->referee.Argomento=pl->world_x;
                    pl->InGioco=FALSE;

                    FermaPalla();
                    TogliPalla();

// Queste due prima erano un toglipallamod();

                    if(p->team[1]->Possesso||training)
                    {
                        pl->sq_palla=p->team[0];
                        p->team[1]->Possesso=0;
                        p->team[0]->Possesso=1;    
                        GL_Fuori(0);
                    }
                    else
                    {
                        pl->sq_palla=p->team[1];
                        p->team[1]->Possesso=1;
                        p->team[0]->Possesso=0;
                        GL_Fuori(1);
                    }
                }
            }
        }
        else /* Questi li controllo SOLO se la palla e' fuori... */
        {
            if(pl->world_y>(209*8)&&pl->world_y<((265+48-4)*8)&&pl->quota<10&&!arcade)
            {
                if(pl->world_x<(100*8))
                {
                    if(pl->world_y<(CENTROCAMPO_Y+80))
                        pezzi_porte[0]->world_x=((15+tipo_porta)<<3)+7;

                    if(pl->world_y>(CENTROCAMPO_Y-80))
                        pezzi_porte[1]->world_x=((7+tipo_porta)<<3)+7;
                }
                else if(pl->world_x>(1180*8))
                {
                    if(pl->world_y<(CENTROCAMPO_Y+80))
                        pezzi_porte[2]->world_x=(1236*8)+7;
                    if(pl->world_y>(CENTROCAMPO_Y-80))
                        pezzi_porte[3]->world_x=(1243*8)+7;
                }
            }

            if(p->goal)
            {
                if(pl->world_y>=GOAL_Y_S&& 
                    (pl->world_x<(35*8) || pl->world_x>(1239*8) ) )
                {
                    pl->world_y=GOAL_Y_S-1;
                    SmorzaPalla();
                    RimbalzoOrizzontale();
                }
                else if(pl->world_y<=GOAL_Y_N &&
                    (pl->world_x<(41*8) || pl->world_x>(1233*8) ) )
                {
                    pl->world_y=GOAL_Y_N+1;
                    SmorzaPalla();
                    RimbalzoOrizzontale();
                }

                if(pl->world_x<(21*8))
                {
                    pl->world_x=(21*8);
                    SmorzaPalla();
                    RimbalzoVerticale();
                }
                else if(pl->world_x>(1258*8))
                {
                    pl->world_x=(1258*8);
                    SmorzaPalla();
                    RimbalzoVerticale();
                }

                if(p->shotheight[pl->Stage+1]>8)
                    p->shotheight[pl->Stage+1]=8;
            }
            else
            {
// Rimbalzi sull'esterno della rete.
                if(pl->world_y>(GOAL_Y_N-16) && pl->world_y<=CENTROCAMPO_Y &&
                     ( pl->world_x<(41*8) || pl->world_x>(1233*8) ) ) 
                {
// Rete alta
                    if( (pl->dir<196&&pl->world_x<CENTROCAMPO_X) ||  
                          (pl->dir>64&&pl->world_x>CENTROCAMPO_X) 
                    )
                    {
                        if(pl->quota<10&&!p->sopra_rete)
                        {
                            pl->world_y=(GOAL_Y_N-16);
                            SmorzaPalla();
                            D(bug("Rebound on the goal\n"));
                            RimbalzoOrizzontale();
                        }
                        else
                        {
                            p->sopra_rete=TRUE;
                            D(bug("Ball over the goal!\n"));

                            if(pl->world_x>(12*8)&&pl->world_x<(1261*8)&&pl->quota<9)
                                pl->quota=9;                
                        }    
                    }

                }
                else if(pl->world_y<(GOAL_Y_S+16+(5*8)) && pl->world_y>CENTROCAMPO_Y &&
                    ( pl->world_x<(35*8) || pl->world_x>(1239*8) ) )
                {
// Rete bassa
                    if( (pl->dir>196&&pl->world_x<CENTROCAMPO_X) ||  
                          (pl->dir<64&&pl->world_x>CENTROCAMPO_X )
                    )
                    {
                        if(pl->quota<10&&!p->sopra_rete)
                        {
                            pl->world_y=GOAL_Y_S+16+(5*8);
                            D(bug("Rebound on the goal\n"));

                            SmorzaPalla();
                            RimbalzoOrizzontale();
                        }
                        else
                        {
                            p->sopra_rete=TRUE;
                            D(bug("Ball over the goal!\n"));

                            if(pl->world_x>(12*8)&&pl->world_x<(1261*8)&&pl->quota<9)
                                pl->quota=9;
                        }
                    }
                }
                else p->sopra_rete=FALSE;
            }

            if( pl->world_x>=(1270*8) || pl->world_x<=0 ||
                pl->world_y<=((SPONSOR_Y_POS+10)*8) || pl->world_y>=((544-pl->anim->Heights[(pl->quota<<2)])<<3) )
            {
                if(pl->world_x<(1270*8) && pl->world_x>0)
                {
                    D(bug("Rimbalzo sui cartelloni!\n"));
                    RimbalzoOrizzontale();
                }
                else
                {
#if 0

// Adesso la palla esce da fondocampo...

                    if(pl->world_y>(GOAL_Y_N-16) && pl->world_y<(GOAL_Y_S+16))
                    {
                        if(pl->world_x<0)
                            pl->world_x=0;
                        else if(pl->world_x>(1270*8))
                        {
                            pl->world_x=(1270*8);
                        }
                        
                    }
                    else RimbalzoVerticale();
#else
                    if(pl->world_x< (-(6*8)) || pl->world_x>(1279*8))
                    {
                        HideBall();
                        FermaPalla();

                        if(pl->world_x<0)
                            pl->world_x=-49; // Cosi' e' fuori dalla porta...
                        else
                            pl->world_x=10233;
                    }
#endif
                }

                pl->velocita-=(GetTable()<<1);

                if(pl->velocita<0)
                {
                    FermaPalla();
                }
                else
                    UpdateBallSpeed();
            }
        }
    }
}

void PostHandleBall(void)
{
    register int16_t x,y;
    register player_t *g;

    if(pl->InGioco) {
        p->team[0]->TempoPossesso+=p->team[0]->Possesso;
        p->team[1]->TempoPossesso+=p->team[1]->Possesso;
    }
  
    if(pl->Hide)
        return;

    x=(pl->world_x>>3)-field_x-quota_mod_x[pl->quota];
    y=(pl->world_y>>3)-field_y-quota_mod_y[pl->quota];

    if((g=pl->gioc_palla)!=NULL)
    {
        if(g->ActualSpeed==3&&g->AnimType==GIOCATORE_CORSA_VELOCE_PALLA)
        {    
// Nota il /2 vale solo finche non aggiorno ai 50 FPS gli avanzamenti!
            register int16_t xa=g->AnimFrame+g->AnimFrame+2-(g->FrameLen>>1);

            if(xa>11||xa<0)
            {
//                D(bug("Errore in av_palla %ld a %ld l %ld\n",xa,g->AnimFrame,g->FrameLen));
                xa=0;
            }

            x+=av_palla_x[g->dir][xa];
            y+=av_palla_y[g->dir][xa];
/*
            x+=av_palla_x[g->dir][g->AnimFrame];
            y+=av_palla_y[g->dir][g->AnimFrame];
*/
            if(g->AnimFrame==0&&g->FrameLen==0)
                PlayBackSound(sound[PALLEGGIO]);
        }

        if(g->AnimType==GIOCATORE_CAMBIO_ANTI)
        {
            x+=cambio_x[g->dir];
            y+=cambio_y[g->dir];
        }
        else if(g->AnimType==GIOCATORE_CAMBIO_ORARIO)
        {
            register int16_t fakedir=g->dir+1;

// Le direzioni antiorarie sono uguali a quelle orarie translate di 1 e
// ribaltate
            if(fakedir==8)
                fakedir=0;

            x-=cambio_x[fakedir];
            y-=cambio_y[fakedir];
        }
        else if(g->AnimType==GIOCATORE_INVERSIONE_MARCIA_PALLA)
        {
// Nota il /2 vale solo finche non aggiorno ai 50 FPS gli avanzamenti!

            register int16_t xa=(g->AnimFrame<<2)+3-(g->FrameLen>>1);

            if(xa<0||xa>7)
            {
                D(bug("Errore in inversione %ld\n",xa));
                xa=0;
            }

            x+=inversione_x[g->dir][xa];
            y+=inversione_y[g->dir][xa];

//            D(bug("Inversione: %ld\n",(g->AnimFrame<<2)+3-g->FrameLen));
        }
    }


    ChangeAnimObj(pl->anim,pl->ActualFrame+(pl->quota<<2));

    D(if(pl->anim->current_frame>=pl->anim->nframes){ bug("Errore sulla palla: AF: %ld quota: %ld\n",pl->ActualFrame,pl->quota); pl->anim->current_frame=pl->anim->nframes-1;} );


    if(x>=WINDOW_WIDTH)
        x=WINDOW_WIDTH-1;

    if(y>=WINDOW_HEIGHT)
        y=WINDOW_HEIGHT-1;

    if(x<(-5))
        x=0;

    if(y<(-5))
        y=0;

    MoveAnimObj(pl->anim,x,y);
}
