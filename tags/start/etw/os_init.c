#include "preinclude.h"
#ifdef WIN32
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

                D(bug("***Eseguo settaggi specifici modalita' ALLENAMENTO"));
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

           
            D(if (network_game) bug("Gioco di rete, giocatore in rete: %d\n", network_player->num));
            
            if (network_game && player_type[network_player->num] == 1) {
                MyReadPort0 = ReadNetworkPort;
                D(bug("Assegno a MyReadPort0 la gestione via RETE.\n"));
            }
            else if (use_key0) {
                MyReadPort0 = ReadKeyPort;
                D(bug("Assegno a MyReadPort0 la gestione via tastiera.\n"));
            }
            else {
                MyReadPort0 = MyReadJoyPort;
                D(bug("Assegno a MyReadPort0 la gestione via joystick.\n"));
            }

            if (network_game && player_type[network_player->num] == 0) {
                MyReadPort1 = ReadNetworkPort;
                D(bug("Assegno a MyReadPort1 la gestione via RETE.\n"));
            }
            else if (use_key1) {
                MyReadPort1 = ReadKeyPort;
                D(bug("Assegno a MyReadPort1 la gestione via tastiera.\n"));
            }
            else {
                MyReadPort1 = MyReadJoyPort;
                D(bug("Assegno a MyReadPort1 la gestione via joystick.\n"));
            }

            MainLoop();

            // Abbasso la priorita' in modo che le D(bug()) vengano piazzate nel punto giusto!

            SetCrowd(FONDO);

            os_delay(20);

            situation_result[0] = p->squadra[0]->Reti;
            situation_result[1] = p->squadra[0]->Reti;

#ifdef DEMOVERSION
            D(bug("Inizio lo slideshow...\n"));

            LoadPLogo("gfx/demoas.gfx");
            WaitOrKey(8);
            LoadPLogo("gfx/demobs.gfx");
            WaitOrKey(8);
            LoadPLogo("gfx/democs.gfx");
            WaitOrKey(8);
            LoadPLogo("gfx/demofs.gfx");
            WaitOrKey(8);
            LoadPLogo("gfx/demods.gfx");
            WaitOrKey(8);
            LoadPLogo("gfx/demoes.gfx");
            WaitOrKey(8);
            LoadPLogo("gfx/order.gfx");
            WaitOrKey(40);
            D(bug("Fine dello slideshow\n"));
#endif
            D(bug("Inizio FreeStuff...\n"));
            FreeStuff();

            D(bug("Fine FreeStuff()...\n"));
        }
    }
#if defined (DEMOVERSION) && defined(AMIGA)
    AssignLock("ETW-DATA", NULL);
    AssignLock("ETW-TCT", NULL);
    AssignLock("ETW-TEAMS", NULL);
#endif
}

void os_audio2fast(void)
{
	extern char *comment_file;

	D(bug("Gestisco audio2fast!\n"));

	if (use_speaker) {
		char buffer[100];
		FILE *f;

		strcpy(buffer, spk_basename);
		strcat(buffer, ".spk");

		if (f = fopen(buffer, "rb")) {
			long l;

			fseek(f, 0, SEEK_END);
			l = ftell(f);
			fseek(f, 0, SEEK_SET);

			if (comment_file = malloc((int) l)) {
				fread(comment_file, 1, (int) l, f);
			} else {
				audio2fast = FALSE;
				return;
			}
			fclose(f);
		}
	}

	if (use_crowd) {
		extern struct SoundInfo *Cori[NUMERO_CORI];
		extern char corobuffer[];
		int i;
		BOOL ok = TRUE;

		use_ahi = TRUE;

		for (i = 0; i < NUMERO_CORI; i++) {
			corobuffer[13] = ((i + 1) / 10) + '0';
			corobuffer[14] = ((i + 1) % 10) + '0';

			if (!(Cori[i] = LoadSound(corobuffer))) {
				D(bug("Errore nel caricamento del coro %ld!\n", i + 1));
				ok = FALSE;
				break;
			}

			convert_sound(Cori[i]);
		}

		if (!ok) {
			int l;

			for (l = 0; l < i; l++)
				FreeSound(Cori[l]);

			if (use_speaker && comment_file)
				free(comment_file);
			audio2fast = FALSE;
		} else {
			D(bug("Cori precaricati correttamente!\n"));
			audio2fast = TRUE;
		}
	}
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

#ifdef WIN32
void kprintf(unsigned char *fmt, ...)
{
	va_list ap;

#ifndef USE_LOGFILE
	char temp[400];
	va_start(ap, fmt);
	vsprintf(temp, fmt, ap);
	OutputDebugString(temp);
	va_end(ap);
#else

	if (logfile) {
		va_start(ap, fmt);
		vfprintf(logfile, fmt, ap);
		va_end(ap);
		fflush(logfile);
	}
#endif							/* USE_LOGFILE */
}

#else							/* NOT WIN */

void kprintf(UBYTE * fmt, ...)
{
	va_list ap;

#if !defined(USE_LOGFILE)
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
#else
	if (logfile) {
		va_start(ap, fmt);
		vfprintf(logfile, fmt, ap);
		va_end(ap);
		fflush(logfile);
	}
#endif
}

#endif

#endif							/* NOT DEBUG_DISABLED && NOT AMIGA && NOT MORPHOS */

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

ULONG MY_CLOCKS_PER_SEC = 0, MY_CLOCKS_PER_SEC_50;

#ifndef STANDARD_CLOCK

ULONG os_get_timer(void)
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
ULONG os_get_timer(void)
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

#ifdef LINUX

#include <sys/dir.h>

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
			  ("Fallito open classico su %s, provo case insensitive... (%s in %s)\n",
			   name, fn, dir));

			if (d = opendir(dir)) {
				struct dirent *e;

				while (e = readdir(d)) {
					if (!stricmp(fn, e->d_name)) {
						strcat(dir, e->d_name);

						D(bug("Trovato, apro: %s\n", dir));
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
