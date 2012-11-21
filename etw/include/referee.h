#ifndef REFEREE_H
#define REFEREE_H

typedef struct referee
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
} referee_t;

// Almost the same as the referee...

typedef struct linesman
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
} linesman_t;

#endif
