#ifndef PLAYER_H
#define PLAYER_H


typedef struct keeper
{
    anim_t *anim;
    int16_t world_x, world_y;
    BOOL OnScreen;
    int16_t AnimType, AnimFrame;
    int8_t otype;  // Tipo di oggetto, fin qui ci devo arrivare!
    int8_t dir;   // Direzione di movimento
    int8_t ActualSpeed; // Velocita' del tipo
    int8_t FrameLen; // Quando e' 0 posso cambiare frame
    int16_t Tick;
    char *name;
    char *surname; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
    struct team *team;
    uint8_t NameLen; // Lunghezza in chars del cognome (per text)
    int8_t SNum;
    BOOL Ammonito, Special, FirePressed;
    int16_t SpecialData, TimePress;
    int8_t number;
    int8_t velocita;
    int8_t Parata;
    int8_t Attenzione;
} keeper_t;

typedef struct player
{
    anim_t *anim;
    int16_t world_x, world_y;
    BOOL OnScreen;
    int16_t AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    int8_t otype;  // Tipo di oggetto, fin qui ci devo arrivare!
    int8_t dir;   // Direzione di movimento
    int8_t ActualSpeed; // Velocita' del tipo
    int8_t FrameLen; // Quando e' 0 posso cambiare frame
    int16_t Tick;
    char *name;
    char *surname; // Subpuntatore a Nome (che contiene Nome e Cognome di fila).
    struct team *team;
    uint8_t NameLen; // Lunghezza in chars del cognome (per text)
    int8_t GNum;
    BOOL Ammonito, Special, FirePressed;
    int16_t SpecialData, TimePress;
    int8_t number, speed, tackle, Tiro;
    int8_t Durata, stamina, quickness, sector;
    int8_t creativity, technique, Posizioni, SNum;
    BOOL Controlled, Marker;
    int16_t WaitForControl;
    int8_t Comando, Argomento, CA[6];  // Relativi ai comandi
    uint8_t OldStat, ArcadeEffect;
    int16_t ArcadeCounter; // Per la gestione dell'arcade
} player_t;

#endif
