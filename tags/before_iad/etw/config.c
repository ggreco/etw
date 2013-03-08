#include "eat.h"

extern char team_name[2][16];
char palette[24]="gfx/eat16.col";
char fieldname[24]="gfx/pitcha.gfx";
char shirt[2][24]={"gfx/playera.obj","gfx/playera.obj"};
int highlight_size=0;

extern BOOL wb_game;
extern int32_t display_id;

BOOL joyonly=FALSE, tutorial = TRUE,
    arcade_teams=FALSE,first_half=TRUE,extratime=FALSE,
    final=FALSE,use_width=FALSE,use_height=FALSE,first_kickoff=TRUE,
    friendly=FALSE,use_direct=FALSE,use_offside=TRUE; // Da mettere false di default
UBYTE team_a=0,team_b=0;
int FIXED_SCALING_WIDTH=320,FIXED_SCALING_HEIGHT=256;

struct MyFastScaleArgs *scaling=NULL;

int8_t player_type[4]={1,
#ifdef DEMOVERSION
-1,
#else
0,
#endif
-1,-1};

int8_t role[4], current_field=0, arcade_team[2], starting_team;

/*
extern int32_t display_id = 0, situation_time = 0;
extern uint32_t ahimode = 0;
*/

extern void *screen;

void read_config(void)
{
    first_half=TRUE;
    extratime=FALSE;
    first_kickoff=TRUE;

    //players=2;

    if(use_scaling&&!scaling)
        scaling=malloc(sizeof(struct MyFastScaleArgs));

    if(training)
        detail_level&=~(USA_RISULTATO|USA_ARBITRO|USA_GUARDALINEE);
}
