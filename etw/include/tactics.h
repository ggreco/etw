#ifndef TACTICS_H

#define TACTICS_H

#define SECTORS 12
#define PLAYERS 10
#define SPECIALS 5

/* Specials sono le posizioni speciali per l'editor */

#define CORNER_N 12
#define CORNER_S 13
#define PENALTY  14
#define GOALKICK 15
#define KICKOFF 16

struct pos
{
    uint16_t x, y;
    uint16_t sector;
};

struct tactic
{
    char *Name;
    struct pos Position[2][PLAYERS][SECTORS+SPECIALS];
    uint8_t NameLen;
};

typedef struct tactic tactic_t;

#endif
