#include "eat.h"

#define OWN_GOAL 64

extern BOOL replay_done;
UBYTE goal_array[GA_SIZE],goal_minute[GA_SIZE],goal_team[GA_SIZE];
char ColoreCartellino[8]={ESPULSIONE,ESPULSIONE,ESPULSIONE,ESPULSIONE,ESPULSIONE,ESPULSIONE,ESPULSIONE,ESPULSIONE};
char golrig[2]={0,0};

struct Pos piazzati[]=
{
	{236*8,115*8},
	{276*8,163*8},
	{283*8,267*8},
	{264*8,364*8},
	{225*8,436*8},
};

Giocatore *PrendiPalla(void)
{
	Giocatore *v=TrovaPiuVicino(pl->sq_palla,G2P_X(pl->world_x),G2P_Y(pl->world_y));

	if(v->Controlled)
	{
		int i;

		for(i=0;i<10;i++)
		{
			if(v->team->giocatore[i].Comando==0&&
				v->team->giocatore[i].AnimType!=GIOCATORE_ESPULSO)
				ChangeControlled(v->team,i);
		}
	}

	SetComando(v,VAI_PALLA,ESEGUI_RIMESSA,0);

	v->Direzione=FindDirection(v->world_x,v->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
	v->Special=FALSE;
	DoAnim(v,GIOCATORE_CORSA_LENTA);
	v->ActualSpeed=1;
	

	return v;
}

void HandleArbitro(void)
{
	register struct Arbitro *g=&p->arbitro;

	g->Tick--;

	if(g->Tick<0)
	{
		if(g->Tick>-25)
			g->Tick=50;

		if(!g->Comando)
		{
		    if(!g->Special)
		    {
			if(SimpleDistance(g->world_x,g->world_y,pl->world_x,pl->world_y)>800 &&
				pl->world_y>(46*8) &&
				pl->world_y<(501*8) &&
				pl->world_x>( 61*8 ) &&
				pl->world_x<( 1219 *8 ) )

			{
				g->Direzione=FindDirection(g->world_x,g->world_y,pl->world_x,pl->world_y);

				if(g->AnimType!=ARBITRO_CORSA)
				{
					g->AnimType=ARBITRO_CORSA;
					g->AnimFrame=-1;
					g->FrameLen=0;
				}
			}
			else
			{
				if(	g->world_y<(40*8) ||
					g->world_y>(480*8) ||
					g->world_x<( 61*8 ) ||
					g->world_x>( 1219 *8 ) )
				{
					g->Direzione=FindDirection(g->world_x,g->world_y,CENTROCAMPO_X,CENTROCAMPO_Y);

					if(g->AnimType!=ARBITRO_CORSA)
					{
						g->AnimType=ARBITRO_CORSA;
						g->AnimFrame=-1;
						g->FrameLen=0;
					}			
				}
				else
				{
					g->Direzione=FindDirection(g->world_x,g->world_y,pl->world_x,pl->world_y);
					g->AnimType=ARBITRO_FERMO;
					g->FrameLen=0;
				}
			}
		    }
		}
		else
		{
			switch(g->Comando)
			{
				case FISCHIA_INIZIO:
					DoSpecialAnim(g,ARBITRO_FISCHIA);
					PlayBackSound(sound[FISCHIO_LUNGO]);
					g->Comando=0;
					break;
				case FISCHIA_FINE:
					if(g->Argomento>0)
					{
						g->Argomento--;
						g->Tick=50;

						pl->InGioco=FALSE;
						TogliPallaMod();

						if(!penalties)
							p->show_panel=PANEL_TIME;

						p->show_time=200;

						DoSpecialAnim(g,ARBITRO_FISCHIA);

						if(g->Argomento>3)
						{
							game_status= (first_half ? S_FINE_PRIMO : S_FINE_SECONDO);

							PlayBackSound(sound[FISCHIO_CORTO]);
						}
						else if(g->Argomento>2)
						{
							PlayBackSound(sound[FISCHIO_LUNGO]);

							if(!penalties)
								p->show_panel=PANEL_TIME|PANEL_RESULT;
							else	
								p->show_panel=PANEL_RESULT;

							g->Tick=120;
						}
						else if(g->Argomento>1&& (!arcade) && (!training) )
						{
							p->show_panel=PANEL_STATS;
							g->Tick=200;
						}
						else if(g->Argomento>0 && (!replay_looped) )
						{
							p->show_panel=PANEL_REPLAY;
							full_replay=TRUE;
							g->Tick=150;

						}
						else if( (!nopari) || (p->team[0]->Reti!=p->team[1]->Reti) )
						{
							g->Comando=0;
							quit_game=TRUE;
							full_replay=FALSE;
//							if(teams_swapped)
//							{
								SetResult("%ld-%ld\n",p->team[1]->Reti,p->team[0]->Reti);
								D(bug("\t\tHandleArbitro: inverto il risultato %s: %d - %s: %d!\n",
									p->team[0]->Nome,p->team[0]->Reti,
									p->team[1]->Nome,p->team[1]->Reti));
//							}
//							else
//							{
//								SetResult("%ld-%ld\n",p->team[0]->Reti,p->team[1]->Reti);
//								D(bug("\t\tHandleArbitro: mantengo il risultato %s: %d - %s: %d!\n",
//									p->team[0]->Nome,p->team[0]->Reti,
//									p->team[1]->Nome,p->team[1]->Reti));
//							}
						}
						else if(!extratime)
						{
							g->Tick=0;
							first_half=TRUE;

							extratime=TRUE;
							SwapTeams();

							time_length/=3;
//							t_l/=3; non lo divido per tre altrimenti arrivo al 45esimo anche nei supplementari

							g->Comando=FISCHIA_RIPRESA;
							pl->settore=KICKOFF;

							p->team[starting_team]->Possesso=1;
							p->team[starting_team^1]->Possesso=0;

							StartGameTime=Timer();
							EndTime=time_length*MY_CLOCKS_PER_SEC+StartGameTime;
						}
						else 
						{
							full_replay=FALSE;
							penalties=TRUE;
							golrig[0]=golrig[1]=0;
							p->team[0]->Falli=p->team[1]->Falli=0;

							StartGameTime=Timer();
							EndTime=60*40*MY_CLOCKS_PER_SEC+StartGameTime; // do 40 minuti per finire i rigori...
							g->Comando=FISCHIA_PREPUNIZIONE;
							p->show_panel=PANEL_RESULT;
							g->Tick=200;
						}
					}
					break;
				case FISCHIA_PRIMO_TEMPO:
					if(g->Argomento>0)
					{
						g->Argomento--;
						g->Tick=40;

						pl->InGioco=FALSE;
						TogliPallaMod();

						p->show_panel=PANEL_TIME;
						p->show_time=200;

						DoSpecialAnim(g,ARBITRO_FISCHIA);

						if(g->Argomento>3)
						{
							PlayBackSound(sound[FISCHIO_CORTO]);
						}
						else if(g->Argomento>2)
						{
							PlayBackSound(sound[FISCHIO_LUNGO]);
							p->show_panel=PANEL_TIME|PANEL_RESULT;
							g->Tick=120;
						}
						else if(g->Argomento>1&&!arcade)
						{
							p->show_panel=PANEL_STATS;
							g->Tick=200;
						}
						else if(g->Argomento>0&&!replay_looped)
						{
							p->show_panel=PANEL_REPLAY;
							full_replay=TRUE;
							g->Tick=150;
						}
						else
						{
#ifdef DEMOVERSION
							g->Comando=0;
							quit_game=TRUE;
							full_replay=FALSE;
#else
							g->Tick=0;

							first_half=FALSE;

							SwapTeams();

							g->Comando=FISCHIA_RIPRESA;
							pl->settore=KICKOFF;

// E' sempre starting team ad avere la palla!
							p->team[starting_team]->Possesso=1;
							p->team[starting_team^1]->Possesso=0;

							StartGameTime=Timer();
							EndTime=time_length*MY_CLOCKS_PER_SEC+StartGameTime;
#endif
						}
					}
					break;
				case FISCHIA_GOL:
					DoSpecialAnim(g,ARBITRO_FISCHIA);

					pl->InGioco=FALSE;

					if( !training || penalties || free_kicks)
						PlayBackSound(sound[FOLLA]);

					PlayBackSound(sound[FISCHIO_CORTO]);

//					SetCrowd(FOLLA);

					pl->sq_palla=NULL;

					if(pl->world_x<CENTROCAMPO_X)
					{
// Gol della squadra 0
						EseguiEsultanza(0);
						EseguiEsultanza(0);
						EseguiEsultanza(0);

						if(p->last_touch>=32)
						{
							p->last_touch|=OWN_GOAL;
						}

						p->team[0]->Reti++;

						if(penalties)
							golrig[0]++;

						p->team[0]->Possesso=0;
						p->team[1]->Possesso=1;

						if(!replay_mode&&!training)
						{
							game_status=S_GOAL_TEAM_A;
							UrgentSpeaker(S_GOAL_TEAM_A);
						}
					}
					else
					{
// Gol della squadra 1
						EseguiEsultanza(1);
						EseguiEsultanza(1);
						EseguiEsultanza(1);

						if(p->last_touch<32)
						{
							p->last_touch|=OWN_GOAL;
						}

						p->team[1]->Reti++;

						if(penalties)
							golrig[1]++;

						p->team[0]->Possesso=1;
						p->team[1]->Possesso=0;

						if(!replay_mode&&!training)
						{
							game_status=S_GOAL_TEAM_B;
							UrgentSpeaker(S_GOAL_TEAM_B);
						}

					}

					pl->settore=KICKOFF;

// Assegno gol e marcatore

					if(!penalties && (p->team[0]->Reti+p->team[1]->Reti)<GA_SIZE )
					{
						mytimer temptime;
						int i=p->team[0]->Reti+p->team[1]->Reti-1;

						goal_array[i]=p->last_touch;

						goal_team[i]= (p->last_touch&32)==0 ? teams_swapped : teams_swapped^1;

						if(p->last_touch&OWN_GOAL)
							goal_team[i]^=1;

						temptime=Timer();

						goal_minute[i]=((((temptime-StartGameTime)/MY_CLOCKS_PER_SEC)*45)/t_l/60);

						if(extratime)
						{
							goal_minute[i]/=3;

							if(first_half)
								goal_minute[i]+=90;
							else
								goal_minute[i]+=105;
						}
						else if(!first_half)
						{
							goal_minute[i]+=45;
						}
						
					}
					
					p->doing_shot=FALSE;

					if(!training)
					{
						MakeResult();

						if(!penalties)
							p->show_panel=PANEL_TIME;
						else
							p->show_panel=PANEL_RESULT;
					}

					if( (!penalties) && (!free_kicks) )
					{
						StopTime();

						g->Tick=120;

						g->Comando=FISCHIA_SCRITTA;
					}
					else
					{
						D(bug("Sono ai rigori o ai calci piazzati...\n"));
						p->show_panel=0L;
						g->Comando=FISCHIA_PREPUNIZIONE;
						g->Tick=150;
					}
					break;
				case FISCHIA_REPLAY:
					g->AnimType=ARBITRO_FERMO;
					g->AnimFrame=0;

					if(use_replay&&!replay_mode)
					{
						game_status=S_INIZIO_REPLAY;
						start_replay=TRUE;
					}

					if(extratime&&golden_gol&& !penalties)
					{
						mytimer temp;

						first_half=FALSE;
						g->Comando=FISCHIA_FINE;
						g->Argomento=6;
						g->Tick=6;
						temp=Timer();
						EndTime=temp+MY_CLOCKS_PER_SEC;

						pl->settore=KICKOFF;
					}
					else
					{
						g->Comando=FISCHIA_RIPRESA;
						pl->settore=KICKOFF;
						g->Tick=100;
					}
					no_record=FALSE;
					break;
				case FISCHIA_SCRITTA:
					g->AnimType=ARBITRO_FERMO;
					g->AnimFrame=0;
					p->show_panel=PANEL_GOAL;

					if(!penalties)
						p->show_panel|=PANEL_TIME;

					p->show_time=GOAL_TIME;
					g->Comando=FISCHIA_REPLAY;
					g->Tick=200;
					break;
				case FISCHIA_KICKOFF:
					{
						BOOL ok=FALSE;
						register Team *s=p->team[0];

						if(p->show_panel&0xff00)
						{
							D(bug("Break per show_panel!\n"));
							g->Tick=30;
							break;
						}

						if(!training && p->team[1]->Possesso)
							s=p->team[1];

						if(!first_kickoff)
						{
							s->giocatore[0].WaitForControl--;

							if(s->giocatore[0].WaitForControl<0)
								ok=TRUE;
						}

						if(s->Joystick<0)
						{
							g->Argomento--;

							if(g->Argomento<=0)
							{
								ok=TRUE;
							}
							else
								g->Tick=2;
						}
						else
						{
							if(r_controls[s->Joystick][counter]&MYBUTTONMASK)
							{
								ok=TRUE;
							}
							else
								g->Tick=2;
						}

						if(ok)
						{
							RestartTime();

							if(first_kickoff)
							{
								mytimer Time;

								Time=Timer();

								EndTime+=(Time-StartGameTime);

								StartGameTime=Time;

								if(arcade)
									PlayBackSound(sound[AS_LETSGO]);

								first_kickoff=FALSE;
							}

							RimuoviComandoSquadra(0,STAI_FERMO);

							if(!training)
								RimuoviComandoSquadra(1,STAI_FERMO);
							
							Passaggio2(pl->gioc_palla,pl->gioc_palla->Direzione);

							if(s->Joystick>=0)
								need_release[s->Joystick]=TRUE;

							pl->delta_y+=2;
							
							pl->InGioco=TRUE;
							g->Comando=0;
						}
					}
					break;
				case METTI_IN_GIOCO:
					pl->InGioco=TRUE;
					break;
				case AMMONIZIONE:
				case ESPULSIONE:
					if(g->Tick<= -25)
					{
						PlayIfNotPlaying(FISCHIO_LUNGO);

// Nel caso l'arbitro sia molto lontano o non presente lo avvicino.

						if(!(detail_level&USA_ARBITRO))
						{
							p->arbitro.world_x=pl->world_x;
							p->arbitro.world_y=pl->world_y;
						}
						else if(p->arbitro.OnScreen==FALSE)
						{
							if(pl->world_x>CENTROCAMPO_X)
								p->arbitro.world_x=(field_x-16)<<3;
							else
								p->arbitro.world_x=(field_x+WINDOW_WIDTH)<<3;

							p->arbitro.world_y=pl->world_y-100;
						}
					}

					pl->Stage=0;

					pl->InGioco=FALSE;

					{
						Giocatore *v;

						if(g->Argomento>=10)
							v=&p->team[1]->giocatore[g->Argomento-10];
						else
							v=&p->team[0]->giocatore[g->Argomento];

						v->ActualSpeed=0;
						v->Comando=STAI_FERMO;

						if(v->AnimType!=GIOCATORE_RESPIRA)
						{
							DoAnim(v,GIOCATORE_RESPIRA);
						}

						if(SimpleDistance(g->world_x,g->world_y,v->world_x,v->world_y)>300)
						{
							g->Direzione=FindDirection(g->world_x,g->world_y,v->world_x,v->world_y);

							if(g->AnimType!=ARBITRO_CORSA)
							{
								DoAnim(g,ARBITRO_CORSA);
							}

							g->Tick=12;
						}
						else
						{
							g->Direzione=FindDirection(g->world_x,g->world_y,v->world_x,v->world_y);

							if(g->Comando!=ColoreCartellino[g->Direzione])
							{
								UBYTE from,to;

								from= ColoreCartellino[g->Direzione]==ESPULSIONE ? Pens[P_ROSSO0] : Pens[P_GIALLO];
								to= g->Comando==ESPULSIONE ? Pens[P_ROSSO0] : Pens[P_GIALLO];

								if(detail_level&USA_ARBITRO)
									RemapMColor(g->immagine->Frames[80+g->Direzione],from,to);

								ColoreCartellino[g->Direzione]=g->Comando;
							}

							g->Tick=200;

							if(g->Comando==ESPULSIONE)
							{
								if(!replay_mode)
									UrgentSpeaker(S_ESPULSIONE);

								g->Tick+=100;
								v->Special=FALSE;
								DoAnim(v,GIOCATORE_RESPIRA);
								v->Comando=ESCI_CAMPO;
							}
							else 
							{
								if(!replay_mode)
									UrgentSpeaker(S_AMMONIZIONE);

								v->Comando=NESSUN_COMANDO;
								v->Ammonito=TRUE;
							}

							g->Comando=BATTI_FALLO;

							DoSpecialAnim(g,ARBITRO_CARTELLINO);

							if(GetTable()>1)
								SetCrowd(BOOU);
						}
					}
					break;
				case FISCHIA_PREPUNIZIONE:
					p->doing_shot=FALSE;
					p->goal=FALSE;
					g->AnimType=ARBITRO_FERMO;
					g->AnimFrame=0;
					g->Tick=0;
					pl->InGioco=FALSE;
					TogliPalla();
					FermaPalla();
					ShowBall();

					if(extratime)
					{
						SwapTeams();

						if( (	p->team[1]->Falli>=5 &&
							p->team[0]->Reti!=p->team[1]->Reti &&
							p->team[0]->Falli==p->team[1]->Falli) ||
						    	p->team[1]->Reti>(p->team[0]->Reti+5-p->team[0]->Falli) ||
							p->team[0]->Reti>(p->team[1]->Reti+5-p->team[1]->Falli)
							)
						{
							g->Comando=FISCHIA_FINE;
							g->Argomento=3;
							break;
						}
					}
					else if(p->team[1]->Falli>=5)
					{
						D(bug("Battuti 5 rigori, fine allenamento!\n"));
						g->Comando=FISCHIA_FINE;
						g->Argomento=3;
						break;
					}

					p->team[1]->portiere.AnimType=PORTIERE_FERMO;
					p->team[1]->portiere.Special=FALSE;


					pl->sq_palla=p->team[0];
					p->team[0]->Possesso=1;
					p->team[1]->Possesso=0;
					p->arbitro.Comando=BATTI_FALLO;

					if(penalties)
					{
						pl->world_x=RIGORE_X_O;
						pl->world_y=RIGORE_Y;
						pl->settore=PENALTY;
					}
					else
					{
						pl->world_x=piazzati[p->team[1]->Falli].x;
						pl->world_y=piazzati[p->team[1]->Falli].y;
						pl->settore = (pl->world_y/1450 << 2) + (pl->world_x/2560);
					}
					g->world_x=pl->world_x;
					g->world_y=pl->world_y-320;
					g->Direzione=4;
					p->flash_pos=0;
					p->flash_mode=TRUE;
					DisponiSquadra(p->team[0],pl->settore,TRUE);
					DisponiSquadra(p->team[1],pl->settore,FALSE);
					break;
				case FISCHIA_OFFSIDE:
					if(g->Argomento<40)
					{
						StopTime();
						p->show_panel=PANEL_TIME|PANEL_OFFSIDE;
						p->show_time=150;
						g->Tick=75;
						g->Comando=FISCHIA_OFFSIDE;	
						PlayBackSound(sound[FISCHIO_LUNGO]);
						DoSpecialAnim(g,ARBITRO_FISCHIA);
						GL_Fuori(2);
						g->Argomento+=40;
					}
					else
					{
						Team *s;

						g->Argomento-=40;

						if(g->Argomento>=10)
						{
							s=p->team[1];
							g->Argomento-=10;
							p->team[0]->Possesso=1;
							p->team[1]->Possesso=0;
						}	
						else
						{
							s=p->team[0];
							p->team[0]->Possesso=0;
							p->team[1]->Possesso=1;
						}

						TogliPalla();
						FermaPalla();
						ShowBall();

						pl->world_x=G2P_X(s->giocatore[g->Argomento].world_x);
						pl->world_y=G2P_Y(s->giocatore[g->Argomento].world_y);

						g->Tick=25;
						g->Comando=BATTI_FALLO;
					}
					break;
				case FISCHIA_FALLO:
					if(penalties||free_kicks)
					{
						g->Tick=0;
						g->Comando=FISCHIA_PREPUNIZIONE;
						break;
					}

					StopTime();
					g->Comando=BATTI_FALLO;
					pl->InGioco=FALSE;
					DoSpecialAnim(g,ARBITRO_FISCHIA);

					GL_Fuori(2); // Equivale ad un fallo

					PlayBackSound(sound[FISCHIO_LUNGO]);
					g->Tick=60; // Diamo tempo al giocatore di rialzarsi
					break;
				case BATTI_FALLO:

					if(p->player_injuried)
					{
// Occhio, in questa funzione g e' player_injuried, non l'arbitro!

						Giocatore *g=p->player_injuried;
						struct Team *s=g->team;
						int j=g->SNum;

						if(s->Joystick<0)
						{
							g->Tick=50;

							if(!g->Comando&!g->Special)
							{
								int k;
								int best=666;

// Algoritmo che decide la sostituzione del computer.
								if(!replay_mode)
								{
									for(k=0;k<TotaleRiserve[j];k++)
									{
										if(Riserve[j][k].Ammonizioni<2&&
											Riserve[j][k].Infortuni<2 && NumeroDiverso(s,Riserve[j][k].Numero) )
										{
											char t1=g->Posizioni&(P_DIFESA|P_CENTRO|P_ATTACCO),
												t2=Riserve[j][k].Posizioni&(P_DIFESA|P_CENTRO|P_ATTACCO);

											if(best==666 || (t1&t2) )
												best=k;
										}
									}

/* questo trucco e' necessario per non perdere la coerenza dei replay!!! Uso il joystick NON usato dal
 * giocatore umano, potrebbe richiedere una modifica quando implementero' la modalita' a 4 giocatori!
 */

// Le if sono dovute al fatto che se si gioca computer contro computer il codice seguente causava un enforcer hit!

									if(p->team[j^1]->Joystick>=0)
										r_controls[p->team[j^1]->Joystick^1][counter]=best;
									else
										r_controls[j][counter]=best;
								}
								else
								{
									if(p->team[j^1]->Joystick>=0)
										best=r_controls[p->team[j^1]->Joystick^1][counter];
									else
										best=r_controls[j][counter];
								}

								if(best!=666)
								{
									D(bug("Setto il pannello di sostituzione per il computer\n"));
									g->CA[0]=best+1;
									p->show_panel=PANEL_SUBSTITUTION_3;
									p->show_time=300;
									g->Comando=ESCI_CAMPO;
								}
								else
								{
									s->NumeroRiserve=0;
									D(bug("Sostituzione non effettuata per mancanza di riserve!\n"));
									p->player_injuried=NULL;
									g->Comando=NESSUN_COMANDO;
								}
							}
						}

						break;
					}

					g->Comando=0;
					pl->MaxQuota=7;

// Gestisco qui anche i rigori!

					if(p->team[0]->Possesso)
					{

						p->team[1]->Falli++;

						if(InArea(1,pl->world_x,pl->world_y))
						{
// y-136 = -31x + 6293
							if(!penalties)
								p->team[0]->Rigori++;
							else
							{
								p->show_panel=PANEL_RESULT;
								p->show_time=150;
							}

							if(!use_speaker&&!penalties)
								PlayBackSound(sound[RIGORE]);
							else
							{
								if(!replay_mode)
									UrgentSpeaker(S_CALCIO_DI_RIGORE);
							}

							pl->settore=PENALTY;
							pl->world_x=RIGORE_X_O;
							pl->world_y=RIGORE_Y;
						}
						pl->sq_palla=p->team[0];

						if(p->team[1]->attivo->Comando!=ESCI_CAMPO&&InArea(1,P2G_X(p->team[1]->attivo->world_x),P2G_Y(p->team[1]->attivo->world_y)))
							p->team[1]->attivo->Comando=ESCI_AREA;
					}
					else
					{

						p->team[0]->Falli++;

						if(InArea(0,pl->world_x,pl->world_y))
						{
// y-136 = 31x - 33139
							p->team[1]->Rigori++;

							PlayBackSound(sound[RIGORE]);
							pl->settore=PENALTY;
							pl->world_x=RIGORE_X_E;
							pl->world_y=RIGORE_Y;
						}
						pl->sq_palla=p->team[1];

						if(p->team[0]->attivo->Comando!=ESCI_CAMPO&&InArea(0,P2G_X(p->team[0]->attivo->world_x),P2G_Y(p->team[0]->attivo->world_y)))
							p->team[0]->attivo->Comando=ESCI_AREA;
					}

					{
						Giocatore *g1=TrovaPiuVicino(pl->sq_palla,pl->world_x,pl->world_y);

						p->mantieni_distanza=TRUE;

						if(CanScore(g1)&&pl->settore!=PENALTY)
						{
//							SetCrowd(CORO1);

							p->flash_mode=TRUE;

							DisponiSquadra(p->team[0],pl->settore,p->team[0]->Possesso);
							DisponiSquadra(p->team[1],pl->settore,p->team[1]->Possesso);

							g1->Direzione=FindDirection(g1->world_x,g1->world_y, (g1->SNum ? PORTA_E_X : PORTA_O_X) ,PORTA_E_Y);

							PreparaBarriera(g1->SNum^1);

							g1->world_x=pl->world_x-avanzamento_x[g1->Direzione];
							g1->world_y=pl->world_y-avanzamento_y[g1->Direzione];

							DoSpecialAnim(g1,GIOCATORE_BATTUTA);
						}
						else 
						{
							g1=PrendiPalla();

							g1->CA[0]=ESEGUI_BATTUTA;
						}
					}

					break;
				case FISCHIA_RIGORE:
					g->Comando=0;
					break;
				case FISCHIA_FUORI:
					if(penalties||free_kicks)
					{
						g->Tick=60;
						g->Comando=FISCHIA_PREPUNIZIONE;
						break;
					}

					DoSpecialAnim(g,ARBITRO_FISCHIA);
					PlayBackSound(sound[FISCHIO_CORTO]);
					g->Tick=100;
					g->Comando=FISCHIA_RIPRESA;	
					break;
				case FISCHIA_RIPRESA:

					if(p->show_panel&0xff00)
					{
						g->Tick=30;
						break;
					}

					DoSpecialAnim(g,ARBITRO_FISCHIA);
					FermaPalla();
					p->sopra_rete=FALSE;
					pl->quota=0;

					StopTime();

					switch(pl->settore)
					{
						case CORNER_N:
							pl->world_y=CORNER_Y_N;

							pl->Direzione=127;

							if(p->team[0]->Possesso)
							{
								p->team[0]->Corner++;
								pl->world_x=CORNER_X_NO;
							}
							else
							{
								p->team[1]->Corner++;
								pl->world_x=CORNER_X_NE;
							}

							p->flash_mode=TRUE;

							p->show_panel=PANEL_RESULT|PANEL_TIME;
							p->show_time=100;

							DisponiSquadra(p->team[0],CORNER_N,p->team[0]->Possesso);

							DisponiSquadra(p->team[1],CORNER_N,p->team[1]->Possesso);

							{
								Giocatore *v;

								if(training)
									pl->sq_palla=p->team[0];

								v=PrendiPalla();

								v->CA[0]=NESSUN_COMANDO;

								v->Direzione=D_SUD;
								v->Comando=NESSUN_COMANDO;

								DoSpecialAnim(v,GIOCATORE_BATTUTA);

								v->ActualSpeed=0;
								v->world_x=pl->world_x-avanzamento_x[v->Direzione];
								v->world_y=pl->world_y-avanzamento_y[v->Direzione];
								pl->MaxQuota=7;
							}	
							ShowBall();
							break;
						case CORNER_S:
							pl->world_y=CORNER_Y_S;

							p->show_panel=PANEL_RESULT|PANEL_TIME;
							p->show_time=100;

							if(p->team[0]->Possesso)
							{
								p->team[0]->Corner++;
								pl->Direzione=4;
								pl->world_x=CORNER_X_SO;
							}
							else
							{
								pl->Direzione=252;
								p->team[1]->Corner++;
								pl->world_x=CORNER_X_SE;
							}

							p->flash_mode=TRUE;
							DisponiSquadra(p->team[0],CORNER_S,p->team[0]->Possesso);
							DisponiSquadra(p->team[1],CORNER_S,p->team[1]->Possesso);

							{
								Giocatore *v;

								v=PrendiPalla();

								v->CA[0]=NESSUN_COMANDO;

								v->Comando=NESSUN_COMANDO;
								v->Direzione=D_NORD;

								DoSpecialAnim(v,GIOCATORE_BATTUTA);

								v->ActualSpeed=0;
								v->world_x=pl->world_x-avanzamento_x[v->Direzione];
								v->world_y=pl->world_y-avanzamento_y[v->Direzione];
								pl->MaxQuota=7;
							}
							ShowBall();
							break;
						case GOALKICK:
							if(pl->world_y>CENTROCAMPO_Y)
							{
								pl->world_y=GOALKICK_Y_S;

								if(p->team[1]->Possesso)
								{
									pl->world_x=GOALKICK_X_SO;
								}
								else
								{
									pl->world_x=GOALKICK_X_SE;
								}
							}
							else
							{
								pl->world_y=GOALKICK_Y_N;

								if(p->team[1]->Possesso)
								{
									pl->world_x=GOALKICK_X_NO;
								}
								else
								{
									pl->world_x=GOALKICK_X_NE;
								}
							}

							if(p->team[1]->Possesso)
							{
								p->team[1]->portiere.Direzione=D_EST;

								if(p->team[1]->Joystick<0)
									p->team[1]->portiere.Tick=30+(GetTable()<<3);
								else
									p->team[1]->portiere.Tick=120;

								DoSpecialAnim((&p->team[1]->portiere),PORTIERE_FERMO);

								if(InArea(1,P2G_X(p->team[0]->attivo->world_x),P2G_Y(p->team[0]->attivo->world_y)))
									p->team[0]->attivo->Comando=ESCI_AREA;

							}
							else
							{
								DoSpecialAnim((&p->team[0]->portiere),PORTIERE_FERMO);

								if(p->team[0]->Joystick<0)
									p->team[0]->portiere.Tick=30+(GetTable()<<3);
								else
									p->team[0]->portiere.Tick=120;

								p->team[0]->portiere.Direzione=D_OVEST;

								if(InArea(0,P2G_X(p->team[1]->attivo->world_x),P2G_Y(p->team[1]->attivo->world_y)))
									p->team[1]->attivo->Comando=ESCI_AREA;
							}

							p->flash_mode=TRUE;
							p->show_panel=PANEL_RESULT|PANEL_TIME;
							p->show_time=100;

							DisponiSquadra(p->team[0],GOALKICK,p->team[0]->Possesso);
							DisponiSquadra(p->team[1],GOALKICK,p->team[1]->Possesso);

							ShowBall();
							break;
						case KICKOFF:
							game_status=S_CALCIO_D_INIZIO;
							
							PlayBackSound(sound[FISCHIO_CORTO]);
							g->Comando=FISCHIA_KICKOFF;
							g->Tick=1;
						
							g->Argomento=10+(GetTable()<<1);

							if(first_kickoff)
								g->Argomento+=15;

							if(detail_level&USA_GUARDALINEE)
							{
								guardalinee[0].world_x=guardalinee[1].world_x=CENTROCAMPO_X;
							}

							p->goal=FALSE;
							pl->world_x=CENTROCAMPO_X;
							pl->world_y=CENTROCAMPO_Y;
							pl->TipoTiro=TIRO_RASOTERRA;
							pl->Direzione=128;
							pl->MaxQuota=1;
							p->flash_mode=TRUE;

							replay_done=FALSE;

							p->show_panel=PANEL_KICKOFF|PANEL_TIME;
							p->show_time=100;

							DisponiSquadra(p->team[0],KICKOFF,p->team[0]->Possesso);
							DisponiSquadra(p->team[1],KICKOFF,p->team[1]->Possesso);

							{
								int i;
								int j;
								Giocatore *g1,*g2;
								Team *s=p->team[0] /*,*ns=p->team[1]*/ ;

								if(p->team[1]->Possesso && !training)
								{
									s=p->team[1];
/*									ns=p->team[0]; 

 Non uso ns...
 */
								}

								s->Possesso=1;

								if(s->Joystick>=0&&!first_kickoff)
									s->giocatore[0].WaitForControl=150;

								g1=TrovaPiuVicino(s,CENTROCAMPO_X,CENTROCAMPO_Y-100);
								g1->world_x=CENTROCAMPO_X-30;
								g1->world_y=CENTROCAMPO_Y-120;
								g2=TrovaPiuVicinoGiocatore(g1);
								g2->world_x=CENTROCAMPO_X-10;
								g2->world_y=CENTROCAMPO_Y+40;
								g1->Direzione=D_SUD;
								g2->Direzione=D_NORD;
								DoAnim(g2,GIOCATORE_RESPIRA);
/*
								g1->Direzione=FindDirection(g1->world_x,g1->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
								g2->Direzione=FindDirection(g2->world_x,g2->world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
*/
								for(j=0;j<2;j++)
								{
									s=p->team[j];

									for(i=0;i<10;i++)
									{
										if(s->giocatore[i].Comando!=ESCI_CAMPO		&&
											s->giocatore[i].Comando!=STAI_FERMO	&&
											s->giocatore[i].AnimType!=GIOCATORE_ESPULSO)
										{
											s->giocatore[i].Comando=STAI_FERMO;
											s->giocatore[i].ActualSpeed=0;
											DoAnim((&s->giocatore[i]),GIOCATORE_FERMO);
										}
									}
								}
							
								DoAnim(g1,GIOCATORE_RESPIRA);
								DoAnim(g2,GIOCATORE_RESPIRA);
								TogliPalla();
								DaiPalla(g1);
							}
							ShowBall();
							break;
						default:

							pl->world_x=g->Argomento;

							if(pl->world_y>CENTROCAMPO_Y)
								pl->world_y=RIMESSA_Y_S;
							else
								pl->world_y=RIMESSA_Y_N;

							PrendiPalla();
							ShowBall();
							break;
					}

					if(g->Comando!=FISCHIA_KICKOFF)
						g->Comando=0;
					break;
				default:
					PlayBackSound(sound[SHOOT]);
					g->Comando=0;
					break;
			}

		}
	}

	if(g->AnimType==ARBITRO_CORSA)
	{
		g->world_x+=velocita_x[1][g->velocita][g->Direzione];
		g->world_y+=velocita_y[1][g->velocita][g->Direzione];
	}

	
	if(g->FrameLen<=0)
	{
		g->FrameLen=ArbAnim[g->AnimType].FrameLen;
		g->AnimFrame++;

		if(g->AnimFrame>=ArbAnim[g->AnimType].Frames)
		{
			if(g->Special)
			{
				g->AnimType=ARBITRO_FERMO;
				g->Special=FALSE;
			}
			g->AnimFrame=0;
		}

		if(g->OnScreen&&(detail_level&USA_ARBITRO))
			ChangeAnimObj(g->immagine,ArbAnim[g->AnimType].Frame[g->AnimFrame]+g->Direzione);
	}

	g->FrameLen--;
}

#define FRAMES_ARBITRO 11

void HandleGuardalinee()
{
	register int i;

// Il guardalinee 0 sta sopra, quello 1 sotto!

	for(i=0;i<2;i++)
	{
		register GuardaLinee *g=&guardalinee[i];

		g->Tick--;

		if(!g->Special&&g->Tick<0)
		{
/* Movimento dei guardalinee, il 0 si muove solo se la palla e' a sinistra del centrocampo, il n.1
   solo se e' a destra)
 */

			if( (i && pl->world_x<CENTROCAMPO_X && pl->world_x>0 ) ||
				(!i && pl->world_x>CENTROCAMPO_X && pl->world_x<(FIELD_WIDTH*8) )
			)
			{
				if(g->world_x< (pl->world_x-300) )
				{
					if(g->AnimType!=GL_CORSA)
					{
						DoAnim(g,GL_CORSA);
						g->Direzione=2;
					}
					else if(g->Direzione!=2)
					{
						DoSpecialAnim(g,GL_INVERSIONE);
					}
				}
				else if(g->world_x > (pl->world_x+300) )
				{
					if(g->AnimType!=GL_CORSA)
					{
						DoAnim(g,GL_CORSA);
						g->Direzione=6;
					}
					else if(g->Direzione!=6)
					{
						DoSpecialAnim(g,GL_INVERSIONE);
					}	
				}
				else
				{
					DoAnim(g,GL_FERMO);

					if(i)
						g->Direzione=6;
					else
						g->Direzione=2;
				}
			}
			else
			{
				DoAnim(g,GL_FERMO);

				if(i)
					g->Direzione=6;
				else
					g->Direzione=2;
			}

			g->Tick=24;
		}

		if(g->AnimType==GL_CORSA)
		{
			g->world_x+=velocita_x[1][4][g->Direzione];
//			g->world_y+=velocita_y[1][4][g->Direzione];
		}

		if(g->FrameLen<=0)
		{
			g->FrameLen=GLAnim[g->AnimType].FrameLen;
			g->AnimFrame++;

			if(g->AnimFrame>=GLAnim[g->AnimType].Frames)
			{
				if(g->Special)
				{
					g->AnimType=GL_FERMO;
					g->Special=FALSE;
				}
				g->AnimFrame=0;
			}

			if(g->OnScreen)
				ChangeAnimObj(g->immagine,GLAnim[g->AnimType].Frame[g->AnimFrame]+(g->Direzione>>2));

			// il >>2 di g->Direzione e' dovuto perche' ci sono solo DUE direzioni, 2 (ovest), 6 (est).
		}
		g->FrameLen--;
	}
}
