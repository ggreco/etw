#include "eat.h"
#include "SDL.h"
#include "network.h"

BOOL soft_scroll = TRUE, pause_mode = FALSE, use_sound = TRUE;
BYTE scroll_type = 0;

#ifndef DEBUG_DISABLED
LONG frames = 0;
#endif

LONG
#ifdef DEMOVERSION
	time_length = (2 * 60), t_l = 2;
#else
	time_length = (5 * 60), t_l = 5;
#endif

mytimer StartGameTime, EndTime, ideal;

WORD field_x, field_y, n_limit, o_limit, s_limit, e_limit;
WORD field_x_limit, field_y_limit, real_fx, real_fy;
void (*HandleSquadra0) (int);
void (*HandleSquadra1) (int);
void (*HandleRadar) (void);
GfxObj *background;

__inline void HandleControl(void)
{
	if (!replay_mode) {
		int i, j;

		UpdatePortStatus();

		i = p->squadra[0]->Joystick;
		j = p->squadra[1]->Joystick;

		if (i == 0 || j == 0)
			r_controls[0][counter] = MyReadPort0(0);

		if (i == 1 || j == 1)
			r_controls[1][counter] = MyReadPort1(1);
	}
}

void HandleScrolling(void)
{
	register WORD xs, ys, max_scroll;
	static WORD scroll_tick = 0, key_tick = 0;
/* Controlli sullo scrolling */

	scroll_tick++;
	key_tick++;


	if (key_tick > 16) {
#ifdef OLDAMIGA
		if (!double_buffering && !triple_buffering)
			os_lock_bitmap();
#endif

		CheckKeys();
#ifdef OLDAMIGA
		if (!double_buffering && !triple_buffering)
			os_unlock_bitmap();
#endif

		key_tick = 0;
	}

	if (scroll_tick > 50) {
		if (p->squadra[0]->Possesso) {
			o_limit = WINDOW_WIDTH * 3 / 5;
			e_limit = WINDOW_WIDTH * 4 / 5;
		} else if (p->squadra[1]->Possesso) {
			o_limit = WINDOW_WIDTH / 5;
			e_limit = WINDOW_WIDTH * 2 / 5;
		} else {
			o_limit = WINDOW_WIDTH * 2 / 5;
			e_limit = WINDOW_WIDTH - o_limit;
		}
		scroll_tick = 0;
	}

	if (!scroll_type) {
		xs = (pl->world_x >> 3) - field_x - quota_mod_x[pl->quota];
		ys = (pl->world_y >> 3) - field_y - quota_mod_y[pl->quota];
// Scroll sulla palla
	}
// Scroll sui giocatori
	else {
		if (scroll_type == 1) {
// Portiere sq. 0
			xs = (p->squadra[0]->portiere.world_x >> 3) - field_x + 5;
			ys = (p->squadra[0]->portiere.world_y >> 3) - field_y + 16;
		} else if (scroll_type == 12) {
// Portiere sq. 1
			xs = (p->squadra[1]->portiere.world_x >> 3) - field_x + 5;
			ys = (p->squadra[1]->portiere.world_y >> 3) - field_y + 16;
		} else if (scroll_type < 12) {
// Giocatore sq. 0
			xs = (p->squadra[0]->giocatore[scroll_type - 2].world_x >> 3) -
				field_x + 5;
			ys = (p->squadra[0]->giocatore[scroll_type - 2].world_y >> 3) -
				field_y + 15;
		} else if (scroll_type < 23) {
// Giocatore sq. 1
			xs = (p->squadra[1]->giocatore[scroll_type - 13].
				  world_x >> 3) - field_x + 5;
			ys = (p->squadra[1]->giocatore[scroll_type - 13].
				  world_y >> 3) - field_y + 16;
		} else {
            D(bug("WARNING, undefined scrolltype!"));
            xs = ys = 0; // scrolltype undefined
        }
	}

	if (pl->gioc_palla)
		max_scroll = (SOFT_PASS / 4);
	else {
		max_scroll = SOFT_PASS;
	}

	if (xs < o_limit && field_x > 0) {
		if (soft_scroll)
			field_x -= min((o_limit - xs), max_scroll);
		else {
			field_x -= (o_limit - xs);
			soft_scroll = TRUE;
		}

//              xs=o_limit;

		if (field_x < 0) {
//                      xs+=field_x;

			field_x = 0;
		}
	} else if (xs > e_limit && field_x < field_x_limit) {
		if (soft_scroll)
			field_x += min((xs - e_limit), max_scroll);
		else {
			field_x += (xs - e_limit);
			soft_scroll = TRUE;
		}

//              xs=e_limit;

		if (field_x > field_x_limit) {
//                      xs+=(field_x-field_x_limit);
			field_x = field_x_limit;
		}
	}


	if (ys < n_limit && field_y > 0) {
		if (soft_scroll)
			field_y -= min((n_limit - ys), max_scroll);
		else {
			field_y -= (n_limit - ys);
			soft_scroll = TRUE;
		}

//                      ys=n_limit;

		if (field_y < 0) {
//                      ys+=field_y;

			field_y = 0;
		}
	} else if (ys > s_limit && field_y < field_y_limit) {
		if (soft_scroll)
			field_y += min((ys - s_limit), max_scroll);
		else {
			field_y += (ys - s_limit);
			soft_scroll = TRUE;
		}

//                      ys=s_limit;

		if (field_y > field_y_limit) {
//                      ys+=(field_y-field_y_limit);
			field_y = field_y_limit;
		}
	}

	if (!replay_mode) {
		p->show_time--;

		if (pl->InGioco) {
			if (p->show_time <= 0) {
				if (p->show_panel & PANEL_KICKOFF) {
					p->show_time = 100;
					p->show_panel = PANEL_RESULT | PANEL_TIME;
				} else
					p->show_panel = 0;

			}

			D(if (p->show_panel & 0xff00)
			  bug
			  ("Errore, pannelli speciali presenti con la palla in gioco!\n"));
		} else if (!p->show_panel) {
			p->show_panel = PANEL_TIME;
		} else if (!(p->show_panel & 0xff00))	// Sostituzioni e cambio tattica fermano il tempo.
		{
			if (p->show_time <= 0) {
				p->show_time = 0;
				p->show_panel &= ~(PANEL_RESULT | PANEL_GOAL);
			}
		}
	}
}

void graphic_frame(void)
{
	BltGfxObj(background, field_x, field_y, main_bitmap, 0, 0,
			  WINDOW_WIDTH, WINDOW_HEIGHT, bitmap_width);

	if (p->doing_shot) {
		UpdateCornerLine();
	}

	if (p->flash_mode)
		DoFlash();
	else {
		SortDrawList();
		DrawAnimObj();

		if (detail_level & USA_RADAR)
			HandleRadar();

		if (detail_level & USA_NOMI)
			PrintNames();

		if (detail_level & USA_RISULTATO)
			PrintResult();

		if (p->show_panel)
			ShowPanel();

		if (replay_mode)
			DrawR();
	}


	ScreenSwap();

#ifndef DEBUG_DISABLED
	frames++;
#endif
}

void logic_frame(void)
{
  	if (network_game)
		HandleNetwork(p->TabCounter, pl->world_x);

	if (!p->flash_mode) {
		HandleReplay();

		HandleControl();

		HandleScrolling();

		if (arcade)
			HandleArcade();

		HandleBall();

		HandleArbitro();

		if (detail_level & USA_GUARDALINEE)
			HandleGuardalinee();

		HandleSquadra0(0);
		HandleSquadra1(1);

		HandlePortiere(0);
		HandlePortiere(1);

		HandleExtras();
		MoveNonControlled();
		PostHandleBall();
	} else {
		p->flash_pos++;
	}

	if (ideal > EndTime) {
		if (p->arbitro.Comando != FISCHIA_FINE
			&& InAnyArea(pl->world_x, pl->world_y)) {
			game_status = S_ULTIMA_AZIONE;
			EndTime += (2 * MY_CLOCKS_PER_SEC);
		} else if (!replay_mode && pl->InGioco) {
			if (!first_half || arcade || training) {
				if (p->arbitro.Comando != FISCHIA_FINE) {
					EndTime += 10 * MY_CLOCKS_PER_SEC;
					p->arbitro.Tick = 0;
					p->arbitro.Comando = FISCHIA_FINE;
					p->arbitro.Argomento = 6;
					game_status = S_FINE_PARTITA;
				}
			} else {
				if (p->arbitro.Comando != FISCHIA_PRIMO_TEMPO) {
					EndTime += 10 * MY_CLOCKS_PER_SEC;
					p->arbitro.Comando = FISCHIA_PRIMO_TEMPO;
					p->arbitro.Tick = 0;
					p->arbitro.Argomento = 5;
					game_status = S_FINE_PRIMO_TEMPO;
				}
			}
		}
	}
}


void MainLoop(void)
{
	int logic = 0, f_skip = 0, rep = 0;
	mytimer start;

	field_x = max((pl->world_x >> 3) - WINDOW_WIDTH / 2, 0);

	field_y = max((pl->world_y >> 3) - WINDOW_HEIGHT / 2, 0);

// Faccio lo scroll dall'alto...
//      field_y=0;

	o_limit = WINDOW_WIDTH * 2 / 5;
	e_limit = WINDOW_WIDTH - o_limit;
	n_limit = WINDOW_HEIGHT * 2 / 5;
	s_limit = WINDOW_HEIGHT - n_limit;

	field_y_limit = FIELD_HEIGHT - WINDOW_HEIGHT - 1;
	field_x_limit = FIELD_WIDTH - WINDOW_WIDTH - 1;

	AddAnimObj(pl->immagine, (pl->world_x >> 3) - field_x,
			   (pl->world_y >> 3) - field_y, pl->ActualFrame);


	if (highlight)
		LoadHighlight();

	if (!training && !highlight)
		PlayBackSound(sound[FOLLA]);

	if (no_sound || (!use_speaker && !use_crowd))
		use_sound = FALSE;

	if (network_game)
		if (!send_netstart()) {
			D(bug("NET: fallita netstart!\n"));
			quit_game = TRUE;
			SendQuit();
		}

    os_start_audio();

  	ideal = start = StartGameTime = Timer();
	EndTime = time_length * MY_CLOCKS_PER_SEC + start;

// Nel caso si giochi una situazione metto il tempo al minuto giusto!

	D(bug
	  ("Start: %ld, End: %ld, Current:%ld, CPS: %ld\n", start, EndTime,
	   Timer(), MY_CLOCKS_PER_SEC));

	if (situation) {
		if (situation_time > 45)
			situation_time -= 45;

		EndTime -= (situation_time * t_l * 60 / 45);
	}

	while (!quit_game) {
		logic_frame();

		ideal += MY_CLOCKS_PER_SEC_50;

		logic++;

		if (Timer() < ideal) {
			graphic_frame();

			while (Timer() < ideal) {
				rep++;

                SDL_Delay(5); // XXX messo solo per facilitare debug

/*				if (network_game)
					HandleNetwork();*/
//                              graphic_frame();
			}
		} else
			f_skip++;
	}

	if (final)
		ShowFinal();

	os_stop_audio();


#ifndef DEBUG_DISABLED
	D(bug
	  ("Totale frames: %ld fisici, %ld logici, saltati %ld, ripetuti %ld\n",
	   frames, logic, f_skip, rep));
	logic = (Timer() - start) / MY_CLOCKS_PER_SEC;
	D(bug("Totale tempo: %ld secs, %ld FPS\n", logic, frames / logic));
#endif
}
