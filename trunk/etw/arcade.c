#include "eat.h"

#define NUM_ARCADE_FRAMES 10

WORD arcade_frame_sequence[NUM_ARCADE_FRAMES]={0,1,1,2,2,2,2,2,1,1};

struct pos bonus_pos[]=
{
    {1100*8,90*8},
    {100*8,180*8},
    {300*8,250*8},
    {500*8,310*8},
    {700*8,380*8},
    {900*8,460*8},
};

void RemoveArcadeEffect(player_t *g,UBYTE effect)
{
    switch(effect)
    {
        case ARCADE_SPEED:
            g->speed=g->OldStat;
            if(g==pl->gioc_palla)
                g->speed-=3;
            break;
        case ARCADE_FREEZE:
            {
                int i;

                g->team->ArcadeEffect=NESSUN_COMANDO;

                for(i=0;i<10;i++)
                {
                    if(g->team->players[i].ArcadeEffect==ARCADE_FREEZE)
                    {
                        g->team->players[i].Comando=NESSUN_COMANDO;
                        g->team->players[i].ArcadeEffect=NESSUN_COMANDO;
                    }
                }
            }
            break;
    }
    g->ArcadeEffect=NESSUN_COMANDO;
}

void AddArcadeEffect(player_t *g,UBYTE effect)
{
    if(g->ArcadeEffect)
        RemoveArcadeEffect(g,g->ArcadeEffect);

    switch(effect)
    {
        case ARCADE_GLUE:
            g->ArcadeCounter=400; // 8 secondi di durata;
            g->ArcadeEffect=effect;    
            break;
        case ARCADE_SPEED:
            g->OldStat=g->speed;
            if(g==pl->gioc_palla)
                g->OldStat+=3;

            g->speed=9;
            g->ArcadeCounter=500; // 10 secondi di durata;
            g->ArcadeEffect=effect;                
            break;
        case ARCADE_CANNON:
            g->ArcadeCounter=500; // 10 secondi di durata;
            g->ArcadeEffect=effect;
            break;
        case ARCADE_FREEZE:
            {
                int i;
                team_t *s=p->team[g->SNum^1];

                s->ArcadeEffect=ARCADE_FREEZE;
                s->ArcadeCounter=400; // 8 secondi di durata.

                for(i=0;i<10;i++)
                {
                    if(s->players[i].AnimType!=GIOCATORE_ESPULSO)
                    {
                        s->players[i].Comando=STAI_FERMO;
                        s->players[i].ArcadeEffect=ARCADE_FREEZE;
                        s->players[i].ArcadeCounter=300;
                        s->players[i].ActualSpeed=0;
                    }
                }
            }
            break;
    }
}

void GetArcadeEffect(player_t *g, object_t *o)
{
    WORD k=(o->AnimType-20)/3;

    switch(k)
    {
        case 0:
        case 1:            
        case 3:
            PlayBackSound(sound[FIRST_ARCADE_SOUND+k]);
        case 2: // Per il cannone niente suono
            AddArcadeEffect(g,k+1);
            break;
        default:
            D(bug("Errore, ArcadeObject inesistente!\n"));
    }

    o->world_x=-1000;
    p->arcade_on_field--;
}

void HandleArcade(void)
{
    register int i;

// Gestisco le animazioni...

    for(i=0;i<MAX_ARCADE_ON_FIELD;i++)
    {
        if(bonus[i]->OnScreen)
        {
/*
            bonus[i]->FrameLen--;

            if(bonus[i]->FrameLen<=0)
            {
                bonus[i]->FrameLen=3;
*/
                bonus[i]->AnimFrame++;

                if(bonus[i]->AnimFrame>=NUM_ARCADE_FRAMES)
                {
                    bonus[i]->AnimFrame=0;
                }

                ChangeAnimObj(bonus[i]->anim,bonus[i]->AnimType+arcade_frame_sequence[bonus[i]->AnimFrame]);

/*            }
*/
        }
    }

// Gestisco il repop...

    p->arcade_counter++;

    if(p->arcade_counter>ARCADE_REPOP_TIME)
    {
        p->arcade_counter=0;

        if(p->arcade_on_field<MAX_ARCADE_ON_FIELD)
        {
            for(;;)
            {
                i=GetTable();

                if(i<MAX_ARCADE_ON_FIELD && bonus[i]->world_x<0)
                {
                    PlayBackSound(sound[AS_REPOP]);
                    p->arcade_on_field++;

                    bonus[i]->world_x=bonus_pos[GetTable()].x;
                    bonus[i]->world_y=bonus_pos[GetTable()].y;
                    break;
                }
            }
        }
    }
}
