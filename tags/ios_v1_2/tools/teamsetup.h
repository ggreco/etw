#ifndef SQUADRE_H
#define SQUADRE_H

#pragma pack(push, 2)

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
    uint8_t number, speed, tackle, shot;
    uint8_t Durata, stamina, quickness, nation;
    uint8_t creativity, technique;
    uint8_t age, injury, Ammonizioni, role;
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
    uint8_t type, matches;
    uint8_t win, draw, loss; // Points awarded in each case
    uint8_t nteams;
};

#pragma pack(pop)

#endif

