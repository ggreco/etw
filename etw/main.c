#include "eat.h"
#include <stdarg.h>
#include "network.h"

ULONG detail_level = 255;
LONG Colors = 0;
LONG WINDOW_WIDTH = 320, WINDOW_HEIGHT = 256, framerate = 50;
extern BOOL free_longpass;
BOOL use_key0 = FALSE, use_key1 = FALSE;

Partita *p;
Pallone *pl;

#ifdef __SASC
char versione[] =
	"\0$VER: ETW-NG " ETW_VERSION " " __AMIGADATE__
	" by Gabriele Greco - Hurricane Studios";
#endif

BOOL highlight = FALSE;

static int progress_called = 0;

void Progress(void)
{
    int x = 10, y, block_width;
    

    D(bug("Progress: %d\n", progress_called));

    y = WINDOW_HEIGHT - 20;

	/* AC: I've noticed that the max block was 22, but now I've reduced them to 20 */
    block_width = ((WINDOW_WIDTH - 20) / 20); //- 1;
    
    x += (progress_called*(block_width /*+ 1*/));
    rectfill(main_bitmap, x, y, x + block_width -2, y + 14, Pens[P_BIANCO],bitmap_width);
    progress_called++;
    
    ScreenSwap();
}

void SetResult(char *fstring, ...)
{
	va_list ap;
	FILE *f;

	if ((f = fopen(RESULT_FILE, "w"))) {
		int i, j, sn;
		Squadra *s;

		va_start(ap, fstring);
		vfprintf(f, fstring, ap);
		va_end(ap);

		if (quit_game) {
			int l;

			if (network_game) {
				if (*fstring == '%')	// verifico che la partita sia finita, altrimenti si e' interrotta.
					SendFinish(p->squadra[0]->
							   Reti | (p->squadra[1]->Reti << 8));
				else
					SendQuit();
            }

			if (penalties) {
				extern char golrig[2];

				p->squadra[0]->Reti -= golrig[0];
				p->squadra[1]->Reti -= golrig[1];
				fprintf(f, "penalties\n%d\n", (int) golrig[0] + golrig[1]);
			}

			l = min(GA_SIZE, (p->squadra[0]->Reti + p->squadra[1]->Reti));

			for (i = 0; i < l; i++)
				fprintf(f, "%d %d %d\n", (int) goal_team[i],
						(int) (goal_array[i] & (~(32))),
						(int) goal_minute[i]);

			for (j = 0; j < 2; j++) {
				s = p->squadra[j];

				sn = 0;

				if ((teams_swapped && j == 0)
					|| (!teams_swapped && j == 1))
					sn = 1;

//                              fprintf("%ld\n",p->squadra[0]->Reti+p->squadra[1]->Reti); Non serve!

				for (i = 0; i < 10; i++) {
					char c = 0;

					if (s->giocatore[i].AnimType == GIOCATORE_ESPULSO)
						c = 'e';
					else if (s->giocatore[i].Ammonito)
						c = 'a';

					if (s->giocatore[i].Resistenza == 0)
						c = 'i';

					if (c)
						fprintf(f, "%d %d %c\n", (int) sn,
								(int) s->giocatore[i].Numero, c);
				}
			}
		}

		fclose(f);
	}
}

/*
 * Questa routine scrive il nome di un giocatore
 */

void DrawName(char *nome, char *cognome, int num, int x, int y)
{
	char buffer[50];
	int k;

	if (*nome && *cognome)
		sprintf(buffer, "%-2d %s %s", num, nome, cognome);
	else if (*cognome)
		sprintf(buffer, "%-2d %s", num, cognome);
	else if (*nome)
		sprintf(buffer, "%-2d %s", num, nome);

	num = strlen(buffer);

	drawtext(buffer, num, x + 1, y + 1, Pens[P_NERO]);
	k = drawtext(buffer, 2, x, y, Pens[P_GIALLO]);
	drawtext(buffer + 2, num - 2, x + k, y, Pens[P_BIANCO]);

}

void Loading(void)
{
	char buffer[40];
	struct myfont *sf = NULL;
	extern struct myfont *gtf;
	int i, x, y, passo, maxlen;
	struct Squadra_Disk s;

// Da metterci le immagini dei campi.

	if (!arcade) {
		sprintf(buffer, "gfx/stadium%02d.gfx", MyRangeRand(NUMERO_STADI));
		LoadLogo(buffer);
	} else {
		LoadLogo("menugfx/worldmap.gfx");
	}

	if (WINDOW_HEIGHT < 320 && WINDOW_WIDTH < 440) {
		if (!(sf = openfont(SMALLGAME_FONT)))
			printf("Unable to open required font!\n");
	}

	if (!sf)
		sf = gtf;

	passo = WINDOW_HEIGHT / 23;

	for (i = 0; i < 2; i++) {
		if (training && i == 1)
			break;

		x = 10 + i * (WINDOW_WIDTH - 35);
		maxlen = 20;

		{

			extern struct Squadra_Disk leftteam_dk, rightteam_dk;
			int k, t, xsup, ysup;
			char *n;

			s = (i == 0 ? leftteam_dk : rightteam_dk);

			for (k = 0; k < s.NumeroPortieri; k++) {
				char *c = s.portiere[k].Nome, *d = s.portiere[k].Cognome;

				t = 0;

				while (*c) {
					*c = toupper(*c);
					c++;
					t++;
				}

				while (*d) {
					*d = toupper(*d);
					d++;
					t++;
				}

				t += 4;

				maxlen = max(t, maxlen);
#ifdef SUPER_DEBUG
				D(bug("maxlen: %ld\n", maxlen));
#endif
			}

			for (k = 0; k < s.NumeroGiocatori; k++) {
				char *c = s.giocatore[k].Nome, *d = s.giocatore[k].Cognome;

				t = 0;

				while (*c) {
					*c = toupper(*c);
					c++;
					t++;
				}

				while (*d) {
					*d = toupper(*d);
					d++;
					t++;
				}

				t += 4;

				maxlen = max(t, maxlen);
#ifdef SUPER_DEBUG
				D(bug("maxlen: %ld\n", maxlen));
#endif
			}

			n = s.nome;
			t = 0;

			while (*n) {
				t++;
				*n = toupper(*n);
				n++;
			}

			maxlen = max(maxlen * sf->width, t * gtf->width);

#ifdef SUPER_DEBUG
			D(bug("maxlen: %ld\n", maxlen));
#endif

			if (i == 1)
				x -= maxlen;

			if (!arcade_teams)
				y = passo * 2;
			else {
				GfxObj *o;
				struct MyScaleArgs scale;

				sprintf(buffer, "menugfx/arcade%d.gfx", arcade_team[i]);

				if ((o = LoadGfxObject(buffer, NULL, NULL))) {
					scale.SrcX = scale.SrcY = 0;

					if (i == 1)
						scale.DestX = WINDOW_WIDTH - 10 - passo * 6;
					else
						scale.DestX = x;

					scale.DestY = passo;
					scale.SrcWidth = 70;	// o->width e o->height valgono 80...
					scale.SrcHeight = 70;
					scale.Dest = main_bitmap;
					scale.SrcSpan = o->width;
					scale.DestSpan = bitmap_width;
					scale.Src = o->bmap;
					scale.DestWidth = passo * 6;
					scale.DestHeight = passo * 6;
					bitmapScale(&scale);
					FreeGfxObj(o);
				}

				y = (passo * 8);
			}

			D(bug("X: %ld maxlen: %ld\n", x, maxlen));

			setfont(gtf);

			if (arcade_teams) {
				if (i == 1) {
					xsup = WINDOW_WIDTH - 9 - t * font_width;
					ysup = y + font_height;
				} else {
					xsup = 11;
					ysup = y + font_height;
				}
			} else {
				xsup = x + 21;
				ysup = y + font_height;
			}


			drawtext(s.nome, t, xsup, ysup, Pens[P_NERO]);

			xsup--;
			ysup--;

			drawtext(s.nome, t, xsup, ysup, Pens[P_BIANCO]);

			y += passo;

			if (player_type[i] != -1 || training) {
				unsigned char c;

				if (player_type[i] == 1)
					c = Pens[RADAR_SQUADRA_A];
				else
					c = Pens[RADAR_SQUADRA_B];


				if (arcade_teams) {
					if (i == 1)
						rectfill(main_bitmap,
								 WINDOW_WIDTH - 10 - t * gtf->width, y + 3,
								 WINDOW_WIDTH - 10, y + 7, c,
								 bitmap_width);
					else
						rectfill(main_bitmap, 10, y + 3,
								 10 + t * gtf->width, y + 7, c,
								 bitmap_width);
				} else
					rectfill(main_bitmap, x + 20, y + 3,
							 x + 20 + t * gtf->width, y + 7, c,
							 bitmap_width);
			}

			y += 2 * passo;

			setfont(sf);

			DrawName(s.portiere[0].Nome, s.portiere[0].Cognome, 1, x,
					 y + sf->height - 1);

			y += passo;

			for (t = 0; t < 10; t++) {
				DrawName(s.giocatore[t].Nome, s.giocatore[t].Cognome,
						 t + 2, x, y + sf->height - 1);
				y += passo;
			}

			if (!arcade_teams) {
				y += passo;

				if (s.NumeroPortieri > 1) {
					DrawName(s.portiere[1].Nome, s.portiere[1].Cognome, 12,
							 x, y + sf->height - 1);
					y += passo;
				}

				for (t = 10; t < min(14, s.NumeroGiocatori); t++) {
					DrawName(s.giocatore[t].Nome, s.giocatore[t].Cognome,
							 t + 3, x, y + sf->height - 1);
					y += passo;
				}
			}

		}
	}

	if (sf != gtf) {
		setfont(gtf);
		closefont(sf);
	}

	ScreenSwap();

	if (triple_buffering)
		ScreenSwap();

	Progress();
}

void FreeStuff(void)
{
	extern BOOL was_using_nosound;

	LiberaPartita(p);

    VuotaCLista();

    D(bug("Freeing scaling data...\n"));
    
    if(scaling) {
        free(scaling->Src);
        free(scaling->XRef);
        free(scaling->YRef);
        free(scaling);
        scaling = NULL;
    }
    
	D(bug("Freeing replay buffers...\n"));
	FreeReplayBuffers();

	D(bug("Freeing fonts...\n"));
	FreeFonts();

    free_crowd();
    
	if (!no_sound || (replay_mode && !was_using_nosound)) {
// non libero piu' il sound system, lo condivido tra menu' e game
		D(bug("Freeing sounds.\n"));
		LiberaSuoni();
	}

	free_joyports();

	os_free_timer();
	close_graphics();
}


#ifdef DEBUG_DISABLED
#ifdef USE_LOGFILE
#undef USE_LOGFILE
#endif
#endif

BOOL LoadStuff(void)
{
    save_back = FALSE;			/* Non mi interessa conservare gli sfondi */
    use_clipping = TRUE;		/* Voglio vedere gli omini parzialmente fuori dallo schermo */
    int i;

    //	srand((int) time(NULL));

    os_init_timer();

    use_remapping = FALSE;

#ifdef USE_TRIPLE
    triple_buffering = TRUE;
#endif
    D(bug("Loading menu palette...\n"));

#ifndef DEMOVERSION
    if (!LoadIFFPalette("gfx/eat16menu.col"))
        return FALSE;
#endif

    for (i = 0; i < 32; i++)
        Pens[i] = i;

    os_set_window_frame();

    if (!InitAnimSystem())
        return FALSE;

    if (!window_open()) {
        close_graphics();
        return FALSE;
    }

    if (scaling) {
        D(bug("Initializing scaling...\n"));

        scaling->Dest = main_bitmap;
        scaling->DestWidth = WINDOW_WIDTH;
        scaling->DestSpan = bitmap_width;
        scaling->DestHeight = WINDOW_HEIGHT;

        if ((scaling->Src =
                    malloc(FIXED_SCALING_WIDTH * FIXED_SCALING_HEIGHT))) {
            if ((scaling->XRef = malloc(FIXED_SCALING_WIDTH * sizeof(char)))) {
                if ((scaling->YRef =
                            malloc(FIXED_SCALING_HEIGHT * sizeof(char)))) {
                    MakeRef(scaling->XRef, FIXED_SCALING_WIDTH,
                            WINDOW_WIDTH);
                    MakeRef(scaling->YRef, FIXED_SCALING_HEIGHT,
                            WINDOW_HEIGHT);
                    main_bitmap = scaling->Src;
                    bitmap_width = scaling->SrcWidth = scaling->SrcSpan =
                        FIXED_SCALING_WIDTH;
                    bitmap_height = scaling->SrcHeight =
                        FIXED_SCALING_HEIGHT;
                    WINDOW_WIDTH = bitmap_width;
                    WINDOW_HEIGHT = bitmap_height;
                    D(bug("Scaling correctly initialized!\n"));
                } else {
                    free(scaling->XRef);
                    free(scaling->Src);
                    free(scaling);
                    scaling = NULL;
                }
            } else {
                free(scaling->Src);
                free(scaling);
                scaling = NULL;
            }
        } else {
            free(scaling);
            scaling = NULL;
        }
    }

    ClipX = WINDOW_WIDTH - 1;
    ClipY = WINDOW_HEIGHT - 1;

    if (!InitFonts()) {
        close_graphics();
        return FALSE;
    }
#ifdef DEMOVERSION
    LoadPLogo("gfx/epic");
    os_delay(75);
#else
    Loading();
#endif

    if (!no_sound) {
        if (training) {
            use_crowd = FALSE;
            use_speaker = FALSE;
        }

        //              wanted_sound=-1;

        if (!CaricaSuoni()) {
            FreeFonts();
            close_graphics();
            return FALSE;
        }

        if (use_crowd)
            init_crowd();
        if (use_speaker)
            init_speaker();

        SetCrowd(FONDO);
    }

    if ((background = LoadGfxObject(fieldname, Pens, NULL))) {
        GfxObj *temp;
        int i, x = 106;

        Progress();

        // Carico e stampo gli sponsors

        if (!arcade) {
            if ((temp = LoadGfxObject("gfx/sponsors", Pens, NULL))) {
                for (i = 0; i < 8; i++) {
                    if (i == 4)
                        x = SPONSOR_X_2;

                    if (training)
                        BltGfxObj(temp, 0, 0, background->bmap, x,
                                SPONSOR_Y_POS, SPONSOR_X - 1, SPONSOR_Y,
                                background->width);
                    else
                        BltGfxObj(temp, 0,
                                (MyRangeRand(SPONSORS - 1) +
                                 1) * SPONSOR_Y, background->bmap, x,
                                SPONSOR_Y_POS, SPONSOR_X - 1, SPONSOR_Y,
                                background->width);

                    x += SPONSOR_X;
                }


                BltGfxObj(temp, 0,
                        (!training
                         ? ((MyRangeRand(SPONSORS - 1) +
                                 1) * SPONSOR_Y) : 0), background->bmap, 485,
                        529, SPONSOR_X - 1, SPONSOR_Y,
                        background->width);
                BltGfxObj(temp, 0,
                        (!training
                         ? ((MyRangeRand(SPONSORS - 1) +
                                 1) * SPONSOR_Y) : 0), background->bmap, 690,
                        529, SPONSOR_X - 1, SPONSOR_Y,
                        background->width);

                FreeGfxObj(temp);
            }
        }

        Progress();

        // Carico e stampo gli spalti
        if (!arcade)
            if ((temp = LoadGfxObject("gfx/spalti", Pens, NULL))) {
                x = 34;

                for (i = 0; i < 4; i++) {
                    if (training)
                        BltGfxObj(temp, 0, SPALTI * SPALTI_Y,
                                background->bmap, x, 0, SPALTI_X,
                                SPALTI_Y, background->width);
                    else
                        BltGfxObj(temp, 0, MyRangeRand(SPALTI) * SPALTI_Y,
                                background->bmap, x, 0, SPALTI_X,
                                SPALTI_Y, background->width);

                    x += SPALTI_X;

                    if (i == 1)
                        x = SPALTI_X_2;
                }
                FreeGfxObj(temp);
            }

        Progress();

        if (!arcade && !training) {
            AnimObj *p;

            if ( (p = LoadAnimObject("gfx/people.obj", Pens))) {
                for (i = 0; i < NUMERO_OGGETTI; i++) {
                    x = MyRangeRand(peoples[i].Range);

                    if (peoples[i].Frame[x] >= 0) {
                        BltAnimObj(p, background->bmap,
                                peoples[i].Frame[x], peoples[i].X,
                                peoples[i].Y, background->width);
                    } else
                        peoples[i].Collisione = FALSE;

                    // FIX: Now we clone the object before modifiying to avoid to have to scale
                    //      back coords again
                    if ((detail_level & USA_FOTOGRAFI)
                            && peoples[i].Collisione) {
                        extern UBYTE people_type[];
                        struct DOggetto *d;

                        if ((d = malloc(sizeof(struct DOggetto)))) {
                            d->Collisione = TRUE;
                            d->Frame = peoples[i].Frame;
                            d->Range = people_type[peoples[i].Frame[x]];
                            d->X = peoples[i].X * 8;
                            d->Y = peoples[i].Y * 8;

                            AggiungiCLista(d);
                        }
                    }
                }

                FreeAnimObj(p);
            }
        }

        Progress();

#ifdef SUPER_DEBUG
        os_delay(100);
        BltGfxObj(background, 0, 0, main_bitmap, 0, 0, WINDOW_WIDTH,
                WINDOW_HEIGHT, bitmap_width);
        ScreenSwap();
        os_delay(200);
#endif
        if (!arcade) {
            char portname[20] = "gfx/porte.obj";
            AnimObj *obj = LoadAnimObject(portname, Pens);

            if (!obj) {
                FreeFonts();

                if (!no_sound) {
                    LiberaSuoni();
                }
                close_graphics();
                return FALSE;
            }

            i = MyRangeRand(TIPI_PORTE);

            tipo_porta = i;

            portname[8] = '0' + i;

            BltAnimObj(obj, background->bmap, i * 2, porte_x[i * 2],
                    PORTE_Y, background->width);
            BltAnimObj(obj, background->bmap, i * 2 + 1,
                    porte_x[i * 2 + 1], PORTE_Y, background->width);

            FreeAnimObj(obj);

            // Carico il file ports adeguato...

            if (!(ports = LoadAnimObject(portname, Pens))) {
                FreeFonts();

                if (!no_sound) {
                    LiberaSuoni();
                }

                close_graphics();
                return FALSE;
            }
        } else {
            detail_level &=
                ~(USA_FOTOGRAFI | USA_POLIZIOTTI | USA_ARBITRO |
                        USA_GUARDALINEE);
        }

#ifdef SUPER_DEBUG
        BltGfxObj(background, 0, 0, main_bitmap, 0, 0, WINDOW_WIDTH,
                WINDOW_HEIGHT, bitmap_width);
        ScreenSwap();
        os_delay(200);
#endif

#ifdef DEMOVERSION
        LoadPLogo("gfx/islona");
        os_delay(60);
#else
        /* AC: I remove this Progress, in order to gain 20 blocks instead of 21
           Progress();*/
#endif
        if (SetupSquadre()) {
            Progress();

            if(!network_game)
                p->TabCounter = MyRangeRand(256);
            else
                p->TabCounter = 0;

            if (free_longpass)
                LongPass = FreePass;
            else
                LongPass = TargetedPass;

            set_controls();

            if (player_type[0] == TYPE_JOYSTICK1) {
                p->squadra[0]->MarkerRed = Pens[RADAR_SQUADRA_A];
                p->squadra[1]->MarkerRed = Pens[RADAR_SQUADRA_B];
            } else {
                p->squadra[0]->MarkerRed = Pens[RADAR_SQUADRA_B];
                p->squadra[1]->MarkerRed = Pens[RADAR_SQUADRA_A];
            }

            if (big)
                HandleRadar = HandleRadarBig;
            else
                HandleRadar = HandleRadarLittle;

            ResizeRadar();

            // Questi li carico soltanto se ho spazio!

            if (last_obj) {
                FreeGfxObj(last_obj);
                last_obj = NULL;
            }

            if ((goal_banner = LoadAnimObject("gfx/goal.obj", Pens)))
                goal_banner->Flags |= AOBJ_OVER;

            pause_gfx = LoadGfxObject("gfx/paused", Pens, NULL);

            if ((replay = LoadAnimObject("gfx/replay.obj", Pens)))
                replay->Flags |= AOBJ_OVER;

            Progress();

            if (situation) {
                p->squadra[0]->Reti = situation_result[0];
                p->squadra[1]->Reti = situation_result[1];
                MakeResult();
            }

            D(bug("Allocating replay buffers...\n"));

            if (!AllocReplayBuffers()) {
                FreeStuff();
                return FALSE;
            }

            Progress();


            /* hide the screen to change palette */

            memset(main_bitmap, Pens[P_NERO],
                    bitmap_width * bitmap_height);

            D(bug("Last swap before match...\n"));

            ScreenSwap();

            /* AC: Just a very little delay, showing the final loading */
            os_delay(6);
            if (triple_buffering)
                ScreenSwap();

            if (!screen_opened)
                FreeIFFPalette();

            if (!(LoadIFFPalette(palette))) {

                FreeStuff();
                return FALSE;
            }
            AdjustSDLPalette();

            return TRUE;
        }

    }

    FreeFonts();
    LiberaListe();

    close_graphics();

    if (!no_sound) {
        LiberaSuoni();
    }

    return FALSE;
}

int game_main(void)
{
    progress_called = 0;

	read_config();

	if (arcade_teams && use_speaker) {
		use_speaker = FALSE;
		use_crowd = TRUE;
	}

	if (arcade) {
		injuries = FALSE;
		bookings = FALSE;
		substitutions = FALSE;
	}

	init_system();

	if (use_speaker)
		free_speaker();


    LiberaListe();
    
	quit_game = FALSE;

#ifdef __CODEGUARD__
	if(access(TEMP_DIR "lock",0) != -1)
#endif
	remove(TEMP_DIR "lock");

	D(bug("Match end!\n"));

	return 0;
}
