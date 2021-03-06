#ifndef BALL_H
#define BALL_H

typedef struct ball
{
    anim_t *anim;
    struct player *gioc_palla;
    struct team *sq_palla;
    int16_t world_x, world_y;
    int16_t delta_x, delta_y;
    int8_t ToTheTop, ThisQuota, Stage, TipoTiro;
    int8_t ActualFrame, MaxQuota, SpeedUp, Rimbalzi;
    int8_t InGioco, Hide;
    int8_t velocita;
    uint8_t dir;
    int8_t quota;
    int8_t sector;
    int8_t special;
} ball_t;

#endif
