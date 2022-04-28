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

struct jersey
{
    uint8_t type, color0, color1, color2;
};

struct player_disk
{
    char name[20];
    char surname[20];
    uint32_t value; /* unused? */
    uint8_t number, speed, tackle, Tiro;
    uint8_t Durata, stamina, quickness, nation;
    uint8_t creativity, technique;
    uint8_t age, injury, Ammonizioni, Posizioni;
};

struct keeper_disk
{
    char name[20];
    char surname[20];
    uint32_t value; /* unused? */
    uint8_t number, speed, Parata, Attenzione;
    uint8_t nation, age, injury, Flags;
};

struct team_disk
{
    uint32_t disponibilita;
    uint8_t nplayers, nkeepers, nation, Flags;
    struct jersey jerseys[2];
    char tactics[3][16];
    char name[52];
    char allenatore[52];
    struct keeper_disk keepers[3];
    struct player_disk players[21];    
};

struct championship_disk
{
    char name[64];
    uint8_t type, Scontri;
    uint8_t win, draw, loss; // Points awarded in each case
    uint8_t nteams;
};

struct control_disk
{
    int16_t ManagerType;
    uint8_t ControlType;
    uint8_t Joystick;
    uint8_t Player;
};

struct Storia
{
    uint8_t Posizione, Punti, Vittorie, Sconfitte, Pareggi, GolFatti, GolSubiti;
    int8_t TurnoEuropea;
    int8_t TurnoNazionale;
};

struct manager_disk
{
    int16_t Punteggio;
    struct Storia storia[16];
};

struct teamstats_disk
{
    unsigned short Punti; // Punti della squadra, negativo significa TURNO.
    int Controllata; // Punteggio manager, solo se "Career"
// Classifica
    uint8_t GolFatti, GolSubiti, Giocate;
    uint8_t Vittorie, Pareggi, Sconfitte;
    int8_t TurnoEuropea;
    int8_t TurnoNazionale;
};

void ReadTeam(FILE *, struct team_disk *);
void WriteTeam(FILE *, struct team_disk *);

#endif

