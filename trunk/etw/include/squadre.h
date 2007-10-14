#ifndef SQUADRE_H
#define SQUADRE_H

#if defined(__PPC__) && defined(__SASC)
#pragma options align=mac68k
#endif

// Flags dei giocatori...

#define P_DIFESA    1
#define P_CENTRO    2
#define P_ATTACCO    4
#define P_DESTRO    8
#define P_MANCINO    16
#define P_PELLE_NERA        32

// Flags delle squadre

#define SERIE_B    1
#define SERIE_C    2
#define SERIE_D 4

#define NO_MANAGER    0
#define MANAGER_ONLY    1
#define PLAYERMANAGER    2
#define ROLEPLAYER    3

#define CAMP_CUP 0
#define CAMP_LEAGUE 1
#define CAMP_WORLDCUP 2
#define CAMP_CHAMPIONSLEAGUE 3

struct Maglia
{
    uint8_t Tipo;
    uint8_t Colore0;
    uint8_t Colore1;
    uint8_t Colore2;
};

struct Giocatore_Disk
{
    char Nome[20];
    char Cognome[20];
    uint32_t valore;
    uint8_t Numero, Velocita, Contrasto, Tiro;
    uint8_t Durata, Resistenza, Prontezza, Nazionalita;
    uint8_t Creativita, Tecnica;
    uint8_t Eta, Infortuni, Ammonizioni, Posizioni;
};

struct Portiere_Disk
{
    char Nome[20];
    char Cognome[20];
    uint32_t valore;
    uint8_t Numero, Velocita, Parata, Attenzione;
    uint8_t Nazionalita, Eta, Infortuni, Flags;
};

struct Squadra_Disk
{
    uint32_t disponibilita;
    uint8_t NumeroGiocatori, NumeroPortieri, Nazione, Flags;
    struct Maglia maglie[2];
    char Tattiche[3][16];
    char nome[52];
    char allenatore[52];
    struct Portiere_Disk portiere[3];
    struct Giocatore_Disk giocatore[21];    
};

struct Campionato_Disk
{
    char Nome[64];
    char Tipo, Scontri;
    char Vittoria,Pareggio,Sconfitta; // Punti assegnati nei vari casi
    char nteams;
};

struct Controlled_Disk
{
    WORD ManagerType;
    UBYTE ControlType;
    UBYTE Joystick;
    UBYTE Player;
};

struct Storia
{
    UBYTE Posizione,Punti,Vittorie,Sconfitte,Pareggi,GolFatti,GolSubiti;
    BYTE TurnoEuropea;
    BYTE TurnoNazionale;
};

struct Manager_Disk
{
    WORD Punteggio;
    struct Storia storia[16];
};

struct DatiSquadra_Disk
{
    unsigned short Punti; // Punti della squadra, negativo significa TURNO.
    BOOL Controllata; // Punteggio manager, solo se "Career"
// Classifica
    UBYTE GolFatti,GolSubiti,Giocate;
    UBYTE Vittorie,Pareggi,Sconfitte;
    BYTE TurnoEuropea;
    BYTE TurnoNazionale;
};

void ReadTeam(FILE *, struct Squadra_Disk *);
void WriteTeam(FILE *, struct Squadra_Disk *);

#endif

