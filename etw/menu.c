#include "etw_locale.h"
#include "menu.h"
#include <SDL.h>
#include <ctype.h>
#include "tactics.h"
#include "chunky.h"

int current_menu = 0, current_button = NO_BUTTON, actual_button = 0;
struct GfxMenu *actual_menu = &menu[0];
void CreateButton(struct Button *b);
void SelectButton(int16_t button);
void DrawBox(int16_t button);
static BOOL new_competition = FALSE;
struct Button req_bottoni[4];
BOOL reqqing = FALSE;
extern BOOL pause_mode;

struct MyFileRequest freq = { 0 };
static const char *teamsel_message = NULL;

BOOL MyEasyRequest(struct EasyStruct *e)
{
    struct GfxMenu *old_menu, fake_menu;
    int i, x, y, width = FixedScaledX(150), button_width =
        FixedScaledX(30), height = FixedScaledY(35), bottoni = 1, linee =
        1, len = 0, leftedge, topedge, old_current_menu = current_menu;
#ifdef MOBILE_VERSION
    height += FixedScaledY(10);
#endif
    struct myfont *tf;
    char text[1024];
    char buttons[100];
    char *c;

    rectfill_pattern(main_bitmap, 0, 0, WINDOW_WIDTH - 1,
                     WINDOW_HEIGHT - 1, 0, bitmap_width);
    
    strncpy(text, e->es_TextFormat, sizeof(text) - 1);
    strncpy(buttons, e->es_GadgetFormat, sizeof(buttons) - 1);

    if (WINDOW_WIDTH > 400)
        tf = bigfont;
    else
        tf = smallfont;

    // Compute how many lines of text there are.
    c = text;
    
    while (*c) {
        if (((len + 4) * tf->width) > WINDOW_HEIGHT) {
            
            while (*c != ' ' && *c != 0 && len > 1) {
                c--;
                len--;
            }
            
            width = max(width, (len + 4) * tf->width);
            *c = 0;
            len = 0;
            linee++;
        }
        else if (*c == '\n') {
            linee++;
            width = max(width, (len + 4) * tf->width);
            len = 0;
            *c = 0;
        } else {
            len++;
            *c = toupper(*c);
        }
        
        c++;
    }

    width = max(width, (len + 4) * tf->width);

    height += (tf->height + FixedScaledY(3)) * linee;

    if (WINDOW_WIDTH <= width) {
        D(bug("Error in width calculation!\n"/*-*/));
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
        req_bottoni[i].Color = Pens[P_VERDE1];
        req_bottoni[i].Highlight = Pens[P_VERDE0];
        req_bottoni[i].Text = strdup(c);
        req_bottoni[i].Y1 =
            topedge + height - tf->height - FixedScaledY(14);
#ifdef MOBILE_VERSION
        req_bottoni[i].Y1 -= FixedScaledY(10);
#endif
        req_bottoni[i].Y2 = topedge + height - FixedScaledY(6);

        CreateButton(&req_bottoni[i]);

        c += strlen(c);

        if (i < (bottoni - 1))
            *c = '|';
        c++;
    }

    // Write the text
    c = text;
    y = topedge + FixedScaledY(13);

    setfont(tf);

    while (linee) {
        len = (int)strlen(c);
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
    fake_menu.Button = req_bottoni;
    actual_button = 0;
    current_menu = MENUS;
    
    D(bug("Creating requester with %d buttons\n"/*-*/, bottoni));

    DrawBox(0);
/*
    draw(Pens[P_GIALLO], req_bottoni[0].X1 - 1, req_bottoni[0].Y1 - 1,
        req_bottoni[0].X2 + 1, req_bottoni[0].Y1 - 1);
    draw(Pens[P_GIALLO], req_bottoni[0].X2 + 1, req_bottoni[0].Y1 - 1,
        req_bottoni[0].X2 + 1, req_bottoni[0].Y2 + 1);
    draw(Pens[P_GIALLO], req_bottoni[0].X2 + 1, req_bottoni[0].Y2 + 1,
        req_bottoni[0].X1 - 1, req_bottoni[0].Y2 + 1);
    draw(Pens[P_GIALLO], req_bottoni[0].X1 - 1, req_bottoni[0].Y2 + 1,
        req_bottoni[0].X1 - 1, req_bottoni[0].Y1);
*/
    ScreenSwap();
    
    reqqing = TRUE;
    
    do {
        SDL_WaitEvent(NULL);
    } while (HandleMenuIDCMP());

    linee = current_button;
    actual_menu = old_menu;
    current_menu = old_current_menu;
    
    reqqing = FALSE;

    if (!game_start || pause_mode)
        ChangeMenu(current_menu);

    for (i = 0; i < bottoni; i++)
        free(req_bottoni[i].Text);

    if (linee < (bottoni - 1))
        return linee + 1;
    else
        return 0;
}

BOOL CheckQuit(void)
{
    easy.es_TextFormat = msg_65;
    easy.es_GadgetFormat = msg_62;

    if (!game_start && (current_menu == MENU_TEAM_SETTINGS ||
                        current_menu == MENU_MATCH_RESULT))
        return TRUE;

    if (MyEasyRequest(&easy))
        return TRUE;
    else
        return FALSE;
}

struct EasyStruct easy = {  msg_57, NULL,  NULL };

void request(const char *t)
{
    easy.es_TextFormat = t;
    easy.es_GadgetFormat = msg_58;

    MyEasyRequest(&easy);
}

BOOL DoWarning(int a)
{
    if (a) {
        easy.es_TextFormat = msg_59;
        easy.es_GadgetFormat = msg_60;

        if (MyEasyRequest(&easy)) {
            turno = 0;
            special = FALSE;
            competition = MENU_TEAMS;
        } else
            return TRUE;
    }

    return FALSE;
}

int16_t *menubase = NULL;

void StoreButtonList(void)
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

    D(bug("Allocated %ld words for default menu datas.\n"/*-*/, k));

    if (!(menubase = malloc(k * sizeof(int16_t))))
        return;

    for (i = 0, k = 0; i < MENUS; i++) {
        for (j = 0; j < menu[i].NumeroBottoni; j++) {
            menubase[k] = menu[i].Button[j].X1;
            menubase[k + 1] = menu[i].Button[j].Y1;
            menubase[k + 2] = menu[i].Button[j].X2;
            menubase[k + 3] = menu[i].Button[j].Y2;
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


void UpdateButtonList(void)
{
    register int i, j, k;

    if (!menubase)
        return;

    for (i = 0, k = 0; i < MENUS; i++) {
        for (j = 0; j < menu[i].NumeroBottoni; j++) {
            menu[i].Button[j].X1 = FixedScaledX(menubase[k]);
            menu[i].Button[j].Y1 = FixedScaledY(menubase[k + 1]);
            menu[i].Button[j].X2 = FixedScaledX(menubase[k + 2]);
            menu[i].Button[j].Y2 = FixedScaledY(menubase[k + 3]);
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

void EraseBox(int16_t button)
{
    if (reqqing) {
// Il colore di fondo del requester e' sempre: Pens[P_ROSSO1]
        struct Button *b = &actual_menu->Button[button];

        draw(Pens[P_ROSSO1], b->X1 - 1, b->Y1 - 1, b->X2 + 1, b->Y1 - 1);
        draw(Pens[P_ROSSO1], b->X2 + 1, b->Y1 - 1, b->X2 + 1, b->Y2 + 1);
        draw(Pens[P_ROSSO1], b->X2 + 1, b->Y2 + 1, b->X1 - 1, b->Y2 + 1);
        draw(Pens[P_ROSSO1], b->X1 - 1, b->Y2 + 1, b->X1 - 1, b->Y1);
    } else if (current_menu < MENU_GAME_PREFS
               || current_menu > MENU_AUDIO_PREFS
               || button == actual_menu->NumeroBottoni - 1) {
        struct Button *b = &actual_menu->Button[button];

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
        struct Button *b = &actual_menu->Button[(button >> 1) << 1];
        struct Button *b2 =
            &actual_menu->Button[((button >> 1) << 1) + 1];

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

void DrawBox(int16_t button)
{
    if (current_menu < MENU_GAME_PREFS || current_menu > MENU_AUDIO_PREFS
        || button == actual_menu->NumeroBottoni - 1) {
        struct Button *b = &actual_menu->Button[button];

        draw(Pens[P_GIALLO], b->X1 - 1, b->Y1 - 1, b->X2 + 1, b->Y1 - 1);
        draw(Pens[P_GIALLO], b->X2 + 1, b->Y1 - 1, b->X2 + 1, b->Y2 + 1);
        draw(Pens[P_GIALLO], b->X2 + 1, b->Y2 + 1, b->X1 - 1, b->Y2 + 1);
        draw(Pens[P_GIALLO], b->X1 - 1, b->Y2 + 1, b->X1 - 1, b->Y1);
    } else {
        struct Button *b = &actual_menu->Button[(button >> 1) << 1];
        struct Button *b2 =
            &actual_menu->Button[((button >> 1) << 1) + 1];

        draw(Pens[P_GIALLO], b->X1 - 1, b->Y1 - 1, b2->X2 + 1, b->Y1 - 1);
        draw(Pens[P_GIALLO], b2->X2 + 1, b->Y1 - 1, b2->X2 + 1,
             b2->Y2 + 1);
        draw(Pens[P_GIALLO], b2->X2 + 1, b2->Y2 + 1, b->X1 - 1,
             b2->Y2 + 1);
        draw(Pens[P_GIALLO], b->X1 - 1, b2->Y2 + 1, b->X1 - 1, b->Y1);

    }
}


void DisplayText(struct Button *b, int xs, int ys, char *text, int len,
                 struct myfont *tf)
{
    switch (*text) {
// A destra
    case '_':
        {
            int c, y;

// Con questo gestisco decentemente la team selection arcade

            if (current_menu == MENU_ARCADE_SELECTION) {
                c = b->Color;
                y = ys + b->Y2 - b->Y1 - 3;    /* +tf_Baseline */
            } else {
                c = P_BIANCO;
                y = ys + (b->Y2 - b->Y1 - tf->height) / 2 + tf->height -
                    1 /*+tf->tf_Baseline */ ;
            }

            text++;
            len--;
            ColorTextShadow(xs + (b->X2 - b->X1 - tf->width * len - 2), y,
                            text, len, c);
        }
        break;
    case '-':
// A sinistra
        if (len > 1) {
            text++;
            len--;
        } else
            goto fallback;

        TextShadow(xs + 3,
                   ys + (b->Y2 - b->Y1 - tf->height) / 2 + tf->height -
                   1 /*+tf->tf_Baseline */ , text, len);
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
        while (*text < 13) {
            struct scaleAnimObjArgs a;
            a.src = symbols->Frames[(uint8_t)*text];
            a.xs = a.ys = 0;
            a.ws = symbols->Widths[(uint8_t)*text];
            a.hs = symbols->Heights[(uint8_t)*text];
            a.xd = xs;
            a.yd = ys;
            a.wd = FixedScaledX(symbols->Widths[(uint8_t)*text]);
            a.hd = min(b->Y2 - b->Y1, FixedScaledY(symbols->Heights[(uint8_t)*text]));
            a.destmod = bitmap_width;
            a.dest = main_bitmap;
            bltanimobjscale(&a);
//            BltAnimObj(symbols, main_bitmap, (uint8_t)*text, xs, ys, bitmap_width);
            xs += a.wd;
            text++;
        }
        break;
    case 5:
    case 6:
    case 7:
    case 13:
    case 14:
    case 15:
        {
            struct scaleAnimObjArgs a;
            a.src = symbols->Frames[(uint8_t)*text];
            a.xs = a.ys = 0;
            a.ws = symbols->Widths[(uint8_t)*text];
            a.hs = symbols->Heights[(uint8_t)*text];
            a.xd = xs;
            a.yd = ys;
            a.wd = FixedScaledX(symbols->Widths[(uint8_t)*text]);
            a.hd = min(b->Y2 - b->Y1, FixedScaledY(symbols->Heights[(uint8_t)*text]));
            a.destmod = bitmap_width;
            a.dest = main_bitmap;
            bltanimobjscale(&a);
        //BltAnimObj(symbols, main_bitmap, (uint8_t)*text, xs, ys, bitmap_width);
        }
        break;
    case 16:
        text++;

        if ((uint8_t)*text < ARCADE_TEAMS) {
            struct MyScaleArgs scale;
            gfx_t *o;

            if (!(o = arcade_gfx[(uint8_t)*text]))
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
                   1 /*+tf->tf_Baseline */ , text, len);
    }
}

void save_prefs()
{
    FILE *f;
    
    char buf[1024];
    snprintf(buf, 1024, "%setw.cfg"/*-*/, TEMP_DIR);
    if (!(f = fopen(buf, "w"/*-*/))) {
        
        if (!(f = fopen("etw.cfg" /*-*/ , "w"/*-*/))) {            easy.es_TextFormat = msg_69;
            easy.es_GadgetFormat = msg_58;
            
            MyEasyRequest(&easy);
            return;
        } else {
            fclose(f);
            write_config(buf);
        }
    } else {
        fclose(f);
        write_config(buf);
    }
}

BOOL DoAction(int16_t button)
{
    struct Button *b = &actual_menu->Button[button];

// i requester non hanno azioni associate con i tasti
    if (reqqing)
        return FALSE;

    if (current_menu == MENU_ARCADE && competition != MENU_TEAMS
        && b->ID > current_menu) {
        easy.es_TextFormat = msg_61;
        easy.es_GadgetFormat = msg_62;

        if (!MyEasyRequest(&easy))
            return TRUE;

        competition = MENU_TEAMS;
    }

    if (actual_menu->SpecialFunction) {
        BOOL value;
        BOOL(*func) (int16_t);

        func = (BOOL(*)(int16_t)) actual_menu->SpecialFunction;

        value = func(button);
        ScreenSwap();
        return value;
    } else if (b->ID >= 0) {
        int16_t ID;
        penalties = FALSE;
        free_kicks = FALSE;

        switch (current_menu) {
        case MENU_HIGHLIGHT:
            savehigh = FALSE;

            if (button == 0) {
                SetHighSelection();
                menu[MENU_HIGH_SELECTION].Title = msg_63;
            } else if (button == 1) {
                SetHighSelection();
                savehigh = TRUE;
                menu[MENU_HIGH_SELECTION].Title = msg_64;
            }
            break;
        case MENU_SELECTION_TYPE:
            if (button == 0 && !arcade_teams) {
                if (!DoWarning((special || competition != MENU_TEAMS)&&!new_competition)) {
                    new_competition = FALSE;
                    LoadTeams("teams/arcade"/*-*/);
                    arcade_teams = TRUE;
                } else
                    return TRUE;
            } else if (button == 1 && arcade_teams) {
                if (!DoWarning((competition != MENU_TEAMS) &&!new_competition)) {
                    new_competition = FALSE;
                    LoadTeams("teams/default"/*-*/);
                    arcade_teams = FALSE;
                } else
                    return TRUE;
            }
            break;
        case MENU_GAME_START:
            if (button == 0) {
                if (!arcade_gfx[0])
                    LoadArcadeGfx();

                if (!arcade_back)
                    LoadArcadeBack();
            } else if (arcade_back) {
                LoadBack();
            }

            if (button == 3) {
                network_game = TRUE;
                friendly = TRUE;
                ClearSelection();
                wanted_number = 1;
            }
            break;
        case MENU_ARCADE:

            arcade = TRUE;
            training = FALSE;

            if (button == 0) {
                teamsel_message = msg_222;                
                friendly = TRUE;
                ClearSelection();
                wanted_number = 2;
            } else if (button == 1)    // Whistle tour
            {
                teamsel_message = msg_220;
                friendly = FALSE;
                if (!DoWarning(((special || competition != MENU_MATCHES)
                                && competition != MENU_TEAMS))) {
                    if (competition != MENU_MATCHES) {
                        new_competition = TRUE;
                        ClearSelection();
                        competition = MENU_MATCHES;
                        wanted_number = 8;
                    } else
                        b->ID = MENU_MATCHES;
                } else
                    return TRUE;
            } else if (button == 2)    // Challenge
            {
                teamsel_message = msg_223;
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

            if (button > 2) {
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

            if (arcade_teams && button < 4) {
                if (!DoWarning
                    ((competition == MENU_MATCHES
                      || competition == MENU_CHALLENGE))) {
                    LoadTeams("teams/default"/*-*/);
                    arcade_teams = FALSE;
                } else
                    return TRUE;
            }

            if (button == 0) {
                if (!DoWarning((!special && competition != MENU_TEAMS)))
                    SetupSpecialEvent(b);
                else
                    return TRUE;
            } else if (button == 1) {
                teamsel_message = msg_222;                
                friendly = TRUE;
                ClearSelection();
                wanted_number = 2;
            } else if (button == 2) {
                if (!DoWarning(((special || competition != MENU_LEAGUE)
                                && competition != MENU_TEAMS))) {
                    if (competition != MENU_LEAGUE) {
                        teamsel_message = msg_221;                
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

            } else if (button == 3) {
                if (!DoWarning(((special || competition != MENU_MATCHES)
                                && competition != MENU_TEAMS))) {
                    if (competition != MENU_MATCHES) {
                        teamsel_message = msg_219;                
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
#ifdef MOBILE_VERSION
            if (button < 2)
                UpdatePrefs(b->ID);
            else if (button == 2 && !has_full_version()) {
                buy_full_version();
            }                
            else if (prefs_changed) {
                prefs_changed = FALSE;
                add_achievement("4_change"/*-*/, 100.0);
                save_prefs();
            }
#else
            if (button < 4) {
                UpdatePrefs(b->ID);
            }
#endif
            break;
        case MENU_CAREER:
            arcade = FALSE;
            training = FALSE;

            if (button >= 0 && button < 3) {
                selected_number = 0;
                wanted_number = 1;
            }
            break;
        case MENU_TRAINING:
            arcade = FALSE;
            training = TRUE;

            teamsel_message = msg_223;                

            if (arcade_teams && button < 3) {
                LoadTeams("teams/default"/*-*/);
                arcade_teams = FALSE;
            }

            if (button >= 0 && button < 3) {
                if (button == 2)
                    penalties = TRUE;
                else if (button == 1)
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
                PlayMatches(ID);
                return TRUE;
            }
        }
        ChangeMenu(ID);

    } else {
        if (b->ID == ID_QUIT_GAME)
            return FALSE;

        switch (current_menu) {
        case MENU_SCORES:
                switch (button) {
                    case 3:
                        easy.es_TextFormat = msg_65;
                        easy.es_GadgetFormat = msg_66;
                        
                        if (MyEasyRequest(&easy))
                            ClearScores();
                        break;
                    case 1:
                        show_world_scores();
                        break;
                    case 2:
                        show_achievements();
                        break;
                }
            break;
        case MENU_MATCHES:
        case MENU_LEAGUE:
        case MENU_WORLD_CUP:
            if (button != 1)
                break;

            if (arcade) {
                easy.es_TextFormat = msg_67;
                easy.es_GadgetFormat = msg_58;

                MyEasyRequest(&easy);
                break;
            }

            freq.Dir = "saves"/*-*/;
            freq.Title = msg_68;
            freq.Save = TRUE;
            freq.Filter = msg_201;

            if (FileRequest(&freq)) {
                strcpy(career_file, freq.File);
                D(bug("Saving to %s\n", career_file));
                SaveLeague();
            }
            break;
        case MENU_PREFS:
            if (button == 5 && prefs_changed) {
                prefs_changed = FALSE;
                add_achievement("4_change"/*-*/, 100.0);
                save_prefs();

                request(msg_70);
            }
            break;
        case MENU_TEAMS:
            if (button == 1) {
                char buffer[130];

                if (competition != MENU_TEAMS) {
                    easy.es_TextFormat = msg_61;
                    easy.es_GadgetFormat = msg_62;

                    if (!MyEasyRequest(&easy))
                        break;
                }

                freq.Title = msg_71;
                freq.Dir = "teams/"/*-*/;

                if (FileRequest(&freq)) {
                    strcpy(buffer, freq.File);

                    D(bug("Loading teams <%s>...\n" /*-*/ , buffer));
                    LoadTeams(buffer);

                    competition = MENU_TEAMS;
                }
            } else if (button == 2) {
                char buffer[130];
                uint8_t temp = competition;

                freq.Title = msg_72;
                freq.Save = TRUE;
                freq.Dir = "teams/"/*-*/;

                if (FileRequest(&freq)) {
                    strcpy(buffer, freq.File);

                    competition = MENU_TEAMS;

                    SaveTeams(buffer);

                    competition = temp;
                }
            } else if (button == 3) {
                extern char teamfile[];

                if (competition != MENU_TEAMS) {
                    easy.es_TextFormat = msg_61;
                    easy.es_GadgetFormat = msg_62;

                    if (!MyEasyRequest(&easy))
                        break;
                }

                competition = MENU_TEAMS;
                turno = 0;
                special = 0;

                LoadTeams(teamfile);
            } else 
                request(msg_73);
            break;
        case MENU_HIGHLIGHT:
            if (button == 2) {
                char buffer[130];

                freq.Title = msg_74;

                if (FileRequest(&freq)) {
                    strcpy(buffer, freq.File);

                    LoadHigh(buffer);
                }
            }
            break;
        case MENU_GAME_START:
            if (button == 2)
                request(msg_75);
            break;
        case MENU_MAIN_MENU:
            if (button == 5)
                ShowCredits();
            break;
        case MENU_SIMULATION:
        case MENU_CAREER:
            if (current_menu == MENU_SIMULATION)
                button--;

            if (button == 3) {
                if (*career_file) {
                    easy.es_TextFormat = msg_61;
                    easy.es_GadgetFormat = msg_62;
                    if (!MyEasyRequest(&easy))
                        break;
                }

                freq.Title = current_menu == MENU_CAREER ? msg_76 : msg_77;
                freq.Dir = "saves/" /*-*/ ;
                freq.Filter = msg_201;

                if (FileRequest(&freq)) {
                    strcpy(career_file, freq.File);
                    D(bug("Loading from %s\n", career_file));
                    LoadLeague();
                }
            } else if (button == 4) {
                if (current_menu == MENU_SIMULATION) {
                    if (competition == MENU_TEAMS)
                        break;

                    easy.es_TextFormat = msg_179;
                    easy.es_GadgetFormat = msg_60;

                    if (MyEasyRequest(&easy)) {
                        turno = 0;
                        special = FALSE;
                        competition = MENU_TEAMS;
                    }
                } else {
                    freq.Title =
                        current_menu == MENU_CAREER ? msg_78 : msg_79;
                    freq.Dir = "saves/" /*-*/ ;
                    freq.Filter = msg_201;
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

void CancelButton(struct Button *b)
{
    bltchunkybitmap(back, b->X1, b->Y1, main_bitmap,
                    b->X1, b->Y1, b->X2 - b->X1 + 1, b->Y2 - b->Y1 + 1,
                    bitmap_width, bitmap_width);
}

void PrintButtonType(struct Button *b, int16_t bl, int16_t bt,
                     struct myfont *tf)
{
    if ((current_menu == MENU_TEAM_SELECTION && b < &actual_menu->Button[TS_RIGHE * TS_COLONNE]) ||
        (current_menu == MENU_ARCADE_SELECTION && b < &actual_menu->Button[ARCADE_TEAMS]) ) {
        char *c;
        
        switch (b->Color) {
            case COLOR_TEAM_A:
#ifndef MOBILE_VERSION
                c = "_J2" /*-*/ ;
#else
                c = "_P" /*-*/;
#endif
                break;
            case COLOR_TEAM_B:
                c = "_J1" /*-*/ ;
                break;
            case COLOR_COMPUTER:
                c = "_C" /*-*/ ;
                break;
            default:
                c = NULL;
        }
        
        if (c)
            DisplayText(b, bl + b->X1, bt + b->Y1, c, (int)strlen(c), tf);
    }
}

void CreateButton(struct Button *b)
{
    char *t = b->Text;
    struct myfont *tf = smallfont;
    int l, top = 1;

    if (!t)
        return;

    rectfill(main_bitmap, b->X1, b->Y1, b->X2, b->Y2, Pens[b->Color],
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

    l = (int)strlen(t);

    if ((l * tf->width) > (b->X2 - b->X1 + 2)) {
        l = (b->X2 - b->X1) / tf->width;
    }

    DisplayText(b, b->X1, b->Y1 + top, t, l, tf);
    PrintButtonType(b, 0, top, tf);
}

void RedrawButton(struct Button *b, uint8_t colore)
{
    char *t = b->Text;
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

    l = (int)strlen(t);

    if ((l * tf->width) > (b->X2 - b->X1 + 2)) {
        l = (b->X2 - b->X1) / tf->width;
    }

    DisplayText(b, b->X1, b->Y1 + top, t, l, tf);
    PrintButtonType(b, 0, top, tf);
}


void blit_scaled_logo()
{
    struct scaleAnimObjArgs a;

    if (!logos) {
        if (!(logos = LoadAnimObject("menugfx/clips.obj" /*-*/ , Pens))) {
            D(bug("Unable to load logos images clips.obj!\n"));
            return;
        }
    }

    if (actual_menu->Immagine == -1) {
        D(bug("Ignored blit_scaled_logo with NO image!\n"));
        return;
    }

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
    // the old code without scaled images
#if 0
    BltAnimObj(logos, main_bitmap, actual_menu->Immagine,
            actual_menu->X, actual_menu->Y, bitmap_width);
#endif
}

void ChangeMenu(int16_t m)
{
    int i;
    
    bltchunkybitmap(back, 0, 0, main_bitmap, 0, 0, WINDOW_WIDTH,
                    WINDOW_HEIGHT, bitmap_width, bitmap_width);

    actual_menu = &menu[m];

// This code let me go back to the right menu when using teamselection;

    if (m != current_menu) {
        if (m != MENU_TEAM_SELECTION
            || (current_menu != MENU_TEAM_SETTINGS
                && current_menu != MENU_MATCH_RESULT)) {
            if (actual_menu->Button == teamselection) {
                teamselection[TS_RIGHE * TS_COLONNE + 1].ID = current_menu;
            } else if (actual_menu->Button == teamsettings) {
                teamsettings[42].ID = current_menu;
            }
        }

        if (m == MENU_ARCADE_SELECTION
            && current_menu != MENU_TEAM_SETTINGS) {
            actual_menu->Button[ARCADE_TEAMS + 1].ID = current_menu;
        }
#ifdef MOBILE_VERSION
        if (m == MENU_PREFS && has_full_version())
            actual_menu->Button[2].Text = NULL;
#endif
        current_menu = m;
    }

    setfont(titlefont);

    rectfill(main_bitmap, 0, FixedScaledY(7), WINDOW_WIDTH - 1,
             FixedScaledY(16), Pens[actual_menu->Color], bitmap_width);
    rectfill(main_bitmap, 0, FixedScaledY(6), WINDOW_WIDTH - 1,
             FixedScaledY(7) - 1, Pens[actual_menu->Highlight],
             bitmap_width);

    if (actual_menu->Immagine >= 0) 
        blit_scaled_logo();

    if (m == MENU_TEAM_SELECTION && teamsel_message) {
        TextShadow(FixedScaledX(8), FixedScaledY(20) + titlefont->height, 
                   teamsel_message, (int)strlen(teamsel_message));
    }
    else if (m == MENU_ARCADE_SELECTION && teamsel_message) {
        int x = (WINDOW_WIDTH - titlefont->width * (int)strlen(teamsel_message)) / 2,
            y = FixedScaledY(124) + titlefont->height;
        TextShadow(x, y, teamsel_message, (int)strlen(teamsel_message));
    }

    if (current_menu == MENU_TEAM_SETTINGS) {
        char *c = msg_80;
        int l, l2;
        int y = FixedScaledY(titlefont->height + 8);

        if (y <= titlefont->height)
            y = titlefont->height + 1;

        i = (int)strlen(teamlist[actual_team].name);

        TextShadow(FixedScaledX(5), y, teamlist[actual_team].name, i);

        setfont(WINDOW_HEIGHT > 300 ? bigfont : smallfont);

        i = (int)strlen(c);
        l = titlefont->width * i;
        l2 = (int)strlen(teamlist[actual_team].allenatore) * titlefont->width;

        y = FixedScaledY(titlefont->height - 1);

        if (WINDOW_HEIGHT < 300 && y <= smallfont->height)
            y = smallfont->height + 1;

        TextShadow(WINDOW_WIDTH - l - l2 - FixedScaledX(4), y, c, i);

        i = (int)strlen(teamlist[actual_team].allenatore);
        TextShadow(WINDOW_WIDTH - l2 - FixedScaledX(4),
                   FixedScaledY(titlefont->height + 8),
                   teamlist[actual_team].allenatore, i);
    } else if (actual_menu->Title) {
        char *t = actual_menu->Title;
        int y = FixedScaledY(titlefont->height + 3);

        if (y <= titlefont->height)
            y = titlefont->height + 1;

        i = (int)strlen(t);

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
        CreateButton(&actual_menu->Button[i]);

    for (i = 0; i < actual_menu->NumeroPannelli; i++)
        CreateButton(&actual_menu->Pannello[i]);

    actual_button = 0;

    if (m == MENU_TEAM_SETTINGS) {
        update_menu_tactic();

        if (ruolo[actual_team] && controllo[actual_team] >= 0) {
            uint8_t oldpen;

            oldpen =
                actual_menu->Button[ruolo[actual_team] * 2 + 1].Color;

            actual_menu->Button[ruolo[actual_team] * 2 + 1].Color =
                P_GIALLO;

            CreateButton(&actual_menu->Button[ruolo[actual_team] * 2 + 1]);

            actual_menu->Button[ruolo[actual_team] * 2 + 1].Color =
                oldpen;
        }

        actual_button = 17 * 2 + 6 + 2;
    }

    if (current_menu == MENU_MAIN_MENU) {
        char c[40];
        int l, x, y;

		snprintf(c, sizeof(c), "%s (%s)", ETW_VERSION, sizeof(void *) > 4 ? "X64" : "X32");

        setfont(smallfont);

        l = (int)strlen(c);

        x = WINDOW_WIDTH - l * smallfont->width - 2;
        y = WINDOW_HEIGHT - smallfont->height - 1;
        TextShadow(x, y, c, l);
        show_ads(0);
    }
    else
        hide_ads();

//      bltchunkybitmap(main_bitmap,0,0,main_bitmap,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,bitmap_width,bitmap_width);
//      GriddedWipe(1,main_bitmap);

    while (!actual_menu->Button[actual_button].Text)
        actual_button++;


    DrawBox(actual_button);
    ScreenSwap();
}

// TODO: handle 2 players

#include "ball.h"
#include "team.h"

const char *get_tactic_name(int button)
{
    return menu[MENU_PAUSE].Button[button].Text;
}

void set_pause_tactic(int button)
{
    int i;
    // iterate through tactic buttons
    for (i = 3; i < 9; ++i) {
        struct Button *b = &(menu[MENU_PAUSE].Button[i]);
        if (i == button) {
            b->Color = 9;
            RedrawButton(b, b->Color);
        }
        else if (b->Color != 14) {
            b->Color = 14;
            RedrawButton(b, b->Color);
        }
    }
}

extern team_t *find_controlled_team();
extern BOOL using_tactic(team_t *, const char *);

void draw_substitutions_menu()
{
    int i;
    team_t *c = find_controlled_team();

   // it's important to set current_menu before CreateButton() since createbutton() uses it!
    actual_menu = &menu[MENU_SUBSTITUTIONS];
    current_menu = MENU_SUBSTITUTIONS;

    rectfill(main_bitmap, 0, FixedScaledY(7), WINDOW_WIDTH - 1,
             FixedScaledY(16), Pens[actual_menu->Color], bitmap_width);
    rectfill(main_bitmap, 0, FixedScaledY(6), WINDOW_WIDTH - 1,
             FixedScaledY(7) - 1, Pens[actual_menu->Highlight],
             bitmap_width);

    if (titlefont && actual_menu->Title) {
        char *t = actual_menu->Title;
        int y = FixedScaledY(titlefont->height + 4);

        if (y <= titlefont->height)
            y = titlefont->height + 1;

        i = (int)strlen(t);

        if (*t != '-') {
            TextShadow(WINDOW_WIDTH / 2 - i * titlefont->width / 2, y, t,
                       i);
        } else {
            t++;
            i--;

            TextShadow(FixedScaledX(5), y, t, i);
        }
    }

    for (i = 0; i < actual_menu->NumeroPannelli; ++i)
        CreateButton(&actual_menu->Pannello[i]);    

    actual_button = 0;

    for (i = 0; i < 6; ++i) {
        struct Button *b = &actual_menu->Button[34 + i];

        if (c && using_tactic(c, b->Text)) { 
            const char *nums[PLAYERS];

            for (i = 0; i < PLAYERS; ++i) { 
                if (c->players[i].Ammonito & 2)
                    nums[i] = NULL;
                else
                    nums[i] = numbers[c->players[i].number];                
            }
            blit_scaled_logo();
            b->Color = 9;
            DisplayTactic(b->Text, actual_menu->X,  actual_menu->Y, nums, c->Joystick);
        }
        else 
            b->Color = 14;
    }    

    for (i = 0; i < actual_menu->NumeroBottoni; ++i)
        CreateButton(&actual_menu->Button[i]);

 
    
    while (!actual_menu->Button[actual_button].Text)
        actual_button++;
      
    

    current_button = actual_button;
    DrawBox(actual_button);
}

void draw_pause_menu()
{
    extern ball_t *pl;
    int i;
    team_t *c;
    c = find_controlled_team();

    // it's important to set current_menu before CreateButton() since createbutton() uses it!
    actual_menu = &menu[MENU_PAUSE];
    current_menu = MENU_PAUSE;
    
    // show substitutions only if we the game is stopped (throw in, kick off...)
    if (pl->InGioco || !c || arcade || training)
        actual_menu->Button[2].Text = NULL;
    else
        actual_menu->Button[2].Text = msg_123;

    for (i = 0; i < actual_menu->NumeroBottoni; ++i) {
        struct Button *b = &actual_menu->Button[i];
        if (i > 2 && i < 9) {
            if (!c) // do not draw tactic buttons in computer vs computer matches
                continue;

            if (using_tactic(c, b->Text)) 
                b->Color = 9;
            else 
                b->Color = 14;
        }
        CreateButton(b);    
    }
    for (i = 0; i < actual_menu->NumeroPannelli; ++i)
        CreateButton(&actual_menu->Pannello[i]);    

    actual_button = 0;

    while (!actual_menu->Button[actual_button].Text)
        actual_button++;
    
    current_button = actual_button;
    DrawBox(actual_button);
}

void SelectButton(int16_t button)
{
    if (button != NO_BUTTON) {
        struct Button *b = &actual_menu->Button[button];

        if ((b->Y2 - b->Y1) > 9) {
            draw(Pens[b->Color], b->X1, b->Y2, b->X1, b->Y1);
            draw(Pens[b->Color], b->X1, b->Y1, b->X2, b->Y1);
        } else
            RedrawButton(b, b->Highlight);
    }
}

void DeselectButton(int16_t button)
{
    if (button != NO_BUTTON) {
        struct Button *b = &actual_menu->Button[button];

        if ((b->Y2 - b->Y1) > 9) {
            draw(Pens[b->Highlight], b->X1, b->Y2 - 1, b->X1, b->Y1);
            draw(Pens[b->Highlight], b->X1, b->Y1, b->X2 - 1, b->Y1);

            if ((b->X2 - b->X1) > 16) {
                main_bitmap[b->X1 + b->Y1 * bitmap_width] =
                    (uint8_t) Pens[P_BIANCO];
            }
        } else
            RedrawButton(b, b->Color);
    }
}

int16_t GetButton(int16_t x, int16_t y)
{
    int16_t i;
    extern struct MyFastScaleArgs *scaling;

    if (scaling) {
        x /= scaling->xRatio;
        y /= scaling->yRatio;
    }

    for (i = 0; i < actual_menu->NumeroBottoni; i++) {
        struct Button *b = &actual_menu->Button[i];

        if (b->Text && x < b->X2 && x > b->X1 && y < b->Y2 && y > b->Y1) {
            return i;
        }
    }

    return NO_BUTTON;
}

void MoveMark(int k)
{
    int16_t old_button = actual_button;

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
    while (!actual_menu->Button[actual_button].Text);
}

BOOL HandleJoy(uint32_t joystatus)
{
    static BOOL clicked = FALSE;
    int16_t old_button = actual_button;

    if (joystatus & JPF_BUTTON_RED) {
        SelectButton(actual_button);
        ScreenSwap();
        clicked = TRUE;
    } else if (clicked) {
        clicked = FALSE;
        DeselectButton(actual_button);
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
        if (actual_button < (TS_RIGHE * TS_COLONNE)) {
            if (joystatus & JPF_JOY_UP) {
                MoveMark(-4);
            } else if (joystatus & JPF_JOY_DOWN) {
                if (actual_button < (TS_RIGHE * TS_COLONNE) - 4)
                    MoveMark(4);
                else
                    MoveMark((TS_RIGHE * TS_COLONNE) - actual_button);
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

BOOL HandleMenuIDCMP(void)
{
    SDL_Event e;
    BOOL returncode = TRUE;
    SDL_PumpEvents();
    while (SDL_PollEvent(&e)) {

        switch (e.type) {
            case SDL_WINDOWEVENT:
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_MINIMIZED:
                    {
                        extern SDL_Window *screen;
                        int w, h;
                        SDL_GetWindowSize(screen, &w, &h);
                        os_stop_audio();
                        D(bug("Received a minimize event, actual window %dx%d\n", w, h));
                        break;
                    }                        
                    case SDL_WINDOWEVENT_RESTORED:
                    {
                        extern SDL_Window *screen;
                        int w, h;
                        SDL_GetWindowSize(screen, &w, &h);
                        SDL_RenderSetLogicalSize(SDL_GetRenderer(screen), WINDOW_WIDTH, WINDOW_HEIGHT);
                        D(bug("Received a restore event, actual window %dx%d (%dx%d)\n", w, h, WINDOW_WIDTH, WINDOW_HEIGHT));
                        os_start_audio();
                        ScreenSwap();
                        break;
                    }
                    case SDL_WINDOWEVENT_RESIZED:
                    {
                        extern SDL_Window *screen;
                        int w, h;
                        SDL_GetWindowSize(screen, &w, &h);
                        D(bug("Received a resize event, actual window %dx%d\n", w, h));
                        if (!reqqing)
                            ScreenSwap();
                        break;
                    }
                    case SDL_WINDOWEVENT_EXPOSED:
                        ScreenSwap();
                        break;
                    case SDL_WINDOWEVENT_CLOSE:
                        if (!reqqing)
                            returncode = FALSE;
                        break;
                }
                break;
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
#ifdef ANDROID
            if (e.key.keysym.scancode == SDL_SCANCODE_AC_BACK) {
                D(bug("Pressed back key!"));
                if (reqqing)
                    return FALSE;
                else if (pause_mode && game_start && current_menu == MENU_PAUSE) {
                    pause_mode = FALSE;
                    return TRUE;
                }
                else if (current_menu >= 0 && current_menu < MENUS &&
                        previous_menu[current_menu] != -1) {
                    ChangeMenu(previous_menu[current_menu]);
                    return TRUE;
                }
            }
#endif
            break;
        case SDL_KEYUP:
            switch (e.key.keysym.sym) {
            case SDLK_p:
                if (pause_mode && game_start) {
                    pause_mode = FALSE;
                    return TRUE;
                }
                break;
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
                    RedrawButton(&actual_menu->
                                 Button[ruolo[actual_team] * 2 + 1],
                                 actual_menu->Button[ruolo[actual_team] *
                                                      2 + 1].Color);
                    ruolo[actual_team] = 0;
                    ScreenSwap();
                } else if (actual_button < 22 && actual_button > 1) {
                    ruolo[actual_team] = actual_button / 2;
                    RedrawButton(&actual_menu->
                                 Button[ruolo[actual_team] * 2 + 1],
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
                current_button = GetButton(e.button.x, e.button.y);
                SelectButton(current_button);

                if (actual_button != current_button && current_button >= 0) {
                    EraseBox(actual_button);

                    actual_button = current_button;
                    
                    DrawBox(actual_button);
                    ScreenSwap();
                }
            } else if (e.button.state == SDL_RELEASED
                       && current_button != NO_BUTTON) {
                int16_t temp;

                DeselectButton(current_button);
                ScreenSwap();

                temp = GetButton(e.button.x, e.button.y);

                if (current_button == temp) 
                    returncode = DoAction(current_button);
            }
            break;
        }
    }

    check_tutorial();
    
    // no popup request during the game 
    if (!returncode && !reqqing && !game_start)
        if (!CheckQuit())
            return TRUE;

    return returncode;
}
