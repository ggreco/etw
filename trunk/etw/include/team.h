#ifndef TEAM_H
#define TEAM_H

#include "player.h"

typedef struct team
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
} team_t;

#endif
