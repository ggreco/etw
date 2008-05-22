#include "etw_locale.h"
#include "menu.h"
#include <SDL.h>
#include <ctype.h>

#include "chunky.h"

int current_menu = 0, current_button = NESSUN_BOTTONE, actual_button = 0;
struct GfxMenu *actual_menu = &menu[0];
void *hwin = NULL;
void CreaBottone(struct Bottone *b);
void SelezionaBottone(WORD bottone);
void DrawBox(WORD button);

struct Bottone req_bottoni[4];
BOOL reqqing = FALSE;

struct MyFileRequest freq = { 0 };


BOOL MyEasyRequest(void *w, struct EasyStruct *e, void *unused)
{
    struct GfxMenu *old_menu, fake_menu;
    int i, x, y, width = FixedScaledX(150), button_width =
        FixedScaledX(30), height = FixedScaledY(30), bottoni = 1, linee =
        1, len = 0, leftedge, topedge;
    struct myfont *tf;
    char text[400];
    char buttons[100];
    char *c;

    strncpy(text, e->es_TextFormat, sizeof(text) - 1);
    strncpy(buttons, e->es_GadgetFormat, sizeof(buttons) - 1);

    /* AC: Se per qualche motivo i font non sono caricati, li ricarico. */
    /* Questo succede ad esempio se c'e` un errore ad inizio partita, dopo */
    /* che e` stata fatta la FreeMenuStuff */

    /*if(bigfont == NULL || smallfont == NULL)
        InitMenuFonts(); da provare` */

    if (WINDOW_WIDTH > 400)
        tf = bigfont;
    else
        tf = smallfont;

// Calcolo quante linee di testo ci sono.

    c = text;

    while (*c) {
        if (*c == '\n') {
            linee++;
            width = max(width, (len + 4) * tf->width);
            len = 0;
            *c = 0;
        } else
            len++;

        *c = toupper(*c);

        c++;
    }

    width = max(width, (len + 4) * tf->width);

    height += (tf->height + FixedScaledY(3)) * linee;

    if (WINDOW_WIDTH <= width) {
        D(bug("Error in width calculation!\n"));
        return TRUE;
    }
//   calcolo quanti bottoni ci sono...

    c = buttons;

    while (*c) {
        if (*c == '|') {
            bottoni++;
            button_width = max(button_width, (len + 2) * tf->width);
            len = 0;
            *c = 0;
        } else
            len++;

        *c = toupper(*c);

        c++;
    }

    button_width = max(button_width, (len + 2) * tf->width);

// Eseguo i calcoli preliminari per la creazione dei bottoni.

    width =
        max(width,
            (button_width + FixedScaledX(4)) * bottoni + FixedScaledX(4));

    width = min(WINDOW_WIDTH, width);

    if (width < (button_width * bottoni)) {
        y = 1;
        button_width = width / bottoni - bottoni - 1;
    } else {
        y = width - button_width * bottoni - FixedScaledX(4);

        if (bottoni > 2)
            y /= (bottoni - 1);
    }

    leftedge = (WINDOW_WIDTH - width) / 2;
    topedge = (WINDOW_HEIGHT - height) / 2;

// Disegno il riquadro del requester.

    rectfill(main_bitmap, leftedge, topedge, leftedge + width,
             topedge + height, Pens[P_ROSSO1], bitmap_width);

    draw(Pens[P_ROSSO0], leftedge, topedge + height - 1, leftedge,
         topedge);
    draw(Pens[P_ROSSO0], leftedge, topedge, leftedge + width - 1, topedge);
    main_bitmap[leftedge + topedge * bitmap_width] = Pens[P_BIANCO];

    x = leftedge + (width - button_width * bottoni -
                    y * (bottoni - 1)) / 2;

// Scrivo e visualizzo i bottoni

    c = buttons;

    for (i = 0; i < bottoni; i++) {
        req_bottoni[i].X1 = x;
        req_bottoni[i].X2 = x + button_width;
        x += (button_width + y);
        req_bottoni[i].Colore = Pens[P_VERDE1];
        req_bottoni[i].Highlight = Pens[P_VERDE0];
        req_bottoni[i].Testo = strdup(c);
        req_bottoni[i].Y1 =
            topedge + height - tf->height - FixedScaledY(14);
        req_bottoni[i].Y2 = topedge + height - FixedScaledY(6);

        CreaBottone(&req_bottoni[i]);

        c += strlen(c);

        if (i < (bottoni - 1))
            *c = '|';
        c++;
    }

// Scrivo il testo

    c = text;

    y = topedge + FixedScaledY(10);

    setfont(tf);

    while (linee) {
        len = strlen(c);
        x = (WINDOW_WIDTH - len * tf->width) / 2;

        ColorTextShadow(x, y, c, len, P_BIANCO);

        y += (tf->height + FixedScaledY(3));

        c += len;

        if (linee > 1)
            *c = '\n';

        c++;
        linee--;
    }

      old_menu = actual_menu;
    actual_menu = &fake_menu;

    fake_menu.NumeroBottoni = bottoni;
    fake_menu.Bottone = req_bottoni;
    actual_button = 0;

    D(bug("Creating requester with %d buttons\n", bottoni));

    /* AC: I think that this routine is too specific for menu selection.
     * In order to work correctly is also necessary to alter the correct
     * value of current_menu.
     * I think is better to put directly here the four needed instructions.
    DrawBox(0);*/
    draw(Pens[P_GIALLO], req_bottoni[0].X1 - 1, req_bottoni[0].Y1 - 1,
        req_bottoni[0].X2 + 1, req_bottoni[0].Y1 - 1);
    draw(Pens[P_GIALLO], req_bottoni[0].X2 + 1, req_bottoni[0].Y1 - 1,
        req_bottoni[0].X2 + 1, req_bottoni[0].Y2 + 1);
    draw(Pens[P_GIALLO], req_bottoni[0].X2 + 1, req_bottoni[0].Y2 + 1,
        req_bottoni[0].X1 - 1, req_bottoni[0].Y2 + 1);
    draw(Pens[P_GIALLO], req_bottoni[0].X1 - 1, req_bottoni[0].Y2 + 1,
        req_bottoni[0].X1 - 1, req_bottoni[0].Y1);

    ScreenSwap();

    reqqing = TRUE;


    
    while (HandleMenuIDCMP());

    linee = current_button;
    actual_menu = old_menu;

    reqqing = FALSE;

    ChangeMenu(current_menu);

    for (i = 0; i < bottoni; i++)
        free(req_bottoni[i].Testo);

    if (linee < (bottoni - 1))
        return linee + 1;
    else
        return 0;
}

BOOL CheckQuit(void)
{
    easy.es_TextFormat = msg_65;
    easy.es_GadgetFormat = msg_62;

    if (current_menu == MENU_TEAM_SETTINGS ||
        current_menu == MENU_MATCH_RESULT)
        return TRUE;

    if (MyEasyRequest(hwin, &easy, NULL))
        return TRUE;
    else
        return FALSE;
}

struct EasyStruct easy = { sizeof(struct EasyStruct), 0L,
    msg_57,
    NULL,
    NULL
};

void request(char *t)
{
    easy.es_TextFormat = t;
    easy.es_GadgetFormat = msg_58;

    MyEasyRequest(hwin, &easy, NULL);
}

BOOL DoWarning(int a)
{
    if (a) {
        easy.es_TextFormat = msg_59;
        easy.es_GadgetFormat = msg_60;

        if (MyEasyRequest(hwin, &easy, NULL)) {
            turno = 0;
            special = FALSE;
            competition = MENU_TEAMS;
        } else
            return TRUE;
    }

    return FALSE;
}

WORD *menubase = NULL;

void StoreButtonList()
{
    register int i, j, k;

    for (i = 0, k = 0; i < MENUS; i++) {
        for (j = 0; j < menu[i].NumeroBottoni; j++)
            k += 4;

        for (j = 0; j < menu[i].NumeroPannelli; j++)
            k += 4;

        if (menu[i].Immagine >= 0)
            k += 2;
    }

    D(bug("Allocated %ld words for default menu datas.\n", k));

    if (!(menubase = malloc(k * sizeof(WORD))))
        return;

    for (i = 0, k = 0; i < MENUS; i++) {
        for (j = 0; j < menu[i].NumeroBottoni; j++) {
            menubase[k] = menu[i].Bottone[j].X1;
            menubase[k + 1] = menu[i].Bottone[j].Y1;
            menubase[k + 2] = menu[i].Bottone[j].X2;
            menubase[k + 3] = menu[i].Bottone[j].Y2;
            k += 4;
        }

        for (j = 0; j < menu[i].NumeroPannelli; j++) {
            menubase[k] = menu[i].Pannello[j].X1;
            menubase[k + 1] = menu[i].Pannello[j].Y1;
            menubase[k + 2] = menu[i].Pannello[j].X2;
            menubase[k + 3] = menu[i].Pannello[j].Y2;
            k += 4;
        }

        if (menu[i].Immagine >= 0) {
            menubase[k] = menu[i].X;
            menubase[k + 1] = menu[i].Y;
            k += 2;
        }
    }
}


void UpdateButtonList()
{
    register int i, j, k;

    if (!menubase)
        return;

    for (i = 0, k = 0; i < MENUS; i++) {
        for (j = 0; j < menu[i].NumeroBottoni; j++) {
            menu[i].Bottone[j].X1 = FixedScaledX(menubase[k]);
            menu[i].Bottone[j].Y1 = FixedScaledY(menubase[k + 1]);
            menu[i].Bottone[j].X2 = FixedScaledX(menubase[k + 2]);
            menu[i].Bottone[j].Y2 = FixedScaledY(menubase[k + 3]);
            k += 4;
        }

        for (j = 0; j < menu[i].NumeroPannelli; j++) {
            menu[i].Pannello[j].X1 = FixedScaledX(menubase[k]);
            menu[i].Pannello[j].Y1 = FixedScaledY(menubase[k + 1]);
            menu[i].Pannello[j].X2 = FixedScaledX(menubase[k + 2]);
            menu[i].Pannello[j].Y2 = FixedScaledY(menubase[k + 3]);
            k += 4;
        }

        if (menu[i].Immagine >= 0) {
            menu[i].X = FixedScaledX(menubase[k]);
            menu[i].Y = FixedScaledY(menubase[k + 1]);
            k += 2;
        }
    }
}

void EraseBox(WORD button)
{
    if (reqqing) {
// Il colore di fondo del requester e' sempre: Pens[P_ROSSO1]
        struct Bottone *b = &actual_menu->Bottone[button];

        draw(Pens[P_ROSSO1], b->X1 - 1, b->Y1 - 1, b->X2 + 1, b->Y1 - 1);
        draw(Pens[P_ROSSO1], b->X2 + 1, b->Y1 - 1, b->X2 + 1, b->Y2 + 1);
        draw(Pens[P_ROSSO1], b->X2 + 1, b->Y2 + 1, b->X1 - 1, b->Y2 + 1);
        draw(Pens[P_ROSSO1], b->X1 - 1, b->Y2 + 1, b->X1 - 1, b->Y1);
    } else if (current_menu < MENU_GAME_PREFS
               || current_menu > MENU_AUDIO_PREFS
               || button == actual_menu->NumeroBottoni - 1) {
        struct Bottone *b = &actual_menu->Bottone[button];

        bltchunkybitmap(back, b->X1 - 1, b->Y1 - 1, main_bitmap,
                        b->X1 - 1, b->Y1 - 1, b->X2 - b->X1 + 2, 1,
                        bitmap_width, bitmap_width);
        bltchunkybitmap(back, b->X1 - 1, b->Y1 - 1, main_bitmap, b->X1 - 1,
                        b->Y1 - 1, 1, b->Y2 - b->Y1 + 2, bitmap_width,
                        bitmap_width);
        bltchunkybitmap(back, b->X1 - 1, b->Y2 + 1, main_bitmap, b->X1 - 1,
                        b->Y2 + 1, b->X2 - b->X1 + 2, 1, bitmap_width,
                        bitmap_width);
        bltchunkybitmap(back, b->X2 + 1, b->Y1 - 1, main_bitmap, b->X2 + 1,
                        b->Y1 - 1, 1, b->Y2 - b->Y1 + 3, bitmap_width,
                        bitmap_width);
    } else {
        struct Bottone *b = &actual_menu->Bottone[(button >> 1) << 1];
        struct Bottone *b2 =
            &actual_menu->Bottone[((button >> 1) << 1) + 1];

        bltchunkybitmap(back, b->X1 - 1, b->Y1 - 1, main_bitmap,
                        b->X1 - 1, b->Y1 - 1, b2->X2 - b->X1 + 2, 1,
                        bitmap_width, bitmap_width);
        bltchunkybitmap(back, b->X1 - 1, b->Y1 - 1, main_bitmap, b->X1 - 1,
                        b->Y1 - 1, 1, b2->Y2 - b->Y1 + 2, bitmap_width,
                        bitmap_width);
        bltchunkybitmap(back, b->X1 - 1, b2->Y2 + 1, main_bitmap,
                        b->X1 - 1, b2->Y2 + 1, b2->X2 - b->X1 + 2, 1,
                        bitmap_width, bitmap_width);
        bltchunkybitmap(back, b2->X2 + 1, b->Y1 - 1, main_bitmap,
                        b2->X2 + 1, b->Y1 - 1, 1, b2->Y2 - b->Y1 + 3,
                        bitmap_width, bitmap_width);
    }
}

void DrawBox(WORD button)
{
    if (current_menu < MENU_GAME_PREFS || current_menu > MENU_AUDIO_PREFS
        || button == actual_menu->NumeroBottoni - 1) {
        struct Bottone *b = &actual_menu->Bottone[button];

        draw(Pens[P_GIALLO], b->X1 - 1, b->Y1 - 1, b->X2 + 1, b->Y1 - 1);
        draw(Pens[P_GIALLO], b->X2 + 1, b->Y1 - 1, b->X2 + 1, b->Y2 + 1);
        draw(Pens[P_GIALLO], b->X2 + 1, b->Y2 + 1, b->X1 - 1, b->Y2 + 1);
        draw(Pens[P_GIALLO], b->X1 - 1, b->Y2 + 1, b->X1 - 1, b->Y1);
    } else {
        struct Bottone *b = &actual_menu->Bottone[(button >> 1) << 1];
        struct Bottone *b2 =
            &actual_menu->Bottone[((button >> 1) << 1) + 1];

        draw(Pens[P_GIALLO], b->X1 - 1, b->Y1 - 1, b2->X2 + 1, b->Y1 - 1);
        draw(Pens[P_GIALLO], b2->X2 + 1, b->Y1 - 1, b2->X2 + 1,
             b2->Y2 + 1);
        draw(Pens[P_GIALLO], b2->X2 + 1, b2->Y2 + 1, b->X1 - 1,
             b2->Y2 + 1);
        draw(Pens[P_GIALLO], b->X1 - 1, b2->Y2 + 1, b->X1 - 1, b->Y1);

    }
}


void StampaTesto(struct Bottone *b, int xs, int ys, char *testo, int len,
                 struct myfont *tf)
{
    switch (*testo) {
// A destra
    case '_':
        {
            int c, y;

// Con questo gestisco decentemente la team selection arcade

            if (current_menu == MENU_ARCADE_SELECTION) {
                c = b->Colore;
                y = ys + b->Y2 - b->Y1 - 3;    /* +tf_Baseline */
            } else {
                c = P_BIANCO;
                y = ys + (b->Y2 - b->Y1 - tf->height) / 2 + tf->height -
                    1 /*+tf->tf_Baseline */ ;
            }

            testo++;
            len--;
            ColorTextShadow(xs + (b->X2 - b->X1 - tf->width * len - 2), y,
                            testo, len, c);
        }
        break;
    case '-':
// A sinistra
        if (len > 1) {
            testo++;
            len--;
        } else
            goto fallback;

        TextShadow(xs + 3,
                   ys + (b->Y2 - b->Y1 - tf->height) / 2 + tf->height -
                   1 /*+tf->tf_Baseline */ , testo, len);
        break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        while (*testo < 13) {
            BltAnimObj(symbols, main_bitmap, *testo, xs, ys, bitmap_width);
            xs += symbols->Widths[*testo];
            testo++;
        }
        break;
    case 5:
    case 6:
    case 7:
    case 13:
    case 14:
    case 15:
        BltAnimObj(symbols, main_bitmap, *testo, xs, ys, bitmap_width);
        break;
    case 16:
        testo++;

        if (*testo < ARCADE_TEAMS) {
            struct MyScaleArgs scale;
            gfx_t *o;

            if (!(o = arcade_gfx[*testo]))
                break;

            scale.SrcX = scale.SrcY = 0;
            scale.DestX = xs;
            scale.DestY = ys;
            scale.SrcSpan = o->width;
            scale.DestSpan = bitmap_width;
            scale.SrcWidth = 70;    // o->width e o->height valgono 80...
            scale.SrcHeight = 70;
            scale.Dest = main_bitmap;
            scale.Src = o->bmap;
            scale.DestWidth = b->X2 - b->X1;
            scale.DestHeight = b->Y2 - b->Y1;

            bitmapScale(&scale);
        }
        break;
    default:
      fallback:
        TextShadow(xs + (b->X2 - b->X1 - len * tf->width) / 2,
                   ys + (b->Y2 - b->Y1 - tf->height) / 2 + tf->height -
                   1 /*+tf->tf_Baseline */ , testo, len);
    }
}

BOOL DoAction(WORD bottone)
{
    struct Bottone *b = &actual_menu->Bottone[bottone];

// i requester non hanno azioni associate con i tasti
    if (reqqing)
        return FALSE;

    if (current_menu == MENU_ARCADE && competition != MENU_TEAMS
        && b->ID > current_menu) {
        easy.es_TextFormat = msg_61;
        easy.es_GadgetFormat = msg_62;

        if (!MyEasyRequest(hwin, &easy, NULL))
            return TRUE;

        competition = MENU_TEAMS;
    }

    if (actual_menu->SpecialFunction) {
        BOOL value;
        BOOL(*func) (WORD);

        func = (BOOL(*)(WORD)) actual_menu->SpecialFunction;

        value = func(bottone);
        ScreenSwap();
        return value;
    } else if (b->ID >= 0) {
        WORD ID;
        penalties = FALSE;
        free_kicks = FALSE;

        switch (current_menu) {
        case MENU_HIGHLIGHT:
            savehigh = FALSE;

            if (bottone == 0) {
                SetHighSelection();
                menu[MENU_HIGH_SELECTION].Titolo = msg_63;
            } else if (bottone == 1) {
                SetHighSelection();
                savehigh = TRUE;
                menu[MENU_HIGH_SELECTION].Titolo = msg_64;
            }
            break;
        case MENU_SELECTION_TYPE:
            if (bottone == 0 && !arcade_teams) {
                if (!DoWarning((special || competition != MENU_TEAMS))) {
                    LoadTeams("teams/arcade");
                    arcade_teams = TRUE;
                } else
                    return TRUE;
            } else if (bottone == 1 && arcade_teams) {
                if (!DoWarning(competition != MENU_TEAMS)) {
                    LoadTeams("teams/default");
                    arcade_teams = FALSE;
                } else
                    return TRUE;
            }
            break;
        case MENU_GAME_START:
            if (bottone == 0) {
                if (!arcade_gfx[0])
                    LoadArcadeGfx();

                if (!arcade_back)
                    LoadArcadeBack();
            } else if (arcade_back) {
                LoadBack();
            }

            if (bottone == 3) {
                network_game = TRUE;
                friendly = TRUE;
                ClearSelection();
                wanted_number = 1;
            }
            break;
        case MENU_ARCADE:

            arcade = TRUE;
            training = FALSE;

            if (bottone == 0) {
                friendly = TRUE;
                ClearSelection();
                wanted_number = 2;
            } else if (bottone == 1)    // Whistle tour
            {
                friendly = FALSE;
                if (!DoWarning(((special || competition != MENU_MATCHES)
                                && competition != MENU_TEAMS))) {
                    if (competition != MENU_MATCHES) {
                        ClearSelection();
                        competition = MENU_MATCHES;
                        wanted_number = 0;
                    } else
                        b->ID = MENU_MATCHES;
                } else
                    return TRUE;
            } else if (bottone == 2)    // Challenge
            {
                friendly = FALSE;

                if (!DoWarning(((special || competition != MENU_CHALLENGE)
                                && competition != MENU_TEAMS))) {
                    if (!arcade_teams) {
                        LoadTeams("teams/arcade" /*-*/ );
                        arcade_teams = TRUE;
                    }

                    if (competition != MENU_CHALLENGE) {
                        competition = MENU_CHALLENGE;
                        b->ID = MENU_ARCADE_SELECTION;
                        ClearSelection();
                        wanted_number = 1;
                    } else
                        b->ID = MENU_CHALLENGE;
                } else
                    return TRUE;

            }

            if (bottone > 2) {
                if (arcade_back)
                    LoadBack();

                if (arcade_teams) {
                    LoadTeams("teams/default" /*-*/ );
                    arcade_teams = FALSE;
                }
            }

            break;
        case MENU_SIMULATION:
            arcade = FALSE;
            training = FALSE;
            friendly = FALSE;
            network_game = FALSE;

            if (arcade_teams && bottone < 4) {
                if (!DoWarning
                    ((competition == MENU_MATCHES
                      || competition == MENU_CHALLENGE))) {
                    LoadTeams("teams/default");
                    arcade_teams = FALSE;
                } else
                    return TRUE;
            }

            if (bottone == 0) {
                if (!DoWarning((!special && competition != MENU_TEAMS)))
                    SetupSpecialEvent(b);
                else
                    return TRUE;
            } else if (bottone == 1) {
                friendly = TRUE;
                ClearSelection();
                wanted_number = 2;
            } else if (bottone == 2) {
                if (!DoWarning(((special || competition != MENU_LEAGUE)
                                && competition != MENU_TEAMS))) {
                    if (competition != MENU_LEAGUE) {
                        ppv = 3;
                        ppp = 1;
                        pps = 0;
                        i_scontri = scontri = 1;
                        competition = MENU_LEAGUE;
                        ClearSelection();
                        b->ID = MENU_TEAM_SELECTION;
                        wanted_number = -3;
                    } else
                        b->ID = MENU_LEAGUE;
                } else
                    return TRUE;

            } else if (bottone == 3) {
                if (!DoWarning(((special || competition != MENU_MATCHES)
                                && competition != MENU_TEAMS))) {
                    if (competition != MENU_MATCHES) {
                        ClearSelection();
                        competition = MENU_MATCHES;
                        b->ID = MENU_TEAM_SELECTION;
                        wanted_number = 0;
                    } else
                        b->ID = MENU_MATCHES;
                } else
                    return TRUE;
            }
            break;
        case MENU_PREFS:
            if (bottone < 4) {
                UpdatePrefs(b->ID);
            }
            break;
        case MENU_CAREER:
            arcade = FALSE;
            training = FALSE;

            if (bottone >= 0 && bottone < 3) {
                selected_number = 0;
                wanted_number = 1;
            }
            break;
        case MENU_TRAINING:
            arcade = FALSE;
            training = TRUE;

            if (arcade_teams && bottone < 3) {
                LoadTeams("teams/default");
                arcade_teams = FALSE;
            }

            if (bottone >= 0 && bottone < 3) {
                if (bottone == 2)
                    penalties = TRUE;
                else if (bottone == 1)
                    free_kicks = TRUE;

                ClearSelection();
                wanted_number = 1;
            }
            break;
        }

        ID = b->ID;

        if (b->ID == MENU_MATCHES || b->ID == MENU_CHALLENGE) {
            if ((current_menu != MENU_CHALLENGE
                 && current_menu != MENU_MATCHES) || make_setup) {
// Qui prepariamo le partite
                SetupMatches();
            } else {
// Qui vengono giocate le partite
                PlayMatches();
            }
        }

        ChangeMenu(ID);
    } else {
        if (b->ID == ID_QUIT_GAME)
            return FALSE;

        switch (current_menu) {
        case MENU_SCORES:
            easy.es_TextFormat = msg_65;
            easy.es_GadgetFormat = msg_66;

            if (MyEasyRequest(hwin, &easy, NULL))
                ClearScores();

            break;
        case MENU_MATCHES:
        case MENU_LEAGUE:
        case MENU_WORLD_CUP:
            if (bottone != 1)
                break;

            if (arcade) {
                easy.es_TextFormat = msg_67;
                easy.es_GadgetFormat = msg_58;

                MyEasyRequest(hwin, &easy, NULL);
                break;
            }

            freq.Dir = "saves";
            freq.Title = msg_68;
            freq.Save = TRUE;
            freq.Filter = "Career files|*.car";

            if (FileRequest(&freq)) {
                strcpy(career_file, freq.File);

                SaveLeague();
            }
            break;
        case MENU_PREFS:
            if (bottone == 5) {
                FILE *f;

                if (!(f = fopen("etw.cfg" /*-*/ , "w"))) {
                    char buf[1024];
                    snprintf(buf, 1024, "%setw.cfg", TEMP_DIR);
                    if (!(f = fopen(buf /*-*/ , "w"))) {
                        easy.es_TextFormat = msg_69;
                        easy.es_GadgetFormat = msg_58;

                        MyEasyRequest(hwin, &easy, NULL);
                        break;
                    } else {
                        fclose(f);
                        write_config(buf /*-*/ );
                    }
                } else {
                    fclose(f);
                    write_config("etw.cfg" /*-*/ );
                }

                easy.es_TextFormat = msg_70;
                easy.es_GadgetFormat = msg_58;

                MyEasyRequest(hwin, &easy, NULL);

            }
            break;
        case MENU_TEAMS:
            if (bottone == 1) {
                char buffer[130];

                if (competition != MENU_TEAMS) {
                    easy.es_TextFormat = msg_61;
                    easy.es_GadgetFormat = msg_62;

                    if (!MyEasyRequest(hwin, &easy, NULL))
                        break;
                }

                freq.Title = msg_71;
                freq.Dir = "teams/";

                if (FileRequest(&freq)) {
                    strcpy(buffer, freq.File);

                    D(bug("Loading teams <%s>...\n" /*-*/ , buffer));
                    LoadTeams(buffer);

                    competition = MENU_TEAMS;
                }
            } else if (bottone == 2) {
                char buffer[130];
                UBYTE temp = competition;

                freq.Title = msg_72;
                freq.Save = TRUE;
                freq.Dir = "teams/";

                if (FileRequest(&freq)) {
                    strcpy(buffer, freq.File);

                    competition = MENU_TEAMS;

                    SaveTeams(buffer);

                    competition = temp;
                }
            } else if (bottone == 3) {
                extern char teamfile[];

                if (competition != MENU_TEAMS) {
                    easy.es_TextFormat = msg_61;
                    easy.es_GadgetFormat = msg_62;

                    if (!MyEasyRequest(hwin, &easy, NULL))
                        break;
                }

                competition = MENU_TEAMS;
                turno = 0;
                special = 0;

                LoadTeams(teamfile);
            } else {
                easy.es_TextFormat = msg_73;
                easy.es_GadgetFormat = msg_58;

                MyEasyRequest(hwin, &easy, NULL);
            }
            break;
        case MENU_HIGHLIGHT:
            if (bottone == 2) {
                char buffer[130];

                freq.Title = msg_74;

                if (FileRequest(&freq)) {
                    strcpy(buffer, freq.File);

                    LoadHigh(buffer);
                }
            }
            break;
        case MENU_GAME_START:
            if (bottone == 2)
                request(msg_75);
            break;
        case MENU_MAIN_MENU:
            if (bottone == 5)
                ShowCredits();
            break;
        case MENU_SIMULATION:
        case MENU_CAREER:
            if (current_menu == MENU_SIMULATION)
                bottone--;

            if (bottone == 3) {
                if (*career_file) {
                    easy.es_TextFormat = msg_61;
                    easy.es_GadgetFormat = msg_62;
                    if (!MyEasyRequest(hwin, &easy, NULL))
                        break;
                }

                freq.Title = current_menu == MENU_CAREER ? msg_76 : msg_77;
                freq.Dir = "saves/" /*-*/ ;
                freq.Filter = "Career files|*.car";

                if (FileRequest(&freq)) {
                    strcpy(career_file, freq.File);

                    LoadLeague();
                }
            } else if (bottone == 4) {
                if (current_menu == MENU_SIMULATION) {
                    if (competition == MENU_TEAMS)
                        break;

                    easy.es_TextFormat = msg_190;
                    easy.es_GadgetFormat = msg_60;

                    if (MyEasyRequest(hwin, &easy, NULL)) {
                        turno = 0;
                        special = FALSE;
                        competition = MENU_TEAMS;
                    }
                } else {
                    freq.Title =
                        current_menu == MENU_CAREER ? msg_78 : msg_79;
                    freq.Dir = "saves/" /*-*/ ;
                    freq.Filter = "Career files|*.car";
                    freq.Save = TRUE;

                    if (FileRequest(&freq)) {
                        strcpy(career_file, freq.File);

                        SaveLeague();
                    }
                }
            }
            break;
        default:
            D(bug("No special procs for this menu!\n"));
        }
    }

    return TRUE;
}

void CancellaBottone(struct Bottone *b)
{
    bltchunkybitmap(back, b->X1, b->Y1, main_bitmap,
                    b->X1, b->Y1, b->X2 - b->X1 + 1, b->Y2 - b->Y1 + 1,
                    bitmap_width, bitmap_width);
}

void PrintButtonType(struct Bottone *b, WORD bl, WORD bt,
                     struct myfont *tf)
{
    char *c;

    if ((current_menu != MENU_TEAM_SELECTION
         || b > &actual_menu->Bottone[63])
        && (current_menu != MENU_ARCADE_SELECTION
            || b > &actual_menu->Bottone[ARCADE_TEAMS - 1]))
        return;

    switch (b->Colore) {
    case COLORE_TEAM_A:
        c = "_J2" /*-*/ ;
        break;
    case COLORE_TEAM_B:
        c = "_J1" /*-*/ ;
        break;
    case COLORE_COMPUTER:
        c = "_C" /*-*/ ;
        break;
    default:
        c = NULL;
    }

    if (c) {
        StampaTesto(b, bl + b->X1, bt + b->Y1, c, strlen(c), tf);
    }
}

void CreaBottone(struct Bottone *b)
{
    char *t = b->Testo;
    struct myfont *tf = smallfont;
    int l, top = 1;

    if (!t)
        return;

    rectfill(main_bitmap, b->X1, b->Y1, b->X2, b->Y2, Pens[b->Colore],
             bitmap_width);

    if ((b->Y2 - b->Y1) > 9) {
        setfont(bigfont);
        tf = bigfont;

        draw(Pens[b->Highlight], b->X1, b->Y2 - 1, b->X1, b->Y1);
        draw(Pens[b->Highlight], b->X1, b->Y1, b->X2 - 1, b->Y1);
    } else {
        setfont(smallfont);
        top -= 1;
    }

    if ((b->X2 - b->X1) > 16) {
        main_bitmap[b->X1 + b->Y1 * bitmap_width] = Pens[P_BIANCO];
    }

    l = strlen(t);

    if ((l * tf->width) > (b->X2 - b->X1 + 2)) {
        l = (b->X2 - b->X1) / tf->width;
    }

    StampaTesto(b, b->X1, b->Y1 + top, t, l, tf);
    PrintButtonType(b, 0, top, tf);
}

void RedrawBottone(struct Bottone *b, UBYTE colore)
{
    char *t = b->Testo;
    struct myfont *tf = smallfont;
    int l, top = 1;

    if (!t)
        return;

    rectfill(main_bitmap, b->X1, b->Y1, b->X2, b->Y2, Pens[colore],
             bitmap_width);

    if ((b->Y2 - b->Y1) > 9) {
        setfont(bigfont);
        tf = bigfont;

        draw(Pens[b->Highlight], b->X1, b->Y2 - 1, b->X1, b->Y1);
        draw(Pens[b->Highlight], b->X1, b->Y1, b->X2 - 1, b->Y1);
    } else {
        setfont(smallfont);
        top -= 1;
    }

    if ((b->X2 - b->X1) > 16) {
        main_bitmap[b->X1 + b->Y1 * bitmap_width] = Pens[P_BIANCO];
    }

    l = strlen(t);

    if ((l * tf->width) > (b->X2 - b->X1 + 2)) {
        l = (b->X2 - b->X1) / tf->width;
    }

    StampaTesto(b, b->X1, b->Y1 + top, t, l, tf);
    PrintButtonType(b, 0, top, tf);
}

void ChangeMenu(WORD m)
{
    extern WORD restore_w, restore_h;

#ifdef CD_VERSION
    static int menu_changes = 0;
#endif
    int i;

    restore_w = restore_h = 0;

    bltchunkybitmap(back, 0, 0, main_bitmap, 0, 0, WINDOW_WIDTH,
                    WINDOW_HEIGHT, bitmap_width, bitmap_width);

#ifdef CD_VERSION
    menu_changes++;

    if (menu_changes > 12) {
        menu_changes = 0;

        StopMenuMusic();

        os_delay(10);

        PlayMenuMusic();
    }
#endif
    actual_menu = &menu[m];

// Questo mi permette di tornare al menu' giusto dalla teamselection;

    if (m != current_menu) {
        if (m != MENU_TEAM_SELECTION
            || (current_menu != MENU_TEAM_SETTINGS
                && current_menu != MENU_MATCH_RESULT)) {
            if (actual_menu->Bottone == teamselection) {
                teamselection[65].ID = current_menu;
            } else if (actual_menu->Bottone == teamsettings) {
                teamsettings[42].ID = current_menu;
            }
        }

        if (m == MENU_ARCADE_SELECTION
            && current_menu != MENU_TEAM_SETTINGS) {
            actual_menu->Bottone[ARCADE_TEAMS + 1].ID = current_menu;
        }
        current_menu = m;
    }

    setfont(titlefont);

    rectfill(main_bitmap, 0, FixedScaledY(7), WINDOW_WIDTH - 1,
             FixedScaledY(16), Pens[actual_menu->Colore], bitmap_width);
    rectfill(main_bitmap, 0, FixedScaledY(6), WINDOW_WIDTH - 1,
             FixedScaledY(7) - 1, Pens[actual_menu->Highlight],
             bitmap_width);

    if (actual_menu->Immagine >= 0) {
            struct scaleAnimObjArgs a;
            
            a.src = logos->Frames[actual_menu->Immagine];
            a.dest = main_bitmap;
            a.destmod = bitmap_width;
            a.xs = 0;
            a.ys = 0;
            a.ws = logos->Widths[actual_menu->Immagine];
            a.hs = logos->Heights[actual_menu->Immagine];
            a.xd = actual_menu->X;
            a.yd = actual_menu->Y;
            a.wd = FixedScaledX(logos->Widths[actual_menu->Immagine]);
            a.hd = FixedScaledY(logos->Widths[actual_menu->Immagine]);
            D(bug("bltanimobjscale: (%d,%d %dx%d) to (%d,%d %dx%d)\n", 
                        a.xs, a.ys, a.ws, a.hs, 
                        a.xd, a.yd, a.wd, a.hd));

            bltanimobjscale(&a);
#if 0
        BltAnimObj(logos, main_bitmap, actual_menu->Immagine,
                   actual_menu->X, actual_menu->Y, bitmap_width);
#endif
    }

    if (current_menu == MENU_TEAM_SETTINGS) {
        char *c = msg_80;
        int l, l2;
        int y = FixedScaledY(titlefont->height + 8);

        if (y <= titlefont->height)
            y = titlefont->height + 1;

        i = strlen(teamlist[actual_team].name);

        TextShadow(FixedScaledX(5), y, teamlist[actual_team].name, i);

        setfont(WINDOW_HEIGHT > 300 ? bigfont : smallfont);

        i = strlen(c);
        l = titlefont->width * i;
        l2 = strlen(teamlist[actual_team].allenatore) * titlefont->width;

        y = FixedScaledY(titlefont->height - 1);

        if (WINDOW_HEIGHT < 300 && y <= smallfont->height)
            y = smallfont->height + 1;

        TextShadow(WINDOW_WIDTH - l - l2 - FixedScaledX(4), y, c, i);

        i = strlen(teamlist[actual_team].allenatore);
        TextShadow(WINDOW_WIDTH - l2 - FixedScaledX(4),
                   FixedScaledY(titlefont->height + 8),
                   teamlist[actual_team].allenatore, i);
    } else if (actual_menu->Titolo) {
        char *t = actual_menu->Titolo;
        int y = FixedScaledY(titlefont->height + 3);

        if (y <= titlefont->height)
            y = titlefont->height + 1;

        i = strlen(t);

        if (*t != '-') {
            TextShadow(WINDOW_WIDTH / 2 - i * titlefont->width / 2, y, t,
                       i);
        } else {
            t++;
            i--;

            TextShadow(FixedScaledX(5), y, t, i);
        }
    }

    for (i = 0; i < actual_menu->NumeroBottoni; i++)
        CreaBottone(&actual_menu->Bottone[i]);

    for (i = 0; i < actual_menu->NumeroPannelli; i++)
        CreaBottone(&actual_menu->Pannello[i]);

    actual_button = 0;

    if (m == MENU_TEAM_SETTINGS) {
        DisplayTactic(0, 0);

        if (ruolo[actual_team] && controllo[actual_team] >= 0) {
            UBYTE oldpen;

            oldpen =
                actual_menu->Bottone[ruolo[actual_team] * 2 + 1].Colore;

            actual_menu->Bottone[ruolo[actual_team] * 2 + 1].Colore =
                P_GIALLO;

            CreaBottone(&actual_menu->Bottone[ruolo[actual_team] * 2 + 1]);

            actual_menu->Bottone[ruolo[actual_team] * 2 + 1].Colore =
                oldpen;
        }

        actual_button = 17 * 2 + 6 + 2;
    }

    if (current_menu == MENU_MAIN_MENU) {
        char *c = ETW_VERSION;
        int l, x, y;

        setfont(smallfont);

        l = strlen(c);

        x = WINDOW_WIDTH - l * smallfont->width - 2;
        y = WINDOW_HEIGHT - smallfont->height - 1;
        TextShadow(x, y, c, l);
    }
//      bltchunkybitmap(main_bitmap,0,0,main_bitmap,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,bitmap_width,bitmap_width);
//      GriddedWipe(1,main_bitmap);

    while (!actual_menu->Bottone[actual_button].Testo)
        actual_button++;


    DrawBox(actual_button);
    ScreenSwap();
}

void SelezionaBottone(WORD bottone)
{
    if (bottone != NESSUN_BOTTONE) {
        struct Bottone *b = &actual_menu->Bottone[bottone];

        if ((b->Y2 - b->Y1) > 9) {
            draw(Pens[b->Colore], b->X1, b->Y2, b->X1, b->Y1);
            draw(Pens[b->Colore], b->X1, b->Y1, b->X2, b->Y1);
        } else
            RedrawBottone(b, b->Highlight);
    }
}

void DeselezionaBottone(WORD bottone)
{
    if (bottone != NESSUN_BOTTONE) {
        struct Bottone *b = &actual_menu->Bottone[bottone];

        if ((b->Y2 - b->Y1) > 9) {
            draw(Pens[b->Highlight], b->X1, b->Y2 - 1, b->X1, b->Y1);
            draw(Pens[b->Highlight], b->X1, b->Y1, b->X2 - 1, b->Y1);

            if ((b->X2 - b->X1) > 16) {
                main_bitmap[b->X1 + b->Y1 * bitmap_width] =
                    (UBYTE) Pens[P_BIANCO];
            }
        } else
            RedrawBottone(b, b->Colore);
    }
}

WORD GetBottone(WORD x, WORD y)
{
    WORD i;

    for (i = 0; i < actual_menu->NumeroBottoni; i++) {
        struct Bottone *b = &actual_menu->Bottone[i];

        if (b->Testo && x < b->X2 && x > b->X1 && y < b->Y2 && y > b->Y1) {
            return i;
        }
    }

    return NESSUN_BOTTONE;
}

void MoveMark(int k)
{
    WORD old_button = actual_button;

    if ((actual_button + k) < 0)
        k = actual_button;
    else if (actual_button + k >= actual_menu->NumeroBottoni)
        k = actual_menu->NumeroBottoni - actual_button - 1;

    if (k == 0)
        return;

    do {
        actual_button += k;

        if (actual_button < 0
            || actual_button >= actual_menu->NumeroBottoni) {
            actual_button = old_button;
            return;
        }
    }
    while (!actual_menu->Bottone[actual_button].Testo);
}

BOOL HandleJoy(uint32_t joystatus)
{
    static BOOL clicked = FALSE;
    WORD old_button = actual_button;

#if 0
    if ((joystatus & JP_TYPE_MASK) == JP_TYPE_MOUSE) {
        return TRUE;
    }
#endif

    if (joystatus & JPF_BUTTON_RED) {
        SelezionaBottone(actual_button);
        ScreenSwap();
        clicked = TRUE;
    } else if (clicked) {
        clicked = FALSE;
        DeselezionaBottone(actual_button);
        ScreenSwap();
// questo serve perche' il requester usa current button
// per capire se si vuole uscire
        current_button = actual_button;

        return DoAction(actual_button);
    }

    if (!(joystatus & JP_DIRECTION_MASK))
        return TRUE;

    if(reqqing) {
        if (joystatus & (JPF_JOY_UP | JPF_JOY_LEFT)) {
            MoveMark(-1);
        } else if (joystatus & (JPF_JOY_DOWN | JPF_JOY_RIGHT)) {
            MoveMark(+1);
        }
    }
    else switch (current_menu) {
    case MENU_ARCADE_SELECTION:
        if (joystatus & JPF_JOY_UP) {
            switch (actual_button) {
            case 4:
            case 9:
            case 10:
            case 11:
                MoveMark(-4);
                break;
            case 12:
            case 13:
                MoveMark(-3);
                break;
            case 5:
            case 6:
                MoveMark(-2);
                break;
            case 7:
            case 8:
                MoveMark(-6);
                break;
            default:            /* 0 1 2 3 */
                break;
            }
        } else if (joystatus & JPF_JOY_DOWN) {
            switch (actual_button) {
            case 10:
            case 11:
            case 12:
                break;
            case 1:
            case 2:
                MoveMark(6);
                break;
            case 3:
            case 4:
                MoveMark(2);
                break;
            case 8:
            case 9:
                MoveMark(3);
                break;
            default:            /* 0 5 6 7  */
                MoveMark(4);
                break;
            }
        }

        if (joystatus & JPF_JOY_LEFT) {
            MoveMark(-1);
        } else if (joystatus & JPF_JOY_RIGHT) {
            MoveMark(1);
        }
        break;
    case MENU_GAME_PREFS:
    case MENU_VIDEO_PREFS:
    case MENU_AUDIO_PREFS:
        if (joystatus & (JPF_JOY_UP | JPF_JOY_LEFT)) {
            MoveMark(-2);
        } else if (joystatus & (JPF_JOY_DOWN | JPF_JOY_RIGHT)) {
            MoveMark(+2);
        }
        break;
#if 0    
    /* At the moment is too complex reordering the menu in order
     * to have EraseBox/DrawBox and HandleJoy working same way for
     * all the options menu.
     */
    case MENU_SYSTEM_PREFS:
        /* There is an erratic behavior, when the button for
         * Joy configuration is hidden.
         */
        if (joystatus & (JPF_JOY_UP | JPF_JOY_LEFT)) {
            if(actual_button > 10)
                MoveMark(-1);
            else
                MoveMark(-2);
        } else if (joystatus & (JPF_JOY_DOWN | JPF_JOY_RIGHT)) {
            if(actual_button > 8 /*10*/)
                MoveMark(+1);
            else
                MoveMark(+2);
        }
    break;
#endif
    case MENU_TEAM_SELECTION:
        if (actual_button < 64) {
            if (joystatus & JPF_JOY_UP) {
                MoveMark(-4);
            } else if (joystatus & JPF_JOY_DOWN) {
                if (actual_button < 60)
                    MoveMark(4);
                else
                    MoveMark(64 - actual_button);
            }

            if (joystatus & JPF_JOY_LEFT) {
                MoveMark(-1);
            } else if (joystatus & JPF_JOY_RIGHT) {
                MoveMark(1);
            }

            break;
        }
    case MENU_TEAM_SETTINGS:
        if (actual_button < 34) {
            if (joystatus & JPF_JOY_UP) {
                MoveMark(-2);
            } else if (joystatus & JPF_JOY_DOWN) {
                MoveMark(2);
            }

            if (joystatus & JPF_JOY_LEFT) {
                MoveMark(-1);
            } else if (joystatus & JPF_JOY_RIGHT) {
                MoveMark(1);
            }

            break;
        } else if (actual_button < 45) {
            if (joystatus & JPF_JOY_UP) {
                MoveMark(-3);
            } else if (joystatus & JPF_JOY_DOWN) {
                MoveMark(3);
            }

            if (joystatus & JPF_JOY_LEFT) {
                MoveMark(-1);
            } else if (joystatus & JPF_JOY_RIGHT) {
                MoveMark(1);
            }

            break;
        }
    case MENU_MAIN_MENU:
        if (actual_button < 6) {
            if (joystatus & JPF_JOY_UP) {
                MoveMark(-1);
            } else if (joystatus & JPF_JOY_DOWN) {
                MoveMark(1);
            }

            if (joystatus & JPF_JOY_LEFT) {
                MoveMark(-3);
            } else if (joystatus & JPF_JOY_RIGHT) {
                MoveMark(3);
            }

            break;
        }
    default:
        if (joystatus & (JPF_JOY_UP | JPF_JOY_LEFT)) {
            MoveMark(-1);
        } else if (joystatus & (JPF_JOY_DOWN | JPF_JOY_RIGHT)) {
            MoveMark(+1);
        }

    }

    if (old_button != actual_button) {
        EraseBox(old_button);
        DrawBox(actual_button);
        ScreenSwap();
    }

    return TRUE;
}

void MenuResizing(int w, int h)
{
//      gfx_t o;
//      unsigned char *back2;

    extern SDL_Surface *screen;

    oldwidth = WINDOW_WIDTH;
    oldheight = WINDOW_HEIGHT;

    if ((screen =
        SDL_SetVideoMode(w, h, 8,
                         SDL_SWSURFACE | (wb_game ? SDL_RESIZABLE :
                                          SDL_FULLSCREEN)))) {
        LoadIFFPalette("gfx/eat16menu.col" /*-*/ );

        free(main_bitmap);
        free(back);
        bitmap_width = WINDOW_WIDTH = w;
        bitmap_height = WINDOW_HEIGHT = h;

        main_bitmap = malloc(WINDOW_WIDTH * WINDOW_HEIGHT);
        back = malloc(WINDOW_WIDTH * WINDOW_HEIGHT);

        if (!main_bitmap || !back) {
            request("Not enough memory.\n");
            FreeMenuStuff();
            exit(0);
        }

        if (arcade_back)
            LoadArcadeBack();
        else
            LoadBack();

        UpdateButtonList();
        ChangeMenu(current_menu);
    } else {
        D(bug("Error reopening screen"));
//              request("Not enough memory.");
        FreeMenuStuff();
        exit(0);
    }
}

BOOL HandleMenuIDCMP(void)
{
    SDL_Event e;
    BOOL returncode = TRUE;
    
    if (SDL_WaitEvent(&e)) {

        switch (e.type) {
        case SDL_VIDEORESIZE:
            if (!reqqing)
                MenuResizing(e.resize.w, e.resize.h);
            break;
/*
            case IDCMP_INTUITICKS:
                returncode=HandleJoy(ReadJoyPort(1));
//                returncode&=HandleJoy(ReadJoyPort(0));
                break;
*/
        case SDL_QUIT:
            D(bug("SDL quit received!\n"));
            if (!reqqing)
                returncode = FALSE;
            break;
        case SDL_KEYDOWN:
            switch (e.key.keysym.sym) {
                case SDLK_SPACE:
                case SDLK_RETURN:
                    returncode = HandleJoy(JPF_BUTTON_RED);
                    break;
                default:
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (e.key.keysym.sym) {
#ifndef WINCE
            case SDLK_UP:
                returncode = HandleJoy(JPF_JOY_UP);
                break;
            case SDLK_DOWN:
                returncode = HandleJoy(JPF_JOY_DOWN);
                break;
            case SDLK_LEFT:
                returncode = HandleJoy(JPF_JOY_LEFT);
                break;
            case SDLK_RIGHT:
                returncode = HandleJoy(JPF_JOY_RIGHT);
                break;
#else
            case SDLK_UP:
                returncode = HandleJoy(JPF_JOY_RIGHT);
                break;
            case SDLK_DOWN:
                returncode = HandleJoy(JPF_JOY_LEFT);
                break;
            case SDLK_LEFT:
                returncode = HandleJoy(JPF_JOY_UP);
                break;
            case SDLK_RIGHT:
                returncode = HandleJoy(JPF_JOY_DOWN);
                break;
#endif
              case SDLK_SPACE:
               case SDLK_RETURN:
                    returncode = HandleJoy(0L);
                    break;
            case SDLK_ESCAPE:
                if (!reqqing)
                    returncode = FALSE;
                break;
            case SDLK_r:
                CheckCheat(e.key.keysym.sym);

                if (current_menu != MENU_TEAM_SETTINGS || !can_modify)
                    break;

                if (ruolo[actual_team]) {
                    RedrawBottone(&actual_menu->
                                  Bottone[ruolo[actual_team] * 2 + 1],
                                  actual_menu->Bottone[ruolo[actual_team] *
                                                       2 + 1].Colore);
                    ruolo[actual_team] = 0;
                    ScreenSwap();
                } else if (actual_button < 22 && actual_button > 1) {
                    ruolo[actual_team] = actual_button / 2;
                    RedrawBottone(&actual_menu->
                                  Bottone[ruolo[actual_team] * 2 + 1],
                                  P_GIALLO);
                    ScreenSwap();
                }
                break;
            case SDLK_q:
                if (!reqqing)
                    returncode = FALSE;
                break;
            default:
                if (!reqqing)
                    CheckCheat(e.key.keysym.sym);

            }
            break;

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.state == SDL_PRESSED) {
                current_button = GetBottone(e.button.x, e.button.y);
                SelezionaBottone(current_button);

                if (actual_button != current_button && current_button >= 0) {
                    EraseBox(actual_button);

                    actual_button = current_button;
                    
                    DrawBox(actual_button);
                    ScreenSwap();
                }
            } else if (e.button.state == SDL_RELEASED
                       && current_button != NESSUN_BOTTONE) {
                WORD temp;

                DeselezionaBottone(current_button);
                ScreenSwap();

                temp = GetBottone(e.button.x, e.button.y);

                if (current_button == temp) {
                    returncode = DoAction(current_button);
                }
            }
            break;
        }
    }

    if (!returncode && !reqqing)
        if (!CheckQuit())
            return TRUE;

    return returncode;
}
