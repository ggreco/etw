#include "etw_locale.h"
#include "menu.h"
#include <time.h>
#include "SDL.h"
#include "highdirent.h"
#include "sound.h"

// char versione[]="\0$VER: ETW-Menu " ETW_VERSION " " __AMIGADATE__ " by Gabriele Greco - Hurricane Studios";

// char team_name[2][16]={"genoa","verona"};
extern SDL_Surface *screen;
// ULONG detail_level=~(USA_RISULTATO);
// LONG Colors=0,basepri=3;
extern LONG Colors;
// LONG WINDOW_WIDTH=320,WINDOW_HEIGHT=256,
LONG oldwidth = 320, oldheight = 256;
WORD players = 1, music_channel = -1;
BOOL quit_game = FALSE, no_sound = FALSE;
BOOL training = FALSE, nosync = FALSE, firsttime = TRUE, arcade_back =
	FALSE;
char control[4] = { CTRL_JOY, CTRL_JOY, CTRL_JOY, CTRL_JOY };
AnimObj *logos = NULL, *symbols = NULL;
GfxObj *arcade_gfx[ARCADE_TEAMS + 1];
char *back;
int bitmap_width, bitmap_height;
struct SoundInfo *music = NULL;
extern void convert_sound(struct SoundInfo *);

#if 0

BOOL FindDataDir(char *base)
{
	char buffer[100];
	int l = strlen(base);
	DIR *lk;

	D(bug("Cerco la dir dei dati in: %s\n" /*-*/ , base));

	strcpy(buffer, base);

	strcpy(buffer + l, "gfx" /*-*/ );

	if (lk = opendir(buffer)) {
		strcpy(buffer + l, "menugfx" /*-*/ );

		closedir(lk);

		if (lk = opendir(buffer)) {
			strcpy(buffer + l, "snd" /*-*/ );

			closedir(lk);

			if (lk = opendir(buffer)) {
				D(bug("Data dir trovata!\n"));

				closedir(lk);

				if (lk = opendir(base)) {
					closedir(lk);

					chdir(base);

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL FindAssign(char *assign, char *path)
{
	char buffer[100];
	BPTR l;

	strcpy(buffer, "PROGDIR:" /*-*/ );
	strcat(buffer, path);

	if (l = Lock(assign, ACCESS_READ)) {
		UnLock(l);
		return TRUE;
	} else if (l = Lock(path, ACCESS_READ)) {
		assign[strlen(assign) - 1] = 0;
		D(bug("Assegno %s a %s\n" /*-*/ , assign, path));
		return (BOOL) AssignLock(assign, l);
	} else if (l = Lock(buffer, ACCESS_READ)) {
		assign[strlen(assign) - 1] = 0;
		D(bug("Assegno %s a %s\n" /*-*/ , assign, buffer));
		return (BOOL) AssignLock(assign, l);
	}

	strcpy(buffer, "EatTheWhistle-CD:" /*-*/ );
	strcat(buffer, path);

	if (l = Lock(buffer, ACCESS_READ)) {
		assign[strlen(assign) - 1] = 0;
		D(bug("Assegno %s a %s\n" /*-*/ , assign, buffer));
		return (BOOL) AssignLock(assign, l);
	} else {
		D(bug("Non riesco a lockare %s -> %s\n" /*-*/ , assign, path));
		return FALSE;
	}
}

#endif

void PlayMenuMusic(void)
{
	char buffer[120];

	if (!menu_music || music_playing || no_sound)
		return;


	sprintf(buffer, "+.music/back%ld.iff" /*-*/ , RangeRand(NUMERO_LOOPS));

	D(bug("Carico %s per i menu...\n" /*-*/ , buffer));

	if (music) {
		FreeSound(music);
		music = NULL;
	}

	if ((music = LoadSound(buffer))) {
//		if (music->Rate < 19000 || music->Rate > 24000) let's ALWAYS convert samples, our rate maybe 44khz!
			convert_sound(music);

		music_channel = PlayBackSound(music);

		if (music_channel >= 0)
			music_playing = TRUE;
	}
}

void StopMenuMusic(void)
{
	if (music_playing && music_channel >= 0 && !no_sound) {
		extern struct SoundInfo *busy[];

		D(bug("Fermo la musica dei menu...\n" /*-*/ ));
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
	GfxObj *background;
	char buffer[120];

	sprintf(buffer, "menugfx/back%ld.gfx" /*-*/ ,
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
	GfxObj *background;

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
	D(bug("Libero i logos...\n"));

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

	D(bug("Libero lo sfondo...\n"));

	if (back) {
		free(back);
		back = NULL;
	}

	D(bug("Libero la bitmap temporanea...\n"));

	free(main_bitmap);

	D(bug("Libero i fonts...\n"));
	FreeMenuFonts();

	D(bug("Libero i suoni...\n"));
	LiberaSuoniMenu();

	D(bug("Inizio FreeGraphics()!\n"));
	FreeGraphics();
	D(bug("Fine FreeGraphics()!\n"));

	if (public_screen && Colors > 0) {
//              int i;

		D(bug("Libero i colori!\n"));

/*		for(i=0;i<Colors;i++)
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
		save_back = FALSE;		/* Non mi interessa conservare gli sfondi */
		use_clipping = FALSE;	/* Voglio vedere gli omini parzialmente fuori dallo schermo */
		use_window = TRUE;

	}

	D(bug("Alloco la palette...\n"));

	{
		int i;

		wanted_width = bitmap_width = WINDOW_WIDTH;
		wanted_height = bitmap_height = WINDOW_HEIGHT;

		use_remapping = FALSE;

		for (i = 0; i < 16; i++)
			Pens[i] = i;
	}

	D(bug("Rimappata la palette dei menu.\n" /*-*/ ));

// ModifyIDCMP

	D(bug("Apro la finestra...\n"));

	ClipX = WINDOW_WIDTH - 1;
	ClipY = WINDOW_HEIGHT - 1;

	D(bug("Inizializzo l'AnimSystem...\n" /*-*/ ));

	if (!InitAnimSystem()) {
		D(bug("Errore in InitAnimSystem!\n" /*-*/ ));
		return FALSE;
	}

	if (!(main_bitmap = malloc(WINDOW_WIDTH * WINDOW_HEIGHT))) {
		FreeGraphics();
	} else
		bitmap_width = WINDOW_WIDTH;

	if (!nointro && firsttime) {
		LoadPLogo("gfx/epic" /*-*/ );
		os_delay(50);

		if (!public_screen)
			os_delay(40);
	}

	D(bug("Apro i fonts...\n" /*-*/ ));

	if (!InitMenuFonts()) {
		free(main_bitmap);
		FreeGraphics();
		return FALSE;
	}

	D(bug("Inizializzo il sound system...\n" /*-*/ ));

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

	if (!nointro && firsttime) {
		LoadPLogo("gfx/islona" /*-*/ );
		os_delay(75);

		if (!public_screen)
			os_delay(40);
	}

	D(bug("Apro la bitmap temporanea\n" /*-*/ ));


	if (firsttime) {
		FILE *b;

		D(bug("Carico il primo logo.\n" /*-*/ ));

		if ((b = fopen("newgfx/hurricane" /*-*/ , "r"))) {
			fclose(b);
			LoadPLogo("newgfx/hurricane" /*-*/ );
			SDL_SaveBMP(screen, "hurricane.bmp");
		} else
			LoadPLogo("gfx/hurricane" /*-*/ );

		if (!nointro)
			Intro();
		else
			os_delay(50);

		StoreButtonList();
	}

	UpdateButtonList();

	if (firsttime) {
		init_joy_config();

		D(bug("Carico il secondo logo.\n" /*-*/ ));

		if (nointro)
			rectfill(main_bitmap, 0, 0, WINDOW_WIDTH - 1,
					 WINDOW_HEIGHT - 1, Pens[P_NERO], bitmap_width);

		ScreenSwap();

		LoadMenuLogo("gfx/etwlogo" /*-*/ );
		SDL_SaveBMP(screen, "etw.bmp");

		D(bug("Carico gli scores...\n"));
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

	D(bug("Carico la nuova palette\n" /*-*/ ));

	if (!public_screen) {
		int i;

		if (firsttime) {
			for (i = 0; i < 16; i++)
				Pens[i] = i;

			Colors = 0;

			FreeIFFPalette();
		}

		if (!LoadIFFPalette("gfx/eat16menu.col" /*-*/ )) {
			D(bug("Non riesco a caricare la palette!\n" /*-*/ ));
			free(main_bitmap);
			free(back);
			FreeMenuFonts();
			FreeGraphics();
			return FALSE;
		}
//      LoadGfxObjPalette("menugfx/arcade0.gfx"/*-*/); Prova x quando era buggata l'altra chiamata

		D(bug("Caricata la palette dei menu.\n" /*-*/ ));
	}

	if ((logos = LoadAnimObject("menugfx/clips.obj" /*-*/ , Pens))) {
		if (LoadBack()) {
			if ((symbols =
				LoadAnimObject("menugfx/simboli.obj" /*-*/ , Pens))) {
				D(bug("Alloco il file requester...\n"));
/* Da fare... gestione file requester... */

				D(bug
				  ("Esco in modo corretto dalla InitStuff()...\n" /*-*/ ));
				firsttime = FALSE;

				PlayMenuMusic();

				return TRUE;
			}
			D(bug("Errore nel caricamento di symbols...\n" /*-*/ ));
		}
	}

	FreeMenuStuff();

	return FALSE;
}

#ifndef DEMOVERSION
int main(int argc, char *argv[])
#else
int disabled_main(int argc, char *argv[])
#endif
{
	DIR *l;

#ifdef USE_LOGFILE
	extern FILE *logfile;

#ifdef AMIGA
	logfile = fopen("dh2:etw.log", "w");
#elif defined(WIN)
	logfile = fopen("debug.txt", "w");
#else
	logfile = fopen("/tmp/etw.log", "w");
#endif
#endif
//	srand(clock());

	InitStrings();

#ifdef LINUX
	gtk_init(&argc, &argv);
#endif

/* Fix per il bug sul catalog... */

	{
		int i;

		for (i = 0; i < 8; i++) {
			if (wcp[32 * 5 + 1 + i * 5].Testo == NULL)
				wcp[32 * 5 + 1 + i * 5].Testo = "GC" /*-*/ ;
		}
	}

	read_menu_config();

	if (!(l = opendir(TEMP_DIR))) {
		printf("Unable to find temp directory %s!\n", TEMP_DIR);
		return 20;
	} else
		closedir(l);

	LoadTeams(TEAMS_DIR "default" /*-*/ );

//      EnableReqs();

#ifndef NO_JOYSTICK
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE) < 0)
#else
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE) < 0)
#endif
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	atexit(SDL_Quit);

	OpenMenuScreen();

	if (screen) {
		if (LoadMenuStuff()) {
			D(bug("Lancio la Change Menu...\n"));

			os_start_audio();

			ChangeMenu(0);

			D(bug("Entro nel loop...\n"));
			while (HandleMenuIDCMP());

			if (audio_to_fast) {
				D(bug("Delete Audio2Fast...\n" /*-*/ ));
				DeleteAudio2Fast();
			}

			D(bug("Inizio FreeMenuStuff...\n" /*-*/ ));
			FreeMenuStuff();
			D(bug("Fine FreeMenuStuff()...\n" /*-*/ ));
		}

		if(SoundStarted())
			FreeSoundSystem();
	}

	D(bug("Fine!\n"));

	return 0;
}
