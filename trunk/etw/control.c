#include "eat.h"

void CheckInfortuni(player_t *g)
{
    if( (injuries&&GetTable()>3) || killer)
    {
        char t;

        if(!g)
            return;

        g->stamina--;

        if(GetTable()>g->stamina || killer)
        {
            g->stamina=0;

            TogliPalla();

            g->speed=2;
            t=GetTable()-2;
            g->tackle=max(1,t);
            g->Tiro=max(1,t);
            g->creativity=max(1,t);

            g->ActualSpeed=0;

            DoSpecialAnim(g,GIOCATORE_INFORTUNIO);

            if(injuries)
            {
                StopTime();

                p->player_injuried=g;

                if(g->team->Joystick>=0)
                    p->show_panel=PANEL_INJURIED;

                p->show_time=200;
            }
        }
    }
}

void MoveNonControlled(void)
{
    register int i;
    register WORD xg,yg;
    register player_t *g;
    register team_t *s;
    WORD xlimit_e,ylimit_s,xlimit_o,ylimit_n,j;

    // Cacho un paio di cose :)

    xlimit_e=field_x+WINDOW_WIDTH;
    ylimit_s=field_y+WINDOW_HEIGHT;
    xlimit_o=field_x-16;
    ylimit_n=field_y-20;

    for(j=0;j<2;j++)
    {
        s= p->team[j];

        if(s->ArcadeEffect)
        {
            s->ArcadeCounter--;

            if(s->ArcadeCounter==0)
                RemoveArcadeEffect(&s->players[0],s->ArcadeEffect);
        }

        for(i=0;i<10;i++)
        {
            g = &(s->players[i]);

            xg=g->world_x>>3;
            yg=g->world_y>>3;

            if(     xg>xlimit_o &&
                    xg<xlimit_e &&
                    yg>ylimit_n &&
                    yg<ylimit_s   )
            {
                if(!g->OnScreen)
                {

#if 0
                    /* this is to debug replay code, cause a segfault */
                    if(arcade && j==1 && i==4)
                    {
                        char *a=NULL;
                        *a=0;
                    }
#endif
                    AddAnimObj(g->anim,xg-field_x,yg-field_y,Animation[g->AnimType].Frame[0]+g->dir);

                    g->OnScreen=TRUE;
                }
                else
                {
                    MoveAnimObj(g->anim,xg-field_x,yg-field_y);
                }

                /* aggiungere qui il check sulle collisioni, le controllo solo se onscreen */

                /* Check collisions with the ball */

            }
            else if(g->OnScreen)
            {
                if(g->Controlled&&s->MarkerOnScreen)
                {
                    RemAnimObj(s->Marker);
                    s->MarkerOnScreen=FALSE;
                }

                RemAnimObj(g->anim);
                g->OnScreen=FALSE;
            }

            if(
                    s!=pl->sq_palla        &&
                    pl->quota<8     &&
                    pl->world_x>(g->world_x-64)     &&
                    pl->world_x<(g->world_x+160)    &&
                    pl->world_y>(g->world_y+96)     &&
                    pl->world_y<(g->world_y+190)    &&
                    pl->InGioco     &&
                    !pl->Hide    &&
                    (!g->Special || g->AnimType==GIOCATORE_SCIVOLATA)
              )
            {

                if (pl->gioc_palla) {
                    if (pl->gioc_palla->ArcadeEffect == ARCADE_GLUE)
                        goto skipchange;

                    if (g->AnimType != GIOCATORE_SCIVOLATA) {
                        if (pl->gioc_palla->technique > (g->tackle+2+GetTable())) {
                            g->ActualSpeed=0;
                            DoSpecialAnim(g,GIOCATORE_CONTRASTO);
                            goto skipchange;
                        }

                        if (GetTable()>2) {
                            DoSpecialAnim(pl->gioc_palla,GIOCATORE_CONTRASTO);
                        }
                        else {
                            DoSpecialAnim(pl->gioc_palla,GIOCATORE_CADUTA);
                        }

                        pl->gioc_palla->ActualSpeed=0;
                    }
                    else {
                        if (GetTable() > 1) {

                            switch (GetTable()) {
                                case 4:
                                case 5:
                                    //                                PlayIfNotPlaying(BOOU);
                                    SetCrowd(BOOU);
                                    break;
                                case 1:
                                    PlayBackSound(sound[OUCH2]);
                                    break;
                                case 2:
                                    PlayBackSound(sound[OUCH]);
                                    break;
                            }

                            if (pl->gioc_palla->ActualSpeed > 0) {
                                DoSpecialAnim(pl->gioc_palla,GIOCATORE_CADUTA);
                                pl->gioc_palla->ActualSpeed=1;
                                CheckInfortuni(pl->gioc_palla);
                            }
                            else {
                                if(GetTable() > 2) {
                                    DoSpecialAnim(pl->gioc_palla,GIOCATORE_CADUTA_FERMO);
                                    CheckInfortuni(pl->gioc_palla);
                                }
                                else {
                                    DoSpecialAnim(pl->gioc_palla,GIOCATORE_CADUTA_CARPONI);
                                }
                            }


                            if(!arcade && GetTable()>2) // arbitro abilita' (da aggiungere)
                            {
                                p->referee.Comando=FISCHIA_FALLO;

                                StopTime();

                                if(!replay_mode)
                                    UrgentSpeaker(S_CALCIO_DI_PUNIZIONE);


                                FermaPalla();
                                TogliPalla();

                                pl->InGioco=FALSE;

                                if(p->team[0]->Possesso)
                                {
                                    pl->sq_palla=p->team[0];
                                }
                                else
                                {
                                    pl->sq_palla=p->team[1];
                                }

                                if(bookings)
                                {
                                    if(GetTable()<4)
                                    {
                                        if(GetTable()>((11-p->referee.cattiveria)>>1))
                                        {
                                            if(!g->Ammonito)
                                            {
                                                p->team[g->SNum]->Ammonizioni++;
                                                p->referee.Comando=AMMONIZIONE;
                                            }
                                            else
                                            {
                                                p->team[g->SNum]->Espulsioni++;
                                                p->referee.Comando=ESPULSIONE;
                                            }

                                            p->referee.Tick=-50;
                                            p->show_panel|=PANEL_TIME|PANEL_CARD;
                                            p->referee.Argomento=g->GNum+g->SNum*10;
                                        }
                                    }
                                    else if(GetTable()>((12-p->referee.cattiveria)>>1))
                                    {
                                        p->team[g->SNum]->Espulsioni++;
                                        p->referee.Comando=ESPULSIONE;
                                        p->referee.Argomento=g->GNum+g->SNum*10;
                                        p->referee.Tick=-50;
                                        p->show_panel|=PANEL_TIME|PANEL_CARD;
                                    }
                                }

                                goto skipchange;
                            }
                            else if(injuries && p->player_injuried)
                            {
                                RestartTime();
                                p->show_panel=PANEL_PLAY_INJURIED;
                                p->show_time=150;
                            }
                        }
                        else
                        {
                            if(GetTable()>2)
                                if(!replay_mode)
                                    UrgentSpeaker(S_PALLA_PERSA);

                            if(GetTable()>2)
                            {
                                DoSpecialAnim(pl->gioc_palla,GIOCATORE_CONTRASTO);
                            }
                            else
                            {
                                DoSpecialAnim(pl->gioc_palla,GIOCATORE_CADUTA);
                            }

                            pl->gioc_palla->ActualSpeed=0;
                        }
                    }

                    TogliPalla();

                    DaiPalla(g);
                    g->WaitForControl=0;

                    if(s->Joystick<0)
                        while((s->Schema=GetTable())>3);

                    ChangeControlled(s, i);

                    pl->world_x = avanzamento_x[g->dir]+g->world_x;
                    pl->world_y = avanzamento_y[g->dir]+g->world_y;

skipchange:
                    PlayIfNotPlaying(CONTRASTO);
                }
                else { // if the ball is not owned by a player
                    if (pl->velocita < (g->technique + 12 - g->ActualSpeed - pl->quota)) { // changed to be more difficult with the quota
                        if (!g->ActualSpeed && 
                             g->AnimType != GIOCATORE_SCIVOLATA) {
                            if (pl->quota < 3) {
                                DoSpecialAnim(g, GIOCATORE_STOP_PIEDE);
                            }
                            else {
                                DoSpecialAnim(g, GIOCATORE_STOP_PETTO);
                            }
                        }

                        pl->quota=0;
                        g->WaitForControl=0;

                        if(g->AnimType == GIOCATORE_SCIVOLATA &&
                           pl->velocita > 4) {
                            g->ActualSpeed = 0;
                            DoSpecialAnim(g, GIOCATORE_ALZATI);
                            pl->MaxQuota = 1+GetTable();
                            pl->velocita += (GetTable()-2);
                            pl->dir = (g->SpecialData<<3);

                            pl->dir += 2;

                            s->Possesso = 1;
                            p->team[g->SNum^1]->Possesso = 0;

                            PlayIfNotPlaying(CONTRASTO);

                            pl->TipoTiro = TIRO_RASOTERRA;
                            pl->Stage = 0;
                            UpdateBallSpeed();
                            UpdateShotHeight();
                        }
                        else {
                            DaiPalla(g);
                            g->WaitForControl = 0;
                            pl->world_x = avanzamento_x[g->dir]+g->world_x;
                            pl->world_y = avanzamento_y[g->dir]+g->world_y;
                        }

                        ChangeControlled(s,i);
                    }
                    else {
/* the ball is too fast for the player */
                        s->Possesso=1;
                        p->team[g->SNum^1]->Possesso=0;

                        RimbalzoCasuale();

                        pl->velocita>>=1;

                        PlayIfNotPlaying(CONTRASTO);

                        UpdateBallSpeed();

                        if(pl->Stage<(SHOT_LENGTH/2))
                            pl->Stage=(SHOT_LENGTH-1)-pl->Stage;
                    }
                }
            }

            // Check the distance
            if(p->mantieni_distanza&&!g->Comando&&g->team!=pl->sq_palla&&g->AnimType!=GIOCATORE_BATTUTA)
            {
                if(SimpleDistance(pl->world_x,pl->world_y,P2G_X(g->world_x),P2G_Y(g->world_y))<700)
                {
                    g->Comando=MANTIENI_DISTANZA;
                    g->Special=FALSE;
                    g->FrameLen=1;
                    g->AnimFrame=0;
                }
            }

            // Check Y coord of ALL the players
            // the ones executing a command may move more freely (for instance to exit the pitch for an injury or a red card).

            if(!arcade)
            {
                if(g->world_y<(RIMESSA_Y_N-200))
                {
                    g->dir=opposto[g->dir];
                    g->ActualSpeed=0;
                    g->world_y=RIMESSA_Y_N-199;
                }
                else if(g->world_y>(RIMESSA_Y_S+20)&&!g->Comando)
                {
                    // Qui si presume che un giocatore con un comando sappia cosa fare!

                    g->dir=opposto[g->dir];
                    g->ActualSpeed=0;
                    g->world_y=RIMESSA_Y_S+19;
                }
            }
            else
            {
                // Controllo gli effetti dei bonus arcade...

                if(g->ArcadeEffect)
                {
                    g->ArcadeCounter--;

                    if(g->ArcadeCounter==0)
                        RemoveArcadeEffect(g,g->ArcadeEffect);    
                }

                if(g->world_y<(RIMESSA_Y_N-132))
                {
                    g->dir=opposto[g->dir];
                    g->ActualSpeed=0;
                    g->world_y=RIMESSA_Y_N-132;
                }
                else if(g->world_y>(RIMESSA_Y_S-70))
                {
                    g->dir=opposto[g->dir];
                    g->ActualSpeed=0;
                    g->world_y=RIMESSA_Y_S-70;
                }
            }

            if(!g->Controlled)
            {
                g->Tick++;

                if(s->gioco_ruolo && g==pl->gioc_palla)
                {
                    HandleRealCPU(g);
                }
                else if(g->Tick>prontezza[g->quickness]&&!g->Special&&g->Comando==NESSUN_COMANDO)
                {
                    /*
                       if(s!=pl->sq_palla)
                       g->settore = (g->world_y/1450 << 2) + (g->world_x/2560);
                     */      

                    if(g->world_x<80)
                    {
                        g->dir=opposto[g->dir];
                        g->ActualSpeed=0;
                        g->world_x=80;
                    }
                    else if(g->world_x>(1270*8))
                    {
                        g->dir=opposto[g->dir];
                        g->ActualSpeed=0;
                        g->world_x=1270*8;
                    }

                    g->settore=s->tactic->Position[s->Possesso][i][p->ball.settore].settore;

                    g->Tick=0;

                    if(g->settore!=pl->settore || s==pl->sq_palla || !pl->InGioco)
                    {
                        // Qui avevo messo degli 0 al posto di p->team[j]->Possesso, chissa' come mai...

                        if(!IsNear(g->world_x,g->world_y,s->tactic->Position[s->Possesso][i][p->ball.settore].x,s->tactic->Position[s->Possesso][i][p->ball.settore].y) || !pl->InGioco)
                        {
                            MoveTo(g,s->tactic->Position[s->Possesso][i][pl->settore].x,s->tactic->Position[s->Possesso][i][pl->settore].y);
                        }
                        else
                        {
                            if(g->AnimType!=GIOCATORE_RESPIRA&&(!g->Special))
                            {
                                g->dir=FindDirection(g->world_x,g->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
                                DoAnim(g,GIOCATORE_RESPIRA);
                                g->ActualSpeed=0;
                            }
                        }
                    }
                    else
                    {

                        // If he is following the ball is TWICE quicker

                        g->Tick=(prontezza[g->quickness]>>2);

                        if(!IsVeryNear(g->world_x,g->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y)))
                            MoveTo(g,G2P_X(pl->world_x),G2P_Y(pl->world_y));
                    }
                }
            }
            else
            {
                /* Player marker handling */

                if(arcade)
                {
                    register int kk;

                    for(kk=0;kk<MAX_ARCADE_ON_FIELD;kk++)
                    {
                        if(    bonus[kk]->world_x>(g->world_x-96) &&
                                bonus[kk]->world_x<(g->world_x+96) &&
                                bonus[kk]->world_y>(g->world_y+32 ) &&
                                bonus[kk]->world_y<(g->world_y+144 )
                          )
                        {
                            GetArcadeEffect(g,bonus[kk]);
                            s->MarkerFrame=0;
                        }
                    }
                }

                if(g->OnScreen)
                {
                    WORD yk=(g->world_y>>3)-field_y;

                    if(yk>7)
                    {
                        if(s->MarkerOnScreen)
                        {
                            s->MarkerFrame++;

                            if(s->MarkerFrame>9)
                                s->MarkerFrame=0;


                            if(g->ArcadeEffect)
                            {
                                ChangeAnimObj(s->Marker,arcade_frame_sequence[s->MarkerFrame]+(g->ArcadeEffect-1)*3+20);
                                MoveAnimObj(s->Marker,(g->world_x>>3)-field_x,yk-10);
                            }
                            else
                            {
                                if(s->MarkerRed==Pens[RADAR_TEAM_B])
                                {
                                    ChangeAnimObj(s->Marker,s->MarkerFrame+MARKER_TEAM_B);
                                }
                                else
                                {
                                    ChangeAnimObj(s->Marker,s->MarkerFrame);
                                }

                                MoveAnimObj(s->Marker,(g->world_x>>3)-field_x+3,yk-8);
                            }


                        }
                        else
                        {
                            s->MarkerOnScreen=TRUE;
                            s->MarkerFrame=0;
                            AddAnimObj(s->Marker,(g->world_x>>3)-field_x+3,yk-7,s->MarkerRed ? MARKER_TEAM_B : 0);

                        }
                    }
                    else if(s->MarkerOnScreen)
                    {
                        RemAnimObj(s->Marker);
                        s->MarkerOnScreen=FALSE;
                    }
                }

                // Check player positions on pitch

                if(g->world_x<(60*8))
                {
                    if( (g->world_y>>3)<(-( g->world_x + (g->world_x>>2) )+620) ) /* era 650 */
                    {
                        g->world_x=496-g->world_y/10; // con 610 era 488

                        if(g->ActualSpeed&&velocita_x[g->ActualSpeed-1][g->speed][g->dir]<0)
                            g->world_x-=velocita_x[g->ActualSpeed-1][g->speed][g->dir];
                    }
                }
                else if(g->world_x>(1210*8))
                {
                    if((g->world_y>>3)<( g->world_x + (g->world_x>>2) - 12051)) /* era 12131 */
                    {
                        g->world_x=g->world_y/10+(12051/5*4);

                        if(g->ActualSpeed&&velocita_x[g->ActualSpeed-1][g->speed][g->dir]>0)
                            g->world_x-=velocita_x[g->ActualSpeed-1][g->speed][g->dir];
                    }
                }
            }

            if(!g->Special&&g->Comando!=NESSUN_COMANDO)
            {
                switch(g->Comando)
                {
                    case ESEGUI_ROTAZIONE:
                        if(g->Argomento<-1||g->Argomento>1)
                        {
                            g->WaitForControl--;

                            if(g->WaitForControl<0)
                            {
                                if(g->Argomento>0)
                                {
                                    DoSpecialAnim(g,GIOCATORE_CAMBIO_ANTI);
                                    g->Argomento--;
                                }
                                else
                                {
                                    DoSpecialAnim(g,GIOCATORE_CAMBIO_ORARIO);
                                    g->Argomento++;
                                }
                                g->WaitForControl=4;
                            }
                        }
                        else
                            EseguiDopoComando(g);
                        break;
                    case VAI_PALLA:
                        g->WaitForControl--;

                        if(g->WaitForControl<0)
                        {
                            g->WaitForControl=4;

                            if(!IsVeryVeryNear(g->world_x,g->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y)))
                            {
                                g->dir=FindDirection(g->world_x,g->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
                            }
                            else
                            {
                                EseguiDopoComando(g);
                            }
                        }
                        break;
                    case ESCI_CAMPO:
                        g->WaitForControl--;

                        if(g->WaitForControl<0)
                        {
                            g->WaitForControl=12;

                            // Let's wait red card refree animation completion.

                            if(p->referee.Tick>240)
                                break;

                            if(SimpleDistance(g->world_x,g->world_y,CENTROCAMPO_X,RIMESSA_Y_S+300)>200)
                            {
                                g->dir=FindDirection(g->world_x,g->world_y,CENTROCAMPO_X,RIMESSA_Y_S+300);

                                if(g->AnimType!=GIOCATORE_CORSA_LENTA)
                                {
                                    DoAnim(g,GIOCATORE_CORSA_LENTA);
                                    g->ActualSpeed=1;
                                }
                            }
                            else if(p->player_injuried!=g) // It's a red card
                            {
                                //                            D(bug("Fine ESCI_CAMPO, giocatore a destinazione!\n"));
                                g->Comando=STAI_FERMO;
                                g->ActualSpeed=0;
                                DoSpecialAnim(g,GIOCATORE_ESPULSO);
                                g->dir=0;
                            }
                            else
                            {
                                /*
                                 * Sostituzioni, la scelta dell'uomo da sostituire per il computer e' effettuata in arbitro.c,
                                 * per il giocatore umano invece in palla.c
                                 *
                                 */
                                if(g->CA[0]>0)
                                {
                                    D(bug("Effettuo la sostutuzione...(s:%ld go:%ld gi:%ld)\n",j,g->number,Riserve[j][g->CA[0]-1].number));                                
                                    ChangePlayer(&Riserve[j][g->CA[0]-1],g);

                                    // Il giocatore sostituito potrebbe essere quello attivo!
                                    if(detail_level&USA_NOMI)
                                        PrintSmall(s->NomeAttivo,s->attivo->surname,s->attivo->NameLen);
                                    s->NumeroRiserve--;
                                    p->player_injuried=NULL;
                                    p->show_panel&=0xff;
                                    p->show_time=50;
                                    g->Comando=NESSUN_COMANDO;
                                    s->Sostituzioni++;
                                }
                            }
                        }
                        break;
                    case MANTIENI_DISTANZA:
                        g->WaitForControl--;

                        if(g->WaitForControl<0)
                        {
                            g->WaitForControl=8;

                            if(SimpleDistance(pl->world_x,pl->world_y,P2G_X(g->world_x),P2G_Y(g->world_y))<700)
                            {
                                g->dir=opposto[FindDirection(g->world_x,g->world_y,G2P_X(pl->world_x), G2P_Y(pl->world_y))];

                                if(g->world_x>(RIGORE_X_E-100))
                                    g->dir=6;

                                if(g->world_x<(RIGORE_X_O+100))
                                    g->dir=2;

                                if(g->AnimType!=GIOCATORE_CORSA_VELOCE)
                                {
                                    DoAnim(g,GIOCATORE_CORSA_VELOCE);
                                }

                                g->ActualSpeed=3;
                            }
                            else
                            {
                                g->Comando=NESSUN_COMANDO;
                                g->dir=FindDirection(g->world_x,g->world_y,G2P_X(pl->world_x), G2P_Y(pl->world_y));
                                g->ActualSpeed=0;
                                DoSpecialAnim(g,GIOCATORE_RESPIRA);
                            }
                        }
                        break;
                    case ESCI_AREA:
                        g->WaitForControl--;

                        if(g->WaitForControl<0)
                        {
                            g->WaitForControl=8;

                            if(InAnyArea(P2G_X(g->world_x),P2G_Y(g->world_y)))
                            {
                                g->dir=FindDirection(g->world_x,g->world_y,CENTROCAMPO_X, CENTROCAMPO_Y);

                                if(g->AnimType!=GIOCATORE_CORSA_VELOCE)
                                {
                                    DoAnim(g,GIOCATORE_CORSA_VELOCE);
                                }

                                g->ActualSpeed=3;
                            }
                            else
                            {
                                g->Comando=NESSUN_COMANDO;
                                g->ActualSpeed=0;
                                DoSpecialAnim(g,GIOCATORE_RESPIRA);
                            }
                        }
                    case STAI_FERMO:
                    case STAI_BARRIERA:
                        break;
                }
            }

            if(!g->Special)
            {
                if(g->ActualSpeed>0)
                {
                    g->AnimType=(GIOCATORE_CORSA_LENTA-1)+g->ActualSpeed;

                    if(g==pl->gioc_palla)
                        g->AnimType+=3; // 3 running types...
                }
                else if(g->AnimType==GIOCATORE_RESPIRA&&g==pl->gioc_palla)
                {
                    g->AnimType++;
                }
            }
            else
            {
                if(g->AnimType==GIOCATORE_TIRO)
                {
                    if(g->AnimFrame==0&&g==pl->gioc_palla&&g->FrameLen==1)
                    {
                        TogliPalla();

                        if(g->SpecialData>-1&&g->SpecialData<10)
                        {
                            if(use_offside&&!arcade)
                            {
                                if(IsOffside(g))
                                {
                                    pl->InGioco=FALSE;
                                    p->referee.Tick=5;
                                    p->referee.Comando=FISCHIA_OFFSIDE;
                                    p->referee.Argomento=g->SNum*10+g->SpecialData;
                                }
                            }

                            ChangeControlled(s,g->SpecialData);
                            g->SpecialData=-1;

                            g->WaitForControl=12;

                            if(s->Joystick<0)
                            {
                                s->attivo->ActualSpeed=0;
                                s->attivo->WaitForControl=24;
                                s->attivo->dir=FindDirection(s->attivo->world_x,s->attivo->world_y,g->world_x,g->world_y);
                            }
                        }
                        else if(use_speaker && GetTable()>1)
                        {
                            if(!replay_mode)
                            {
                                if(CanScore(g)==CS_SI)
                                    UrgentSpeaker(S_TIRO);
                            }
                        }
                    }
                }
            }

            if(g->FrameLen<=0)
            {
                /*

                   Prima FrameLen si impostava qui, per semplificare le cose l'ho messo
                   in fondo, pero' potrebbe avere effetti collaterali, per questo nelle
                   Special che lo modificavano ho lasciato l'impostazione commentata.

                   g->FrameLen=Animation[g->AnimType].FrameLen;
                 */
                g->AnimFrame++;

                if(g->AnimFrame>=Animation[g->AnimType].Frames)
                {
                    g->AnimFrame=0;

                    if(g->Special)
                    {
                        switch(g->AnimType)
                        {
                            case GIOCATORE_STACCO:
                            case GIOCATORE_PREGIRATA:
                            case GIOCATORE_PRETUFFO:
                                if( NeiPressiPalla(g) && pl->quota<13 && !pl->gioc_palla)
                                    ColpoDiTesta(g);
                                else
                                {
                                    switch(g->AnimType)
                                    {
                                        case GIOCATORE_STACCO:
                                            g->AnimType=GIOCATORE_ATTERRA;
                                            break;
                                        case GIOCATORE_PREGIRATA:
                                            g->AnimType=GIOCATORE_ALZATI;
                                            break;
                                        case GIOCATORE_PRETUFFO:
                                            g->AnimType=GIOCATORE_RIALZA_AVANTI;
                                    }

                                    g->ActualSpeed=0;
                                }
                                break;
                            case GIOCATORE_TESTA_AVANTI:
                            case GIOCATORE_TESTA_DESTRA:
                            case GIOCATORE_TESTA_SINISTRA:
                                g->AnimType=GIOCATORE_ATTERRA;
                                g->ActualSpeed=0;
                                break;
                            case GIOCATORE_CADUTA_CARPONI:
                                g->AnimType=GIOCATORE_RIALZA_AVANTI;
                                break;

                            case GIOCATORE_GIRATA_SINISTRA:
                            case GIOCATORE_GIRATA_DESTRA:
                                g->dir=opposto[g->dir];
                            case GIOCATORE_TUFFO_TESTA:
                            case GIOCATORE_CADUTA_FERMO:
                            case GIOCATORE_CADUTA:
                                g->AnimType=GIOCATORE_RIALZA_AVANTI;
                                g->ActualSpeed=0;
                                break;
                            case GIOCATORE_ROVESCIATA:
                            case GIOCATORE_SCIVOLATA:
                            case GIOCATORE_INFORTUNIO:
                                g->AnimType=GIOCATORE_ALZATI;
                                g->ActualSpeed=0;
                                break;
                            case GIOCATORE_PREPARA_RIMESSA:
                                HandleRimessa(g);
                                break;
                            case GIOCATORE_RIMESSA:
                                RimessaLaterale(g);
                                TogliPalla();

                                if(g->SpecialData>-1&&g->SpecialData<10)
                                {
                                    ChangeControlled(s,g->SpecialData);
                                    g->SpecialData=-1;
                                    g->WaitForControl=40;
                                    return;
                                }
                                break;
                            case GIOCATORE_BATTUTA:
                                PunizioneCorner(g);
                                break;
                            case GIOCATORE_RECUPERA_PALLA:
                                g->AnimType=GIOCATORE_PREPARA_RIMESSA;
                                break;
                            case GIOCATORE_ESPULSO:
                                //                            D(bug("Giocatore %ld: C:%ld S:%ld\n",g->GNum,g->Comando,g->Special));
                                g->Comando=STAI_FERMO;
                                g->world_x=CENTROCAMPO_X;
                                g->world_y=RIMESSA_Y_S+160;
                                break;
                            case GIOCATORE_CAMBIO_ORARIO:
                                g->dir+=2;

                                if(g->dir>8)
                                    g->dir-=8;
                            case GIOCATORE_CAMBIO_ANTI:
                                g->dir--;

                                if(g->dir<0)
                                    g->dir+=8;

                                g->Special=FALSE;
                                g->AnimType=GIOCATORE_CORSA_LENTA;

                                if(g==pl->gioc_palla)
                                {
                                    pl->world_x=avanzamento_x[g->dir]+g->world_x;
                                    pl->world_y=avanzamento_y[g->dir]+g->world_y;
                                }
                                break;
                            case GIOCATORE_INVERSIONE_MARCIA:
                            case GIOCATORE_INVERSIONE_MARCIA_PALLA:
                                g->dir=opposto[g->dir];

                                if(g==pl->gioc_palla)
                                {
                                    pl->world_x=avanzamento_x[g->dir]+g->world_x;
                                    pl->world_y=avanzamento_y[g->dir]+g->world_y;
                                }

                                /*
                                   case GIOCATORE_ALZATI:
                                   case GIOCATORE_TIRO:
                                   case GIOCATORE_ATTERRA:
                                 */
                            default:
                                g->Special=FALSE;
                                g->AnimType=GIOCATORE_RESPIRA;
                                break;
                        }
                    }
                }

                g->FrameLen=Animation[g->AnimType].FrameLen;

                if(g->OnScreen)
                    /* I had an access underrun here. The conditions was:
                     * g->AnimType = 18
                     * g->AnimFrame = -1
                     * g->dir = 2
                     */
                    ChangeAnimObj(g->anim,Animation[g->AnimType].Frame[g->AnimFrame]+g->dir);
            }

            if(g->ActualSpeed>0)
            {
                g->world_x+=velocita_x[g->ActualSpeed-1][g->speed][g->dir];
                g->world_y+=velocita_y[g->ActualSpeed-1][g->speed][g->dir];
            }

            if(g->Special)
            {
                switch(g->AnimType)
                {
                    case GIOCATORE_SCIVOLATA:
                        if(g->SpecialData>=0&&g->SpecialData<32)
                        {
                            g->world_x+=velocita_scivolata_x[g->SpecialData];
                            g->world_y+=velocita_scivolata_y[g->SpecialData];
                        }
                        break;
                    case GIOCATORE_STACCO:
                    case GIOCATORE_PREGIRATA:
                    case GIOCATORE_PRETUFFO:
                        if(g->SpecialData>=0&&g->SpecialData<256)
                        {
                            g->world_y+=-(cos_table[g->SpecialData]*g->CA[0])>>7;
                            g->world_x+=(sin_table[g->SpecialData]*g->CA[0])>>7;
                        }
                        break;
                    default:
                        break;
                }
            }

            g->FrameLen--;
        }
    }
}


void ChangeControlled(team_t *s,WORD i)
{
// Occhio qui, potrebbe dar problemi.

    if(s->gioco_ruolo||s->players[i].Comando||s->players[i].AnimType==GIOCATORE_ESPULSO)
        return;

        if(s->attivo->Controlled && s->MarkerOnScreen)
        {
                s->MarkerOnScreen=FALSE;
                RemAnimObj(s->Marker);
        }

    if(s->attivo->ArcadeEffect)
    {
        if(s->attivo->ArcadeEffect!=ARCADE_FREEZE)
            RemoveArcadeEffect(s->attivo,s->attivo->ArcadeEffect);
    }

        s->attivo->Controlled=FALSE;

        s->attivo=&s->players[i];
        s->attivo->Controlled=TRUE;

    if(detail_level&USA_NOMI)
        PrintSmall(s->NomeAttivo,s->attivo->surname,s->attivo->NameLen);

    if(s->Joystick>=0)
        need_release[s->Joystick]=TRUE;
}

void HandleExtras(void)
{
    register int i=0;
    register object_t *g;
    register WORD xg;
    register WORD yg;

    while(object_list[i]) {
        g=object_list[i];
        xg=g->world_x>>3;
        yg=g->world_y>>3;

        if( xg>(field_x-g->anim->Widths[g->AnimType]) &&
                xg<(field_x+WINDOW_WIDTH) &&
                yg>(field_y-g->anim->Heights[g->AnimType]) &&
                yg<(field_y+WINDOW_HEIGHT) 
          ) {
            struct Rect *r=&ingombri[g->otype];

            if(!g->OnScreen) {
                AddAnimObj(g->anim,xg-field_x,yg-field_y,g->AnimType);
                g->OnScreen=TRUE;
            }
            else { /* move the ALSO the nets if(g->otype!=TIPO_PORTA || nosync) */
                MoveAnimObj(g->anim,xg-field_x,yg-field_y);
            }


            if(     pl->world_x>(g->world_x+r->MinX)     &&
                    pl->world_x<(g->world_x+r->MaxX)    &&
                    pl->world_y>(g->world_y+r->MinY)    &&
                    pl->world_y<(g->world_y+r->MaxY)    &&
                    pl->quota<6    ) {
                if(g->otype==TIPO_ARBITRO) {
                    PlayIfNotPlaying(CONTRASTO);
                    TogliPallaMod();

                    RimbalzoCasuale();

                    if(pl->velocita<4) {
                        pl->velocita=4;
                        UpdateBallSpeed();
                    }
                }

                /*
                   switch(g->otype)
                   {
                   case TIPO_ARBITRO:
                   PlayIfNotPlaying(CONTRASTO);
                   break;
                   case TIPO_PORTIERE:
                   case TIPO_PORTA:
                   break;
                   default:
                   PlayIfNotPlaying(CRASH);
                   }
                 */

                /*
                   Commentato finche' non aggiungo la fisica del pallone...
                 */
            }
        }
        else if(g->OnScreen) {
            RemAnimObj(g->anim);
            g->OnScreen=FALSE;
        }

        i++;
    }

    if(*c_list)    {
        register struct Rect *r;
        register struct DOggetto *g;
        register int px = pl->world_x, py = pl->world_y;

        i=0;

        while ((g=c_list[i])) {
            // Range is used to find the object type!
            r=&ingombri[g->Range];

            if(    pl->quota<6        &&
                    px >(g->X+r->MinX)    &&
                    px <(g->X+r->MaxX)    &&
                    py >(g->Y+r->MinY)    &&
                    py <(g->Y+r->MaxY)    
              ) {
                switch(g->Range)
                {
                    case TIPO_POLIZIOTTO_CANE:
                        PlayIfNotPlaying(DOG);
                        break;
                    case TIPO_TUTA:
                        PlayIfNotPlaying(CONTRASTO);
                        break;
                    default:
                        PlayIfNotPlaying(CRASH);                        
                }

                TogliPallaMod();

                RimbalzoCasuale();

                if(pl->velocita<4)
                {
                    pl->velocita=4;
                    UpdateBallSpeed();
                }
            }
            i++;
        }
    }
}

void CheckChange(player_t *g)
{
    if(substitutions && !arcade && !(p->show_panel&0xff00) )
    {
        if(r_controls[g->team->Joystick][counter]&(JPF_JOY_DOWN|JPF_JOY_UP))
        {
            g->team->ArcadeCounter++;

            if(g->team->ArcadeCounter>70)
            {
                if(r_controls[g->team->Joystick][counter]&JPF_JOY_DOWN)
                {
                    if(g->team->NumeroRiserve>0&&g->team->Sostituzioni<3)
                    {
                        StopTime();
                        p->player_injuried=g;
                        p->RiservaAttuale=-1;
                        p->show_panel=PANEL_SUBSTITUTION;
                    }
                }
                else
                {
                    StopTime();
                    p->player_injuried=g;
                    p->RiservaAttuale=0;
                    p->show_panel=PANEL_CHANGE_TACTIC;
                }
                g->team->ArcadeCounter=0;
            }
        }
        else g->team->ArcadeCounter=0;
    }
}

void NoPlayerControl(player_t *g)
{
    g->Tick++;

    if(g->Tick>36)
    {
        g->Tick=0;

        if(!IsNear(g->world_x,g->world_y,g->team->tactic->Position[g->team->Possesso][g->GNum][p->ball.settore].x,g->team->tactic->Position[g->team->Possesso][g->GNum][p->ball.settore].y) )
        {
            MoveTo(g,g->team->tactic->Position[g->team->Possesso][g->GNum][pl->settore].x,g->team->tactic->Position[g->team->Possesso][g->GNum][pl->settore].y);
        }
        else
        {
            if(g->AnimType!=GIOCATORE_RESPIRA&&(!g->Special))
            {
                g->dir=FindDirection(g->world_x,g->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
                DoAnim(g,GIOCATORE_RESPIRA);
                g->ActualSpeed=0;
            }
        }
    }
}
