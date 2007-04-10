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
#include "squadre.h"
#include "sound.h"
#include "demo.h"
#include "animation.h"
#include "defines.h"
#include "macros.h"

#if defined(__PPC__) && defined(__SASC)
#pragma options align=mac68k
#endif

struct Elemento
{
    AnimObj *immagine;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    int8_t ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
};

struct ElementoMobile
{
    AnimObj *immagine;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    int8_t ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
    int8_t Direzione;   // Direzione di movimento
    int8_t ActualSpeed; // Velocita' del tipo
    int8_t FrameLen; // Quando e' 0 posso cambiare frame
};

struct Arbitro
{
    AnimObj *immagine;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    int8_t ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
    int8_t Direzione;   // Direzione di movimento
    int8_t ActualSpeed; // Velocita' del tipo
    int8_t FrameLen; // Quando e' 0 posso cambiare frame
    WORD Tick, Argomento;
    BOOL Special;
    char *Nome;
    char *Cognome; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
    int8_t NameLen; // Lunghezza in int8_ts del cognome (per text) 
    int8_t Comando;  // Se ci sono falli o simili...
    int8_t velocita;
    int8_t abilita;
    int8_t recupero;
    int8_t cattiveria;
};

// Molto simile all'arbitro...

struct GuardaLinee
{
    AnimObj *immagine;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    char ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
    char Direzione;   // Direzione di movimento
    char ActualSpeed; // Velocita' del tipo
    char FrameLen; // Quando e' 0 posso cambiare frame
    WORD Tick, Argomento;
    BOOL Special;
    char *Nome;
    char *Cognome; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
    char NameLen; // Lunghezza in chars del cognome (per text) 
    char Comando;  // Se ci sono falli o simili...
};

struct Pallone
{
    AnimObj *immagine;
    struct Giocatore *gioc_palla;
    struct Squadra *sq_palla;
    WORD world_x, world_y;
    WORD delta_x, delta_y;
    int8_t ToTheTop, ThisQuota, Stage, TipoTiro;
    int8_t ActualFrame, MaxQuota, SpeedUp, Rimbalzi;
    int8_t InGioco, Hide;
    int8_t velocita;
    uint8_t Direzione;
    int8_t quota;
    int8_t settore;
};

struct Portiere
{
    AnimObj *immagine;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame;
    int8_t ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
    int8_t Direzione;   // Direzione di movimento
    int8_t ActualSpeed; // Velocita' del tipo
    int8_t FrameLen; // Quando e' 0 posso cambiare frame
    WORD Tick; 
    char *Nome;
    char *Cognome; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
    struct Squadra *squadra;
    int8_t NameLen; // Lunghezza in chars del cognome (per text) 
    int8_t SNum;
    BOOL Ammonito, Special, FirePressed;
    WORD SpecialData, TimePress;
    int8_t Numero;
    int8_t velocita;
    int8_t Parata;
    int8_t Attenzione;
};

struct Giocatore
{
    AnimObj *immagine;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    int8_t ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
    int8_t Direzione;   // Direzione di movimento
    int8_t ActualSpeed; // Velocita' del tipo
    int8_t FrameLen; // Quando e' 0 posso cambiare frame
    WORD Tick; 
    char *Nome;
    char *Cognome; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
    struct Squadra *squadra;
    int8_t NameLen; // Lunghezza in chars del cognome (per text) 
    int8_t GNum;
    BOOL Ammonito, Special, FirePressed;
    WORD SpecialData, TimePress;
    int8_t Numero;
    int8_t Velocita;
    int8_t Contrasto;
    int8_t Tiro;
    int8_t Durata;
    int8_t Resistenza;
    int8_t Prontezza;
    int8_t settore;
    int8_t Creativita;
    int8_t Tecnica;
    int8_t Posizioni;
    int8_t SNum;
    BOOL Controlled, Marker;
    WORD WaitForControl;
    int8_t Comando, Argomento, CA[6];  // Relativi ai comandi
    uint8_t OldStat, ArcadeEffect;
    WORD ArcadeCounter; // Per la gestione dell'arcade
};

struct Squadra
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
    struct Portiere portiere;
    struct Giocatore giocatore[10];
    struct Tactic *tattica;
    ULONG TempoPossesso;
    struct Giocatore *attivo;
    AnimObj *Marker;
    WORD Marker_X, Marker_Y, MarkerFrame;
    BOOL MarkerOnScreen;
    bitmap NomeAttivo;
    int8_t gioco_ruolo;
    char Nome[52];
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

struct Partita
{
    struct Pallone palla;
    struct Arbitro arbitro;
    struct Squadra *squadra[2];
    bitmap result;
    struct Giocatore *player_injuried;
    AnimObj *extras; // Ci metto bandierine, fotografi, poliziotti...
    struct Squadra *possesso;
    ULONG TempoPassato;
    LONG show_panel, show_time;
    WORD check_sector, shotheight[SHOT_LENGTH], flash_pos;
    BOOL goal, sopra_rete, flash_mode, doing_shot, mantieni_distanza, penalty_onscreen;
    WORD arcade_counter, marker_x, marker_y, penalty_counter, adder;
    uint8_t TabCounter, result_len, last_touch;
    int8_t arcade_on_field, RiservaAttuale;
//    struct GuardaLinee guardalinee[2]; tolti per non modificare la struttura
};

struct DOggetto
{
    WORD X, Y, Range;
    BOOL Collisione;
    BYTE *Frame;
};

typedef struct GuardaLinee GuardaLinee;
typedef struct Partita Partita;
typedef struct Squadra Squadra;
typedef struct Giocatore Giocatore;
typedef struct Pallone Pallone;
typedef struct Tactic Tattica;
typedef struct Elemento Oggetto;

// new defines used for better internationalization...
typedef struct Arbitro Refree;
typedef struct GuardaLinee Linesman;
typedef struct Giocatore Player;
typedef struct Squadra Team;
typedef struct Partita Match;
typedef struct Elemento Object;
typedef struct Pallone Ball;
typedef struct Tactic Tactic;

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

