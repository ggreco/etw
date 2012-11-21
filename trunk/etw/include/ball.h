#ifndef BALL_H
#define BALL_H

typedef struct ball
{
    anim_t *anim;
    struct player *gioc_palla;
    struct team *sq_palla;
    WORD world_x, world_y;
    WORD delta_x, delta_y;
    int8_t ToTheTop, ThisQuota, Stage, TipoTiro;
    int8_t ActualFrame, MaxQuota, SpeedUp, Rimbalzi;
    int8_t InGioco, Hide;
    int8_t velocita;
    uint8_t dir;
    int8_t quota;
    int8_t sector;
} ball_t;

#endif
