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
	WORD world_x,world_y;
	BOOL OnScreen;
	WORD AnimType,AnimFrame; // Animazione in uso e frame a cui si e'
	char ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
};

struct ElementoMobile
{
	AnimObj *immagine;
	WORD world_x,world_y;
	BOOL OnScreen;
	WORD AnimType,AnimFrame; // Animazione in uso e frame a cui si e'
	char ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
	char Direzione;   // Direzione di movimento
	char ActualSpeed; // Velocita' del tipo
	char FrameLen; // Quando e' 0 posso cambiare frame
};

struct Arbitro
{
	AnimObj *immagine;
	WORD world_x,world_y;
	BOOL OnScreen;
	WORD AnimType,AnimFrame; // Animazione in uso e frame a cui si e'
	char ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
	char Direzione;   // Direzione di movimento
	char ActualSpeed; // Velocita' del tipo
	char FrameLen; // Quando e' 0 posso cambiare frame
	WORD Tick,Argomento;
	BOOL Special;
	char *Nome;
	char *Cognome; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
	char NameLen; // Lunghezza in chars del cognome (per text) 
	char Comando;  // Se ci sono falli o simili...
	char velocita;
	char abilita;
	char recupero;
	char cattiveria;
};

// Molto simile all'arbitro...

struct GuardaLinee
{
	AnimObj *immagine;
	WORD world_x,world_y;
	BOOL OnScreen;
	WORD AnimType,AnimFrame; // Animazione in uso e frame a cui si e'
	char ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
	char Direzione;   // Direzione di movimento
	char ActualSpeed; // Velocita' del tipo
	char FrameLen; // Quando e' 0 posso cambiare frame
	WORD Tick,Argomento;
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
	WORD world_x,world_y;
	WORD delta_x,delta_y;
	BYTE ToTheTop,ThisQuota,Stage,TipoTiro;
	BYTE ActualFrame,MaxQuota,SpeedUp,Rimbalzi;
	BOOL InGioco,Hide;
	char velocita;
	UBYTE Direzione;
	char quota;
	char settore;
};

struct Portiere
{
	AnimObj *immagine;
	WORD world_x,world_y;
	BOOL OnScreen;
	WORD AnimType,AnimFrame;
	char ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
	char Direzione;   // Direzione di movimento
	char ActualSpeed; // Velocita' del tipo
	char FrameLen; // Quando e' 0 posso cambiare frame
	WORD Tick; 
	char *Nome;
	char *Cognome; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
	struct Squadra *squadra;
	char NameLen; // Lunghezza in chars del cognome (per text) 
	char SNum;
	BOOL Ammonito,Special,FirePressed;
	WORD SpecialData,TimePress;
	char Numero;
	char velocita;
	char Parata;
	char Attenzione;
};

struct Giocatore
{
	AnimObj *immagine;
	WORD world_x,world_y;
	BOOL OnScreen;
	WORD AnimType,AnimFrame; // Animazione in uso e frame a cui si e'
	char ObjectType;  // Tipo di oggetto, fin qui ci devo arrivare!
	char Direzione;   // Direzione di movimento
	char ActualSpeed; // Velocita' del tipo
	char FrameLen; // Quando e' 0 posso cambiare frame
	WORD Tick; 
	char *Nome;
	char *Cognome; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
	struct Squadra *squadra;
	char NameLen; // Lunghezza in chars del cognome (per text) 
	char GNum;
	BOOL Ammonito,Special,FirePressed;
	WORD SpecialData,TimePress;
	char Numero;
	char Velocita;
	char Contrasto;
	char Tiro;
	char Durata;
	char Resistenza;
	char Prontezza;
	char settore;
	char Creativita;
	char Tecnica;
	char Posizioni;
	char SNum;
	BOOL Controlled,Marker;
	WORD WaitForControl;
	BYTE Comando,Argomento,CA[6];  // Relativi ai comandi
	UBYTE OldStat,ArcadeEffect;
	WORD ArcadeCounter; // Per la gestione dell'arcade
};

struct Squadra
{
	char TPossesso;
	char Reti;
	char Falli;
	char Ammonizioni;
	char Espulsioni;
	char Tiri;
	char Rigori;
	char Corner;
	char Sostituzioni;
	char Possesso;
	char Schema;
	char Joystick;
	struct Portiere portiere;
	struct Giocatore giocatore[10];
	struct Tactic *tattica;
	ULONG TempoPossesso;
	struct Giocatore *attivo;
	AnimObj *Marker;
	WORD Marker_X,Marker_Y,MarkerFrame;
	BOOL MarkerOnScreen;
	bitmap NomeAttivo;
	BOOL gioco_ruolo;
	char Nome[52];
	BOOL MarkerRed;
	UBYTE ArcadeEffect,NumeroRiserve;
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
	LONG show_panel,show_time;
	WORD check_sector,shotheight[SHOT_LENGTH],flash_pos;
	BOOL goal,sopra_rete,flash_mode,doing_shot,mantieni_distanza,penalty_onscreen;
	WORD arcade_counter,marker_x,marker_y,penalty_counter,adder;
	UBYTE TabCounter,result_len,last_touch;
	BYTE arcade_on_field,RiservaAttuale;
//	struct GuardaLinee guardalinee[2]; tolti per non modificare la struttura
};

struct DOggetto
{
	WORD X,Y,Range;
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

#include "externs.h"

// Definizioni del compilatore


#if defined(__SASC) && !defined(__PPC__)
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
	#define max(a,b) ( (a)<(b) ? (b) : (a))
#endif
	#ifndef MAX
		#define MAX(a,b) max(a,b)
	#endif
#endif

