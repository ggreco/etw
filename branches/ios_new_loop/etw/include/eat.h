#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Questo simbolo disabilita le info di debug...

/*  #define DEBUG_DISABLED
 *  #define DEMOVERSION
 *
 *  Li gestisco in modo piu' pulito tramite makefile!
 */

#include "version.h"
#include "mydebug.h"
#include "os_defs.h"
#include "mytypes.h"
#include "lists.h"
#include "structs.h"
#include "chunky.h"
#include "gfx.h"
#include "tactics.h"
#include "teamsetup.h"
#include "sound.h"
#include "demo.h"
#include "animation.h"
#include "defines.h"
#include "macros.h"

#if defined(__PPC__) && defined(__SASC)
#pragma options align=mac68k
#endif

// generic object
struct object
{
    anim_t *anim;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animation type and current frame
    int8_t otype;  // object type, this is used to find what kind of object we are handling
};

struct movingobject
{
    anim_t *anim;
    WORD world_x, world_y;
    BOOL OnScreen;
    WORD AnimType, AnimFrame; // Animazione in uso e frame a cui si e'
    int8_t otype;  // Tipo di oggetto, fin qui ci devo arrivare!
    int8_t dir;   // Direzione di movimento
    int8_t ActualSpeed; // Velocita' del tipo
    int8_t FrameLen; // Quando e' 0 posso cambiare frame
};



struct Animazione
{
    char FrameLen;
    char Frames;
    WORD *Frame;
};


#include "team.h"
#include "referee.h"
#include "ball.h"

struct game
{
    struct ball ball;
    struct referee referee;
    struct team *team[2];
    uint8_t * result;
    struct player *player_injuried;
    anim_t *extras; // Ci metto bandierine, fotografi, poliziotti...
    struct team *possesso;
    ULONG TempoPassato;
    LONG show_panel, show_time;
    UWORD check_sector;
    WORD shotheight[SHOT_LENGTH], flash_pos;
    BOOL goal, sopra_rete, flash_mode, doing_shot, mantieni_distanza, penalty_onscreen;
    WORD arcade_counter, marker_x, marker_y, penalty_counter, adder;
    uint8_t TabCounter, result_len, last_touch;
    int8_t arcade_on_field, RiservaAttuale;
//    struct linesman linesman[2]; tolti per non modificare la struttura
    int16_t corner_sx, corner_sy, corner_ex, corner_ey;
};

struct DOggetto
{
    int X, Y, Range;
    BOOL Collisione;
    int8_t *Frame;
};

// new defines used for better internationalization...
typedef struct game game_t;
typedef struct object object_t;
typedef struct movingobject movingobject_t;

#include "externs.h"

// compiler defines... this is a bit a mess ATM


#if defined(__SASC) && !defined(__PPC__)
#   define min(a, b) __builtin_min(a, b)
#   define max(a, b) __builtin_max(a, b)
#else
#   ifndef min
#       define min(a, b) ((a)>(b) ? (b) : (a))
#   endif
#   ifndef MIN
#       define MIN(a, b) min(a, b)
#   endif
#   ifndef max
#       define max(a, b) ( (a)<(b) ? (b) : (a))
#   endif
#   ifndef MAX
#       define MAX(a, b) max(a, b)
#   endif
#endif

