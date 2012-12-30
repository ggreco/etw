#include "eat.h"

int8_t need_release[MAX_PLAYERS]={1,1};


static void DoLongPass(player_t *g, uint32_t joystate);
static void DoCross(player_t *g);
static void DoShot(player_t *g, uint32_t joystate);
static void HumanShot(player_t *g, uint32_t joystate);

void DoLongPass(player_t *g, uint32_t joystate)
{
    LongPass(g,joystate);

    if( abs(CambioDirezione[g->dir][(pl->dir>>5)])<2 )
        Tira(g);
    else
        SetComando(g,ESEGUI_ROTAZIONE,ESEGUI_TIRO,(pl->dir + 4)>>5);
}

void DoCross(player_t *g)
{
    EseguiCross(g);

    if( abs(CambioDirezione[g->dir][(pl->dir>>5)])<2 )
        Tira(g);
    else
        SetComando(g,ESEGUI_ROTAZIONE,ESEGUI_TIRO,pl->dir>>5);
}

void DoShot(player_t *g, uint32_t joystate)
{
    if( (g->SNum&&(joystate&JPF_JOY_LEFT))||(g->SNum==0&&(joystate&JPF_JOY_RIGHT)) )
    {
//        D(bug("Pallonetto!\n"));
        pl->TipoTiro=TIRO_PALLONETTO;
    }
    else if( (g->SNum&&(joystate&JPF_JOY_RIGHT))||(g->SNum==0&&(joystate&JPF_JOY_LEFT)) )
    {
//        D(bug("Tiro rasoterra!\n"));
        pl->TipoTiro=TIRO_RASOTERRA;
    }
    else
    {
//        D(bug("Tiro alto!\n"));
        pl->TipoTiro=TIRO_ALTO;
    }

    SetShotSpeed(g, IndirizzaTiro(g, joystate));

    if(CanScore(g)!=CS_SI)
        pl->velocita-=(1+GetTable());

    if (pl->TipoTiro == TIRO_RASOTERRA)
        pl->velocita++;
    
    CheckPortiere(g->SNum^1);

    if( abs(CambioDirezione[g->dir][(pl->dir+4)>>5])<2 )
        Tira(g);
    else
        SetComando(g, ESEGUI_ROTAZIONE, ESEGUI_TIRO, (pl->dir + 4) >> 5);
}

void HumanShot(player_t *g, uint32_t joystate)
{
    switch(CanScore(g))
    {
// Il tiro qui viene direzionato a seconda della direzione della porta
        case CS_SI:
        DoShot(g,joystate);
        break;
// Fissa per permettere i lanci.
        case CS_NO:
        DoLongPass(g,joystate);
        break;
        case CS_CROSS:
        DoCross(g);
    }
}

void HandleControlled(int squadra)
{
    uint32_t joystate;
    register player_t *g=p->team[squadra]->attivo;

    //      g->WaitForControl++;

    if(g->Special || g->Comando)
        return;

    if(!pl->InGioco) {
        NoPlayerControl(g);
        return;
    }

    if (g->team->Joystick < 0) {
        D(bug("HandleControlled with Joystick < 0!\n"));
        return;
    }

    joystate=r_controls[g->team->Joystick][counter];

    // Gestione del fire

    if((joystate&JPF_BUTTON_RED)&&!need_release[g->team->Joystick])
    {
        if(g->FirePressed)
        {
            g->TimePress++;

            if(g->TimePress>8)
            {
                need_release[g->team->Joystick]=TRUE;

                if(pl->gioc_palla==g)
                    HumanShot(g,joystate);
                else
                {
                    DoSpecials(g);
                }
                g->FirePressed=FALSE;
                g->TimePress=0;
            }
        }
        else
        {
            g->FirePressed=TRUE;
            g->TimePress=0;
        }

        return;
    }
    else if (g->FirePressed)
    {
        if(g==pl->gioc_palla)
        {
            // Tiro
            g->SpecialData=-1;

            if(g->TimePress>8)
            {
                HumanShot(g,joystate);

            }
            // Passaggio
            else
            {
                WORD d=FindDirection(g->world_x,G2P_Y(g->world_y),(g->SNum ? PORTA_E_X : PORTA_O_X),PORTA_Y);

                if(abs(CambioDirezione[g->dir][d])>1 || !InArea(g->SNum^1,g->world_x,g->world_y) )
                {
                    if(joystate&JP_DIRECTION_MASK)
                        Passaggio2(g,GetJoyDirection(joystate));
                    else
                        Passaggio2(g,g->dir);
                }
                else
                {
                    D(bug("converto passaggio vicino porta in tiro!\n"));
                    HumanShot(g,joystate);
                }
            }


        }
        else DoSpecials(g);

        g->FirePressed=FALSE;
        g->TimePress=0;

        return;
    }
    /*
       else need_release[g->team->Joystick]=FALSE;
     */
    if(!(joystate&JPF_BUTTON_RED))
        need_release[g->team->Joystick]=FALSE;

    if(g->ActualSpeed==3 && g->AnimFrame!=0 && g==pl->gioc_palla)
        return;

    g->AnimType=GIOCATORE_RESPIRA;

    /* Gestione delle otto direzioni */

    if( (joystate&JP_DIRECTION_MASK) ) 
    {
        register WORD NewDir=GetJoyDirection(joystate);

        if(g->dir == opposto[NewDir])
        {
            if(g==pl->gioc_palla)
            {
                if(g->ActualSpeed!=3)
                {
                    DoSpecialAnim(g,GIOCATORE_INVERSIONE_MARCIA_PALLA);
                    g->ActualSpeed=0;
                }
                else
                {
                    g->ActualSpeed--;
                }
            }
            else
            {
                DoSpecialAnim(g,GIOCATORE_INVERSIONE_MARCIA);
                g->ActualSpeed=0;
            }

            g->WaitForControl=0;
        }
        else if(g->dir != NewDir)
        {
            if(g->ActualSpeed==3)
            {
                WORD tv=abs(g->dir-NewDir);

                if(tv>4)
                    tv=8-tv;

                if(tv>2)
                {
                    DoSpecialAnim(g,GIOCATORE_CORSA_PARZIALE);

                    if(BestRotation(g->dir,NewDir))
                    {
                        NewDir--;

                        if(NewDir<0)
                            NewDir=7;
                    }
                    else
                    {
                        NewDir++;

                        if(NewDir>7)
                            NewDir=0;
                    }
                }
                else if(tv>1)
                {
                    if(BestRotation(g->dir,NewDir))
                    {
                        NewDir--;

                        if(NewDir<0)
                            NewDir=7;
                    }
                    else
                    {
                        NewDir++;

                        if(NewDir>7)
                            NewDir=0;
                    }

                    g->ActualSpeed--;
                }
            }
            else
            {
                if(g==pl->gioc_palla)
                {
                    WORD tv=abs(g->dir-NewDir);

                    if(tv>4)
                        tv=8-tv;

                    tv--;

                    if(tv>0)
                    {
                        if(BestRotation(g->dir,NewDir))
                        {
                            NewDir-=tv;

                            if(NewDir<0)
                                NewDir+=8;
                        }
                        else
                        {
                            NewDir+=tv;

                            if(NewDir>7)
                                NewDir-=8;
                        }
                    }
                }
            }

            g->WaitForControl=0;

            if( !AllowRotation(g,NewDir) )
                g->dir = NewDir;
        }
        else                            
        {
            g->AnimType=GIOCATORE_CORSA_LENTA;

            if(g->ActualSpeed<3)
            {
                if(g->ActualSpeed==2)
                {
                    g->WaitForControl++;

                    if(g->WaitForControl>50)
                    {
                        g->WaitForControl=0;
                        g->ActualSpeed++;
                        g->AnimFrame=-1;
                        g->FrameLen=0;
                    }
                }
                else
                {
                    g->ActualSpeed++;
                    g->AnimFrame=-1;
                    g->FrameLen=0;
                }
            }
        }
    }
    else if(g->ActualSpeed>0)
    {
        if(g->ActualSpeed==2)
        {
            if(counter>0 && r_controls[g->team->Joystick][counter-1]&JP_DIRECTION_MASK)
                g->WaitForControl=0;

            g->WaitForControl++;

            if(g->WaitForControl>16)
            {
                g->ActualSpeed--;
                g->WaitForControl=0;
            }
        }
        else
        {
            g->ActualSpeed--;
        }
    }
}

player_t *find_touch_player(player_t *g)
{
    int x = r_controls[g->team->Joystick ^ 1][counter] & 0xffff,
        y = r_controls[g->team->Joystick ^ 1][counter] >> 16;

    return FindNearest(g->team, (x + field_x) << 3, (y + field_y) << 3);
}

void HandleControlledJ2B(int squadra)
{
    register uint32_t joystate;
    register player_t *g = p->team[squadra]->attivo;

    //      g->WaitForControl++;

    if(g->Special || g->Comando)
        return;

    if (g->team->Joystick < 0) {
        D(bug("HandleControlled with Joystick < 0!\n"));
        return;
    }

    joystate=r_controls[g->team->Joystick][counter];

     if(!pl->InGioco) {
        NoPlayerControl(g);
        return;
    }

   // handling fire button

    if(!need_release[g->team->Joystick])
    {
        if(joystate&JPF_BUTTON_RED)
        {
            need_release[g->team->Joystick]=TRUE;

            if(pl->gioc_palla==g)
            {
                DoShot(g,joystate);

                if(CanScore(g)!=CS_SI)
                    pl->velocita-=4;
            }
            else
                DoSpecials(g);

            return;
        }

        if(joystate&JPF_BUTTON_BLUE)
        {
            if(g->FirePressed)
            {
                g->TimePress++;

                if(g->TimePress>20) // this was 8, the value was still at 25 fps logic? Very likely.
                {
                    need_release[g->team->Joystick]=TRUE;

                    if(pl->gioc_palla==g)
                    {
                        g->SpecialData=-1;

                        if(CanScore(g)==CS_NO)
                            DoLongPass(g,joystate);
                        else
                            DoCross(g);
                    }
                    else
                        DoSpecials(g);

                    g->FirePressed=FALSE;
                    g->TimePress=0;
                }
            }
            else
            {
                g->FirePressed=TRUE;
                g->TimePress=0;
            }

            return;
        }
        else if (g->FirePressed)
        {
            if(g==pl->gioc_palla)
            {
                // Passaggio
                g->SpecialData=-1;

                if(joystate&JP_DIRECTION_MASK)
                    Passaggio2(g,GetJoyDirection(joystate));
                else
                    Passaggio2(g,g->dir);
            }
            else DoSpecials(g);

            g->FirePressed=FALSE;
            g->TimePress=0;

            return;
        }
    }
    else if(!(joystate&(JPF_BUTTON_RED|JPF_BUTTON_BLUE)))
        need_release[g->team->Joystick]=FALSE;

    if(g->ActualSpeed==3 && g->AnimFrame!=0 && g==pl->gioc_palla)
        return;

    g->AnimType=GIOCATORE_RESPIRA;

    /* Gestione delle otto direzioni */

    if( (joystate&JP_DIRECTION_MASK) ) 
    {
        register WORD NewDir=GetJoyDirection(joystate);

        if(g->dir == opposto[NewDir])
        {
            if(g==pl->gioc_palla)
            {
                if(g->ActualSpeed!=3)
                {
                    DoSpecialAnim(g,GIOCATORE_INVERSIONE_MARCIA_PALLA);
                    g->ActualSpeed=0;
                }
                else
                {
                    g->ActualSpeed--;
                }
            }
            else
            {
                DoSpecialAnim(g,GIOCATORE_INVERSIONE_MARCIA);
                g->ActualSpeed=0;
            }

            g->WaitForControl=0;
        }
        else if(g->dir != NewDir)
        {
            if(g->ActualSpeed==3)
            {
                WORD tv=abs(g->dir-NewDir);

                if(tv>4)
                    tv=8-tv;

                if(tv>2)
                {
                    DoSpecialAnim(g,GIOCATORE_CORSA_PARZIALE);

                    if(BestRotation(g->dir,NewDir))
                    {
                        NewDir--;

                        if(NewDir<0)
                            NewDir=7;
                    }
                    else
                    {
                        NewDir++;

                        if(NewDir>7)
                            NewDir=0;
                    }
                }
                else if(tv>1)
                {
                    if(BestRotation(g->dir,NewDir))
                    {
                        NewDir--;

                        if(NewDir<0)
                            NewDir=7;
                    }
                    else
                    {
                        NewDir++;

                        if(NewDir>7)
                            NewDir=0;
                    }

                    g->ActualSpeed--;
                }
            }
            else
            {
                if(g==pl->gioc_palla)
                {
                    WORD tv=abs(g->dir-NewDir);

                    if(tv>4)
                        tv=8-tv;

                    tv--;

                    if(tv>0)
                    {
                        if(BestRotation(g->dir,NewDir))
                        {
                            NewDir-=tv;

                            if(NewDir<0)
                                NewDir+=8;
                        }
                        else
                        {
                            NewDir+=tv;

                            if(NewDir>7)
                                NewDir-=8;
                        }
                    }
                }
            }

            g->WaitForControl=0;

            if( !AllowRotation(g,NewDir) )
                g->dir = NewDir;
        }
        else                            
        {
            g->AnimType=GIOCATORE_CORSA_LENTA;

            if(g->ActualSpeed<3)
            {
                if(g->ActualSpeed==2)
                {
                    g->WaitForControl++;

                    if(g->WaitForControl>50)
                    {
                        g->WaitForControl=0;
                        g->ActualSpeed++;
                        g->AnimFrame=-1;
                        g->FrameLen=0;
                    }
                }
                else
                {
                    g->ActualSpeed++;
                    g->AnimFrame=-1;
                    g->FrameLen=0;
                }
            }
        }
    }
    else if(g->ActualSpeed>0)
    {
        if(g->ActualSpeed==2)
        {
            if(counter>0 && r_controls[g->team->Joystick][counter-1]&JP_DIRECTION_MASK)
                g->WaitForControl=0;

            g->WaitForControl++;

            if(g->WaitForControl>16)
            {
                g->ActualSpeed--;
                g->WaitForControl=0;
            }
        }
        else
        {
            g->ActualSpeed--;
        }
    }
}

void HandleControlledTouch(int squadra)
{
    player_t *g = p->team[squadra]->attivo;
    uint32_t joystate = r_controls[g->team->Joystick][counter];
    
    if(joystate&JPF_BUTTON_PLAY) {
        DoPause();
        return;
    }
    
    if(g->Special || g->Comando)
        return;
    
    if (g->team->Joystick < 0) {
        D(bug("HandleControlled with Joystick < 0!\n"));
        return;
    }
    
    if(!pl->InGioco) {
        NoPlayerControl(g);
        return;
    }
    
    // handling free touch
    if (joystate & JPF_TOUCH) {
        player_t *g2 = find_touch_player(g);
        
        if (!pl->gioc_palla || pl->gioc_palla->team != g->team) {
            // we can change the active player to the nearest one...
            
            D(bug("Detected free touch change player from %s to %s\n", g->name, g2 ? g2->name : "NONE"));
            if(g2 != g && g2 != NULL)
                ChangeControlled(g->team, g2->GNum);
        }
        else if (g == pl->gioc_palla && g2 != g)
            TouchTargetedPass(g, g2);
    }    
    
    // handling fire button
    
    if(!need_release[g->team->Joystick])
    {
        if(joystate&JPF_BUTTON_RED)
        {
            need_release[g->team->Joystick]=TRUE;
            
            if(pl->gioc_palla==g)
            {
                DoShot(g,joystate);
                
                if(CanScore(g) != CS_SI)
                    pl->velocita -= 3;
            }
            else
                DoSpecials(g);
            
            return;
        }
        
        if(joystate&JPF_BUTTON_BLUE)
        {
            if(g->FirePressed)
            {
                g->TimePress++;
                
                if(g->TimePress>20) // this was 8, the value was still at 25 fps logic? Very likely.
                {
                    need_release[g->team->Joystick]=TRUE;
                    
                    if(pl->gioc_palla==g)
                    {
                        g->SpecialData=-1;
                        
                        if(CanScore(g)==CS_NO)
                            DoLongPass(g,joystate);
                        else
                            DoCross(g);
                    }
                    else {
                        CheckActive();
                    }
                    
                    g->FirePressed=FALSE;
                    g->TimePress=0;
                }
            }
            else
            {
                g->FirePressed=TRUE;
                g->TimePress=0;
            }
            
            return;
        }
        else if (g->FirePressed)
        {
            if(g==pl->gioc_palla)
            {
                // Passaggio
                g->SpecialData=-1;
                
                if(joystate&JP_DIRECTION_MASK)
                    Passaggio2(g,GetJoyDirection(joystate));
                else
                    Passaggio2(g,g->dir);
            }
            else
                CheckActive();
            
            g->FirePressed=FALSE;
            g->TimePress=0;
            
            return;
        }
    }
    else if(!(joystate&(JPF_BUTTON_RED|JPF_BUTTON_BLUE)))
        need_release[g->team->Joystick]=FALSE;

    
    if(g->ActualSpeed==3 && g->AnimFrame!=0 && g==pl->gioc_palla)
        return;
    
    g->AnimType=GIOCATORE_RESPIRA;
    
    /* Gestione delle otto direzioni */
    
    if( (joystate&JP_DIRECTION_MASK) )
    {
        register WORD NewDir=GetJoyDirection(joystate);
        
        if(g->dir == opposto[NewDir])
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
        else if(g->dir != NewDir)
        {
            if(g->ActualSpeed==3)
            {
                WORD tv=abs(g->dir-NewDir);
                
                if(tv>4)
                    tv=8-tv;
                
                if(tv>2)
                {
                    DoSpecialAnim(g,GIOCATORE_CORSA_PARZIALE);
                    
                    if(BestRotation(g->dir,NewDir))
                    {
                        NewDir--;
                        
                        if(NewDir<0)
                            NewDir=7;
                    }
                    else
                    {
                        NewDir++;
                        
                        if(NewDir>7)
                            NewDir=0;
                    }
                }
                else if(tv>1)
                {
                    if(BestRotation(g->dir,NewDir))
                    {
                        NewDir--;
                        
                        if(NewDir<0)
                            NewDir=7;
                    }
                    else
                    {
                        NewDir++;
                        
                        if(NewDir>7)
                            NewDir=0;
                    }
                    
                    g->ActualSpeed--;
                }
            }
            else
            {
                if(g==pl->gioc_palla)
                {
                    WORD tv=abs(g->dir-NewDir);
                    
                    if(tv>4)
                        tv=8-tv;
                    
                    tv--;
                    
                    if(tv>0)
                    {
                        if(BestRotation(g->dir,NewDir))
                        {
                            NewDir-=tv;
                            
                            if(NewDir<0)
                                NewDir=7;
                        }
                        else
                        {
                            NewDir+=tv;
                            
                            if(NewDir>7)
                                NewDir=0;
                        }
                    }
                }
            }
            
            if(!AllowRotation(g,NewDir))
                g->dir = NewDir;
        }
        else
        {
            // Dovrebbe essere superfluo, g->AnimType=GIOCATORE_CORSA_LENTA;
            
            if(g->ActualSpeed<3)
            {
                if(g->ActualSpeed==2)
                {
                    if(joystate&JPF_BUTTON_FORWARD)
                    {
                        g->WaitForControl++;
                        
                        if(g->WaitForControl>20)
                        {
                            g->WaitForControl=0;
                            g->ActualSpeed++;
                            g->AnimFrame=-1;
                            g->FrameLen=0;
                        }
                    }
                }
                else
                {
                    g->ActualSpeed++;
                    g->AnimFrame=-1;
                    g->FrameLen=0;
                }
            }
        }
        
        if(g->ActualSpeed==3 && !(joystate&JPF_BUTTON_FORWARD) && g->AnimFrame==0)
            g->ActualSpeed--;
    }
    else if(g->ActualSpeed>0)
    {
        if(g->ActualSpeed==2&&pl->gioc_palla)
        {
            g->WaitForControl++;
            
            if(g->WaitForControl>18)
            {
                g->ActualSpeed--;
                g->WaitForControl=0;
            }
        }
        else
        {
            if(!(joystate&JPF_BUTTON_FORWARD))
                g->ActualSpeed--;
        }
    }
}

void HandleControlledJoyPad(int squadra)
{
    register uint32_t joystate;
    register player_t *g = p->team[squadra]->attivo;

    if(g->Special || g->Comando)
        return;

    if(!pl->InGioco) {
        NoPlayerControl(g);
        return;
    }

    if (g->team->Joystick < 0) {
        D(bug("HandleControlled with Joystick < 0!\n"));
        return;
    }
    
    joystate=r_controls[g->team->Joystick][counter];

    // Gestione del fire

    if(!need_release[g->team->Joystick])
    {
        if(joystate&JPF_BUTTON_RED)
        {
            need_release[g->team->Joystick]=TRUE;

            if(g==pl->gioc_palla)
            {
                DoShot(g,joystate);

                if(CanScore(g)!=CS_SI)
                    pl->velocita-=4;
            }
            else
                DoSpecials(g);

            return;
        }

        if(joystate&JPF_BUTTON_BLUE)
        {
            need_release[g->team->Joystick]=TRUE;

            if(g==pl->gioc_palla)
            {
                if(joystate&JP_DIRECTION_MASK)
                    Passaggio2(g,GetJoyDirection(joystate));
                else
                    Passaggio2(g,g->dir);
            }
            else DoSpecials(g);

            return;
        }

        if(joystate&JPF_BUTTON_YELLOW)
        {
            need_release[g->team->Joystick]=TRUE;

            if(g==pl->gioc_palla)
            {
                DoLongPass(g,joystate);
            }
            else DoSpecials(g);

            return;
        }

        if(joystate&JPF_BUTTON_GREEN)
        {
            need_release[g->team->Joystick]=TRUE;

            if(g==pl->gioc_palla)
            {
                DoCross(g);
            }
            else DoSpecials(g);

            return;
        }
    }
    else if(!(joystate&(JPF_BUTTON_RED|JPF_BUTTON_YELLOW|JPF_BUTTON_BLUE)))
        need_release[g->team->Joystick]=FALSE;

    if(joystate&JPF_BUTTON_REVERSE)
    {
        if(pl->gioc_palla!=g)
        {
            CheckActive();
        }
    }

    if(joystate&JPF_BUTTON_PLAY)
    {
        DoPause();
        return;
    }

    if(g->ActualSpeed==3 && g->AnimFrame!=0 && g==pl->gioc_palla)
        return;

    g->AnimType=GIOCATORE_RESPIRA;

    /* Gestione delle otto direzioni */

    if( (joystate&JP_DIRECTION_MASK) ) 
    {
        register WORD NewDir=GetJoyDirection(joystate);

        if(g->dir == opposto[NewDir])
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
        else if(g->dir != NewDir)
        {
            if(g->ActualSpeed==3)
            {
                WORD tv=abs(g->dir-NewDir);

                if(tv>4)
                    tv=8-tv;

                if(tv>2)
                {
                    DoSpecialAnim(g,GIOCATORE_CORSA_PARZIALE);

                    if(BestRotation(g->dir,NewDir))
                    {
                        NewDir--;

                        if(NewDir<0)
                            NewDir=7;
                    }
                    else
                    {
                        NewDir++;

                        if(NewDir>7)
                            NewDir=0;
                    }
                }
                else if(tv>1)
                {
                    if(BestRotation(g->dir,NewDir))
                    {
                        NewDir--;

                        if(NewDir<0)
                            NewDir=7;
                    }
                    else
                    {
                        NewDir++;

                        if(NewDir>7)
                            NewDir=0;
                    }

                    g->ActualSpeed--;
                }
            }
            else
            {
                if(g==pl->gioc_palla)
                {
                    WORD tv=abs(g->dir-NewDir);

                    if(tv>4)
                        tv=8-tv;

                    tv--;

                    if(tv>0)
                    {
                        if(BestRotation(g->dir,NewDir))
                        {
                            NewDir-=tv;

                            if(NewDir<0)
                                NewDir=7;
                        }
                        else
                        {
                            NewDir+=tv;

                            if(NewDir>7)
                                NewDir=0;
                        }
                    }
                }
            }

            if(!AllowRotation(g,NewDir))
                g->dir = NewDir;
        }
        else                            
        {
            // Dovrebbe essere superfluo, g->AnimType=GIOCATORE_CORSA_LENTA;

            if(g->ActualSpeed<3)
            {
                if(g->ActualSpeed==2)
                {
                    if(joystate&JPF_BUTTON_FORWARD)
                    {
                        g->WaitForControl++;

                        if(g->WaitForControl>20)
                        {
                            g->WaitForControl=0;
                            g->ActualSpeed++;
                            g->AnimFrame=-1;
                            g->FrameLen=0;
                        }
                    }
                }
                else
                {
                    g->ActualSpeed++;
                    g->AnimFrame=-1;
                    g->FrameLen=0;
                }
            }
        }

        if(g->ActualSpeed==3 && !(joystate&JPF_BUTTON_FORWARD) && g->AnimFrame==0)
            g->ActualSpeed--;
    }
    else if(g->ActualSpeed>0)
    {
        if(g->ActualSpeed==2&&pl->gioc_palla)
        {
            g->WaitForControl++;

            if(g->WaitForControl>18)
            {
                g->ActualSpeed--;
                g->WaitForControl=0;
            }
        }
        else
        {
            if(!(joystate&JPF_BUTTON_FORWARD))
                g->ActualSpeed--;
        }
    }
}

WORD GetJoyDirection(uint32_t joystate)
{
    if(joystate & JPF_JOY_UP)
    {
        if(joystate & JPF_JOY_LEFT)
            return D_NORD_OVEST;

        if(joystate & JPF_JOY_RIGHT)
            return D_NORD_EST;

        return D_NORD;
    }

    if(joystate & JPF_JOY_DOWN)
    {
        if(joystate & JPF_JOY_LEFT)
            return D_SUD_OVEST;

        if(joystate & JPF_JOY_RIGHT)
            return D_SUD_EST;

        return D_SUD;
    }

    if(joystate & JPF_JOY_LEFT)
    {
        return D_OVEST;
    }

    return D_EST;
}

