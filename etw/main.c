#include "eat.h"

#include <stdarg.h>

#include "network.h"

uint32_t detail_level = 0xffffffff;
long Colors = 0;
int WINDOW_WIDTH = 320, WINDOW_HEIGHT = 256, framerate = 40;
extern BOOL free_longpass;
BOOL use_key0 = FALSE, use_key1 = FALSE, use_touch = FALSE, ads_present = FALSE;

game_t *p;
ball_t *pl;

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

void SetResult(const char *fstring, ...)
{
    va_list ap;
    FILE *f;

    if ((f = fopen(RESULT_FILE, "w"))) {
        int i, j, sn;
        team_t *s;

        va_start(ap, fstring);
        vfprintf(f, fstring, ap);
        va_end(ap);

        if (quit_game) {
            int l;

            if (network_game) {
                if (*fstring == '%')    // verifico che la partita sia finita, altrimenti si e' interrotta.
                    SendFinish(p->team[0]->
                               Reti | (p->team[1]->Reti << 8));
                else
                    SendQuit();
            }

            if (penalties) {
                extern char golrig[2];

                p->team[0]->Reti -= golrig[0];
                p->team[1]->Reti -= golrig[1];
                fprintf(f, "penalties\n%d\n", (int) golrig[0] + golrig[1]);
            }
            else {
                if ((p->team[0]->Reti > p->team[1]->Reti &&
                     p->team[0]->Joystick >= 0 && p->team[1]->Joystick == -1) ||
                    (p->team[1]->Reti > p->team[0]->Reti &&
                     p->team[1]->Joystick >= 0 && p->team[0]->Joystick == -1))  {

                    // update victory achievements

                    if (arcade)
                        add_achievement("1_arcadewin", 100.0f);
                    else
                        add_achievement("2_win", 100.0f);

                    add_achievement("6_amateur", 20.0);
                    add_achievement("10_semipro", 10.0);
                    add_achievement("11_pro", 4.0);
                }
            }

            l = min(GA_SIZE, (p->team[0]->Reti + p->team[1]->Reti));

            for (i = 0; i < l; i++)
                fprintf(f, "%d %d %d\n", (int) goal_team[i],
                        (int) (goal_array[i] & (~(32))),
                        (int) goal_minute[i]);

            for (j = 0; j < 2; j++) {
                s = p->team[j];

                sn = 0;

                if ((teams_swapped && j == 0)
                    || (!teams_swapped && j == 1))
                    sn = 1;

//                              fprintf("%ld\n",p->team[0]->Reti+p->team[1]->Reti); Non serve!

                for (i = 0; i < 10; i++) {
                    char c = 0;

                    if (s->players[i].AnimType == GIOCATORE_ESPULSO)
                        c = 'e';
                    else if (s->players[i].Ammonito)
                        c = 'a';

                    if (s->players[i].stamina == 0)
                        c = 'i';

                    if (c)
                        fprintf(f, "%d %d %c\n", (int) sn,
                                (int) s->players[i].number, c);
                }
            }
        }

        fclose(f);
    }
}

/*
 * Questa routine scrive il nome di un giocatore
 */

void DrawName(char *name, char *surname, int num, int x, int y)
{
    char buffer[50];
    int k;

    if (*name && *surname)
        sprintf(buffer, "%-2d %s %s", num, name, surname);
    else if (*surname)
        sprintf(buffer, "%-2d %s", num, surname);
    else if (*name)
        sprintf(buffer, "%-2d %s", num, name);

    num = (int)strlen(buffer);

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
    struct team_disk s;

// set the position of the timer based on the status of ads
    ads_present = !has_full_version();
    
// random pitch background for loading screen, except if arcade
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

            extern struct team_disk leftteam_dk, rightteam_dk;
            int k, t, xsup, ysup;
            char *n;

            s = (i == 0 ? leftteam_dk : rightteam_dk);

            for (k = 0; k < s.nkeepers; k++) {
                char *c = s.keepers[k].name, *d = s.keepers[k].surname;

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

            for (k = 0; k < s.nplayers; k++) {
                char *c = s.players[k].name, *d = s.players[k].surname;

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

            n = s.name;
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
                gfx_t *o;
                struct MyScaleArgs scale;

                sprintf(buffer, "menugfx/arcade%d.gfx", arcade_team[i]);

                if ((o = LoadGfxObject(buffer, NULL))) {
                    scale.SrcX = scale.SrcY = 0;

                    if (i == 1)
                        scale.DestX = WINDOW_WIDTH - 10 - passo * 6;
                    else
                        scale.DestX = x;

                    scale.DestY = passo;
                    scale.SrcWidth = 70;    // o->width e o->height valgono 80...
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


            drawtext(s.name, t, xsup, ysup, Pens[P_NERO]);

            xsup--;
            ysup--;

            drawtext(s.name, t, xsup, ysup, Pens[P_BIANCO]);

            y += passo;

            if (player_type[i] != -1 || training) {
                unsigned char c;

                if (player_type[i] == 1)
                    c = Pens[RADAR_TEAM_A];
                else
                    c = Pens[RADAR_TEAM_B];


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

            DrawName(s.keepers[0].name, s.keepers[0].surname, 1, x,
                     y + sf->height - 1);

            y += passo;

            for (t = 0; t < 10; t++) {
                DrawName(s.players[t].name, s.players[t].surname,
                         t + 2, x, y + sf->height - 1);
                y += passo;
            }

            if (!arcade_teams) {
                y += passo;

                if (s.nkeepers > 1) {
                    DrawName(s.keepers[1].name, s.keepers[1].surname, 12,
                             x, y + sf->height - 1);
                    y += passo;
                }

                for (t = 10; t < min(14, s.nplayers); t++) {
                    DrawName(s.players[t].name, s.players[t].surname,
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

    Progress();
}

void FreeStuff(void)
{
    extern BOOL was_using_nosound;

    if (logos) {
        FreeAnimObj(logos);
        logos = NULL;
    }

    LiberaPartita(p);

    VuotaCLista();

    D(bug("Freeing scaling data...\n"));
    
    if(scaling) {
        main_bitmap = scaling->Dest;
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
    FreeGraphics();
}


#ifdef DEBUG_DISABLED
#ifdef USE_LOGFILE
#undef USE_LOGFILE
#endif
#endif

BOOL LoadStuff(void)
{
    int i;
    save_back = FALSE;            /* Non mi interessa conservare gli sfondi */
    use_clipping = TRUE;        /* Voglio vedere gli omini parzialmente fuori dallo schermo */

    //    srand((int) time(NULL));

    os_init_timer();

    D(bug("Loading menu palette...\n"));

    if (!LoadIFFPalette("gfx/eat16menu.col"))
        return FALSE;

    for (i = 0; i < 32; i++)
        Pens[i] = i;

    if (!InitAnimSystem())
        return FALSE;

    if (scaling) {
        D(bug("Initializing scaling...\n"));

        scaling->Dest = main_bitmap;
        scaling->DestWidth = WINDOW_WIDTH;
        scaling->DestSpan = bitmap_width;
        scaling->DestHeight = WINDOW_HEIGHT;
        scaling->xRatio = (float)scaling->DestWidth / (float)FIXED_SCALING_WIDTH;
        scaling->yRatio = (float)scaling->DestHeight / (float)FIXED_SCALING_HEIGHT;

        if ((scaling->Src =
                    malloc(FIXED_SCALING_WIDTH * FIXED_SCALING_HEIGHT))) {
            if ((scaling->XRef = malloc(FIXED_SCALING_WIDTH * sizeof(uint8_t)))) {
                if ((scaling->YRef =
                            malloc(FIXED_SCALING_HEIGHT * sizeof(uint8_t)))) {
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
                    UpdateButtonList();
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
        FreeGraphics();
        return FALSE;
    }

    Loading();

    if (!no_sound) {
        if (training) {
            use_crowd = FALSE;
            use_speaker = FALSE;
        }
        else
            use_crowd = TRUE; // sempre attivo

        //              wanted_sound=-1;

        if (!CaricaSuoni()) {
            FreeFonts();
            FreeGraphics();
            return FALSE;
        }

        if (use_crowd)
            init_crowd();
        if (use_speaker)
            init_speaker();

        SetCrowd(FONDO);
    }

    if ((background = LoadGfxObject(fieldname, Pens))) {
        gfx_t *temp;
        int x = 106;

        Progress();

        // Loading and displaying sponsor tablets

        if (!arcade) {
            if ((temp = LoadGfxObject("gfx/sponsors", Pens))) {
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

        // loading and printing stadium gfx
        if (!arcade)
            if ((temp = LoadGfxObject("gfx/spalti", Pens))) {
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
            anim_t *ap;

            if ( (ap = LoadAnimObject("gfx/people.obj", Pens))) {
                for (i = 0; i < NUMERO_OGGETTI; i++) {
                    x = MyRangeRand(peoples[i].Range);

                    if (peoples[i].Frame[x] >= 0) {
                        BltAnimObj(ap, background->bmap,
                                peoples[i].Frame[x], peoples[i].X,
                                peoples[i].Y, background->width);
                    } else
                        peoples[i].Collisione = FALSE;

                    // FIX: Now we clone the object before modifiying to avoid to have to scale
                    //      back coords again
                    if ((detail_level & USA_FOTOGRAFI)
                            && peoples[i].Collisione) {
                        extern uint8_t people_type[];
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

                FreeAnimObj(ap);
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
            anim_t *obj = LoadAnimObject(portname, Pens);

            if (!obj) {
                FreeFonts();

                if (!no_sound) {
                    LiberaSuoni();
                }
                FreeGraphics();
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

            // Loading the right net type...

            if (!(ports = LoadAnimObject(portname, Pens))) {
                FreeFonts();

                if (!no_sound) {
                    LiberaSuoni();
                }

                FreeGraphics();
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
                p->team[0]->MarkerRed = Pens[RADAR_TEAM_A];
                p->team[1]->MarkerRed = Pens[RADAR_TEAM_B];
            } else {
                p->team[0]->MarkerRed = Pens[RADAR_TEAM_B];
                p->team[1]->MarkerRed = Pens[RADAR_TEAM_A];
            }

            if (big)
                HandleRadar = HandleRadarBig;
            else
                HandleRadar = HandleRadarLittle;

            ResizeRadar();

            if (last_obj) {
                FreeGfxObj(last_obj);
                last_obj = NULL;
            }

            if ((goal_banner = LoadAnimObject("gfx/goal.obj", Pens)))
                goal_banner->Flags |= AOBJ_OVER;

            pause_gfx = LoadGfxObject("gfx/paused", Pens);

            if ((replay = LoadAnimObject("gfx/replay.obj", Pens)))
                replay->Flags |= AOBJ_OVER;

            Progress();

            if (situation) {
                p->team[0]->Reti = situation_result[0];
                p->team[1]->Reti = situation_result[1];
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

            if (!(LoadIFFPalette(palette))) {

                FreeStuff();
                return FALSE;
            }

            // show ads if present
            show_ads(1);
            return TRUE;
        }

    }

    FreeFonts();
    LiberaListe();

    FreeGraphics();

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


    // injuries, bookings and substitutions are NOT available in arcade mode
    // but are always enabled otherwise
    if (arcade) {
        injuries = FALSE;
        bookings = FALSE;
        substitutions = FALSE;
    }
    else {
        injuries = TRUE;
        bookings = TRUE;
        substitutions = TRUE;
    }

    // offside and golden goal are always disabled
    use_offside = FALSE;
    golden_gol = FALSE;

    init_system();
    free_game();
    return 0;
}
