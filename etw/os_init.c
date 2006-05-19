#include "preinclude.h"

#if defined(WIN32) && !defined(WINCE)
#include <windows.h>
#endif
#include "eat.h"
#include "network.h"

#ifdef __SASC
#include <dos.h>
#endif

extern SDL_Surface *screen;

extern void FreeStuff(void);
extern BOOL LoadStuff(void);

ULONG(*MyReadPort0) (ULONG);
ULONG(*MyReadPort1) (ULONG);

ULONG MyReadJoyPort(ULONG);

#ifdef DEBUG_DISABLED
#ifdef USE_LOGFILE
#undef USE_LOGFILE
#endif
#endif

#ifdef USE_LOGFILE
FILE *logfile = NULL;
#endif

void init_system(void)
{
    SetResult("error");

    OpenTheScreen();

    if (screen) {
        if (LoadStuff()) {
            if (training && !penalties && !free_kicks) {
                int i;
                struct Squadra *s = p->squadra[1];

                D(bug("*** TRAINING mode settings"));
                s->Joystick = -1;

                for (i = 0; i < 10; i++) {
                    s->giocatore[i].world_x = 12000;
                    s->giocatore[i].world_y = 10;

                    DoSpecialAnim((&(s->giocatore[i])), GIOCATORE_ESPULSO);
                    s->giocatore[i].Comando = STAI_FERMO;
                }
            } else if (!penalties && !free_kicks) {
                p->show_panel = PANEL_TIME | PANEL_KICKOFF;
                p->show_time = 50;
            }
            // questo e' indipendente dalle squadre, dipende solo da chi gioca...

           
            D(if (network_game) bug("Network play, net player: %d\n", network_player->num));
            
            if (network_game && player_type[network_player->num] == 1) {
                MyReadPort0 = ReadNetworkPort;
                D(bug("Assign MyReadPort0: NET\n"));
            }
            else if (use_key0) {
                MyReadPort0 = ReadKeyPort;
                D(bug("Assign MyReadPort0: KEYBOARD\n"));
            }
            else {
                MyReadPort0 = MyReadJoyPort;
                D(bug("Assign MyReadPort0: JOYSTICK\n"));
            }

            if (network_game && player_type[network_player->num] == 0) {
                MyReadPort1 = ReadNetworkPort;
                D(bug("Assign MyReadPort1: NET\n"));
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

            SetCrowd(FONDO);

            os_delay(20);

            situation_result[0] = p->squadra[0]->Reti;
            situation_result[1] = p->squadra[0]->Reti;

            D(bug("Start: FreeStuff...\n"));
            FreeStuff();

            D(bug("End: FreeStuff()...\n"));
        }
    }
}

void os_audio2fast(void)
{
    audio2fast = FALSE;
    
    D(bug("Handling audio2fast!\n"));

    if (use_speaker && !speaker2memory())
        return;

    if (use_crowd && !crowd2memory()) {
        free_speaker();
        return;
    }
        
    audio2fast = TRUE;
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


#if !defined(DEBUG_DISABLED) && !defined(MORPHOS) && !defined(AMIGA)

#include <stdarg.h>

void kprintf(char *fmt, ...)
{
    va_list ap;

#ifndef USE_LOGFILE
#   ifdef WIN32
    char temp[400];
    va_start(ap, fmt);
    vsprintf(temp, fmt, ap);
    OutputDebugString(temp);
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
    char *c = strdup(b), *d = strdup(a), *e, *f;
    int r;

    e = c;
    f = d;

    while (*c) {
        *c = tolower(*c);
        c++;
    }

    while (*d) {
        *d = tolower(*d);
        d++;
    }

    r = strncmp(e, f, l);

    free(e);
    free(f);

    return r;
}
#endif

// Gestione dei timer

unsigned long MY_CLOCKS_PER_SEC = 0, MY_CLOCKS_PER_SEC_50;

#ifndef STANDARD_CLOCK

unsigned long os_get_timer(void)
{
    return SDL_GetTicks();
}


void os_wait_timer(ULONG clk)
{
    while (SDL_GetTicks() < clk);
}

void os_init_timer(void)
{
    MY_CLOCKS_PER_SEC = 1000;
    MY_CLOCKS_PER_SEC_50 = 1000 / framerate;
}


void os_free_timer(void)
{
}
#else
unsigned long os_get_timer(void)
{
    return clock();
}


void os_wait_timer(ULONG clk)
{
    while (clock() < clk);
}

void os_init_timer(void)
{
    MY_CLOCKS_PER_SEC = CLOCKS_PER_SEC;
    MY_CLOCKS_PER_SEC_50 = CLOCKS_PER_SEC / framerate;
}

void os_free_timer(void)
{
}
#endif

#if defined(LINUX) || defined(SOLARIS_X86)
#if defined(LINUX) && !defined(SOLARIS_X86)
#include <sys/dir.h>
#else
#include <dirent.h>
#endif
#undef fopen

FILE *os_open(char *name, char *mode)
{
	if (*mode == 'w') {
		return fopen(name, mode);
	} else {
		FILE *f;

		if (!(f = fopen(name, mode))) {
			char *fn = strrchr(name, '/');
			char dir[120];
			DIR *d;

			if (!fn)
				fn = name;
			else {
				strcpy(dir, name);
				fn++;
			}

			dir[fn - name] = 0;

			if (!*dir)
				strcpy(dir, "./");

			D(bug
			  ("open on %s failed, trying case insensitive... (%s in %s)\n",
			   name, fn, dir));

			if ((d = opendir(dir))) {
				struct dirent *e;

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
		}

		return f;
	}
}

#endif
