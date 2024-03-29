#ifndef OS_DEFS_H

#include "SDL_endian.h"

#define OS_DEFS_H

#ifndef __AMIGADATE__
#ifndef __DATE__
#   define __AMIGADATE__ (3.9.2003)
#else
#define __AMIGADATE__ __DATE__
#endif
#endif

#if defined(IPHONE) || defined(ANDROID)
#define MOBILE_VERSION
#endif
/* set the default arch to Linux */
#if !defined(WIN) && !defined(IPHONE) && !defined(LINUX) && !defined(MACOSX) && !defined(AMIGA) && !defined(ANDROID)
#   define LINUX
#endif

#if defined(LINUX) || defined(IPHONE) || defined(ANDROID) 
    extern FILE *os_open(const char *, const char *);
#   define fopen os_open
#elif defined(WIN) || defined(MACOSX)
#define os_open fopen
#endif

#if !defined(__SASC)
#   undef __saveds
#   define __saveds
#endif

#define LOAD_ALL

extern char *TEMP_DIR;
extern char *HIGH_FILE;
extern char *CONFIG_FILE;
extern char *RESULT_FILE;
extern char *SCORE_FILE;

#   define EXEC_DIR        ""
#   define GAME_DIR        ""

#ifndef AMIGA
#   define TCT_DIR         "tct/"
#   define TEAMS_DIR       "teams/"
#   define NEWGFX_DIR      "newgfx/"
#else
#   define TCT_DIR         "TCT/"
#   define TEAMS_DIR       "TEAMS/"
#   define NEWGFX_DIR      EXEC_DIR "newgfx/"
#endif

#define KEY_BLUE_FILE   EXEC_DIR "key_blue"
#define KEY_RED_FILE    EXEC_DIR "key_red"
#define SMALLGAME_FONT  NEWGFX_DIR "smallgame.font"

#ifdef WIN
#   if !defined( __GNUC__)
#       pragma warning( disable : 4761)
#   endif
//#   define USE_LOGFILE

#elif defined(LINUX) || defined (MACOSX) || defined(IPHONE) || defined(ANDROID)
#   define stricmp strcasecmp
#   define strnicmp strncasecmp

#elif !defined AMIGA
    typedef int fpos_t;

#endif

#define Timer() os_get_timer()
#define mytimer unsigned long
extern unsigned long MY_CLOCKS_PER_SEC, MY_CLOCKS_PER_SEC_50;
extern unsigned int os_get_timer(void);




#define JPB_BUTTON_BLUE    23    /* Blue - Stop; Right Mouse                */
#define JPB_BUTTON_RED     22    /* Red - Select; Left Mouse; Joystick Fire */
#define JPB_BUTTON_YELLOW  21    /* Yellow - Repeat                         */
#define JPB_BUTTON_GREEN   20    /* Green - Shuffle                         */
#define JPB_BUTTON_FORWARD 19    /* Charcoal - Forward                      */
#define JPB_BUTTON_REVERSE 18    /* Charcoal - Reverse                      */
#define JPB_BUTTON_PLAY    17    /* Grey - Play/Pause; Middle Mouse         */
#define JPF_BUTTON_BLUE    (1 << JPB_BUTTON_BLUE)
#define JPF_BUTTON_RED     (1 << JPB_BUTTON_RED)
#define JPF_BUTTON_YELLOW  (1 << JPB_BUTTON_YELLOW)
#define JPF_BUTTON_GREEN   (1 << JPB_BUTTON_GREEN)
#define JPF_BUTTON_FORWARD (1 << JPB_BUTTON_FORWARD)
#define JPF_BUTTON_REVERSE (1 << JPB_BUTTON_REVERSE)
#define JPF_BUTTON_PLAY    (1 << JPB_BUTTON_PLAY)
#define JP_BUTTON_MASK     (JPF_BUTTON_BLUE|JPF_BUTTON_RED|JPF_BUTTON_YELLOW|JPF_BUTTON_GREEN|JPF_BUTTON_FORWARD|JPF_BUTTON_REVERSE|JPF_BUTTON_PLAY)
// new flags added for touch compatibility
#define JPF_SWIPE      (1 << 29)
#define JPF_TOUCH_DOWN (1 << 30)
#define JPF_TOUCH      (1 << 31)

/* Direction types, valid for JP_TYPE_GAMECTLR and JP_TYPE_JOYSTK */
#define JPB_JOY_UP         3
#define JPB_JOY_DOWN       2
#define JPB_JOY_LEFT       1
#define JPB_JOY_RIGHT      0
#define JPF_JOY_UP         (1 << JPB_JOY_UP)
#define JPF_JOY_DOWN       (1 << JPB_JOY_DOWN)
#define JPF_JOY_LEFT       (1 << JPB_JOY_LEFT)
#define JPF_JOY_RIGHT      (1 << JPB_JOY_RIGHT)
#define JP_DIRECTION_MASK (JPF_JOY_UP|JPF_JOY_DOWN|JPF_JOY_LEFT|JPF_JOY_RIGHT)

/* AC: We not need to define VOID under Windows, it's defined in winnt.h */
#if !defined(VOID) && !defined(WIN)
#   define VOID void
#endif

#endif

