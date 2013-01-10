#include "eat.h"
#include "network.h"

#define TextShadow(x,y,text,len) drawtext(text,len,x+1,y+1,Pens[P_NERO]);drawtext(text,len,x,y,Pens[P_BIANCO]);

#define ColorTextShadow(x,y,text,len,c) drawtext(text,len,x+1,y+1,Pens[P_NERO]);drawtext(text,len,x,y,c);

anim_t *goal_banner = NULL, *replay = NULL;
gfx_t *pause_gfx = NULL;

// static gfx_t *last_obj=NULL;
static gfx_t *gsmallfont = NULL, *verysmallfont = NULL;
struct myfont *gtf = NULL;

BOOL left_sel, right_sel;

struct Stats {
    char *name;
    UBYTE address, len;
};

typedef struct change_data_
{
    struct change_data_ *next;
    team_t *team;
    char in[40], out[40];
} change_data;

static change_data *changes = NULL;

void add_change(team_t *t, const char *out, const char *in)
{
    change_data *c = malloc(sizeof(change_data));

    if (!c)
        return;

    c->team = t;
    c->next = changes;
    strcpy(c->in, in + 1);
    strcpy(c->out, out + 1);

    changes = c;
} 

size_t how_many_changes()
{
    size_t count = 0;
    change_data *c = changes;
    while (c) {
        count++;
        c = c->next;
    }
    return count;
}

static void free_change_data()
{
    change_data *c = changes;
    while (c) {
        change_data *next = c->next;
        free(c);
        c = next;
    }
    changes = NULL;
}
#define NUMERO_STATS 9

struct Stats stats[] = {
    {"GOALS", 1, 5},
    {"SHOTS", 5, 5},
    {"FOULS", 2, 5},
    {"PENALTIES", 6, 9},
    {"YELLOW CARDS", 3, 12},
    {"RED CARDS", 4, 9},
    {"SUBSTITUTIONS", 8, 13},
    {"CORNER", 7, 6},
    {"POSSESSION", 0, 10},
};

BOOL draw_r, replay_done;

void PrintTeamName(player_t * g, int y)
{
    char *c = g->team->name;
    int l, x;

    l = strlen(c);

    y -= ((font_height << 1) + 1);

    x = WINDOW_WIDTH - l * font_width;
    x >>= 1;

    ColorTextShadow(x, y, c, l, g->team->MarkerRed);
}

BOOL InitFonts(void)
{
    if (!(gsmallfont = LoadGfxObject("gfx/smallfont", Pens, NULL)))
        return FALSE;

    if (!(verysmallfont = LoadGfxObject("gfx/verysmallfont", Pens, NULL)))
            return FALSE;

    if ((gtf = openfont(GAME_FONT))) {
        setfont(gtf);
        D(bug("Font opened.\n"));
    } else {
        printf("Unable to open the ETW font!\n");
        return FALSE;
    }
    return TRUE;
}

void FreeFonts(void)
{
    if (gtf) {
        closefont(gtf);
        gtf = NULL;
    }

    if (gsmallfont) {
        FreeGfxObj(gsmallfont);
        gsmallfont = NULL;
    }

    if (verysmallfont) {
        FreeGfxObj(verysmallfont);
        verysmallfont = NULL;
    }
}

void PrintSmall(uint8_t *dest, char *text, char len)
{
    int i, x = 0;

    for (i = 0; i < len; i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            BltGfxObj(gsmallfont, (text[i] - 'A') * CHAR_X, 0,
                      dest, x, 0, CHAR_X, CHAR_Y, CHAR_X * MAX_LEN);
        } else if (text[i] == ' ') {
            rectfill(dest, x, 0, x + CHAR_X - 1, CHAR_Y - 1,
                     Pens[P_VERDE0], CHAR_X * MAX_LEN);
        }
        x += CHAR_X;
    }
}

void PrintVerySmall(uint8_t *dest, char *text, char len, int width)
{
    int i, x = 0;

    for (i = 0; i < len; i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            BltGfxObj(verysmallfont, (text[i] - 'A') * VS_CHAR_X, 0,
                      dest, x, 1, VS_CHAR_X, VS_CHAR_Y, width);
        } else if (text[i] >= '0' && text[i] <= '9') {
            BltGfxObj(verysmallfont, (text[i] - '0' + 29) * VS_CHAR_X, 0,
                      dest, x, 1, VS_CHAR_X, VS_CHAR_Y, width);
        } else {
            int code = 0;

            switch (text[i]) {
            case '\'':
            case '`':
                code = 28;
                break;
            case '-':
                code = 27;
                break;
            case '.':
                code = 26;
                break;
            case ' ':
                rectfill(dest, x, 0, x + VS_CHAR_X - 1, VS_CHAR_Y - 1,
                         Pens[P_VERDE2], width);
                break;
            }

            if (code)
                BltGfxObj(verysmallfont, code * VS_CHAR_X, 0,
                          dest, x, 1, VS_CHAR_X, VS_CHAR_Y, width);
        }

        x += VS_CHAR_X;
    }
}

void PrintNames(void)
{
    if (!training)
        bltchunkybitmap(p->team[1]->NomeAttivo, 0, 0, main_bitmap,
                        0, WINDOW_HEIGHT - (CHAR_Y + 1),
                        p->team[1]->attivo->NameLen * 6, CHAR_Y,
                        (CHAR_X * MAX_LEN), bitmap_width);

    bltchunkybitmap(p->team[0]->NomeAttivo, 0, 0, main_bitmap,
                    WINDOW_WIDTH - p->team[0]->attivo->NameLen * 6 - 1,
                    WINDOW_HEIGHT - (CHAR_Y + 1),
                    p->team[0]->attivo->NameLen * 6, CHAR_Y,
                    (CHAR_X * MAX_LEN), bitmap_width);
}

void LoadLogo(char *name)
{
    gfx_t *o;

    if (last_obj) {
        FreeGfxObj(last_obj);
        last_obj = NULL;
    }

    o = LoadGfxObject(name, Pens, NULL);

    if (o) {
        if (o->width == WINDOW_WIDTH && o->height == WINDOW_HEIGHT) {
            BltGfxObj(o, 0, 0, main_bitmap, 0, 0, WINDOW_WIDTH,
                      WINDOW_HEIGHT, bitmap_width);
        } else {
            struct MyScaleArgs scale;

            scale.SrcX = scale.SrcY = scale.DestX = scale.DestY = 0;
            scale.SrcWidth = scale.SrcSpan = o->width;
            scale.SrcHeight = o->height;
            scale.Dest = main_bitmap;
            scale.DestSpan = bitmap_width;
            scale.Src = o->bmap;
            scale.DestWidth = WINDOW_WIDTH;
            scale.DestHeight = WINDOW_HEIGHT;

            D(bug("Before scaling (font.c): %ldx%ld\n", o->width, o->height));
            bitmapScale(&scale);
            D(bug("After: %ldx%ld\n", scale.DestWidth, scale.DestHeight));

            FreeGfxObj(o);
        }
    }
}

void split_string(char *string, int y)
{
    int l = strlen(string);
    int x = font_width * 2;

    if (((l + 3) * font_width) < bitmap_width) {
        TextShadow(x, y, string, l);
    } else {
        int line = bitmap_width / font_width - 4;

        while (l > 0) {
            line = min(line, l);
            TextShadow(x, y, string, line);
            string += line;
            y += font_height + 2;
            l -= line;
        }
    }
}

void ShowPanel(void)
{
    BOOL goal_onscreen = InAnimList(goal_banner);

    if (replay_mode) {
        if (slow_motion) {
            const char *txt = "SLOW MOTION";
            int l = strlen(txt);
            int x = (WINDOW_WIDTH - l * font_width) >> 1;                
            TextShadow(x, font_height + 2, txt, l);
        }
        if (p->show_panel & PANEL_HIGHSAVE) {
            const char *txt =  "HIGHLIGHT SAVED";
            int l = strlen(txt);
            int x = (WINDOW_WIDTH - l * font_width) >> 1,
            y = (WINDOW_HEIGHT - font_height) >> 1;
            TextShadow(x, y, txt, l);
        }
        
        if (goal_onscreen) {
            RemAnimObj(goal_banner);
        }
        return;
    }

    if (p->show_panel & PANEL_CHAT) {
        rectfill_pattern(main_bitmap, font_width,
                         WINDOW_HEIGHT - (font_height * 6),
                         WINDOW_WIDTH - 1 - (font_width),
                         WINDOW_HEIGHT - font_height, 0, bitmap_width);

        if (*outgoing_chat) {
            split_string(outgoing_chat, WINDOW_HEIGHT - (font_height * 4));
        } else
            rectfill(main_bitmap, font_width * 2,
                     WINDOW_HEIGHT - (font_height * 4), font_width * 3,
                     WINDOW_HEIGHT - (font_height * 3), Pens[P_BIANCO],
                     bitmap_width);
    }

    if (p->show_panel & PANEL_DISPLAYCHAT) {
        if (*chat_message)
            split_string(chat_message, font_height * 2);
    }

    if ((p->show_panel & PANEL_CARD)
        && (p->referee.Comando == AMMONIZIONE
            || p->referee.Comando == ESPULSIONE)) {
        int x, y, l;
        player_t *g;

        rectfill_pattern(main_bitmap, (font_width << 1),
                         WINDOW_HEIGHT - (font_height << 2),
                         WINDOW_WIDTH - 1 - (font_width << 1),
                         WINDOW_HEIGHT - font_height,
                         current_field !=
                         7 ? Pens[P_VERDE2] : Pens[P_GRIGIO1],
                         bitmap_width);


        rectfill(main_bitmap,
                 (font_width << 2),
                 WINDOW_HEIGHT - (font_height << 2) + (font_height >> 1),
                 (font_width << 2) + font_width + 3,
                 WINDOW_HEIGHT - font_height - (font_height >> 1),
                 Pens[P_NERO], bitmap_width);

        rectfill(main_bitmap,
                 (font_width << 2) + 1,
                 WINDOW_HEIGHT - (font_height << 2) + (font_height >> 1) +
                 1, (font_width << 2) + font_width + 2,
                 WINDOW_HEIGHT - font_height - (font_height >> 1) - 1,
                 p->referee.Comando ==
                 AMMONIZIONE ? Pens[P_GIALLO] : Pens[P_ROSSO0],
                 bitmap_width);

        x = font_width * 7;
        y = WINDOW_HEIGHT - (font_height << 2) + (font_height << 1) - 1;

        if (p->referee.Argomento >= 10)
            g = &p->team[1]->players[p->referee.Argomento - 10];
        else
            g = &p->team[0]->players[p->referee.Argomento];

        l = strlen(g->name);

        TextShadow(x, y, g->name, l);
    }

    if (p->show_panel & PANEL_OFFSIDE) {
        int x, y, l;
        const char *sp = "O F F S I D E";

        l = strlen(sp);

        x = WINDOW_WIDTH - l * font_width;
        x >>= 1;
        y = (WINDOW_HEIGHT >> 1) - font_height;

        TextShadow(x, y, sp, l);
    }

    // always show the clock
    if (!penalties && !free_kicks /* && p->show_panel & PANEL_TIME */) {
        char buffer[8];
        int mins, secs;
        mytimer temptime;

        if (first_kickoff) {
            mins = secs = 0;
        } else {
            extern BOOL time_stopped;

            if (time_stopped) {
                extern mytimer StopTimeVal;

                temptime = StopTimeVal;
            } else
                temptime = Timer();

            temptime -= StartGameTime;

            mins = temptime / t_l;
            mins /= (MY_CLOCKS_PER_SEC / 45);
            secs = mins % 60;
            mins /= 60;
        }

        if (extratime) {
            if (first_half)
                mins += 90;
            else
                mins += 105;
        } else if (!first_half)
            mins += 45;

        sprintf(buffer, "%3ld:%02d", mins + situation_time, (int) secs);
        // center the timer, it was in the right corner where we have the pause button now
        TextShadow((WINDOW_WIDTH - (font_width * 6)) / 2, font_height + 2, buffer, 6);
    }

    if (!pause_mode ) {
        if (p->show_panel & PANEL_SUBSTITUTION) {
            int l = how_many_changes();

            if (l > 0) {
                change_data *c = changes;
                int x = (font_width << 1), x2 = WINDOW_WIDTH/2, i,
                    y =  WINDOW_HEIGHT - font_height * (l + 5) - font_height * (l - 1);

                rectfill_pattern(main_bitmap, x, y,
                        WINDOW_WIDTH - 1 - (font_width << 1),
                        WINDOW_HEIGHT - font_height,
                        0, // not used anymore
                        bitmap_width);

                x += font_width * 2;
                y += font_height * 3;
                ColorTextShadow(x, y, "OUT:", 4, changes->team->MarkerRed);
                ColorTextShadow(x2, y, "IN:", 3, changes->team->MarkerRed);
                x += font_width * 5;
                x2 += font_width * 5;
                
                for (i = 0; i < l; ++i) {
                    TextShadow(x, y, c->out, strlen(c->out));
                    TextShadow(x2, y, c->in, strlen(c->in));
                    y += font_height * 2;
                    c = c->next;
                }
            }
        }
        else
            free_change_data();
    }

    if ( (p->show_panel & PANEL_PLAY_INJURIED) && p->player_injuried) {
        player_t *g = p->player_injuried;
        char buffer[80];
        int x, y, l;

        sprintf(buffer, "%s INJURIED.", g->name);

        l = strlen(buffer);

        x = WINDOW_WIDTH - l * font_width;
        x >>= 1;
        y = (WINDOW_HEIGHT >> 1) - font_height;

        TextShadow(x, y, buffer, l);

        if (p->show_time < 10) {
            p->show_panel &= ~PANEL_PLAY_INJURIED;
            p->player_injuried = NULL;
        }
    }

#ifndef MOBILE_VERSION
    if (p->show_panel & PANEL_REPLAY) {
        char *c = "PRESS R TO REPLAY THE PERIOD";
        int x, y, l = strlen(c);

        x = WINDOW_WIDTH - l * font_width;
        x >>= 1;
        y = (WINDOW_HEIGHT >> 1) - font_height;

        if (friendly)
            y -= font_height;

        TextShadow(x, y, c, l);

        if (friendly && !first_half) {
            c = "PRESS P TO PLAY AGAIN";
            l = strlen(c);

            y += (font_height << 1);
            x = WINDOW_WIDTH - l * font_width;
            x >>= 1;

            TextShadow(x, y, c, l);
        }
    }
#else
    
    // new panels used only in the mobile version
    if (p->show_panel & PANEL_THROW_IN) {
        const char *txt =  "TOUCH A PLAYER TO THROW IN TO HIM";
        int l = strlen(txt);
        int x = (WINDOW_WIDTH - l * font_width) >> 1,
            y = (WINDOW_HEIGHT - font_height) >> 1;
        TextShadow(x, y, txt, l);
    }

    if (p->show_panel & PANEL_CORNER) {
        const char *txt =  "SWIPE TO SHOOT OR TOUCH TO PASS";
        int l = strlen(txt);
        int x = (WINDOW_WIDTH - l * font_width) >> 1,
            y = (WINDOW_HEIGHT - font_height) >> 1;
        TextShadow(x, y, txt, l);
    }
#endif

    if (p->show_panel & PANEL_STATS) {
        char buffer[20], c;
        register int t, l, x, y, xs;

        t = (WINDOW_HEIGHT - ((font_height + 3) * 11)) >> 1;

        rectfill_pattern(main_bitmap, (font_width << 1), t - font_height,
                         WINDOW_WIDTH - (font_width << 1),
                         (t + ((font_height + 3) * 11)),
                         current_field !=
                         7 ? Pens[P_VERDE2] : Pens[P_GRIGIO1],
                         bitmap_width);

        y = t + 5;

        t = p->team[0]->TempoPossesso + p->team[1]->TempoPossesso;
        l = p->team[0]->TempoPossesso * 100 / t;
        p->team[0]->TPossesso = l;
        l = p->team[1]->TempoPossesso * 100 / t;
        p->team[1]->TPossesso = l;

        xs = (font_width << 1) + font_width;

        l = strlen(p->team[0]->name);

        TextShadow(xs, y, p->team[0]->name, l);

        l = strlen(p->team[1]->name);

        x = WINDOW_WIDTH - (l + 3) * font_width;

        TextShadow(x, y, p->team[1]->name, l);

        y += (font_height * 2);

        for (t = 0; t < NUMERO_STATS; t++) {
            x = (WINDOW_WIDTH - stats[t].len * font_width) >> 1;

            TextShadow(x, y, stats[t].name, stats[t].len);

            c = *((char *) (((char *) (p->team[0])) +
                            stats[t].address));

            l = sprintf(buffer, "%d", (int) c);

            TextShadow(xs, y, buffer, l);

            c = *((char *) (((char *) (p->team[1])) +
                            stats[t].address));

            l = sprintf(buffer, "%d", (int) c);

            x = WINDOW_WIDTH - (l + 3) * font_width;

            TextShadow(x, y, buffer, l);

            y += font_height;
            y += 3;
        }
    }

    if (p->show_panel & PANEL_RESULT) {
        char temp[4];
        int x, y, l;

        rectfill_pattern(main_bitmap, (font_width << 1),
                         WINDOW_HEIGHT - (font_height << 3) + font_height,
                         WINDOW_WIDTH - 1 - (font_width << 1),
                         WINDOW_HEIGHT - (font_height << 1),
                         current_field !=
                         7 ? Pens[P_VERDE2] : Pens[P_GRIGIO1],
                         bitmap_width);


        x = font_width << 2;
        y = WINDOW_HEIGHT - (font_height << 3) + (font_height << 1) +
            font_height;

        l = strlen(p->team[0]->name);

        TextShadow(x, y, p->team[0]->name, l);

        x = WINDOW_WIDTH - (font_width << 2) - font_width;
        y = WINDOW_HEIGHT - (font_height << 3) + (font_height << 1) +
            font_height;

        l = sprintf(temp, "%d", p->team[0]->Reti);

        TextShadow(x, y, temp, l);

        if (!training) {
            x = font_width << 2;
            y = WINDOW_HEIGHT - (font_height << 2) + font_height;

            l = strlen(p->team[1]->name);

            TextShadow(x, y, p->team[1]->name, l);

            x = WINDOW_WIDTH - (font_width << 2) - font_width;
            y = WINDOW_HEIGHT - (font_height << 2) + font_height;

            l = sprintf(temp, "%d", p->team[1]->Reti);
            TextShadow(x, y, temp, l);
        }
    }

    if (p->show_panel & PANEL_GOAL) {
        if (goal_banner) {
            if (!goal_onscreen) {
//                              value=(WINDOW_WIDTH+288)/50;

//                              AddAnimObj(goal_banner,WINDOW_WIDTH,WINDOW_HEIGHT/3,0);

                AddAnimObj(goal_banner,
                           (WINDOW_WIDTH - goal_banner->Widths[0]) >> 1,
                           (WINDOW_HEIGHT * 3 / 5) -
                           (goal_banner->Heights[0] >> 1), 0);

            }
            else if (p->show_time <= 1) {
                RemAnimObj(goal_banner);
            }
        } else {
            rectfill_pattern(main_bitmap, (WINDOW_WIDTH >> 1) - 80,
                             (WINDOW_HEIGHT >> 1) - 40,
                             (WINDOW_WIDTH >> 1) + 79,
                             (WINDOW_HEIGHT >> 1) + 29,
                             current_field !=
                             7 ? Pens[P_VERDE2] : Pens[P_GRIGIO1],
                             bitmap_width);

            drawtext("G O A L", 7, (WINDOW_WIDTH - 7 * font_width) >> 1,
                     (WINDOW_HEIGHT - font_height) >> 1, Pens[P_BIANCO]);
        }
    } else if (goal_onscreen) {
        RemAnimObj(goal_banner);
    }

    if (p->show_panel & PANEL_KICKOFF) {
        int x, y;

        rectfill_pattern(main_bitmap, (WINDOW_WIDTH >> 1) - 80,
                         (WINDOW_HEIGHT >> 1) - 40,
                         (WINDOW_WIDTH >> 1) + 79,
                         (WINDOW_HEIGHT >> 1) + 29,
                         current_field !=
                         7 ? Pens[P_VERDE2] : Pens[P_GRIGIO1],
                         bitmap_width);

        x = (WINDOW_WIDTH - 8 * font_width) >> 1;
        y = (WINDOW_HEIGHT - font_height) >> 1;
        TextShadow(x, y, "KICK OFF", 8);
    }

/*
// it's no more a panel

    if(p->show_panel&PANEL_PAUSE)
    {
        if(pause_gfx)
        {
            BltGfxObj(pause_gfx,0,0,main_bitmap,WINDOW_WIDTH/2-pause_gfx->width/2,WINDOW_HEIGHT/2-pause_gfx->height/2,pause_gfx->width,pause_gfx->height,bitmap_width);
        }
        else
        {
            drawtext("PAUSED",6,(WINDOW_WIDTH-6*font_width)/2,WINDOW_HEIGHT/2,Pens[P_BIANCO]);
        }
        p->show_panel&=~PANEL_PAUSE;
    }
*/
}

void DrawPause(void)
{
    rectfill_pattern(main_bitmap, 0, 0, WINDOW_WIDTH - 1,
                     WINDOW_HEIGHT - 1, 0, bitmap_width);

    if (pause_gfx) {
        BltGfxObj(pause_gfx, 0, 0, main_bitmap,
                  WINDOW_WIDTH / 2 - pause_gfx->width / 2,
                  pause_gfx->height,
                  pause_gfx->width, pause_gfx->height, bitmap_width);
    } else {
        drawtext("PAUSED", 6, (WINDOW_WIDTH - 6 * font_width) / 2,
                 10, Pens[P_BIANCO]);
    }

    draw_pause_menu();
}

void DrawR(void)
{
    BOOL replay_onscreen = InAnimList(replay);

    if (draw_r) {
        if (replay) {
            if (!replay_onscreen) 
                AddAnimObj(replay, 2, 2, 0);
        } else { 
            drawtext("R", 1, 2, font_height + 2, Pens[P_NERO]);
        }
    } else if (replay_onscreen) {
        RemAnimObj(replay);
    }
}

void ShowFinal(void)
{
    team_t *s = p->team[0]->Reti > p->team[1]->Reti ? p->team[0] :
                p->team[1]->Reti > p->team[0]->Reti ? p->team[1] :
                NULL;

    if (!s)
         return;
         
    if (s->Joystick >= 0) {
        uint8_t *mask;

        D(bug("Showing final sequence...\n"));

        if (pause_gfx) {
            FreeGfxObj(pause_gfx);
            pause_gfx = NULL;
        }

        if (goal_banner) {
            FreeAnimObj(goal_banner);
            goal_banner = NULL;
        }

        if (replay) {
            FreeAnimObj(replay);
            replay = NULL;
        }

        if ((mask = malloc(WINDOW_WIDTH * WINDOW_HEIGHT))) {
            extern uint8_t cols[2][4];
            uint8_t *temp;
            gfx_t *o;
            int i = s->players[0].SNum;
            char buffer[100];

            BltGfxObj(background, (FIELD_WIDTH - WINDOW_WIDTH) / 2 - 1,
                      (FIELD_HEIGHT - WINDOW_HEIGHT) - 1, main_bitmap, 0,
                      0, WINDOW_WIDTH, WINDOW_HEIGHT, bitmap_width);

            if (teams_swapped)
                i ^= 1;

            sprintf(buffer, "gfx/finis%lc%lc.gfx",
                    (current_field == 7 ? 's' : 'h'), cols[i][3]);

            if ((o = LoadGfxObject(buffer, Pens, NULL))) {
                if (cols[i][0] != COLOR_STANDARD_MAGLIE)
                    RemapColor(o->bmap, Pens[COLOR_STANDARD_MAGLIE],
                               Pens[cols[i][0]], o->width * o->height);
                if (cols[i][1] != COLOR_STANDARD_CALZONI)
                    RemapColor(o->bmap, Pens[COLOR_STANDARD_CALZONI],
                               Pens[cols[i][1]], o->width * o->height);
                if (cols[i][2] != P_ARANCIO0)
                    RemapColor(o->bmap, Pens[P_ARANCIO0], Pens[cols[i][2]],
                               o->width * o->height);

                bltbitmap_x(o->bmap, 0, 0, main_bitmap,
                            (WINDOW_WIDTH - FINAL_REAL_WIDTH) / 2,
                            (WINDOW_HEIGHT - FINAL_HEIGHT) / 2,
                            FINAL_REAL_WIDTH, FINAL_HEIGHT, o->width,
                            bitmap_width, Pens[0]);

                ScreenSwap();

                memset(mask, Pens[P_BIANCO], bitmap_width * bitmap_height);

                PlayBackSound(sound[FOLLA]);
                os_delay(70);

                PlayBackSound(sound[HALLELUJAH]);

                temp = main_bitmap;

                for (i = 0; i < 10; i++) {
                    os_delay(MyRangeRand(100) + 30);

                    main_bitmap = mask;
                    ScreenSwap();

                    main_bitmap = temp;
                    os_delay(MyRangeRand(2) + 1);
                    ScreenSwap();
                }
                os_delay(100);

                FreeGfxObj(o);
            }
            free(mask);
        }
    } else {
        D(bug("Winner is not an human player...\n"));
    }
}
