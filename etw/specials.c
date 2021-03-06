#include "etw_locale.h"
#include "menu.h"
#include <ctype.h>
#include "highdirent.h"

#include "SDL.h"

BOOL prefs_changed = FALSE;

extern void MenuResizing(int, int);
extern uint8_t joycfg_buttons[2][8];
extern int query[20];

int actual_joystick = 0;

char *buttons[] =
{
    msg_185,
    msg_190,
    msg_191,
    msg_192,
    msg_193,
    msg_194,
    msg_195,
    msg_196,
    msg_197,
    msg_186,
    msg_187,
    msg_188,
    msg_189,
};

/* Space for the names of the configured keys */
char keys_names[20][20];

/* AC: Here it's possible to decide which keys to exclude from those possible
 * ones */
SDL_Keycode keys[] =
{
    /* The keyboard syms have been cleverly chosen to map to ASCII */

    //SDLK_UNKNOWN, ???
    //SDLK_FIRST, ???
    SDLK_BACKSPACE,
    SDLK_TAB,
    SDLK_CLEAR,
    SDLK_RETURN,
    SDLK_PAUSE,
    //SDLK_ESCAPE,            // Playgame interruption and end game
    //SDLK_SPACE,            // Changes game visual

    /* AC: Some older Macs have these keys obtained without pressing the shift
     * in place of the numbers */
    SDLK_EXCLAIM,
    SDLK_QUOTEDBL,
    SDLK_HASH,
    SDLK_DOLLAR,
    SDLK_AMPERSAND,
    SDLK_QUOTE,
    SDLK_LEFTPAREN,
    SDLK_RIGHTPAREN,
    SDLK_ASTERISK,
    SDLK_PLUS,
    SDLK_COMMA,
    SDLK_MINUS,
    SDLK_PERIOD,
    SDLK_SLASH,

    SDLK_0,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_5,
    SDLK_6,
    SDLK_7,
    SDLK_8,
    SDLK_9,

    /* AC: This ones like the comment above */
    SDLK_COLON,
    SDLK_SEMICOLON,
    SDLK_LESS,
    SDLK_EQUALS,
    SDLK_GREATER,
    SDLK_QUESTION,
    SDLK_AT,

    /*
       Skip uppercase letters
     */
    //SDLK_LEFTBRACKET,
    SDLK_BACKSLASH,
    //SDLK_RIGHTBRACKET,
    //SDLK_CARET,
    SDLK_UNDERSCORE,
    //SDLK_BACKQUOTE,
    SDLK_a,
    SDLK_b,
    SDLK_c,
    SDLK_d,                    // DEBUG: Penalty
    SDLK_e,
    SDLK_f,
    SDLK_g,                    // DEBUG: Team 0 makes a goal
    //SDLK_h,                // Shows chat panel
    SDLK_i,                    // DEBUG: Shows men positions
    //SDLK_l,                // Shows stats panel
    SDLK_j,
    SDLK_k,
    SDLK_m,
    SDLK_n,
    SDLK_o,
    //SDLK_p,                // Pauses game
    //SDLK_q,                // Quits game
    //SDLK_r,                // Shows the replay
    //SDLK_s,                // Enables or disables the radar
    SDLK_t,
    SDLK_u,
    SDLK_v,
    SDLK_w,                    // DEBUG: Forces time to finish
    //SDLK_x,                // Changes position and dimension of the radar
    SDLK_y,
    SDLK_z,
    //SDLK_DELETE,            // Centers the visuale on the ball

    /* Numeric keypad */
    SDLK_KP_0,
    SDLK_KP_1,
    SDLK_KP_2,
    SDLK_KP_3,
    SDLK_KP_4,
    SDLK_KP_5,
    SDLK_KP_6,
    SDLK_KP_7,
    SDLK_KP_8,
    SDLK_KP_9,
    SDLK_KP_PERIOD,
    SDLK_KP_DIVIDE,
    SDLK_KP_MULTIPLY,
    SDLK_KP_MINUS,
    SDLK_KP_PLUS,
    SDLK_KP_ENTER,
    SDLK_KP_EQUALS,

    /* Arrows + Home/End pad */
    SDLK_UP,
    SDLK_DOWN,
    SDLK_RIGHT,
    SDLK_LEFT,
    SDLK_INSERT,
    SDLK_HOME,
    SDLK_END,
    SDLK_PAGEUP,
    SDLK_PAGEDOWN,

#if 0
    /* Function keys */
    SDLK_F1,
    SDLK_F2,
    SDLK_F3,
    SDLK_F4,
    SDLK_F5,
    SDLK_F6,
    SDLK_F7,
    SDLK_F8,
    SDLK_F9,
    SDLK_F10,
    SDLK_F11,
    SDLK_F12,
    SDLK_F13,
    SDLK_F14,
    SDLK_F15,

    /* Key state modifier keys */
    SDLK_NUMLOCK,
    SDLK_CAPSLOCK,
    SDLK_SCROLLOCK,
#endif

    SDLK_RSHIFT,
    SDLK_LSHIFT,
    SDLK_RCTRL,
    SDLK_LCTRL,
    SDLK_RALT,
    SDLK_LALT,
    SDLK_RGUI,
    SDLK_LGUI,

    /* AC: I think isn't a good idea allowing the use of this keys */
#if 0
    SDLK_LSUPER,        /* Left "Windows" key */
    SDLK_RSUPER,        /* Right "Windows" key */
    SDLK_MODE,            /* "Alt Gr" key */
    SDLK_COMPOSE,        /* Multi-key compose key */

    /* Miscellaneous function keys */
    SDLK_HELP,
    SDLK_PRINT,
    SDLK_SYSREQ,
    SDLK_BREAK,
    SDLK_MENU,
    SDLK_POWER,            /* Power Macintosh power key */
    SDLK_EURO,            /* Some european keyboards */
    SDLK_UNDO,            /* Atari keyboard has Undo */

#endif

    /* Add any other keys here */
};

static int jingle = -1;

extern struct GfxMenu *actual_menu;
extern int FIXED_SCALING_WIDTH, FIXED_SCALING_HEIGHT;

BOOL make_setup = FALSE, game_start = FALSE, can_modify = TRUE,
     savehigh = FALSE, use_gfx_scaling = FALSE;
int8_t selected_number = 0, wanted_number = 0, duration = 1,
       field_type = 0, daytime = 0;
char *enabled = msg_7, *disabled = msg_8;
extern int os_videook(int, int);

int8_t arcade_sequence[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
int8_t mondiali[] = { 8, 4, 2, 2 };

int8_t current_resolution = 0, current_scaling = 0;

char *resolutions[] =
{
#ifdef OLD_RES
    "320x200"/*-*/,
    "320X240"/*-*/,
    "400X300"/*-*/,
    "640x400"/*-*/,
#endif
    "640X480"/*-*/,
    "800X600"/*-*/,
    "1024X768"/*-*/,
    "1280X1024"/*-*/,
    NULL
};

void UpdateJoyCfg(int joy);
void UpdateKeyCfg(void);

void init_joy_config(void)
{
    int i, k;

    for (i = 0; i < 2; i++)
        for (k = 0; k < 8; k++)
            joycfg_buttons[i][k] = k;
}

char *scaling_resolutions[] =
{
    "320X240"/*-*/,
    "320X256"/*-*/,
    "356X288"/*-*/,
    "400X300"/*-*/,
    "512X384"/*-*/,
    "640X512"/*-*/,
    NULL
};

char *daytimes[] =
{
    msg_2,
    msg_9,
    msg_10,
    NULL
};

char *field_types[] =
{
    msg_2,
    "A"/*-*/,
    "B"/*-*/,
    "C"/*-*/,
    "D"/*-*/,
    "E"/*-*/,
    "F"/*-*/,
    NULL,
};

void SetCurrentResolution(void)
{
#ifdef OLD_RES
    if (WINDOW_WIDTH <= 360)
        current_resolution = (WINDOW_HEIGHT < 210) ? 0 : 1;
    else if (WINDOW_WIDTH <= 450)
        current_resolution = 2;
    else if (WINDOW_WIDTH < 700)
        current_resolution = (WINDOW_HEIGHT < 410) ? 3 : 4;
    else
        current_resolution = 5;
#else
    if (WINDOW_WIDTH <= 640)
        current_resolution = 0;
    else if (WINDOW_WIDTH <= 800)
        current_resolution = 1;
    else if (WINDOW_WIDTH < 1024)
        current_resolution = 2;
    else
        current_resolution = 3;
#endif
}

char *radar_options[] =
{
    msg_11,
    msg_12,
    msg_13,
    msg_14,
    msg_15,
    msg_16,
    msg_17,
    msg_18,
    msg_19,
    msg_20,
    msg_21,
    msg_22,
    NULL,
};

char *field_options[] =
{
    msg_2,
    msg_23,
    msg_24,
    msg_25,
    msg_26,
    msg_27,
    msg_28,
    msg_29,
    msg_30,
    NULL,
};

char *time_options[] =
{
    "1"/*-*/,
    "2"/*-*/,
    "3"/*-*/,
    "5"/*-*/,
    "10"/*-*/,
    "20"/*-*/,
    "45"/*-*/,
    NULL,
};

static void RandomDraw(int);
static void NewTurn(void);
static BOOL CanContinue(void);

// Teams shuffle!

void RandomDraw(int n)
{
    int i = RangeRand(50), k, s1, s2;
    int8_t temp;

    for (k = 0; k < i; k++)
    {
        s1 = RangeRand(n);
        s2 = RangeRand(n);

        if (s1 == s2)
            continue;

        // We don't need to swap controllo because is fixed on the teams!
        temp = teamarray[s1];
        teamarray[s1] = teamarray[s2];
        teamarray[s2] = temp;
    }
}

void SwapAllTeams(void)
{
    int i, k;
    uint8_t t;

    for (i = 0; i < totale_giornate; i++)
    {
        for (k = 0; k < (nteams + 1) / 2; k++)
        {
            t = turni[i][k].t1;
            turni[i][k].t1 = turni[i][k].t2;
            turni[i][k].t2 = t;
        }
    }
}

void ClearMatches(int k)
{
    int i, j;

    for (i = k * 4; i < 64; i += 4)
    {
        mp[i].Color = P_VERDE1;
        mp[i].Highlight = P_VERDE0;

        if (mp[i + 3].Text)
            free(mp[i + 3].Text);

        for (j = 0; j < 4; j++)
            mp[i + j].Text = NULL;
    }
}

void ClearSelection(void)
{
    int i;

    selected_number = 0;

    if (!arcade_teams)
    {
        for (i = 0; i < (TS_RIGHE * TS_COLONNE); i++)
            teamselection[i].Color = COLOR_UNSELECTED;
    }
    else
    {
        for (i = 0; i < ARCADE_TEAMS; i++)
            asb[i].Color = asb[i].Highlight = COLOR_UNSELECTED;
    }
}

void NewTurn(void)
{
    int i, j;

    turno++;

    for (i = 0; i < campionato.nteams; i++) {
        for (j = 0; j < teamlist[i].nplayers; j++) {
            if (teamlist[i].players[j].injury > 0)
                teamlist[i].players[j].injury--;

            if (teamlist[i].players[j].Ammonizioni > 1)
                teamlist[i].players[j].Ammonizioni -= 2;
        }
    }
}

BOOL CanContinue(void)
{
    if (wanted_number == 0)
    {
        int i;

        for (i = 1; i < 7; i++)
        {
            if (selected_number == (1 << i))
                return TRUE;
        }

        return FALSE;
    }

    return (BOOL)((wanted_number > 0 && selected_number == wanted_number) ||
                  (wanted_number < 0 && selected_number >= -wanted_number));
}

BOOL ChangeMRMenu(int16_t button)
{
    extern struct Button mrb[];
    ChangeMenu(mrb[button].ID);
    return TRUE;
}

#ifdef MOBILE_VERSION
static BOOL has_human_selection()
{
    int i;
    
    for (i = 0; i < (TS_RIGHE * TS_COLONNE); ++i) {
        struct Button *b = &menu[MENU_TEAM_SELECTION].Button[i];
        
        if (b->Color != COLOR_UNSELECTED & controllo[-b->ID - 1] >= 0)
            return TRUE;
    }
    return FALSE;
}

static BOOL has_arcade_human_selection()
{
    int i;
    
    for (i = 0; i < ARCADE_TEAMS; ++i) {
        struct Button *b = &menu[MENU_ARCADE_SELECTION].Button[i];
        
        if (b->Color != COLOR_UNSELECTED & controllo[b->ID] >= 0)
            return TRUE;
    }
    return FALSE;
}
#endif

BOOL TeamSelection(int16_t button)
{
    static int8_t selected = -1;
    static BOOL team1_selected = FALSE, team2_selected = FALSE;
    struct Button *b;

    if (button < 0)
        return TRUE;

    b = &actual_menu->Button[button];

    if (button < ((TS_RIGHE * TS_COLONNE) ))
    {
        struct Button *b2 = &actual_menu->Button[TS_RIGHE * TS_COLONNE];
        struct Button *b3 = &actual_menu->Button[(TS_RIGHE * TS_COLONNE) + 2];

        if (selected >= 0)
            MyRestoreBack();

        selected = button;

        if (competition != MENU_WORLD_CUP || wanted_number != 32)
        {
            if (b->Color != COLOR_COMPUTER)
            {
                char buffer[128];
// No more needed, we write the team name in the button                
//                PrintShadow(FixedScaledX(1), FixedScaledY(210),
//                        b->Text, strlen(b->Text), bigfont);

                if (b3->Text)
                    free(b3->Text);

                sprintf(buffer, msg_231, b->Text);
                b3->Text = strdup(buffer);
                RedrawButton(b3, b3->Color);
            }
            else if (b3->Text) {
                free(b3->Text);
                b3->Text = NULL;
                CancelButton(b3);
            }
        }

        if (b->Color == COLOR_UNSELECTED) {
            selected_number++;

            controllo[-b->ID - 1] = 0;

#ifdef MOBILE_VERSION
            if (has_human_selection()) {
                b->Color = COLOR_COMPUTER;
                controllo[-b->ID - 1] = -1;
            }
            else {
                controllo[-b->ID - 1] = 1;
                b->Color = COLOR_TEAM_A;
            }
#else
            if (!team1_selected || wanted_number > 2
                || wanted_number <= 0 || selected_number > 2)
            {
                team1_selected = TRUE;
                controllo[-b->ID - 1] = 1;
                b->Color = COLOR_TEAM_A;
            }
            else
            {
                b->Color = COLOR_TEAM_B;
                controllo[-b->ID - 1] = 0;
                team2_selected = TRUE;
            }
#endif
        }
        else if (b->Color == COLOR_COMPUTER) {
            b->Color = COLOR_UNSELECTED;
            selected_number--;
        } // on mobile devices we can have only a single player
#ifndef MOBILE_VERSION        
        else if (b->Color == COLOR_TEAM_A
                 && (!team2_selected || wanted_number > 2
                      || wanted_number <= 0 || selected_number > 2))
        {
            team1_selected = FALSE;
            controllo[-b->ID - 1] = 0;
            b->Color = COLOR_TEAM_B;
        }
#endif
        else
        {
            if (b->Color == COLOR_TEAM_A)
                team1_selected = FALSE;
            else
                team2_selected = FALSE;

            b->Color = COLOR_COMPUTER;
            controllo[-b->ID - 1] = -1;
        }

        RedrawButton(b, b->Color);

        if (CanContinue())
        {
            if (!b2->Text)
            {
                if (competition != MENU_WORLD_CUP || wanted_number != 32)
                    b2->Text = msg_0;
                else
                {
                    b2->Text = msg_1;
                    if (b3->Text)
                        free(b3->Text);
                    b3->Text = strdup(msg_2);
                    RedrawButton(b3, b3->Color);
                }

                RedrawButton(b2, b2->Color);
            }
        }
        else if (b2->Text)
        {
            b2->Text = NULL;
            CancelButton(b2);

            if (competition == MENU_WORLD_CUP && wanted_number == 32)
            {
                if (b3->Text) {
                    free(b3->Text);
                    b3->Text = NULL;
                    CancelButton(b3);
                }
            }
        }
    }
    else if (button == ((TS_RIGHE * TS_COLONNE) + 2)
              && (competition != MENU_WORLD_CUP || wanted_number != 32))
    {
        if (selected > -1)
        {
            can_modify = FALSE;
            SetTeamSettings(-actual_menu->Button[selected].ID - 1, FALSE);
            can_modify = TRUE;

            ChangeMenu(MENU_TEAM_SETTINGS);
        }
    }
    else if (button == (TS_RIGHE * TS_COLONNE) || 
            (button == ((TS_RIGHE * TS_COLONNE) + 2) && competition == MENU_WORLD_CUP))
    {
        int i, j;

        team1_selected = FALSE;
        team2_selected = FALSE;

        for (i = 0, j = 0; i < (TS_RIGHE * TS_COLONNE); i++)
            if (actual_menu->Button[i].Color != COLOR_UNSELECTED)
            {
/*
    This is disabled because controllo is fixed on the team!

                if (actual_menu->Button[i].Color == COLOR_TEAM_A)
                    controllo[j] = 0;
                else if (actual_menu->Button[i].Color == COLOR_TEAM_B)
                    controllo[j] = 1;
                else
                    controllo[j] = -1;
*/
                teamarray[j] = -actual_menu->Button[i].ID - 1;

                j++;
            }

        if (j != selected_number)
            D(bug("Warning, wrong number of teams selected! (%ld instead of %ld)\n", j, selected_number));

        actual_menu->Button[TS_RIGHE * TS_COLONNE].Text = actual_menu->Button[(TS_RIGHE * TS_COLONNE) + 2].Text = NULL;

        if (friendly || training)
        {
            goto singlematch;
        }
        else if (competition == MENU_WORLD_CUP && wanted_number == 32)
        {
            if (button == (TS_RIGHE * TS_COLONNE))
                random_draw = FALSE;
            else
                random_draw = TRUE;

            if (GroupsClear())
            {
                GroupsUpdate();
                ChangeMenu(MENU_WORLD_CUP);
            }
        }
        else if (competition == MENU_MATCHES)
        {
            // Cup
            RandomDraw(selected_number);

            nteams = selected_number;

            menu[MENU_MATCHES].Title = msg_32;
            ViewEliminazioneDiretta(nteams / 2);
            mb[0].ID = MENU_MATCHES;
            ChangeMenu(MENU_MATCHES);
        }
        else if (competition == MENU_LEAGUE)
        {
            // League
            if (j > 20)
            {
                request(msg_178);
                j = 20;
            }

            RandomDraw(j);
            MakeLeague(j);

            InitTable();
            UpdateLeagueTable();

            ChangeMenu(MENU_LEAGUE);
        }
        else
        {
singlematch:
            team1_selected = FALSE;
            team2_selected = FALSE;

            if (j == 1 && training)
            {
                StartMatch(teamarray[0], teamarray[0]);
            }
            else if (j == 2)
            {
                StartMatch(teamarray[0], teamarray[1]);
            }
            else if (network_game)
            {
                if ((network_player = connect_server(network_server, teamarray[0])))
                {
                    int8_t team = teamarray[0] != 0 ? 0 : 1;
                    controllo[team] = -1;
                    StartMatch(teamarray[0], team);
                }
            }
            else
            {
                D(bug("Career not implemented yet!"/*-*/));
            }
        }

        // To finish!
    }
    else if (button == ((TS_RIGHE * TS_COLONNE) +1) && b->ID >= 0)
    {
        team1_selected = team2_selected = FALSE;

        selected = -1;

        actual_menu->Button[(TS_RIGHE * TS_COLONNE) + 2].Text = actual_menu->Button[TS_RIGHE * TS_COLONNE].Text = NULL;

        special = FALSE;
        competition = MENU_TEAMS;

        ClearSelection();

        ChangeMenu(b->ID);
    }

    return TRUE;
}

BOOL JoyCfg(int16_t button)
{
    if (button >= (actual_menu->NumeroBottoni - 1)
         && actual_menu->Button[button].ID >= 0)
        ChangeMenu(actual_menu->Button[button].ID);
    else
    {
        char *temp;
        int k;

        button /= 2;

        temp = actual_menu->Button[button * 2].Text;

        actual_menu->Button[button * 2].Text = msg_232;

        RedrawButton(&actual_menu->Button[button * 2],
                     actual_menu->Button[button * 2].Highlight);

        ScreenSwap();

        k = os_get_joy_button(actual_joystick);

        if (k >= 0)
        {
            actual_menu->Button[button * 2 + 1].Text = buttons[k];
            joycfg_buttons[actual_joystick][button] = k;
            RedrawButton(&actual_menu->Button[button * 2 + 1],
                         actual_menu->Button[button * 2 + 1].Color);
        }

        actual_menu->Button[button * 2].Text = temp;

        RedrawButton(&actual_menu->Button[button * 2],
                     actual_menu->Button[button * 2].Color);
        ScreenSwap();
        prefs_changed = TRUE;
    }

    return TRUE;
}

BOOL KeyCfg(int16_t button)
{
    /* AC: 27/05/04 - First rudimental keyboard configuration */
    if (button>=(actual_menu->NumeroBottoni - 3)
         && actual_menu->Button[button].ID >= 0)
    {
        /* Which "main" button the user have pressed? */
        switch(actual_menu->Button[button].ID)
        {
            /* Why if I include externs.h I obtain 55 compilation error? */
            extern void SaveKeyDef(int, char *);

            /* Save RED keyboard configuration */
            case 0:
                SaveKeyDef(0, KEY_RED_FILE);

                request(msg_70);
                break;

            /* Save BLUE keyboard configuration */
            case 1:
                SaveKeyDef(1, KEY_BLUE_FILE);

                request(msg_70);
                break;

            /* Go back to previuos menu */
            default:
                ChangeMenu(actual_menu->Button[button].ID);
            break;
        }
    }
    else
    {
        char *temp;
        int k = 0;
        BOOL ok = FALSE;
        SDL_Event e;

        button /= 2;

        /* If we have selected a six keys RED control skip the configuration
         * of the extended keys. */
        if (button >= 6 && button < 10 && control[1] == CTRL_KEY_1)
            return TRUE;

        /* If we have selected a six keys BLUE control skip the configuration
         * of the extended keys. */
        if (button >= 16 && control[0] == CTRL_KEY_1)
            return TRUE;

        /* Alternative method that leaves visible the association of the key during key pressing */
        temp = actual_menu->Button[button * 2 + 1].Text;

        actual_menu->Button[button * 2 + 1].Text = msg_232;

        RedrawButton(&actual_menu->Button[button * 2 + 1],
                     actual_menu->Button[button * 2 + 1].Highlight);

        ScreenSwap();

        prefs_changed = TRUE;


        while (!ok)
        {
            SDL_WaitEvent(&e);
            switch (e.type)
            {
                /* Update button as soon a key is pressed... */
                case SDL_KEYDOWN:
                    D(bug("Key pressed C: %d, S: %d, M: %d, N: %s\n",
                        e.key.keysym.scancode,
                        e.key.keysym.sym,
                        e.key.keysym.mod,
                        SDL_GetKeyName(e.key.keysym.sym)));

                    /* Hit ESC to quit the modification */
                    if (e.key.keysym.sym == SDLK_ESCAPE)
                    {
                        k = -1;
                    }
                    else
                    {
                        int i, n_keys = sizeof(keys) / sizeof(SDL_Keycode) ;

                        /* Search the key selected in the valid keys table */
                        k = -2;
                        for (i = 0; i < n_keys; i++)
                            if (e.key.keysym.sym == keys[i])
                            {
                                k = i;
                                break;
                            }
                    }

                    if (k >= 0)
                    {
                        int s = 0, i, n_keys = sizeof(query) / sizeof(int);
                        const char *tmp;
                        /* Verify if the key selected is already used */
                        while (s < n_keys)
                        {
                            /* Skip the currently selected button */
                            if (s != button)
                            {
                                /* Key already mapped? */
                                if (keys[k] == query[s])
                                {
                                    /* Delete the old used association */
                                    actual_menu->Button[s * 2 + 1].Text = NULL;
                                    query[s] = 0;
                                    CancelButton(&actual_menu->Button[s * 2 + 1]);
                                    break;
                                }
                            }
                            s++;
                        }

                        /* Uppercase conversion for ETW font */
                        tmp = SDL_GetKeyName(keys[k]);
                        i = 0;
                        while(*tmp)
                        {
                            keys_names[button][i] = toupper(*tmp);
                            tmp++;
                            i++;
                        }
                        keys_names[button][i] = 0;
                        query[button] = SDL_GetScancodeFromKey(keys[k]);
                        actual_menu->Button[button * 2 + 1].Text = keys_names[button];
                        RedrawButton(&actual_menu->Button[button * 2 + 1],
                                     actual_menu->Button[button * 2 + 1].Color);
                    }
                    else
                    {
                        /* Restore the old button */
                        actual_menu->Button[button * 2 + 1].Text = temp;
                        RedrawButton(&actual_menu->Button[button * 2 + 1],
                                     actual_menu->Button[button * 2 + 1].Color);
                    }
                    ScreenSwap();
                break;
                /* ...but exit when the key is released in order to skip this event
                 * in the main cycle
                 */
                case SDL_KEYUP:
                    ok = TRUE;
                break;
                default:
                break;
            }
        }

        /* Flush the event queue. *
         It doesn't work...
        while(SDL_PollEvent(&e)); */

        /* Signal the user he has pressed a reserved key */
        if (k == -2) {
            request(msg_226);
        }
    }
    return TRUE;
}

BOOL ArcadeTeamSelection(int16_t button)
{
    static int8_t selected = -1;
    static BOOL team1_selected = FALSE, team2_selected = FALSE;
    struct Button *b;

    if (button < 0)
        return TRUE;

    b = &actual_menu->Button[button];

    if (button < ARCADE_TEAMS)
    {
        struct Button *b2 = &actual_menu->Button[ARCADE_TEAMS];
        struct Button *b3 = &actual_menu->Button[ARCADE_TEAMS + 2];

        if (selected >= 0)
            MyRestoreBack();

        if (menu_music && music_playing)
            StopMenuMusic();

        if (jingle >= 0 && (selected != button || b->Color == COLOR_COMPUTER))
        {
            D(bug("Interrupt channel %ld\n", jingle));
            Mix_HaltChannel(jingle);
            jingle = -1;
        }

        if (selected != button && b->Color != COLOR_COMPUTER&&!no_sound)
        {
            jingle = PlayBackSound(menusound[FIRST_ARCADE + b->ID]);
            D(bug("Play sound %ld on channel %ld\n",
                  FIRST_ARCADE + b->ID, jingle));
        }

        selected = button;

        // if color is computer this click is gonna unselect the team, so it should
        // remove the "VIEW TEAM" button
        if (b->Color != COLOR_COMPUTER) {
            if (!b3->Text) {
                char buffer[128];
                sprintf(buffer, msg_231, teamlist[b->ID].name);
                b3->Text = strdup(buffer);
                RedrawButton(b3, b3->Color);
            }
            else {
                free(b3->Text);
                b3->Text = NULL;
                CancelButton(b3);
            }
        }

        if (b->Color == COLOR_UNSELECTED)
        {
            selected_number++;

            controllo[b->ID] = 0;

#ifdef MOBILE_VERSION
            if (has_arcade_human_selection()) {
                // max one human team in mobile version!
                team2_selected = TRUE;
                b->Color = COLOR_COMPUTER;
                controllo[b->ID] = -1;
            }
            else {
                team1_selected = TRUE;
                controllo[b->ID] = 1;
                b->Color = COLOR_TEAM_A;
            }
#else
            if (!team1_selected || wanted_number > 2
                 || wanted_number <= 0 || selected_number > 2)
            {
                team1_selected = TRUE;
                controllo[b->ID] = 1;
                b->Color = COLOR_TEAM_A;
            }
            else
            {
                team2_selected = TRUE;
                controllo[b->ID] = 0;
                b->Color = COLOR_TEAM_B;
            }
#endif
        }
        else if (b->Color == COLOR_COMPUTER)
        {
            b->Color = COLOR_UNSELECTED;
            selected_number--;
        }
#ifndef MOBILE_VERSION
        else if (b->Color == COLOR_TEAM_A
                  && (!team2_selected || wanted_number > 2
                       || wanted_number <= 0 || selected_number > 2))
        {
            team1_selected = FALSE;
            controllo[b->ID] = 0;
            b->Color = COLOR_TEAM_B;
        }
#endif
        else
        {
            if (b->Color == COLOR_TEAM_A)
                team1_selected = FALSE;
            else
                team2_selected = FALSE;

            b->Color = COLOR_COMPUTER;
            controllo[b->ID] = -1;
        }

        b->Highlight = b->Color;
        RedrawButton(b, b->Color);

        if (CanContinue())
        {
            if (!b2->Text)
            {
                b2->Text = msg_0;
                RedrawButton(b2, b2->Color);
            }
        }
        else if (b2->Text)
        {
            b2->Text = NULL;
            CancelButton(b2);
        }
    }
    else if (button == ARCADE_TEAMS + 2)
    {
        if (selected > -1 && selected < ARCADE_TEAMS)
        {
            if (jingle >= 0)
            {
                Mix_HaltChannel(jingle);
                D(bug("Interrupt channel %ld\n", jingle));
                jingle = -1;
            }

            can_modify = FALSE;
            SetTeamSettings( actual_menu->Button[selected].ID, FALSE);
            can_modify = TRUE;

            ChangeMenu(MENU_TEAM_SETTINGS);
        }
    }
    else if (button == ARCADE_TEAMS)
    {
        int i, j;

        D(bug("Continue selected\n"));

        team1_selected = FALSE;
        team2_selected = FALSE;

        if (jingle >= 0)
        {
            D(bug("Interrupt channel %ld\n", jingle));
            Mix_HaltChannel(jingle);
            jingle = -1;
        }

        for (i = 0, j = 0; i < ARCADE_TEAMS; i++)
            if (actual_menu->Button[i].Color != COLOR_UNSELECTED)
            {
/*
                if (actual_menu->Button[i].Color == COLOR_TEAM_A)
                    controllo[j] = 0;
                else if (actual_menu->Button[i].Color == COLOR_TEAM_B)
                    controllo[j] = 1;
                else
                    controllo[j] = -1;
*/
                teamarray[j] = actual_menu->Button[i].ID;
                D(bug("Selected %ld team\n", teamarray[j]));
                j++;
            }

        if (j != selected_number)
            D(bug("Warning, wrong number of selected teams! (%ld instead of %ld)\n", j, selected_number));

        if (friendly || training)
        {
            goto friendlymatch;
        }
        else if (competition == MENU_MATCHES)
        {
            actual_menu->Button[ARCADE_TEAMS].Text
                = actual_menu->Button[ARCADE_TEAMS + 2].Text = NULL;

            PlayMenuMusic();

            nteams = selected_number;

            menu[MENU_MATCHES].Title = msg_33;
            ViewEliminazioneDiretta(nteams / 2);
            mb[0].ID = MENU_MATCHES;
            ChangeMenu(MENU_MATCHES);
        }
        else if (competition == MENU_CHALLENGE)
        {
            // To do!
            turno = 0;
            cb[0].ID = MENU_CHALLENGE;
            SetupMatches();
            ChangeMenu(MENU_CHALLENGE);
        }
        else
        {
friendlymatch:
            D(bug("Starting friendly match/practice...\n"));
            actual_menu->Button[ARCADE_TEAMS].Text
                = actual_menu->Button[ARCADE_TEAMS + 2].Text = NULL;

            PlayMenuMusic();

            team1_selected = FALSE;
            team2_selected = FALSE;

            if (j == 1 && training)
            {
                StartMatch(teamarray[0], teamarray[0]);
            }
            else if (j == 2)
            {
                D(bug("->Entering startmatch!\n"));
                StartMatch(teamarray[0], teamarray[1]);
            }
            else
            {
                D(bug("Career not implemented yet!"/*-*/));
            }
        }

    }
    else if (button == (ARCADE_TEAMS + 1) && b->ID >= 0)
    {
        team1_selected = team2_selected = FALSE;

        selected = -1;

        competition = MENU_TEAMS;

        if (jingle >= 0)
        {
            D(bug("Interrupt channel %ld\n", jingle));
            Mix_HaltChannel(jingle);
            jingle = -1;
        }

        actual_menu->Button[ARCADE_TEAMS].Text
            = actual_menu->Button[ARCADE_TEAMS + 2].Text = NULL;

        ClearSelection();

        PlayMenuMusic();

        ChangeMenu(b->ID);
    }

    return TRUE;
}

BOOL GamePrefs(int16_t button)
{
    if (button == (actual_menu->NumeroBottoni - 1)
         && actual_menu->Button[button].ID >= 0)
    {
        ChangeMenu(actual_menu->Button[button].ID);
        return TRUE;
    }

    button |= 1;

    switch(button)
    {
        case 1:
            duration++;

            if (!time_options[duration])
                duration = 0;

            actual_menu->Button[button].Text = time_options[duration];

            t_l = atoi(time_options[duration]);
            break;
        case 3:
            field++;

            if (!field_options[field])
                field = 0;

            actual_menu->Button[button].Text = field_options[field];

            if (field >= 7)
            {
                actual_menu->Button[12].Text = actual_menu->Button[13].Text = NULL;
                CancelButton(&actual_menu->Button[12]);
                CancelButton(&actual_menu->Button[13]);
            }
            else if (!actual_menu->Button[12].Text)
            {
                field_type = 0;
                actual_menu->Button[12].Text = msg_35;
                actual_menu->Button[13].Text = field_types[0];
                RedrawButton(&actual_menu->Button[12],
                              actual_menu->Button[12].Color);
                RedrawButton(&actual_menu->Button[13],
                              actual_menu->Button[13].Color);
            }
            break;
        case 5:
            strictness++;

            if (strictness > 10)
                strictness = 0;

            if (strictness == 10)
                actual_menu->Button[button].Text = msg_2;
            else
                actual_menu->Button[button].Text = numero[strictness];

            break;
        case 7:
            if (actual_menu->Button[button].Text == enabled)
                actual_menu->Button[button].Text = disabled;
            else
                actual_menu->Button[button].Text = enabled;

            use_replay = !use_replay;
            break;
        case 9:
            free_longpass = !free_longpass;
            actual_menu->Button[button].Text = free_longpass ? msg_37 : msg_36;
            break;
        case 11:
            daytime++;

            if (!daytimes[daytime])
                daytime = 0;

            actual_menu->Button[button].Text = daytimes[daytime];
            break;
        case 13:
            field_type++;

            if (!field_types[field_type])
                field_type = 0;

            actual_menu->Button[button].Text = field_types[field_type];
            break;
        case 15:
            newchange = !newchange;
            actual_menu->Button[button].Text = newchange ? "ETW"/*-*/ : msg_200;
            break;
        default:
            D(bug("Error, option (%ld) not previewed!\n"/*-*/, button));
            return FALSE;
    }

    prefs_changed = TRUE;
    
    RedrawButton(&actual_menu->Button[button],
                 actual_menu->Button[button].Color);

    return TRUE;
}

BOOL AudioPrefs(int16_t button)
{
    if (button == (actual_menu->NumeroBottoni - 1)
         && actual_menu->Button[button].ID >= 0)
    {
        ChangeMenu(actual_menu->Button[button].ID);
        return TRUE;
    }

    button |= 1;

    switch(button)
    {
        case 1:
            /* AC: When the program starts with no_sound = true, but the options
             * are modified to no_sound = false, the sounds of the menu are not
             * loaded and the PlayBackSound is called with a NULL element
             * (it has happened in the arcade menu).
             * According to me it must be called the CaricaSuoniMenu.
             */
            no_sound = no_sound ? FALSE : TRUE;

            /* If the sound is off, unload the sounds, else load them */
            if (no_sound) {
                StopMenuMusic();
                FreeMenuMusic();
                LiberaSuoniMenu();
                FreeSoundSystem();
            }
            else {
                /* AC: Surely some controls are needed */
                InitSoundSystem();
                CaricaSuoniMenu();
                PlayMenuMusic();
            }

            /* AC: I think this func is needed. */
            os_start_audio();

            break;
        case 3:
            if (use_crowd)
                use_crowd = FALSE;
            else
            {
                use_speaker = FALSE;
                use_crowd = TRUE;
            }
            break;
        case 5:
            if (use_speaker)
                use_speaker = FALSE;
            else
            {
                use_speaker = TRUE;
                use_crowd = FALSE;
            }
            break;
        case 7:
            menu_music = menu_music ? FALSE : TRUE;

            if (menu_music == FALSE)
                StopMenuMusic();
            else
                PlayMenuMusic();
            break;
    }

    prefs_changed = TRUE;

    UpdatePrefs(MENU_AUDIO_PREFS);

    RedrawButton(&actual_menu->Button[button],
                 actual_menu->Button[button].Color);

    switch(button)
    {
        case 5:
            RedrawButton(&actual_menu->Button[3],
                         actual_menu->Button[3].Color);
            break;
        case 3:
            RedrawButton(&actual_menu->Button[5],
                         actual_menu->Button[5].Color);
            break;
    }

    return TRUE;
}

BOOL SystemPrefs(int16_t button)
{
    if (button >= (actual_menu->NumeroBottoni - 3)
         && actual_menu->Button[button].ID >= 0)
    {
        if (button == (actual_menu->NumeroBottoni - 3))
        {
            if (!os_check_joy(0))
            {
                request(msg_213);
                return TRUE;
            }
            else UpdateJoyCfg(actual_joystick);
        }
        /* AC: Update keyboard configuration */
        else if (button == (actual_menu->NumeroBottoni - 2))
        {
            UpdateKeyCfg();
        }

        ChangeMenu(actual_menu->Button[button].ID);
        return TRUE;
    }

    button |= 1;

    switch(button)
    {
        // disabled 1, 3, 9
        case 1:
            break;
        case 3:
            break;
        case 5:
        case 7:
            control[(button - 5) / 2]++;


            if (!controls[control[(button - 5) / 2]])
                control[(button - 5) / 2] = 0;

            if (control[(button - 5) / 2] < CTRL_KEY_1)
            {
                if (!os_check_joy((button - 5) / 2))
                    control[(button - 5) / 2] = CTRL_KEY_1;
            }

            actual_menu->Button[button].Text = controls[control[(button - 5) / 2]];
            break;

        case 9:
            break;
    }
    prefs_changed = TRUE;

    UpdatePrefs(MENU_SYSTEM_PREFS);

    RedrawButton(&actual_menu->Button[button],
                 actual_menu->Button[1].Color);

    return TRUE;
}


BOOL MobilePrefs(int16_t button)
{
    if (button == (actual_menu->NumeroBottoni - 1)
         && actual_menu->Button[button].ID >= 0)
    {
        ChangeMenu(actual_menu->Button[button].ID);
        return TRUE;
    }
    button |= 1;
    switch(button)
    {
        case 1:
            if (detail_level & USA_RADAR) {
                radar_position++;

                if (radar_position >= 12) {
                    detail_level &= ~USA_RADAR;
                    radar_position = -1;
                    actual_menu->Button[button].Text = disabled;
                }
                else
                    actual_menu->Button[button].Text
                        = radar_options[radar_position];
            }
            else {
                radar_position = 0;
                detail_level |= USA_RADAR;

                actual_menu->Button[button].Text
                    = radar_options[radar_position];
            }
            break;
        case 3:
            nointro = nointro ? FALSE : TRUE;
            break;
        case 5:   
            no_sound = no_sound ? FALSE : TRUE;

            /* If the sound is off, unload the sounds, else load them */
            if (no_sound) {
                StopMenuMusic();
                FreeMenuMusic();
                LiberaSuoniMenu();
                FreeSoundSystem();
            } else {
                /* AC: Surely some controls are needed */
                InitSoundSystem();
                CaricaSuoniMenu();
                PlayMenuMusic();
            }

            /* AC: I think this func is needed. */
            os_start_audio();
            break;
        case 7:
            menu_music = menu_music ? FALSE : TRUE;

            if (menu_music == FALSE)
                StopMenuMusic();
            else
                PlayMenuMusic();
            break;
        case 9:
            tutorial = tutorial ? FALSE : TRUE;
            break;
    }
    prefs_changed = TRUE;

    UpdatePrefs(MENU_MOBILE_PREFS);

    RedrawButton(&actual_menu->Button[button],
                 actual_menu->Button[1].Color);

    return TRUE;
}

BOOL VideoPrefs(int16_t button)
{
    if (button == (actual_menu->NumeroBottoni - 1)
         && actual_menu->Button[button].ID >= 0)
    {
        ChangeMenu(actual_menu->Button[button].ID);
        return TRUE;
    }

    button |= 1;

    switch(button)
    {
        case 1:
            if (wb_game)
            {
                wb_game = FALSE;
                /* AC: if we are on screen, disable the scaling */
                use_gfx_scaling = FALSE;

                FullScreen(TRUE);
                
                /* AC: Since these buttons are restored later on, I think
                 * that we don't have to eliminate them. They are Scaling
                 * and Buffering. */
                //CancelButton(&actual_menu->Button[16]);
                //CancelButton(&actual_menu->Button[17]);
                CancelButton(&actual_menu->Button[18]);
                CancelButton(&actual_menu->Button[19]);

                /* AC: The same one is said for these */
                //CancelButton(&actual_menu->Button[20]);
                //CancelButton(&actual_menu->Button[21]);
            }
            else
            {
                wb_game = TRUE;

                FullScreen(FALSE);

                RedrawButton(&actual_menu->Button[16],
                             actual_menu->Button[0].Color);
                RedrawButton(&actual_menu->Button[17],
                             actual_menu->Button[1].Color);
                RedrawButton(&actual_menu->Button[20],
                             actual_menu->Button[0].Color);
                RedrawButton(&actual_menu->Button[21],
                             actual_menu->Button[1].Color);

                if (use_gfx_scaling)
                {
                    RedrawButton(&actual_menu->Button[18],
                                 actual_menu->Button[0].Color);
                    RedrawButton(&actual_menu->Button[19],
                                 actual_menu->Button[1].Color);
                }
            }
        break;
        case 3:
            if (detail_level & USA_RADAR)
            {
                radar_position++;

                if (radar_position >= 12)
                {
                    detail_level &= ~USA_RADAR;
                    radar_position = -1;
                    actual_menu->Button[button].Text = disabled;
                }
                else
                    actual_menu->Button[button].Text
                        = radar_options[radar_position];
            }
            else
            {
                radar_position = 0;
                detail_level |= USA_RADAR;

                actual_menu->Button[button].Text
                    = radar_options[radar_position];
            }
            break;
        case 7:
        case 5:
        case 9:
        case 11:
        case 15:
            // disabled, always on!
            break;
        case 13:
            nointro = (nointro) ? FALSE : TRUE;
            break;
        case 17:
            if (!use_gfx_scaling)
            {
                if (wb_game)
                {
                    actual_menu->Button[18].Text = msg_218;
                    actual_menu->Button[19].Text = scaling_resolutions[current_scaling];
                    RedrawButton(&actual_menu->Button[19],
                                 actual_menu->Button[1].Color);
                    RedrawButton(&actual_menu->Button[18],
                                 actual_menu->Button[0].Color);
                    use_gfx_scaling = TRUE;
                }
                else
                    request(msg_234);
            }
            else
            {
                use_gfx_scaling = FALSE;
                CancelButton(&actual_menu->Button[18]);
                CancelButton(&actual_menu->Button[19]);
            }
            break;
        case 19:
            {
                char buffer[20];

                current_scaling++;

                if (!scaling_resolutions[current_scaling])
                    current_scaling = 0;

                // AC: A small misprint :-)
                //scaling_resolutions[current_scaling][3] = 0;

                strncpy(buffer, scaling_resolutions[current_scaling], 3);

                buffer[3] = 0;

                FIXED_SCALING_WIDTH = atoi(buffer);
                FIXED_SCALING_HEIGHT = atoi(scaling_resolutions[current_scaling] + 4);
            }
            break;
        case 21:
            do
            {
                current_resolution++;

                if (!resolutions[current_resolution])
                    current_resolution = 0;

                wanted_width = atoi(resolutions[current_resolution]);
                wanted_height = atoi(resolutions[current_resolution] + 4);

                /* AC: Trying to increase screen resolution upto 1024x768 */
                if (wanted_height == 0)
                    wanted_height = atoi(resolutions[current_resolution] + 5);
            }
            while (!os_videook(wanted_width, wanted_height));

            if (!ResizeWindow(wanted_width, wanted_height)) {
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

            break;
       case 23:
           tutorial = tutorial ? FALSE : TRUE;
            break;
       default:
            D(bug("Unknown menu button clicked: %d\n", button));
            break;
    }
    prefs_changed = TRUE;

    UpdatePrefs(MENU_VIDEO_PREFS);

    RedrawButton(&actual_menu->Button[button],
                 actual_menu->Button[1].Color);

    return TRUE;
}

void UpdatePrefs(int8_t set)
{
    struct GfxMenu *m = &menu[set];

    switch(set)
    {
    case MENU_GAME_PREFS:
        m->Button[1].Text = time_options[duration];
        m->Button[3].Text = field_options[field];
        m->Button[5].Text = (strictness == 10) ? msg_2 : numero[strictness];
        m->Button[7].Text = use_replay ? enabled : disabled;
        m->Button[9].Text = free_longpass ? msg_37 : msg_36;
        m->Button[11].Text = daytimes[daytime];
        m->Button[12].Text = (field < 7) ? msg_35 : NULL;
        m->Button[13].Text = (field < 7) ? field_types[field_type] : NULL;
#ifndef MOBILE_VERSION
        m->Button[15].Text = newchange ? "ETW"/*-*/ : msg_200;
#endif
        break;
    case MENU_MOBILE_PREFS:
        m->Button[1].Text = (detail_level & USA_RADAR)
                          ? radar_options[radar_position] : disabled;
        m->Button[3].Text = nointro ? disabled : enabled;
        m->Button[5].Text = no_sound ? disabled : enabled;
        m->Button[7].Text = menu_music ? enabled : disabled;
        m->Button[9].Text = tutorial ? enabled : disabled;
        break;
    case MENU_AUDIO_PREFS:
        m->Button[1].Text = no_sound ? disabled : enabled;
        m->Button[3].Text = use_crowd ? enabled : disabled;
        m->Button[5].Text = use_speaker ? enabled : disabled;
        m->Button[7].Text = menu_music ? enabled : disabled;
        break;

    case MENU_VIDEO_PREFS:
        m->Button[1].Text = wb_game ? "WINDOW"/*-*/ : "FULLSCREEN"/*-*/;
        m->Button[3].Text = (detail_level & USA_RADAR)
                          ? radar_options[radar_position] : disabled;
        m->Button[13].Text = nointro ? disabled : enabled;

        m->Button[17].Text = use_gfx_scaling ? enabled : disabled;
        m->Button[18].Text = use_gfx_scaling ? msg_218 : NULL;
        m->Button[19].Text = use_gfx_scaling
                           ? scaling_resolutions[current_scaling] : NULL;
        m->Button[21].Text = resolutions[current_resolution];
        m->Button[23].Text = tutorial ? enabled : disabled;
        break;

    case MENU_SYSTEM_PREFS:
        m->Button[5].Text = controls[control[0]];
        m->Button[7].Text = controls[control[1]];
        break;
  }
}

void SetupMatches(void)
{
    make_setup = FALSE;

    switch(competition)
    {
        case MENU_CHALLENGE:

            // Reset the control for all except the team in use...
            if (turno == 0) {
                int i;

                cp[6].Text = NULL;

                for (i = 0; i < ARCADE_TEAMS + 1; i++)
                    if (i != *teamarray)
                        controllo[i] = -1;
            }

            if (*teamarray == arcade_sequence[turno])
                turno++;

            cb[0].ID = MENU_CHALLENGE;

            if (turno < 10)
            {
                struct team_disk *s = &teamlist[arcade_sequence[turno]];
                int i;

                menu[MENU_CHALLENGE].Title = msg_41;

                for (i = 0; i < s->nkeepers; i++)
                {
                    s->keepers[i].Parata = min(9, s->keepers[i].Parata + (turno + 1) / 2);
                    s->keepers[i].speed = min(9, s->keepers[i].speed + (turno + 1) / 2);
                    s->keepers[i].Attenzione = min(9, s->keepers[i].Attenzione + (turno + 1) / 2);
                }

                for (i = 0; i < s->nplayers; i++)
                {
                    s->players[i].speed = min(9, s->players[i].speed + (turno + 1) / 2);
                    s->players[i].Tiro = min(9, s->players[i].Tiro + (turno + 1) / 2);
                    s->players[i].tackle = min(9, s->players[i].tackle + (turno + 1) / 2);
                    s->players[i].quickness = min(9, s->players[i].quickness + (turno + 1) / 2);
                    s->players[i].technique = min(9, s->players[i].technique + (turno + 1) / 2);
                }
            }
            else if (turno == 10)
                menu[MENU_CHALLENGE].Title = msg_42;
            else
            {
                // Add here the final visualization one of the arcade.
                Outro();
                arcade_score += 500; // Final bonus

                AddScore(*teamarray);
                add_achievement("13_challenge"/*-*/, 100.0);
                LoadTeams("teams/arcade"/*-*/);
                turno = 0;
                competition = MENU_TEAMS;
                cb[0].ID = MENU_ARCADE;
            }
            // Team A
            cp[0].Text[1] = *teamarray;
            cp[4].Color = cp[0].Color = cp[0].Highlight = cp[2].Color = colore_team[controllo[*teamarray] + 1];
            cp[2].Text = teamlist[*teamarray].name;
            cp[4].Highlight = cp[2].Highlight = highlight_team[controllo[*teamarray] + 1];

            jingle = PlayBackSound(menusound[FIRST_ARCADE + arcade_sequence[turno]]);
            D(bug("Playing sound %ld on channel %ld...\n", FIRST_ARCADE + arcade_sequence[turno], jingle));

            if (cp[4].Text)
            {
                free(cp[4].Text);
                cp[4].Text = NULL;
                cp[5].Text = NULL;
            }
            // Team B
            cp[1].Text[1] = arcade_sequence[turno];
            cp[3].Text = teamlist[arcade_sequence[turno]].name;
            cp[5].Color = cp[1].Color = cp[1].Highlight = cp[3].Color = colore_team[0];
            cp[5].Highlight = cp[3].Highlight = highlight_team[0];

            break;
        case MENU_MATCHES:
            mb[0].ID = MENU_MATCHES;

            if (arcade)
                menu[MENU_MATCHES].Title = msg_33;
            else
                menu[MENU_MATCHES].Title = msg_43;

            ViewEliminazioneDiretta(nteams / 2);
            break;
        case MENU_LEAGUE:
            menu[MENU_MATCHES].Title = msg_44;

            if (turno < totale_giornate)
            {
                int8_t a, b;
                int k;

                for (k = 0; k < nteams / 2; k++)
                {
                    a = teamarray[turni[turno][k].t1 - 1];
                    b = teamarray[turni[turno][k].t2 - 1];

                    ClearMatches((nteams + 1) / 2);

                    if (a == FAKE_TEAM)
                    {
                        a = b;
                        b = FAKE_TEAM;
                    }

                    if (b == FAKE_TEAM)
                    {
                        mp[k * 4].Text = teamlist[a].name;
                        mp[k * 4].Color = colore_team[controllo[a] + 1];
                        mp[k * 4].Highlight = highlight_team[controllo[a] + 1];
                        mp[k * 4 + 1].Text = NULL;
                        mp[k * 4 + 2].Text = "NO GAME"/*-*/;
                        mp[k * 4 + 2].Color = colore_team[0];
                        mp[k * 4 + 2].Highlight = highlight_team[0];

                    }
                    else
                    {
                        mp[k * 4].Color = colore_team[controllo[a] + 1];
                        mp[k * 4].Highlight = highlight_team[controllo[a] + 1];
                        mp[k * 4 + 2].Color = colore_team[controllo[b] + 1];
                        mp[k * 4 + 2].Highlight = highlight_team[controllo[b] + 1];

                        mp[k * 4].Text = teamlist[a].name;
                        mp[k * 4 + 1].Text = "-"/*-*/;
                        mp[k * 4 + 2].Text = teamlist[b].name;
                    }

                    if (mp[k * 4 + 3].Text)
                    {
                        free(mp[k * 4 + 3].Text);
                        mp[k * 4 + 3].Text = NULL;
                    }
                }

                if (turno < totale_giornate)
                    mb[0].ID = MENU_MATCHES;
                else if (scontri < 2)
                {
                    mb[0].ID = MENU_LEAGUE;
                    turno = 0;
                    competition = MENU_TEAMS;
                }
                else
                {
                    mb[0].ID = MENU_MATCHES;
                    SwapAllTeams();
                    turno = 0;
                    scontri--;
                }
            }
            else
            {
                mb[0].ID = MENU_SIMULATION;
                competition = MENU_TEAMS;
                turno = 0;
            }
            break;
        case MENU_WORLD_CUP:
            if (turno < 3)
            {
                int i, j, k = 0;
                extern struct Match camp4[3][2];
                extern int8_t start_groups[8][4];

                menu[MENU_MATCHES].Title = ( turno == 0 ? msg_45 : (turno == 1 ? msg_46 : msg_47));

                for (i = 0; i < 8; i++)
                {
                    for (j = 0; j < 2; j++)
                    {
                        mp[k * 4].Text = teamlist[start_groups[i][camp4[turno][j].t1 - 1]].name;
                        mp[k * 4 + 1].Text = "-"/*-*/;
                        mp[k * 4 + 2].Text = teamlist[start_groups[i][camp4[turno][j].t2 - 1]].name;
                        mp[k * 4].Color = colore_team[controllo[start_groups[i][camp4[turno][j].t1 - 1]] + 1];
                        mp[k * 4].Highlight = highlight_team[controllo[start_groups[i][camp4[turno][j].t1 - 1]] + 1];
                        mp[k * 4 + 2].Color = colore_team[controllo[start_groups[i][camp4[turno][j].t2 - 1]] + 1];
                        mp[k * 4 + 2].Highlight = highlight_team[controllo[start_groups[i][camp4[turno][j].t2 - 1]] + 1];

                        if (mp[k * 4 + 3].Text)
                        {
                            free(mp[k * 4 + 3].Text);
                            mp[k * 4 + 3].Text = NULL;
                        }

                        k++;
                    }
                }
                mb[0].ID = MENU_MATCHES;
            }
            else if (turno < 7)
            {
                ViewEliminazioneDiretta(mondiali[turno - 3]);


                if (turno != 6)
                    menu[MENU_MATCHES].Title = msg_48;
                else
                {
                    int i;

                    menu[MENU_MATCHES].Title = msg_49;

                    for (i = 0; i < 4; i++)
                    {
                        mp[5 * 4 + i].Text = mp[1 * 4 + i].Text;
                        mp[2 * 4 + i].Text = mp[i].Text;
                        mp[2 * 4 + i].Color = mp[i].Color;
                        mp[2 * 4 + i].Highlight = mp[i].Highlight;
                        mp[5 * 4 + i].Color = mp[4 + i].Color;
                        mp[5 * 4 + i].Highlight = mp[4 + i].Highlight;
                        mp[i].Text = NULL;
                        mp[4 + i].Text = NULL;
                        mp[3 * 4 + i].Text = NULL;
                        mp[4 * 4 + i].Text = NULL;
                    }

                    mp[4].Text = msg_50;
                    mp[4].Color = P_GIALLO;
                    mp[4].Highlight = P_BIANCO;

                    mp[4 * 4].Text = msg_51;
                    mp[4 * 4].Color = P_GIALLO;
                    mp[4 * 4].Highlight = P_BIANCO;
                }

                mb[0].ID = MENU_MATCHES;
            }

            break;
    }
}

extern struct Button mrb[];
static int final_menu = -1;

static void handle_challenge()
{
    char *c;
    int16_t result;
    
    if (jingle >= 0)
    {
        D(bug("Interrupt channel %ld\n", jingle));
        Mix_HaltChannel(jingle);
        jingle = -1;
    }
    
    if (turno == 10)
        final = TRUE;

    result = StartMatch(*teamarray, arcade_sequence[turno]);
    
    cp[2].Text = teamlist[*teamarray].name;
    cp[3].Text = teamlist[arcade_sequence[turno]].name;

    c = cp[4].Text = strdup(ElaboraRisultato(*teamarray, arcade_sequence[turno], result));

    while(*c != '-')
        c++;

    *c++ = 0;

    cp[5].Text = c;

    make_setup = TRUE;

    arcade_score -= ((result >> 8) * 10);
    arcade_score += ((result & 0xff) * 15);

    if ((result & 0xff) > (result >> 8) || turno == 10) {
        cb[0].ID = MENU_CHALLENGE;
        arcade_score += turno * 20;
        turno++;
    }
    else {
        if (turno > 0) {
            D(bug("GAME OVER, adding to hall of fame arcade score %d\n", arcade_score));
            AddScore(*teamarray);
        }
        turno = 0;
        competition = MENU_TEAMS;
        LoadTeams("teams/arcade"/*-*/); // Reload original teams...
        cp[2].Text = teamlist[*teamarray].name;
        cp[3].Text = teamlist[arcade_sequence[turno]].name;
        cp[6].Text = msg_52;
        cb[0].ID = MENU_ARCADE;
    }
    
    D(bug("Setting destination menu to %d\n", final_menu));
    ChangeMenu(final_menu);
}

static int cup_team_number = 0, isworldcupfinal = 0;
extern BOOL interrupted;

static void PostMatches()
{
    nteams /= 2;

    mb[0].ID = final_menu;

    if (nteams > 1)
    {
        NewTurn();
        make_setup = TRUE;
    }
    else
    {
        if (!arcade)
            mb[0].ID = MENU_SIMULATION;
        else
            mb[0].ID = MENU_ARCADE;

        turno = 0;
        competition = MENU_TEAMS;
    }
    // if "all cpu" we need to switch to matches screen
    ChangeMenu(MENU_MATCHES);
}

static void prepare_table(int i, int16_t risultato)
{
    int8_t a=teamarray[i*2], b=teamarray[i*2+1];

    mp[i*4+3].Text=strdup(ElaboraRisultato(a, b, risultato));

    if((risultato>>8)>(risultato&0xff))
    {
        teamarray[i]=b;
        teamarray[i+cup_team_number*2]=a;
    }
    else
    {
        teamarray[i]=a;
        teamarray[i+cup_team_number*2]=b;
    }
}

static void EliminazioneDiretta()
{
    int i;
    nopari=TRUE;

    // if we have only a game it's a final
    if (cup_team_number == 1)
        final = TRUE;

    // then we can start to parse the following match
    for(i = 0; i < cup_team_number; i++)
    {
        int8_t a=teamarray[i*2], b=teamarray[i*2+1];
        int16_t risultato;

        // on human players we mark the point that we reach
        // and we leave the loop...
        if (i == 0 && isworldcupfinal)
            final = TRUE;
        
        mrb[0].ID = MENU_MATCHES;
        risultato = PlayMatch(a, b);

        final=FALSE;

        prepare_table(i, risultato);
    }

// Riordino le squadre...

    for(i=0; i<cup_team_number; i++)
        teamarray[i+cup_team_number]=teamarray[i+cup_team_number*2];

    nopari=FALSE;
}

int16_t PlayMatch(int8_t a, int8_t b)
{
    if(controllo[a]>=0 || controllo[b]>=0) {
        return StartMatch(a, b);
    }
    else
        return ComputerMatch(a, b);
}

static void handle_league()
{
    int16_t risultato;
    int8_t a, b;
    int k;
    
    menu[MENU_MATCHES].Title = msg_55;
    nopari = FALSE;

    for (k = 0; k < nteams / 2; k++) {
        a = teamarray[turni[turno][k].t1 - 1];
        b = teamarray[turni[turno][k].t2 - 1];

        if (a != FAKE_TEAM && b != FAKE_TEAM)
        {
            mrb[0].ID = MENU_MATCHES;
            risultato = PlayMatch(a, b);
            mp[k * 4 + 3].Text = strdup(ElaboraRisultato(a, b, risultato));
        }
    }

    D(bug("Completed league round %d (%d)\n", turno, scontri));

    NewTurn();
    UpdateLeagueTable();
    mb[0].ID = MENU_LEAGUE;

    if (turno == totale_giornate)
    {
        turno = 0;

        if (scontri < 2)
        {
            if (controllo[league_pos[0]] >= 0 && nteams >= 8)
                add_achievement("12_league"/*-*/, 100.0);
            lb[0].ID = MENU_LEAGUE;
            competition = MENU_TEAMS;
        }
        else
        {
            scontri--;
            SwapAllTeams();
        }
    }
    // if "all cpu" we need to switch to matches screen (check if we need the interrupted if)
    ChangeMenu(MENU_MATCHES);
}

static void handle_worldcup()
{
    int i;
    if (turno < 3)
    {
        int j, k = 0;
        int8_t a, b;
        int16_t risultato;
        extern struct Match camp4[3][2];
        extern int8_t start_groups[8][4], groups[8][4];
        
        final = FALSE;
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 2; j++)
            {
                a = start_groups[i][camp4[turno][j].t1 - 1];
                b = start_groups[i][camp4[turno][j].t2 - 1];

                risultato = PlayMatch(a, b);
                               
                mp[k * 4 + 3].Text = strdup(ElaboraRisultato(a, b, risultato));
                
                k++;
            }
        }

        NewTurn();
        GroupsUpdate();
        mb[0].ID = MENU_WORLD_CUP;
        
        // Create array to use for direct elimination
        if (turno == 3) {
            for (i = 0; i < 8; i++) {
                teamarray[i * 2] = groups[i][0];
                teamarray[i * 2 + 1] = groups[7 - i][1];
            }
            nteams = 16;
        }
    }
    else if (turno < 7)
    {
        int i;

        // the sixth round has also the 3rd place final
        if (turno == 6) {
            cup_team_number = nteams;
            isworldcupfinal = 1;
        }
        else
            cup_team_number = nteams / 2;

        EliminazioneDiretta();
        make_setup = TRUE;
        NewTurn();
        isworldcupfinal = 0;
        
        if (turno == 7) {
            int8_t temp = teamarray[2];
            
            teamarray[2] = teamarray[1];
            teamarray[1] = temp;
            
            mb[0].ID = MENU_WORLD_CUP_END;
            
            mp[5 * 4 + 3].Text = mp[4 + 3].Text;
            mp[2 * 4 + 3].Text = mp[3].Text;
            mp[3].Text = mp[4 + 3].Text = NULL;
            
            nteams *= 2;
            
            for (i = 0; i < 4; i++) {
                wcfp[i].Text = teamlist[teamarray[i]].name;
                wcfp[i].Color = colore_team[controllo[teamarray[i]] + 1];
                wcfp[i].Highlight = highlight_team[controllo[teamarray[i]] + 1];
            }
            
            if (controllo[teamarray[0]] >= 0)
                add_achievement("13_worldcup"/*-*/, 100.0);
            
            turno = 0;
            competition = MENU_TEAMS;
        }

        nteams /= 2;
    }
    else
    {
        D(bug("I wouldn't arrive here! (eighth world-wide turn)\n"/*-*/));
    }

    ChangeMenu(MENU_MATCHES);
}

void PlayMatches(int destmenu)
{
    // let's set the final menu we want to go
    final_menu = destmenu;

    switch(competition)
    {
        case MENU_CHALLENGE:
            handle_challenge();
            break;
        case MENU_MATCHES:
            menu[MENU_MATCHES].Title = arcade ? msg_53 : msg_54;

            cup_team_number = nteams / 2;
            EliminazioneDiretta();
            PostMatches();
            break;
        case MENU_LEAGUE:
            if (turno < totale_giornate)
                handle_league();
            break;
        case MENU_WORLD_CUP:
            menu[MENU_MATCHES].Title = msg_56;

            handle_worldcup();
            break;
    }
}

void FreeHighSelection(void)
{
    register int i;
// highlights and teamselection must have the same number of items
    for (i = 0; i < TS_RIGHE * TS_COLONNE; i++) {
        if (hl[i].Text) {
            free(hl[i].Text);
            hl[i].Text = NULL;
        }
    }
}

BOOL HighSelection(int16_t button)
{
    struct Button *b;

    if (button < 0)
        return TRUE;

    b = &actual_menu->Button[button];

    if (button == TS_RIGHE * TS_COLONNE)
    {
        FreeHighSelection();
        ChangeMenu(MENU_HIGHLIGHT);
    }
    else
    {
        char buffer[1024];

        snprintf(buffer, 1024, "%sreplay.%s"/*-*/, TEMP_DIR, b->Text);

        if (!savehigh)
        {
            D(bug("Load highlight %s...\n", b->Text));

            LoadHigh(buffer);
        }
        else
        {
#ifndef MOBILE_VERSION
            FILE *fh;

            if ((fh = fopen(buffer, "rb"/*-*/)))
            {
                ssize_t l;

                freq.Title = msg_228;
                freq.Save = TRUE;

                if (FileRequest(&freq))
                {
                    char *a;

                    fseek(fh, 0, SEEK_END);
                    l = fseek(fh, 0, SEEK_SET);

                    strcpy(buffer, freq.File);

                    if ((a = malloc(l)))
                    {
                        FILE *f2;

                        fread(a, 1, l, fh);

                        if ((f2 = fopen(buffer, "wb"/*-*/)))
                        {
                            fwrite(a, 1, l, f2);
                            fclose(f2);
                        }
                        free(a);
                    }
                }

                fclose(fh);
            }
#else
            // on mobile we don't load/save highlight, we just remove them
            remove(buffer);
            SetHighSelection();
            ChangeMenu(MENU_HIGH_SELECTION);
#endif
        }
    }

    return TRUE;
}

void SetHighSelection(void)
{
    register int i, n = 0;
    char *highs[TS_RIGHE * TS_COLONNE];
    int righe, start;
    DIR *lock;

    // cleanup previous selection
    FreeHighSelection();

    D(bug("Scan dir %s...\n", TEMP_DIR));

    if ((lock = opendir(TEMP_DIR)))
    {
        struct dirent *ent;

        while( (ent = readdir(lock)) != NULL)
        {
            if (!strnicmp(ent->d_name, "replay."/*-*/, 7)) {
                highs[n] = strdup(ent->d_name + 7);
                n++;

                if (n == TS_RIGHE * TS_COLONNE)
                    break;
            }
        }
        closedir(lock);
    }

    righe = n / TS_COLONNE;

    if ((righe * TS_COLONNE) < n)
        righe++;

    start = TS_RIGHE / 2 - righe / 2;

    for (i = 0; i < n; i++) {
        hl[i + start * TS_COLONNE].ID = i;
        hl[i + start * TS_COLONNE].Text = highs[i];
    }
}

void UpdateJoyCfg(int joy)
{
    extern struct Button joycfg_bottoni[];
    int i;

    for (i = 0; i < 7; i++)
    {
        joycfg_bottoni[i * 2 + 1].Text = buttons[joycfg_buttons[joy][i]];
    }
}

void UpdateKeyCfg(void)
{
    extern struct Button keycfg_bottoni[];
    int i;

    for (i = 0; i < 20; i++)
    {
        /* AC: SDL_GetKeyName function wants SDL_Keycode instead of SDL_Scancode.
         * I'm using SDL_GetKeyFromScancode instead of changing query declaration.
         */
        const char *tmp = SDL_GetKeyName(SDL_GetKeyFromScancode(query[i]));
        int j = 0;
        /* Uppercase conversion for ETW font */
        while(*tmp)
        {
            keys_names[i][j] = toupper(*tmp);
            tmp++;
            j++;
        }
        keys_names[i][j] = 0;
        keycfg_bottoni[i * 2 + 1].Text = keys_names[i];
    }

    /* If we have selected a six keys BLUE control */
    if (control[0] == CTRL_KEY_1)
        for (i = 16; i < 20; i++)
            keycfg_bottoni[i * 2 + 1].Text = NULL;

    /* If we have selected a six keys RED control */
    if (control[1] == CTRL_KEY_1)
        for (i = 6; i < 10; i++)
            keycfg_bottoni[i * 2 + 1].Text = NULL;
}

