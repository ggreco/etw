#include "preinclude.h"
#include "eat.h"
#include "network.h"

extern struct Window *win;
SDL_Joystick *joy[2] = {NULL, NULL};
char joybuttons[2];
char joycfg_buttons[2][8];
char num_joys=0;
BOOL has_joystick=FALSE;
extern unsigned long NetJoyPos[];

extern void ResizeWin(SDL_Event *);

#ifdef DEMOVERSION
void WaitOrKey(int secs)
{
	int i;
	SDL_event ev;

	while(secs>0)
	{
		for(i=0;i<5;i++)
		{
			BOOL ok=TRUE;

			os_delay(10);

			if(SDL_PollEvent(&ev))
			{
				if(ev.type==SDL_MOUSEBUTTONDOWN ||
					ev.type==SDL_KEYDOWN )
					ok=FALSE;
			}
			if( (MyReadPort0(0)&MYBUTTONMASK) ||
				(MyReadPort1(1)&MYBUTTONMASK) )
				ok=FALSE;

			if(ok==FALSE)
				return;
		}
		secs--;
	}
}

#endif

ULONG ReadNetworkPort(ULONG l)
{
//    fprintf(stderr, "Read network %ld: %ld\n", l, NetJoyPos[l]);
	return NetJoyPos[l];
}

ULONG MyReadJoyPort(ULONG l)
{	
//	extern struct joy_config joycfg[];
	int xc,yc;
	ULONG mask=0;

	if(l<0 || !joy[l])
		return 0L;

	if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][0]))
		mask=JPF_BUTTON_RED;

	if(joybuttons[l]>1)
	{
		if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][1]))
			mask|=JPF_BUTTON_BLUE;

		if(joybuttons[l]>5)
		{
			if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][2]))
				mask|=JPF_BUTTON_YELLOW;
			if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][3]))
				mask|=JPF_BUTTON_GREEN;
			if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][5]))
				mask|=JPF_BUTTON_FORWARD;
			if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][4]))
				mask|=JPF_BUTTON_REVERSE;

			if(joybuttons[l]>6)
			{
				if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][6]))
					mask|=JPF_BUTTON_PLAY;
			}
		}
	}

	xc=SDL_JoystickGetAxis(joy[l],0);
	yc=SDL_JoystickGetAxis(joy[l],1);

	if(xc>3000)
		mask|=JPF_JOY_RIGHT;
	else if(xc<-3000)
		mask|=JPF_JOY_LEFT;

	if(yc>3000)
		mask|=JPF_JOY_DOWN;
	else if(yc<-3000)
		mask|=JPF_JOY_UP;

	if(!network_game)
		return mask;
	else {
		SendNetwork(mask);
		return NetJoyPos[l];
	}
}


void free_joyports(void)
{
	int i;

	for(i=0;i<num_joys;i++)
	{
		if(!SDL_JoystickOpened(i))
			num_joys++;
		else
			SDL_JoystickClose(joy[i]);
	}

	has_joystick=FALSE;
	num_joys=0;
}


BOOL os_check_joy(int i)
{
	SDL_Joystick *j;

	if((j=SDL_JoystickOpen(i))) {
		SDL_JoystickClose(j);
		return TRUE;
	}

	return FALSE;
}

int os_get_joy_button(int i)
{
	SDL_Joystick *j;

	
	SDL_JoystickEventState(SDL_ENABLE);

	if ((j=SDL_JoystickOpen(i))) {
		int k=-1;
		BOOL ok=FALSE;
		SDL_Event e;


		while(SDL_PollEvent(&e));

		while(!ok)
		{
			SDL_WaitEvent(&e);

			if(e.type==SDL_JOYBUTTONDOWN)
			{
				k=e.jbutton.button;
				ok=TRUE;
			}

			if(e.type==SDL_KEYUP)
				ok=TRUE;
		}

		SDL_JoystickClose(j);

		SDL_JoystickEventState(SDL_DISABLE);

		return k;
	}

	SDL_JoystickEventState(SDL_DISABLE);

	return -1;
}

void set_controls(void)
{
	p->squadra[0]->Joystick=player_type[0];

	if(control[0]>=CTRL_KEY_1)
	{
		use_key0=TRUE;
		LoadKeyDef(0,KEY_RED_FILE);
	}

	if(control[1]>=CTRL_KEY_1)
	{
		use_key1=TRUE;
		LoadKeyDef(1,KEY_BLUE_FILE);
	}

	if(p->squadra[0]->Joystick>=0)
	{
		if(control[p->squadra[0]->Joystick]<CTRL_KEY_1)
		{
			D(bug("Opening joystick %ld\n",p->squadra[0]->Joystick));

			if ((joy[p->squadra[0]->Joystick]=SDL_JoystickOpen(p->squadra[0]->Joystick))) {
				num_joys++;
				has_joystick=TRUE;

				D(bug("Opened Joystick %ld\n",p->squadra[0]->Joystick));
				D(bug("Name: %s\n", SDL_JoystickName(p->squadra[0]->Joystick)));
				D(bug("Number of Axes: %ld\n", SDL_JoystickNumAxes(joy[p->squadra[0]->Joystick])));
				D(bug("Number of Hats: %ld\n", SDL_JoystickNumAxes(joy[p->squadra[0]->Joystick])));
				D(bug("Number of Buttons: %ld\n", SDL_JoystickNumButtons(joy[p->squadra[0]->Joystick])));
				D(bug("Number of Balls: %ld\n", SDL_JoystickNumBalls(joy[p->squadra[0]->Joystick])));
				D(bug("Joystick event manager: %ld\n",SDL_JoystickEventState(SDL_QUERY)));

				if(SDL_JoystickEventState(SDL_QUERY)==SDL_ENABLE)
				{
					D(bug("** Forzo la disabilitazione dell'event manager x i joystick\n"));
					SDL_JoystickEventState(SDL_DISABLE);
				}

				joybuttons[p->squadra[0]->Joystick]=SDL_JoystickNumButtons(joy[p->squadra[0]->Joystick]);
			}
			else {
				// Prevedo fallback a tastiera o ritorno un errore?
				D(bug(" ->Error opening joystick!\n"));
                has_joystick = FALSE;
			}
		}

		if(control[p->squadra[0]->Joystick]==CTRL_JOYPAD ||
			control[p->squadra[0]->Joystick]==CTRL_KEY_2)
		{
			HandleSquadra0=HandleControlledJoyPad;
//			SetJoyPortAttrs(p->squadra[0]->Joystick,SJA_Type,SJA_TYPE_GAMECTLR,TAG_DONE);
			D(bug("Joypad for team 0\n"));
		}
		else
		{
//			SetJoyPortAttrs(p->squadra[0]->Joystick,SJA_Type,SJA_TYPE_JOYSTK,TAG_DONE);

			if(control[p->squadra[0]->Joystick]==CTRL_JOY2B || 
				control[p->squadra[0]->Joystick]==CTRL_KEY_1)
			{
				HandleSquadra0=HandleControlledJ2B;
			}
			else
			{
				HandleSquadra0=HandleControlled;
			}
		}
	}
	else
	{
		HandleSquadra0=HandleCPU;

		if(nocpu)
			p->squadra[0]->Joystick=TYPE_JOYSTICK1;
	}

	if(player_type[1]==player_type[0]&&player_type[0]!=TYPE_COMPUTER)
	{
			player_type[1]^=1;
	}

	p->squadra[1]->Joystick=player_type[1];

	if(p->squadra[1]->Joystick<0) {
		HandleSquadra1=HandleCPU;

		if(nocpu)
			p->squadra[1]->Joystick=TYPE_JOYSTICK1;
	}
	else {
		if(control[p->squadra[1]->Joystick]<CTRL_KEY_1) {
			D(bug("Opening joystick %ld\n",p->squadra[1]->Joystick));

			if ((joy[p->squadra[1]->Joystick]=SDL_JoystickOpen(p->squadra[1]->Joystick))) {
				has_joystick=TRUE;
				num_joys++;
				D(bug("Opened Joystick %ld\n",p->squadra[1]->Joystick));
				D(bug("Name: %ld\n", SDL_JoystickName(p->squadra[1]->Joystick)));
				D(bug("Number of Axes: %ld\n", SDL_JoystickNumAxes(joy[p->squadra[1]->Joystick])));
				D(bug("Number of Buttons: %ld\n", SDL_JoystickNumButtons(joy[p->squadra[1]->Joystick])));
				D(bug("Number of Balls: %ld\n", SDL_JoystickNumBalls(joy[p->squadra[1]->Joystick])));
				D(bug("Joystick event manager: %ld\n",SDL_JoystickEventState(SDL_QUERY)));

				if(SDL_JoystickEventState(SDL_QUERY)==SDL_ENABLE) {
					D(bug("** Forzo la disabilitazione dell'event manager x i joystick\n"));
					SDL_JoystickEventState(SDL_DISABLE);
				}
 
				joybuttons[p->squadra[1]->Joystick]=SDL_JoystickNumButtons(joy[p->squadra[1]->Joystick]);
			}
			else {
				D(bug(" ->Error opening joystick!\n"));
				// Vedi sopra!
			}
		}
		if(control[p->squadra[1]->Joystick]==CTRL_JOYPAD ||
			control[p->squadra[1]->Joystick]==CTRL_KEY_2) {
			HandleSquadra1=HandleControlledJoyPad;
//			SetJoyPortAttrs(p->squadra[1]->Joystick,SJA_Type,SJA_TYPE_GAMECTLR,TAG_DONE);
			D(bug("Joypad for team 1\n"));
		}
		else 
		{
//			SetJoyPortAttrs(p->squadra[1]->Joystick,SJA_Type,SJA_TYPE_JOYSTK,TAG_DONE);

			if(control[p->squadra[1]->Joystick]==CTRL_JOY2B ||
				control[p->squadra[1]->Joystick]==CTRL_KEY_1)
			{
				HandleSquadra1=HandleControlledJ2B;
			}
			else
			{
				HandleSquadra1=HandleControlled;

			}
		}
	}
}

int os_wait_end_pause(void)
{
	extern SDL_Event lastevent;
	int ok=FALSE;
	Uint8 key;

	switch(lastevent.type)
	{
		case SDL_QUIT:
			ok=2;
			break;
		case SDL_KEYDOWN:
			key=lastevent.key.keysym.sym;

			if(key==SDLK_ESCAPE)
				ok=2;
			if(key==SDLK_p)
				ok=TRUE;
			break;
		case SDL_VIDEORESIZE:
			ResizeWin(&lastevent);
			DrawPause();
			ScreenSwap();

			if(triple_buffering)
				ScreenSwap();

			break;
        default:
          // not handled keys...
            break;
	}

	return ok;
}


void CheckKeys(void)
{
	SDL_Event e;

	while(SDL_PollEvent(&e))
	{
	switch(e.type)
	{
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
		case SDL_JOYAXISMOTION:
			D(bug("Eventi joystick, non dovrei prenderli!!!!\n"));
			break;

		case SDL_QUIT:
			SetResult("break");
            final = FALSE;
			quit_game=TRUE;
			break;

		case SDL_VIDEORESIZE:
			ResizeWin(&e);
			break;
		case SDL_KEYDOWN:
            if(!(p->show_panel&PANEL_CHAT)) {
                switch(e.key.keysym.sym)
                {
#ifndef DEBUG_DISABLED

                    // I - per debug, mostra le posizioni degli omini

                    case SDLK_i:
                        {
                            Squadra *s=p->squadra[1];
                            int i;

                            for(i=0;i<10;i++)
                            {
                                D(bug("G: %ld A: %ld S: %ld C: %ld\n",i+2,s->giocatore[i].AnimType,s->giocatore[i].Special,s->giocatore[i].Comando));

                                if(s->giocatore[i].world_x<0 || s->giocatore[i].world_y<0 ||
                                        s->giocatore[i].world_x>11000 || s->giocatore[i].world_y>4200 )
                                {
                                    int j;
                                    Giocatore *g=&s->giocatore[i];

                                    D(bug("Parametri: Sp:%ld Cmd:%ld Dir:%ld Anim:%ld Spd:%ld Sect:%ld\n",
                                                g->Special,g->Comando,g->Direzione,g->AnimType,g->ActualSpeed,g->settore));

                                    for(j=0;j<(SECTORS+SPECIALS);j++)
                                    {
                                        D(bug("S:%ld - senza: X:%ld Y:%ld - con: X:%ld Y:%ld\n",j,
                                                    g->squadra->tattica->Position[0][i][j].x,g->squadra->tattica->Position[0][i][j].y,
                                                    g->squadra->tattica->Position[1][i][j].x,g->squadra->tattica->Position[1][i][j].y));
                                    }
                                }
                            }
                        }
                        break;
                    case SDLK_1:
                        PlayBackSound(sound[DOG]);
                        break;
                    case SDLK_2:
                        PlayBackSound(sound[CRASH]);
                        break;
                    case SDLK_3:
                        PlayBackSound(sound[BOOU]);
                        break;
                    case SDLK_w:
                        // W - fa finire un tempo!
                        EndTime=Timer();
                        D(bug("Forzo la fine del tempo!\n"));
                        break;

                        // D - causa un fallo
                    case SDLK_d:
                        pl->InGioco=FALSE;
                        FermaPalla();

                        p->arbitro.Comando=FISCHIA_FALLO;
                        TogliPalla();
                        if(p->squadra[0]->Possesso)
                        {
                            pl->sq_palla=p->squadra[0];
                            p->squadra[1]->Falli++;
                        }
                        else
                        {
                            pl->sq_palla=p->squadra[1];
                            p->squadra[0]->Falli++;
                        }
                        break;

                        /* AC: faccio fare un gol fittizio alla squadra 0. */
                    case SDLK_g:
                        p->squadra[0]->Reti++;
                        if(!penalties && (p->squadra[0]->Reti+p->squadra[1]->Reti)<GA_SIZE )
                        {
                            mytimer temptime;
                            int i=p->squadra[0]->Reti+p->squadra[1]->Reti-1;

                            goal_array[i]=p->last_touch;

                            goal_team[i]= (p->last_touch&32)==0 ? teams_swapped : teams_swapped^1;

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
                        D(bug("Ho segnato un GOL per la squadra %s\n",p->squadra[0]->Nome));
                        break;
#endif
                    case SDLK_r:
                        // R - replay
                        if(!replay_mode&&!start_replay)
                            start_replay=TRUE;
                        else
                        {
                            if(!slow_motion)
                            {
                                MY_CLOCKS_PER_SEC_50<<=2;
                                slow_motion=TRUE;
                            }
                            else
                            {
                                slow_motion=FALSE;
                                MY_CLOCKS_PER_SEC_50>>=2;
                            }
                        }
                        break;
                        // S - abilita/disabilita radar
                    case SDLK_s:

                        if(!use_key1)
                        {
                            if(detail_level&USA_RADAR)
                                detail_level&=(~USA_RADAR);
                            else
                                detail_level|=USA_RADAR;
                        }
                        break;
                        // L - mostra stats
                        /* C'era una sovrapposizione con SDLK_RETURN, veniva usato della definizione
                         * della tastiera uno.
                         */
                    case SDLK_h:
                        p->show_panel|=PANEL_CHAT;
                        p->show_time=120;
                        break;
                    case SDLK_l:
                        p->show_panel|=PANEL_STATS;
                        p->show_time=80;
                        break;

                        // X - cambia posizione/dimensioni del radar
                    case SDLK_x:
                        if(use_key1)
                            break;

                        radar_position++;
                        ResizeRadar();
                        break;

#ifdef USE_ROLEPLAY

                        // Alt sinistro, gioco di ruolo attivo/inattivo sq. 1
                    case SDLK_LALT:
                        p->squadra[1]->gioco_ruolo = (p->squadra[1]->gioco_ruolo ? FALSE : TRUE);
                        // shift sinistro, cambia giocatore attivo sq.1 se gioco di ruolo
                        break;
                    case SDLK_LSHIFT:
                        if(p->squadra[1]->gioco_ruolo)
                        {
                            char num=p->squadra[1]->attivo->GNum;

                            if(num<9)
                                num++;
                            else
                                num=0;

                            p->squadra[1]->gioco_ruolo=FALSE;
                            ChangeControlled(p->squadra[1],num);
                            p->squadra[1]->gioco_ruolo=TRUE;
                        }
                        // Alt destro, gioco di ruolo attivo/inattivo sq. 0
                        break;
                    case SDLK_RALT:
                        p->squadra[0]->gioco_ruolo = (p->squadra[0]->gioco_ruolo ? FALSE : TRUE);
                        // shift destro, cambia giocatore attivo sq.0 se gioco di ruolo
                        break;
                    case SDLK_RSHIFT:
                        if(p->squadra[0]->gioco_ruolo)
                        {
                            char num=p->squadra[0]->attivo->GNum;

                            if(num<9)
                                num++;
                            else
                                num=0;

                            p->squadra[0]->gioco_ruolo=FALSE;
                            ChangeControlled(p->squadra[0],num);
                            p->squadra[0]->gioco_ruolo=TRUE;
                        }
                        break;
#endif

                        // P - pausa
                    case SDLK_p:
                        if(!replay_mode)
                        {
                            if( (p->show_panel&PANEL_REPLAY) && friendly)
                            {
                                RestartGame();
                            }
                            else
                            {
                                DoPause();
                            }
                        }
                        break;
                        // DEL - visuale sulla palla
                    case SDLK_DELETE:
                        scroll_type=0;
                        break;

                        // Q - quit game
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        SetResult("break");
                        final = FALSE;

                        quit_game=TRUE;
                        break;

                        // BARRA - cambia visuale
                    case SDLK_SPACE:
                        if(!replay_mode) {
                            if(!use_key1) {

                                if(scroll_type<23)
                                    scroll_type++;
                                else
                                    scroll_type=0;
                            }
                            //							ChangeControlled(p->squadra[0],selected);
                        }
                        else {
                            SaveReplay();
                        }
                        break;
                    default:
                        // not handled keys...
                        break;
                }
            }
			else {
				if( (e.key.keysym.sym>=SDLK_a && e.key.keysym.sym<=SDLK_z) ||
					(e.key.keysym.sym>=SDLK_0 && e.key.keysym.sym<=SDLK_9) ||
					 e.key.keysym.sym==SDLK_SPACE ) {
					int l=strlen(outgoing_chat);

					if(l<(sizeof(outgoing_chat)-1)) {
						outgoing_chat[l]=toupper(e.key.keysym.sym);
						outgoing_chat[l+1]=0;
					}
					p->show_time=150;
				}
				switch(e.key.keysym.sym) {
					case SDLK_BACKSPACE:
						if(!*outgoing_chat)
							p->show_panel&=(~PANEL_CHAT);
						else {
							p->show_time=120;
							outgoing_chat[strlen(outgoing_chat)-1]=0;
						}
						break;
					case SDLK_RETURN:
						if(network_game)
							SendChatMessage();

						*outgoing_chat=0;
						p->show_panel&=(~PANEL_CHAT);
						break;
					case SDLK_ESCAPE:
						*outgoing_chat=0;
						p->show_panel&=(~PANEL_CHAT);
						break;
                    default:
                        // not handled keys...
                        break;
				}
			}
		}
	}
}


int query[]=
{
	SDLK_UP,SDLK_RIGHT,SDLK_DOWN,SDLK_LEFT,
	SDLK_BACKSPACE,SDLK_RETURN,
	SDLK_PAGEUP,SDLK_PAGEDOWN,
	SDLK_RSHIFT,SDLK_END,
// Query1
	SDLK_s,SDLK_c,SDLK_x,SDLK_z, // dir: up->senso orario (s->c->x->z)
	SDLK_SPACE,SDLK_b,
	SDLK_n,SDLK_m,SDLK_LSHIFT,SDLK_TAB
};

void UpdatePortStatus(void)
{
	if(has_joystick)
		SDL_JoystickUpdate();
	
	SDL_PumpEvents();
}

ULONG ReadKeyPort(ULONG port)
{
	register ULONG v=0;
	register Uint8 *keys;
	register int *q;

	keys=SDL_GetKeyState(NULL);

	q= (port) ? &query[10] : query;

// Spostamenti

	if(keys[q[0]]==SDL_PRESSED)
		v|=JPF_JOY_UP;
	else if(keys[q[2]]==SDL_PRESSED)
		v|=JPF_JOY_DOWN;

	if(keys[q[1]]==SDL_PRESSED)
		v|=JPF_JOY_RIGHT;
	else if(keys[q[3]]==SDL_PRESSED)
		v|=JPF_JOY_LEFT;

// Bottoni!

	if(keys[q[4]]==SDL_PRESSED)
		v|=JPF_BUTTON_RED;
	
	if(keys[q[5]]==SDL_PRESSED)
		v|=JPF_BUTTON_BLUE;

	if(keys[q[6]]==SDL_PRESSED)
		v|=JPF_BUTTON_YELLOW;

	if(keys[q[7]]==SDL_PRESSED)
		v|=JPF_BUTTON_GREEN;

	if(keys[q[8]]==SDL_PRESSED)
		v|=JPF_BUTTON_FORWARD;

	if(keys[q[9]]==SDL_PRESSED)
		v|=JPF_BUTTON_REVERSE;

	if(!network_game)
		return v;
	else {
		SendNetwork(v);
		return NetJoyPos[port];
	}
}

void LoadKeyDef(int port,char *file)
{
	FILE *f;

	if ((f=fopen(file,"r"))) {
		char buffer[120];
		int i=0;
		int *q;

		q= (port) ? &query[10] : query;

		D(bug("Carico una custom keydefinition...\n"));

		while(fgets(buffer,119,f)) {
			if(buffer[0]<'0' || buffer[0]>9) {
				continue;
			}

			if(i>=9) {
				D(bug("Troppe definizioni!\n"));
				break;
			}

			q[i]=atoi(buffer);

			i++;
		}

		fclose(f);
	}
}

void os_getevent(void)
{
	extern SDL_Event lastevent;

	SDL_PollEvent(&lastevent);
}
