#include "preinclude.h"

#if defined(WIN32) && !defined(WINCE)
#   include <windows.h>
#endif
#include "eat.h"
#include "network.h"

#ifdef __SASC
#   include <dos.h>
#endif

extern SDL_Surface *screen;

extern void FreeStuff(void);
extern BOOL LoadStuff(void);

uint32_t(*MyReadPort0) (uint32_t);
uint32_t(*MyReadPort1) (uint32_t);

uint32_t MyReadJoyPort(uint32_t);
uint32_t MyReadTouchPort(uint32_t);

#ifdef DEBUG_DISABLED
#   ifdef USE_LOGFILE
#       undef USE_LOGFILE
#   endif
#endif

#ifdef USE_LOGFILE
FILE *logfile = NULL;
#endif

void init_system(void)
{
    SetResult("error");

    if (!screen)
        return;

    if (!LoadStuff())
        return;

    if (training && !penalties && !free_kicks) {
        int i;
        team_t *s = p->team[1];

        D(bug("*** TRAINING mode settings"));
        s->Joystick = -1;

        for (i = 0; i < 10; i++) {
            s->players[i].world_x = 12000;
            s->players[i].world_y = 10;

            DoSpecialAnim((&(s->players[i])), GIOCATORE_ESPULSO);
            s->players[i].Comando = STAI_FERMO;
        }
    } else if (!penalties && !free_kicks) {
        p->show_panel = PANEL_TIME | PANEL_KICKOFF;
        p->show_time = 50;
    }
    // questo e' indipendente dalle squadre, dipende solo da chi gioca...

   
    D(if (network_game) bug("Network play, net player: %d\n", network_player->num));
    
    if (network_game && network_player->num == 1) {
        MyReadPort0 = ReadNetworkPort;
        D(bug("Assign MyReadPort0: NET\n"));
    }
    else if (use_touch) {
        MyReadPort0 = MyReadTouchPort;
        D(bug("Assign MyReadPort0: TOUCH INTERFACE\n"));
    }
    else if (use_key0) {
        MyReadPort0 = ReadKeyPort;
        D(bug("Assign MyReadPort0: KEYBOARD\n"));
    }
    else {
        MyReadPort0 = MyReadJoyPort;
        D(bug("Assign MyReadPort0: JOYSTICK\n"));
    }

    if (network_game && network_player->num == 0) {
        MyReadPort1 = ReadNetworkPort;
        D(bug("Assign MyReadPort1: NET\n"));
    }
    else if (use_touch) {
        MyReadPort1 = MyReadTouchPort;
        D(bug("Assign MyReadPort1: TOUCH INTERFACE\n"));
    }
    else if (use_key1) {
        MyReadPort1 = ReadKeyPort;
        D(bug("Assign MyReadPort1: KEYBOARD\n"));
    }
    else {
        MyReadPort1 = MyReadJoyPort;
        D(bug("Assign MyReadPort1: JOYSTICK\n"));
    }

    MainLoop();

    // Abbasso la priorita' in modo che le D(bug()) vengano piazzate nel punto giusto!
}


void os_flush_events()
{
    SDL_Event e;

    while (SDL_PollEvent(&e));
}
// Ritarda di s 50esimi di secondo

void os_delay(int s)
{
    SDL_Delay(s * 20);
}

int os_avail_mem(void)
{
    // Ritorno sempre memoria abbondante.

    return (int) 10000000;
}

#ifdef ANDROID
#include <android/log.h>
#endif

#if !defined(DEBUG_DISABLED) && !defined(MORPHOS) && !defined(AMIGA)

#include <stdarg.h>

void kprintf(const char *fmt, ...)
{
    va_list ap;

#ifndef USE_LOGFILE
#   ifdef WIN32
    char temp[400];
    va_start(ap, fmt);
    vsprintf(temp, fmt, ap);
    OutputDebugString(temp);
    va_end(ap);
#elif defined(ANDROID)
    char temp[500];
    va_start(ap, fmt);
    vsprintf(temp, fmt, ap);
    __android_log_write(ANDROID_LOG_INFO, "ETW", temp);
    va_end(ap);
#   else
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
#   endif
#else

    if (logfile) {
        va_start(ap, fmt);
        vfprintf(logfile, fmt, ap);
        va_end(ap);
        fflush(logfile);
    }
#endif /* USE_LOGFILE */
}

#endif/* NOT DEBUG_DISABLED && NOT AMIGA && NOT MORPHOS */

#if defined(__PPC__) && !defined(__SASC)

int strnicmp(const char *a, const char *b, size_t l)
{
    while(l && *a && *b && tolower(*a) == tolower(*b))
    {
        a++; b++; l--;
    }

    if(l)
        return (int)tolower(*a) - (int)tolower(*b);

    return 0;
}
#endif

// Gestione dei timer

unsigned long MY_CLOCKS_PER_SEC = 0, MY_CLOCKS_PER_SEC_50;

unsigned long os_get_timer(void)
{
    return SDL_GetTicks();
}

void os_wait_timer(uint32_t clk)
{
    while (SDL_GetTicks() < clk) {
        SDL_Delay(1);
    }
}

void os_init_timer(void)
{
    MY_CLOCKS_PER_SEC = 1000;
    MY_CLOCKS_PER_SEC_50 = 1000 / framerate;
}

void os_free_timer(void)
{
}

#ifdef ANDROID
#include <android/asset_manager.h>
#endif

#if defined(LINUX) || defined(SOLARIS_X86) || defined (IPHONE) || defined(ANDROID)
#if defined(LINUX) && !defined(SOLARIS_X86) 
#   include <sys/dir.h>
#else
#   include <dirent.h>
#endif
#undef fopen

FILE *os_open(const char *name, const char *mode)
{
    char buf[2048], dir[512];
    const char *orig_name = name;
    const char *fn;
    struct dirent *e;
    FILE *f;
    DIR *d;

    if(name[0] != '/')
    {
#ifndef ANDROID
        sprintf(buf, GAME_DIR "%s", name);
#else
        sprintf(buf, "%s/%s", TEMP_DIR, name);
#endif
        name = buf;
    }

    if (*mode == 'w')
        return fopen(name, mode);

    f = fopen(name, mode);
    if (f)
        return f;

    fn = strrchr(name, '/');

    if (!fn)
        fn = name;
    else {
        strcpy(dir, name);
        fn++;
    }

    dir[fn - name] = 0;

    if (!*dir)
        strcpy(dir, "./");

    D(bug("open on %s failed, trying case insensitive... (%s in %s)\n", name, fn, dir));

    if (!(d = opendir(dir))) {
#ifdef ANDROID
        if (dir[strlen(dir) - 1] == '/')
            dir[strlen(dir) - 1] = 0;

        D(bug("directory %s not found, creating it\n", dir)); 
        if (mkdir(dir, 0777)) {
            char parent[512], *c;
            strcpy(parent, dir);
            if ((c = strrchr(parent, '/'))) {
                *c = 0;
                D(bug("Unable to create directory, trying to create parent %s\n", parent));

                if (!mkdir(parent, 0777)) {
                    if (mkdir(dir, 0777)) {
                        D(bug("Unable to create directory %s\n", dir));
                        return NULL;
                    }
                }
                else {
                    D(bug("Unable to create directory %s\n", parent));
                    return NULL;
                }
            }
        }
#else
        return NULL;
#endif
    }
    else {
        while ((e = readdir(d))) {
            if (!stricmp(fn, e->d_name)) {
                strcat(dir, e->d_name);

                D(bug(" FOUND, opening: %s\n", dir));
                closedir(d);

                return fopen(dir, mode);
            }
        }

        closedir(d);
    }

#ifdef ANDROID
    D(bug("File %s not found, loading it from assets manager...\n", orig_name));
    {
        extern AAssetManager* asset_mgr;
        AAsset* asset;
        long size;
        char *buffer;
        
        if (!asset_mgr) {
            D(bug("Asset Manager not available!\n"));
            return NULL;
        }

        if (!(asset = AAssetManager_open(asset_mgr, orig_name, AASSET_MODE_STREAMING))) {
            D(bug("Asset %s not found!", orig_name));
            return NULL;
        }
        size = AAsset_getLength(asset);
        if ((buffer = (char*) malloc (sizeof(char)*size))) {
            FILE *out;
            AAsset_read (asset,buffer,size);
            if ((out = fopen(name, "wb"))) {
                fwrite(buffer, 1, size, out);
                fclose(out);

                return fopen(name, mode);
            }
            else
                D(bug("Unable to open %s for writing asset of %d bytes.\n", name, size));

            free(buffer);
        }
        AAsset_close(asset);
    }
#endif
    return NULL;
}

#endif

