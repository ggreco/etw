#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Questo simbolo disabilita le info di debug...

/*  #define DEBUG_DISABLED
 *  #define CD_VERSION
 *  #define DEMOVERSION
 *
 *  Li gestisco in modo piu' pulito tramite makefile!
 */

#include "version.h"
#include "mydebug.h"
#include "os_defs.h"
#include "mytypes.h"
#include "lists.h"
#include "structs.h"
#include "chunky.h"
#include "gfx.h"
#include "tactics.h"
#include "teamsetup.h"
#include "sound.h"
#include "demo.h"
#include "animation.h"
#include "defines.h"
#include "macros.h"

#if defined(__PPC__) && defined(__SASC)
#pragma options align=mac68k
#endif

struct object
{
    anim_t *anim;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    int8_t otype;  // Tipo di oggetto, fin qui ci devo arrivare!
};

struct movingobject
{
    anim_t *anim;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    int8_t otype;  // Tipo di oggetto, fin qui ci devo arrivare!
    int8_t dir;   // Direzione di movimento
    int8_t ActualSpeed; // Velocita' del tipo
    int8_t FrameLen; // Quando e' 0 posso cambiare frame
};

struct referee
{
    anim_t *anim;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    int8_t otype;  // Tipo di oggetto, fin qui ci devo arrivare!
    int8_t dir;   // Direzione di movimento
    int8_t ActualSpeed; // Velocita' del tipo
    int8_t FrameLen; // Quando e' 0 posso cambiare frame
    WORD Tick, Argomento;
    BOOL Special;
    char *name;
    char *surname; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
    uint8_t NameLen; // Lunghezza in chars del cognome (per text)
    int8_t Comando;  // Se ci sono falli o simili...
    int8_t velocita;
    int8_t abilita;
    int8_t recupero;
    int8_t cattiveria;
};

// Molto simile all'arbitro...

struct linesman
{
    anim_t *anim;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    char otype;  // Tipo di oggetto, fin qui ci devo arrivare!
    uint8_t dir;   // Direzione di movimento
    char ActualSpeed; // Velocita' del tipo
    char FrameLen; // Quando e' 0 posso cambiare frame
    WORD Tick, Argomento;
    BOOL Special;
    char *name;
    char *surname; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
    uint8_t NameLen; // Lunghezza in chars del cognome (per text)
    int8_t Comando;  // Se ci sono falli o simili...
};

struct ball
{
    anim_t *anim;
    struct player *gioc_palla;
    struct team *sq_palla;
    WORD world_x, world_y;
    WORD delta_x, delta_y;
    int8_t ToTheTop, ThisQuota, Stage, TipoTiro;
    int8_t ActualFrame, MaxQuota, SpeedUp, Rimbalzi;
    int8_t InGioco, Hide;
    int8_t velocita;
    uint8_t dir;
    int8_t quota;
    int8_t sector;
};

struct keeper
{
    anim_t *anim;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame;
    int8_t otype;  // Tipo di oggetto, fin qui ci devo arrivare!
    int8_t dir;   // Direzione di movimento
    int8_t ActualSpeed; // Velocita' del tipo
    int8_t FrameLen; // Quando e' 0 posso cambiare frame
    WORD Tick;
    char *name;
    char *surname; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
    struct team *team;
    uint8_t NameLen; // Lunghezza in chars del cognome (per text)
    int8_t SNum;
    BOOL Ammonito, Special, FirePressed;
    WORD SpecialData, TimePress;
    int8_t number;
    int8_t velocita;
    int8_t Parata;
    int8_t Attenzione;
};

struct player
{
    anim_t *anim;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    int8_t otype;  // Tipo di oggetto, fin qui ci devo arrivare!
    int8_t dir;   // Direzione di movimento
    int8_t ActualSpeed; // Velocita' del tipo
    int8_t FrameLen; // Quando e' 0 posso cambiare frame
    WORD Tick;
    char *name;
    char *surname; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
    struct team *team;
    uint8_t NameLen; // Lunghezza in chars del cognome (per text)
    int8_t GNum;
    BOOL Ammonito, Special, FirePressed;
    WORD SpecialData, TimePress;
    int8_t number, speed, tackle, Tiro;
    int8_t Durata, stamina, quickness, sector;
    int8_t creativity, technique, Posizioni, SNum;
    BOOL Controlled, Marker;
    WORD WaitForControl;
    int8_t Comando, Argomento, CA[6];  // Relativi ai comandi
    uint8_t OldStat, ArcadeEffect;
    WORD ArcadeCounter; // Per la gestione dell'arcade
};

struct team
{
    int8_t TPossesso;
    int8_t Reti;
    int8_t Falli;
    int8_t Ammonizioni;
    int8_t Espulsioni;
    int8_t Tiri;
    int8_t Rigori;
    int8_t Corner;
    int8_t Sostituzioni;
    int8_t Possesso;
    int8_t Schema;
    int8_t Joystick;
    struct keeper keepers;
    struct player players[10];
    struct tactic *tactic;
    ULONG TempoPossesso;
    struct player *attivo;
    anim_t *Marker;
    WORD Marker_X, Marker_Y, MarkerFrame;
    BOOL MarkerOnScreen;
    uint8_t *NomeAttivo;
    int8_t gioco_ruolo;
    char name[52];
    BOOL MarkerRed;
    uint8_t ArcadeEffect, NumeroRiserve;
    WORD ArcadeCounter;
};

struct Animazione
{
    char FrameLen;
    char Frames;
    WORD *Frame;
};

struct game
{
    struct ball ball;
    struct referee referee;
    struct team *team[2];
    uint8_t * result;
    struct player *player_injuried;
    anim_t *extras; // Ci metto bandierine, fotografi, poliziotti...
    struct team *possesso;
    ULONG TempoPassato;
    LONG show_panel, show_time;
    WORD check_sector, shotheight[SHOT_LENGTH], flash_pos;
    BOOL goal, sopra_rete, flash_mode, doing_shot, mantieni_distanza, penalty_onscreen;
    WORD arcade_counter, marker_x, marker_y, penalty_counter, adder;
    uint8_t TabCounter, result_len, last_touch;
    int8_t arcade_on_field, RiservaAttuale;
//    struct linesman linesman[2]; tolti per non modificare la struttura
};

struct DOggetto
{
    int X, Y, Range;
    BOOL Collisione;
    int8_t *Frame;
};

// new defines used for better internationalization...
typedef struct referee referee_t;
typedef struct linesman linesman_t;
typedef struct player player_t;
typedef struct keeper keeper_t;
typedef struct team team_t;
typedef struct game game_t;
typedef struct object object_t;
typedef struct movingobject movingobject_t;
typedef struct ball ball_t;

#include "externs.h"

// compiler defines... this is a bit a mess ATM


#if defined(__SASC) && !defined(__PPC__)
#   define min(a, b) __builtin_min(a, b)
#   define max(a, b) __builtin_max(a, b)
#else
#   ifndef min
#       define min(a, b) ((a)>(b) ? (b) : (a))
#   endif
#   ifndef MIN
#       define MIN(a, b) min(a, b)
#   endif
#   ifndef max
#       define max(a, b) ( (a)<(b) ? (b) : (a))
#   endif
#   ifndef MAX
#       define MAX(a, b) max(a, b)
#   endif
#endif

