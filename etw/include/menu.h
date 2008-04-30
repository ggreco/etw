/* Li gestisco tramite makefile!
 *
 * #define DEBUG_DISABLED
 * #define CD_VERSION
 */
#ifndef ETW_MENU_H
#define ETW_MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mytypes.h"
#include "lists.h"
#include "os_defs.h"
#include "version.h"
#include "gfx.h"
// #include "anim.h"
#include "tactics.h"
#include "sound.h"

#include "freq.h"
#include "squadre.h"
#include "mymacros.h"
#include "network.h"

#define RangeRand(v) (rand()%v)

#define BIG_FONT "newgfx/eat2.font"
#define TITLE_FONT "newgfx/eatinfo.font"
#define SMALL_FONT "newgfx/eat.font"

#define FIRST_ARCADE 3

#define NUMERO_CAMPI 6
#define NUMERO_LOOPS 15
#define ARCADE_TEAMS 10

#ifndef CD_VERSION
    #define NUMERO_INTRO 4
    #define NUMERO_SUONI_MENU ARCADE_TEAMS+1+FIRST_ARCADE
#else
    #define NUMERO_SUONI_MENU ARCADE_TEAMS+5+FIRST_ARCADE
    #define NUMERO_INTRO 6
#endif


#define FAKE_TEAM 127

#define NESSUN_BOTTONE -1
#define ID_QUIT_GAME -127
#define MAX_PLAYERS 2

/* AC: some day we need to reorder this figures. */
#define MENU_MAIN_MENU    0
#define MENU_PREFS    1
#define MENU_HIGHLIGHT    2
#define MENU_GAME_START 4
#define MENU_TEAMS    3
#define MENU_ARCADE    5
#define MENU_SIMULATION 6
#define MENU_CAREER    7
#define MENU_TRAINING    8
#define MENU_TEAM_SELECTION 9
#define MENU_SCORES    10
#define MENU_TEAM_SETTINGS 11
#define MENU_GAME_PREFS 12
#define MENU_VIDEO_PREFS 13
#define MENU_AUDIO_PREFS 14
#define MENU_WORLD_CUP 15
#define MENU_MATCHES 16
#define MENU_LEAGUE 17
#define MENU_SELECTION_TYPE 18
#define MENU_ARCADE_SELECTION 19
#define MENU_CHALLENGE 20
#define MENU_WORLD_CUP_END 21
#define MENU_MATCH_RESULT 22
#define MENU_VIEW_SCORES 23
#define MENU_HIGH_SELECTION 24
#define MENU_SYSTEM_PREFS 25
#define MENU_KEYCFG 26
#define MENU_JOYCFG 27
#define MENUS MENU_JOYCFG+1

#define NUMERO_SFONDI 12

// Defines per teamselection

#define TS_RIGHE    16
#define TS_COLONNE    4


struct EasyStruct
{
    int es_Size, es_Flags;
    char *es_Title,*es_TextFormat,*es_GadgetFormat;
};

struct Bottone
{
    WORD X1,Y1,X2,Y2,ID;
    UBYTE Colore,Highlight;
    char *Testo;
};

struct Match
{
    UBYTE t1,t2;
};

struct GfxMenu
{
    char *Titolo;
    struct Bottone *Bottone;
    struct Bottone *Pannello;
    WORD NumeroBottoni,NumeroPannelli,ID;
    WORD X,Y;
    BYTE Immagine;
    UBYTE Colore,Highlight;
    APTR SpecialFunction;
};

// Palette pens of the MENUs

#ifndef SOUND_C

#define P_VERDE0 0
#define P_BIANCO 1
#define P_GRIGIO4 2
#define P_VERDE1 3
#define P_NERO 4
#define P_GRIGIO3 5
#define P_BLU2   6
#define P_ROSSO1 7
#define P_BLU1 8
#define P_ROSSO0 9
#define P_GRIGIO2 10
#define P_ARANCIO 11
#define P_GIALLO 12
#define P_GRIGIO0 13
#define P_GRIGIO1 14
#define P_BLU0 15

#endif

#define COLORE_UNSELECTED P_GRIGIO1
#define COLORE_TEAM_A P_BLU1
#define COLORE_TEAM_B P_ROSSO1
#define COLORE_COMPUTER P_GRIGIO0

#define COLORE_TATTICA_NON_SELEZIONATA P_GRIGIO1
#define COLORE_TATTICA_SELEZIONATA P_BLU2

// Tipi di controlli

#define CTRL_JOY    0
#define CTRL_JOYPAD    1
#define CTRL_JOY2B    2
#define CTRL_KEY_1    3
#define CTRL_KEY_2    4

#define CONTROLS    (CTRL_KEY_2+2)

// Per detail_level (bits)

#define USA_RADAR    1
#define USA_ARBITRO    2
#define USA_POLIZIOTTI    4
#define USA_FOTOGRAFI    8
#define USA_NOMI    16
#define USA_RISULTATO    32
#define USA_GUARDALINEE    64

#ifdef __SASC
    #define min(a,b) __builtin_min(a,b)
    #define max(a,b) __builtin_max(a,b)
#else
    #ifndef min
        #define min(a,b) ((a)>(b) ? (b) : (a))
    #endif

    #ifndef MIN
        #define MIN(a,b) min(a,b)
    #endif

    #ifndef max
        #define max(a,b) ((a)<(b) ? (b) : (a) )
    #endif

    #ifndef MAX
        #define MAX(a,b) max(a,b)
    #endif
#endif

#include "menu_externs.h"

#endif
