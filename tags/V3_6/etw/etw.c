#include "etw_locale.h"
#include "menu.h"
#include <time.h>
#include "SDL.h"
#include "highdirent.h"
#include "sound.h"

#if defined(LINUX) || defined(SOLARIS_X86)
#include <gtk/gtk.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h> // For home directory
#endif

// char versione[]="\0$VER: ETW-Menu " ETW_VERSION " " __AMIGADATE__ " by Gabriele Greco - Hurricane Studios";

extern SDL_Surface *screen;
// ULONG detail_level=~(USA_RISULTATO);
extern int Colors;
int oldwidth = 320, oldheight = 256;
//WORD players = 1;
static int music_channel = -1;
BOOL quit_game = FALSE, no_sound = FALSE;
BOOL training = FALSE, nosync = FALSE, firsttime = TRUE, arcade_back =
    FALSE;
char control[4] = { CTRL_JOY, CTRL_JOY, CTRL_JOY, CTRL_JOY };
anim_t *logos = NULL, *symbols = NULL;
gfx_t *arcade_gfx[ARCADE_TEAMS + 1];
uint8_t *back;
int bitmap_width, bitmap_height;
struct SoundInfo *music = NULL;
char *TEMP_DIR, *HIGH_FILE, *CONFIG_FILE, *RESULT_FILE;

void PlayMenuMusic(void)
{
    char buffer[120];

    if (!menu_music || music_playing || no_sound)
        return;


    sprintf(buffer, "+.music/back%d.wav" /*-*/ , RangeRand(NUMERO_LOOPS));

    D(bug("Loading %s as menu music...\n" /*-*/ , buffer));

    if (music) {
        FreeSound(music);
        music = NULL;
    }

    if ((music = LoadSound(buffer))) {
        music_channel = PlayBackSound(music);

        if (music_channel >= 0)
            music_playing = TRUE;
    }
}

void StopMenuMusic(void)
{
    if (music_playing && music_channel >= 0 && !no_sound) {
        extern struct SoundInfo *busy[];

        D(bug("Stopping menu music on channel %d...\n" /*-*/, music_channel ));
        SDL_LockAudio();
        busy[music_channel] = NULL;
// sblocco il canale            
        SDL_UnlockAudio();
        music_playing = FALSE;
        music_channel = -1;
    }
}

BOOL LoadBack(void)
{
    gfx_t *background;
    char buffer[120];

    sprintf(buffer, "menugfx/back%d.gfx" /*-*/ ,
            RangeRand(NUMERO_SFONDI));

    if ((background = LoadGfxObject(buffer, Pens, NULL))) {
        if (background->width != WINDOW_WIDTH
            || background->height != WINDOW_HEIGHT)
            ScaleGfxObj(background, back);
        else
            BltGfxObj(background, 0, 0, back, 0, 0, WINDOW_WIDTH,
                      WINDOW_HEIGHT, bitmap_width);

        FreeGfxObj(background);
        arcade_back = FALSE;
        return TRUE;
    }

    return FALSE;
}

void LoadArcadeGfx(void)
{
    int i;
    char buffer[50];

    for (i = 0; i < ARCADE_TEAMS + 1; i++) {
        sprintf(buffer, "menugfx/arcade%d.gfx", i);
        arcade_gfx[i] = LoadGfxObject(buffer, Pens, NULL);
    }
}


BOOL LoadArcadeBack(void)
{
    gfx_t *background;

    if ((background =
        LoadGfxObject("menugfx/worldmap.gfx" /*-*/ , Pens, NULL))) {
        if (background->width != WINDOW_WIDTH
            || background->height != WINDOW_HEIGHT)
            ScaleGfxObj(background, back);
        else
            BltGfxObj(background, 0, 0, back, 0, 0, WINDOW_WIDTH,
                      WINDOW_HEIGHT, bitmap_width);

        FreeGfxObj(background);
        arcade_back = TRUE;
        return TRUE;
    }

    return FALSE;
}

void FreeMenuStuff(void)
{
    D(bug("Freeing logos...\n"));

/*
    Non dovrebbe servire...

    .... e invece sembra esser necessario...
*/
    if (menu_music && music_playing)
        StopMenuMusic();

    if (logos) {
        FreeAnimObj(logos);
        logos = NULL;
    }

    if (symbols) {
        FreeAnimObj(symbols);
        symbols = NULL;
    }

    D(bug("Freeing background picture...\n"));

    if (back) {
        free(back);
        back = NULL;
    }

    D(bug("Freeing main bitmap...\n"));

    free(main_bitmap);

    D(bug("Freeing fonts...\n"));
    FreeMenuFonts();

    D(bug("Freeing sounds...\n"));
    LiberaSuoniMenu();

    D(bug("Begin: FreeGraphics()!\n"));
    FreeGraphics();
    D(bug("End: FreeGraphics()!\n"));

    if (public_screen && Colors > 0) {
//              int i;

        D(bug("Freeing pens!\n"));

/*        for(i=0;i<Colors;i++)
            os_releasepen(Pens[i]);
        */

    } else
        FreeIFFPalette();

    if (music) {
        FreeSound(music);
        music = NULL;
    }
}

BOOL LoadMenuStuff(void)
{
    if (firsttime) {
        save_back = FALSE;        /* Non mi interessa conservare gli sfondi */
        use_clipping = FALSE;    /* Voglio vedere gli omini parzialmente fuori dallo schermo */
        use_window = TRUE;

    }

    D(bug("Palette allocation...\n"));

    {
        int i;

        wanted_width = bitmap_width = WINDOW_WIDTH;
        wanted_height = bitmap_height = WINDOW_HEIGHT;

        use_remapping = FALSE;

        for (i = 0; i < 16; i++)
            Pens[i] = i;
    }

    D(bug("Menu palette remapped.\n" /*-*/ ));

// ModifyIDCMP

    D(bug("Opening game window...\n"));

    ClipX = WINDOW_WIDTH - 1;
    ClipY = WINDOW_HEIGHT - 1;

    D(bug("Anim System initialization...\n" /*-*/ ));

    if (!InitAnimSystem()) {
        D(bug("Error in InitAnimSystem!\n" /*-*/ ));
        return FALSE;
    }

    if (!(main_bitmap = malloc(WINDOW_WIDTH * WINDOW_HEIGHT))) {
        FreeGraphics();
    } else
        bitmap_width = WINDOW_WIDTH;

    if (firsttime) {
        LoadPLogo("newgfx/hurricane" /*-*/ );
        os_delay(50);

        if (!public_screen)
            os_delay(40);
    }

    D(bug("Opening fonts...\n" /*-*/ ));

    if (!InitMenuFonts()) {
        free(main_bitmap);
        FreeGraphics();
        return FALSE;
    }

    D(bug("Sound system initialization...\n" /*-*/ ));

    if (!no_sound) {
        FILE *fh;

        if (firsttime) {
            if (!(fh = fopen("intro/intro.anim" /*-*/ , "r")))
                nointro = TRUE;
            else
                fclose(fh);

            if(!InitSoundSystem()) {
                free(main_bitmap);
                FreeMenuFonts();
                FreeGraphics();
                return FALSE;
            }
        }

        if (!CaricaSuoniMenu()) {
            FreeSoundSystem();
            free(main_bitmap);
            FreeMenuFonts();
            FreeGraphics();
            return FALSE;
        }
    }
// Creo tre RP per evitare di usare SetAPen che e' molto lenta.


    D(bug("Main bitmap creation\n" /*-*/ ));


    if (firsttime) {
        D(bug("Loading logo...\n" /*-*/ ));

        LoadPLogo("gfx/etwlogo" /*-*/ );

        if (!nointro)
            Intro();
        else
            os_delay(50);

        StoreButtonList();
    }

    UpdateButtonList();

    if (firsttime) {
        init_joy_config();

#if 0
        D(bug("Loading logo N.2\n" /*-*/ ));

        if (nointro)
            rectfill(main_bitmap, 0, 0, WINDOW_WIDTH - 1,
                     WINDOW_HEIGHT - 1, Pens[P_NERO], bitmap_width);

        ScreenSwap();

        LoadMenuLogo("gfx/etwlogo" /*-*/ );
#endif
        D(bug("Updating scores...\n"));
        LoadScores();

        os_delay(50);

        if (!public_screen)
            os_delay(50);

//              GriddedWipe(0,NULL);

        rectfill(main_bitmap, 0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1,
                 Pens[P_NERO], bitmap_width);

        ScreenSwap();

        if (last_obj)
            FreeGfxObj(last_obj);
    }

    if (!(back = malloc(WINDOW_WIDTH * WINDOW_HEIGHT))) {
        free(main_bitmap);
        FreeMenuFonts();
        FreeGraphics();
    }

    D(bug("Palette reinitialization\n" /*-*/ ));

    if (!public_screen) {
        int i;

        if (firsttime) {
            for (i = 0; i < 16; i++)
                Pens[i] = i;

            Colors = 0;

            FreeIFFPalette();
        }

        if (!LoadIFFPalette("gfx/eat16menu.col" /*-*/ )) {
            D(bug("Unable to load the menu palette!\n" /*-*/ ));
            free(main_bitmap);
            free(back);
            FreeMenuFonts();
            FreeGraphics();
            return FALSE;
        }
//      LoadGfxObjPalette("menugfx/arcade0.gfx"/*-*/); Prova x quando era buggata l'altra chiamata

        D(bug("Loaded menu palette.\n" /*-*/ ));
    }

    if ((logos = LoadAnimObject("menugfx/clips.obj" /*-*/ , Pens))) {
        if (LoadBack()) {
            if ((symbols =
                LoadAnimObject("menugfx/simboli.obj" /*-*/ , Pens))) {

                D(bug
                  ("InitStuff() OK...\n" /*-*/ ));
                firsttime = FALSE;

                PlayMenuMusic();

                return TRUE;
            }
            D(bug("Error in symbols loading...\n" /*-*/ ));
        }
    }

    FreeMenuStuff();

    return FALSE;
}

// AC: Where is errno in Microsoft Visual C++? In which lib is contained?
#ifdef _MBCS
int errno;
#endif

#ifdef linux
#include "prefix.h"
#endif

int main(int argc, char *argv[])
{
    /* AC: Why if I include externs.h I obtain 55 compilation error? */
    extern void LoadKeyDef(int, char *);
    

    /* LINUX programs aren't relocatable, except with this trick
     */
#if defined(linux)
    DIR *l;

    if ((l = opendir("newgfx"))) {
        closedir(l);
    }
    else {
        char *dir = strdup(SELFPATH), *d;

        if ((d = strrchr(dir, '/'))) {
            char buff[512];
            *d = 0;
#ifdef AUTOPACKAGED
            sprintf(buff, "%s/../share/etwdata", dir);
#else
            strcpy(buff, dir);
#endif
            D(bug("Setting default directory to %s", buff));

            chdir(buff);
        }
        
        free(dir);
    }
    
#endif
    
#ifdef USE_LOGFILE
    extern FILE *logfile;

#ifdef AMIGA
    logfile = fopen("t:etw.log", "w");
#elif defined(WIN)
    logfile = fopen("etw.log", "w");
#else
    logfile = fopen("/tmp/etw.log", "w");
#endif
#endif

#ifdef MACOSX
    chdir("ETW.app/Contents/Resources/data");
#endif
//    srand(clock());
    
    InitStrings();

#if defined(LINUX) || defined(SOLARIS_X86)
    gtk_init(&argc, &argv);
#endif

/* Fix of an old language catalog bug... */

    {
        int i;

        for (i = 0; i < 8; i++) {
            if (wcp[32 * 5 + 1 + i * 5].Testo == NULL)
                wcp[32 * 5 + 1 + i * 5].Testo = "GC" /*-*/ ;
        }
    } 

#if defined(LINUX) || defined(SOLARIS_X86)
    /* Find data and temporary directories */
    {
        struct passwd *pwd;
        char *home = getenv("HOME");
        if(home == NULL)
        {
            /* Try looking in password file for home dir. */
            pwd = getpwuid(getuid());
            if(pwd)
                home = pwd->pw_dir;
        }

        /* Do not get fooled by a corrupted $HOME */
        if(home && strlen(home) < PATH_MAX)
        {
            TEMP_DIR = malloc(strlen(home) + strlen("/.etw/") + 1);
            sprintf(TEMP_DIR, "%s/.etw/", home);
            /* Always try to make the directory, just to be sure. */
            mkdir(TEMP_DIR, 0755);
        }
        else
        {
            TEMP_DIR = strdup("/tmp/");
        }

        l = opendir(TEMP_DIR);
        if(!l)
        {
            printf("Unable to find temp directory %s!\n", TEMP_DIR);
            return 20;
        }
        closedir(l);

        HIGH_FILE = malloc(strlen(TEMP_DIR) + strlen("high") + 1);
        sprintf(HIGH_FILE, "%shigh", TEMP_DIR);
        CONFIG_FILE = malloc(strlen(TEMP_DIR) + strlen("thismatch") + 1);
        sprintf(CONFIG_FILE, "%sthismatch", TEMP_DIR);
        RESULT_FILE = malloc(strlen(TEMP_DIR) + strlen("result") + 1);
        sprintf(RESULT_FILE, "%sresult", TEMP_DIR);
    }
#elif defined(AMIGA)
    TEMP_DIR = "t:";
    HIGH_FILE = "t:high";
    CONFIG_FILE = "t:thismatch";
    RESULT_FILE = "t:result";    
#else
    TEMP_DIR = "t/";
    HIGH_FILE = "t/high";
    CONFIG_FILE = "t/thismatch";
    RESULT_FILE = "t/result";
#endif
    
    read_menu_config();

    LoadTeams(TEAMS_DIR "default" /*-*/ );

    /* AC: I put here the load of keyboard configuration files
     * 
     */
    LoadKeyDef(0,KEY_RED_FILE);
    LoadKeyDef(1,KEY_BLUE_FILE);

//      EnableReqs();

    /* AC: Under MacOS X and SDL 1.2.7 seems that if there aren't any koystick connected to the
     * computer, the SDL_Init fails. 
     * Now I'm trying the fallback config.
     */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE) < 0)
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO /*| SDL_INIT_NOPARACHUTE */) < 0)
        {
            fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
            exit(1);
        }

    atexit(SDL_Quit);

    OpenMenuScreen();

    if (screen) {
        if (LoadMenuStuff()) {
            
            D(bug("Starting ChangeMenu...\n"));

            os_start_audio();

            ChangeMenu(0);

            D(bug("Entering main loop...\n"));
            while (HandleMenuIDCMP());

            if (audio_to_fast) {
                D(bug("Delete Audio2Fast...\n" /*-*/ ));
                DeleteAudio2Fast();
            }

            D(bug("Start: FreeMenuStuff...\n" /*-*/ ));
            FreeMenuStuff();
            D(bug("End: FreeMenuStuff()...\n" /*-*/ ));
        }

        if(SoundStarted())
            FreeSoundSystem();
    }

    D(bug("Program exited cleanly!\n"));

    return 0;
}
