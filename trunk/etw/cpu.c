#include "eat.h"

// Corretti tick counter e waitfor...

void HandleCPU(int squadra)
{
    HandleRealCPU(p->team[squadra]->attivo);
}

void HandleRealCPU(player_t *g)
{
    if(g->Special || g->Comando)
        return;

        g->WaitForControl--;

        if(g->WaitForControl<0)
        {
                g->WaitForControl=10;

                g->settore = (g->world_y/1450 << 2) + (g->world_x/2560);

                if(pl->gioc_palla==g)
                {
            char schema=g->team->Schema;

            if(g->SNum==1)
            {
                            if(g->settore!=g->team->tactic->Position[1][g->GNum][pl->settore].settore&&GetTable()>0)
                            {
                                    switch(g->settore)
                                    {
                                            case 2:
                            switch(GetTable())
                            {
                                case 5:
                                    CPUShot(g);
                                    break;
                                case 3:
                                case 4:
                                                            TargetedPass(g,0);
                                    break;
                                case 2:
                                    EseguiCross(g);
                                    break;
                                default:
                                                            Passaggio2(g,3);
                            }
                                                    break;
                                            case 3:
                            switch(GetTable())
                            {
                                case 0:
                                                            Passaggio2(g,4);
                                    break;
                                case 5:
                                    CPUShot(g);
                                    break;
                                default:
                                    EseguiCross(g);
                            }
                                                    break;
                                            case 0:
                                            case 1:
                                            case 4:
                                            case 5:
                                            case 6:
                                            case 8:
                                            case 9:
                            if(GetTable()<3)
                                                           Passaggio2(g,2);
                            else
                                                           TargetedPass(g,0L);
                                                    break;
                                            case 10:
                            switch(GetTable())
                            {
                                case 5:
                                    CPUShot(g);
                                    break;
                                case 3:
                                case 4:
                                                            TargetedPass(g,0);
                                    break;
                                case 2:
                                    EseguiCross(g);
                                    break;
                                default:
                                                            Passaggio2(g,1);
                            }
                                                    break;
                                            case 11:
                            switch(GetTable())
                            {
                                case 0:
                                                            Passaggio2(g,0);
                                    break;
                                case 5:
                                    CPUShot(g);
                                    break;
                                default:
                                    EseguiCross(g);
                            }
                                                    break;
                                            case 7:
                            if(GetTable()>1)
                                CPUShot(g);
                            else
                                Passaggio2(g,2);
                            break;
                                    }
                            }
                            else if(g->settore!=7)
                            {
                    switch(GetTable())
                    {
                        case 1:
                            MoveTo(g,CORNER_X_NE,CORNER_Y_N);
                            break;
                        case 2:
                            MoveTo(g,CORNER_X_SE,CORNER_Y_S);
                            break;
                        default:
                            if(schema==SFONDAMENTO_CENTRALE || GetTable()<2 )
                                                MoveTo(g,FONDO_E_X,CENTROCAMPO_Y);  // Punto verso la porta;
                            else if(schema==FASCE_LATERALE_N)
                            {
                                if(SimpleDistance(g->world_x,g->world_y,FONDO_E_X,CORNER_Y_N)>300)
                                                    MoveTo(g,FONDO_E_X,CORNER_Y_N);  // Punto verso la fascia nord;
                                else if(GetTable()>1)
                                    EseguiCross(g);
                                else
                                    Passaggio2(g,4);
                            }
                            else if(schema==FASCE_LATERALE_S)
                            {
                                if(SimpleDistance(g->world_x,g->world_y,FONDO_E_X,CORNER_Y_S)>300)
                                                    MoveTo(g,FONDO_E_X,CORNER_Y_S);  // Punto verso la fascia sud;
                                else if(GetTable()>1)
                                    EseguiCross(g);
                                else
                                    Passaggio2(g,0);
                            }
                            else // per ora questo else indica PALLA_LUNGA
                                Passaggio2(g,2);
                                
                    }
                            }
                            else switch (GetTable())
                            {
                    case 0:
                                        MoveTo(g,FONDO_E_X,CENTROCAMPO_Y);  // Punto verso la porta;
                        break;
                    case 4:
                        Passaggio2(g,g->dir);
                        break;
                    default:
                        CPUShot(g);
                            }
            }
            else
            {
                            if(g->settore!=g->team->tactic->Position[1][g->GNum][pl->settore].settore||GetTable()>4)
                            {
                                    switch(g->settore)
                                    {
                                            case 1:
                            switch(GetTable())
                            {
                                case 5:
                                    CPUShot(g);
                                    break;
                                case 3:
                                case 4:
                                                            TargetedPass(g,0);
                                    break;
                                case 2:
                                    EseguiCross(g);
                                    break;
                                default:
                                                            Passaggio2(g, (schema!=FASCE_LATERALE_N ? 5 : 6) );
                            }
                                                    break;
                                            case 0:
                            switch(GetTable())
                            {
                                case 0:
                                                            Passaggio2(g,4);
                                    break;
                                case 5:
                                    CPUShot(g);
                                    break;
                                default:
                                    EseguiCross(g);
                            }
                                                    break;
                                            case 3:
                                            case 2:
                                            case 7:
                                            case 11:
                                            case 10:
                                            case 5:
                                            case 6:
                            if(GetTable()<3)
                                                           Passaggio2(g,6);
                            else
                                                           TargetedPass(g,0L);
                                                    break;
                                            case 9:
                            switch(GetTable())
                            {
                                case 1:
                                    CPUShot(g);
                                    break;
                                case 3:
                                case 4:
                                                            TargetedPass(g,0);
                                    break;
                                case 2:
                                    EseguiCross(g);
                                    break;
                                default:
                                                            Passaggio2(g, (schema!=FASCE_LATERALE_S ? 7 : 6) );
                            }
                                                    break;
                                            case 8:
                            switch(GetTable())
                            {
                                case 0:
                                                            Passaggio2(g,0);
                                    break;
                                case 5:
                                    CPUShot(g);
                                    break;
                                default:
                                    EseguiCross(g);
                            }
                                                    break;
                                            case 4:
                            if(GetTable()>1)
                                CPUShot(g);
                            else
                                Passaggio2(g,2);
                            break;
                                    }
                            }
                            else if(g->settore!=4)
                            {
                    switch(GetTable())
                    {
                        case 1:
                            MoveTo(g,CORNER_X_NO,CORNER_Y_N);
                            break;
                        case 2:
                            MoveTo(g,CORNER_X_SO,CORNER_Y_S);
                            break;
                        default:
                            if(schema==SFONDAMENTO_CENTRALE || GetTable()<2 )
                                                MoveTo(g,FONDO_O_X,CENTROCAMPO_Y);  // Punto verso la porta;
                            else if(schema==FASCE_LATERALE_N)
                            {
                                if(SimpleDistance(g->world_x,g->world_y,FONDO_O_X,CORNER_Y_N)>300)
                                                    MoveTo(g,FONDO_O_X,CORNER_Y_N);  // Punto verso la fascia nord;
                                else if(GetTable()>1)
                                    EseguiCross(g);
                                else
                                    Passaggio2(g,4);
                            }
                            else if(schema==FASCE_LATERALE_S)
                            {
                                if(SimpleDistance(g->world_x,g->world_y,FONDO_O_X,CORNER_Y_S)>300)
                                                    MoveTo(g,FONDO_O_X,CORNER_Y_S);  // Punto verso la fascia sud;
                                else if(GetTable()>1)
                                    EseguiCross(g);
                                else
                                    Passaggio2(g,0);
                            }
                            else // per ora questo else indica PALLA_LUNGA
                                Passaggio2(g,2);
                    }
                            }
                            else switch (GetTable())
                            {
                    case 4:
                        Passaggio2(g,g->dir);
                        break;
                    case 0:
                                        MoveTo(g,FONDO_O_X,CENTROCAMPO_Y);  // Punto verso la porta;
                        break;
                    default:
                        CPUShot(g);
                            }
            }
                }
                else
                {
            if(/* pl->settore==g->settore && */ pl->InGioco && !pl->Hide &&
                (!pl->gioc_palla || (pl->gioc_palla && pl->gioc_palla->SNum!=g->SNum) )
            )
            {
// Cerco la palla prima dell'uomo se posso.
                if(pl->gioc_palla)
                {
                    if(g->SNum)
                        MoveTo(g,G2P_X(pl->world_x)-80,G2P_Y(pl->world_y));
                    else
                        MoveTo(g,G2P_X(pl->world_x)+80,G2P_Y(pl->world_y));
                }
                else
                        MoveTo(g,G2P_X(pl->world_x),G2P_Y(pl->world_y));
            }
            else
                            MoveTo(g,g->team->tactic->Position[g->team->Possesso][g->GNum][pl->settore].x,g->team->tactic->Position[g->team->Possesso][g->GNum][pl->settore].y);

            if(pl->InGioco && !pl->Hide &&
                 !g->team->Possesso && GetTable()>3)
                DoSpecials(g);
        }
    }
}

