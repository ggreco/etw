/*
Routine di controllo dei samples
*/
#define SOUND_C

#include "eat.h"
#include "preinclude.h"
#include "menu.h"

struct BufferInfo *BufferInfo;

static Mix_Music *mmusic = NULL;
static int sound_started = 0;
static int sound_loaded = FALSE;

/*
            if (i == AUDIO_CROWD) {
                if (use_crowd && game_start)
                    busy[i] = handle_crowd();
            } else {
                if (use_speaker && game_start)
                    busy[i] = handle_speaker();
            }
*/

static int crowd_channel = -1;

static void played_effect(int chn) {
    if (chn == crowd_channel) {
        crowd_channel = -1;
        Mix_ResumeMusic();
    }
}
void SetCrowd(int s)
{
    if (use_crowd && !no_sound) {
        if (crowd_channel >= 0) {
            Mix_HaltChannel(crowd_channel);
            crowd_channel = -1;
        }
        
        if (s == FONDO) {
            if (Mix_PausedMusic())
                Mix_ResumeMusic();
        }
        else if (s >= 0) {
            Mix_PauseMusic();
            crowd_channel = Mix_PlayChannel(-1, sound[s]->SoundData, sound[s]->Flags & SOUND_LOOP ? -1 : 0);
        }
    }
}



int PlayBackSound(struct SoundInfo * si)
{
    int i;

    if (no_sound)
        return -1;

    i = Mix_PlayChannel(-1, si->SoundData, si->Flags & SOUND_LOOP ? -1 :
                                           si->Loops > 0 ? si->Loops : 0);
    
    if (i != -1)
        return i;
    
    D(bug("All virtual channels busy!\n"));

    return -1;
}

void PlayIfNotPlaying(int s)
{
    int i;
    struct SoundInfo *si = sound[s];

    for (i = 0; i < AVAILABLE_CHANNELS; i++)
        if (Mix_GetChunk(i) == si->SoundData)
            return;

    PlayBackSound(si);
}

BOOL InitSoundSystem(void)
{
    if (Mix_Init(MIX_INIT_OGG) < 0) {
        D(bug("Unable to intialize SDL mixer!\n"));
        return FALSE;
    }
    
    D(bug("Initializing audio channels...\n"));

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0) {
        D(bug("Unable to open audio: %s\n", Mix_GetError()));
        return FALSE;
    }
    
    Mix_AllocateChannels(AVAILABLE_CHANNELS);
    Mix_ChannelFinished(played_effect);
    sound_started = 1;
    return TRUE;
}

void PlayMenuMusic(void)
{
    if (!menu_music || music_playing || no_sound)
        return;
    
    if (!mmusic) {
        D(bug("Loading menu music...\n"));
        
        if ((mmusic = Mix_LoadMUS("music/noisytube.ogg"))) {
            if (Mix_PlayMusic(mmusic, -1) < 0) {
                D(bug("Error playing menu music: %s\n", Mix_GetError()));
            }
            else
                music_playing = TRUE;
        }
        else {
            D(bug("Error loading menu music: %s\n", Mix_GetError()));
        }
    }
    else 
        Mix_PlayMusic(mmusic, -1);
    
    Mix_VolumeMusic(MIX_MAX_VOLUME/6);
}

void FreeMenuMusic()
{
   if (mmusic) {
       D(bug("Freeing menu music %lx\n", mmusic));
        Mix_FreeMusic(mmusic);
        mmusic = NULL;
   }
}

void
StopMenuMusic(void)
{
    if (music_playing && !no_sound && mmusic) {
        Mix_PauseMusic();
        D(bug("Stopping menu music...(%lx)\n", mmusic));
        music_playing = FALSE;
    }
}

void FreeSoundSystem(void)
{
    D(bug("Freeing sound system..."));
    Mix_CloseAudio();

    sound_started = 0;
    Mix_ChannelFinished(NULL);
    crowd_channel = -1;
    Mix_Quit();
    D(bug("Done.\n"));
}

void FreeSound(struct SoundInfo *s)
{   
    Mix_FreeChunk(s->SoundData);

    free(s);
}

struct SoundInfo *LoadSound(char const *Name)
{
    Mix_Chunk *m;
    char buf[1024];
    BOOL loop = FALSE;

// this version of ETW doesn't support anymore DISK or FILE samples
    if (*Name == '+' || *Name == '-')
        Name++;

    if (*Name == '.') {
        Name++;
        loop = TRUE;
    }

    D(bug("Loading a WAV sample... [%s]", Name));

    sprintf(buf, GAME_DIR "%s", Name);

    if ( (m = Mix_LoadWAV(buf))) {
        struct SoundInfo *s;
        
        if ( (s = (struct SoundInfo *) calloc(sizeof(struct SoundInfo), 1))) {
            if (loop)
                s->Flags |= SOUND_LOOP;

            s->SoundData = m;

            D(bug(" length: %d\n", m->alen));

            return s;
        }
        Mix_FreeChunk(m);
    }

    D(bug("\n*** Unable to load sample!\n"));

    return NULL;
}


BOOL CaricaSuoni(void)
{
    BOOL ok = TRUE;
    int i;

    for (i = 0; i <= NUMERO_SUONI; i++)
        sound[i] = NULL;

    i = 0;

    while (soundname[i]) {
        if (!arcade)
            if (i >= FIRST_ARCADE_SOUND) {
                for (i = FIRST_ARCADE_SOUND; i < NUMERO_SUONI; i++)
                    sound[i] = NULL;

                break;
            }

        if (!(sound[i] = LoadSound(soundname[i]))) {
            ok = FALSE;
            D(bug("Unable to find sample %s!\n", soundname[i]));
        }

        if (ok == FALSE) {
            int j;

            for (j = 0; j < i; j++)
                FreeSound(sound[j]);

            //                      CloseLibrary(IFFParseBase);

            return FALSE;
        } else
            i++;
    }

    sound[COMMENTO] = calloc(sizeof(struct SoundInfo), 1);
    sound_loaded = TRUE;

    return TRUE;
}


void LiberaSuoni(void)
{
    int i = 0;

    os_stop_audio();

    D(bug("Freeing sounds...\n"));

	while (soundname[i]) {
		if (sound[i])
			FreeSound(sound[i]);
        sound[i] = NULL;
		i++;
	}

	if (sound[NUMERO_SUONI + 1]) {
		free(sound[NUMERO_SUONI + 1]);
        sound[NUMERO_SUONI + 1] = NULL; 
    }

	D(bug("Ok.\n"));
}

BOOL SoundStarted(void)
{
    return sound_started;
}

void os_start_audio(void)
{
    if (!no_sound) {
        Mix_Resume(-1);
        Mix_ResumeMusic();
    }
}

void os_stop_audio(void)
{
    if (!no_sound) {
        Mix_Pause(-1);
        Mix_PauseMusic();
    }
}


BOOL CaricaSuoniMenu(void)
{
    BOOL ok = TRUE;
    int i;

    for (i = 0; i < (NUMERO_SUONI_MENU); i++)
        menusound[i] = NULL;

    i = 0;

    while (menu_soundname[i]) {
        if (!(menusound[i] = LoadSound(menu_soundname[i]))) {
            ok = FALSE;
            D(bug("Unable to find sample %s!\n", menu_soundname[i]));
        }

        if (ok == FALSE) {
            int j;

            for (j = 0; j < i; j++)
                FreeSound(menusound[j]);

            return FALSE;
        } else
            i++;
    }

    sound_loaded = TRUE;

    return TRUE;

}

void LiberaSuoniMenu(void)
{
    int i = 0;

    os_stop_audio();

    if(!sound_loaded)
        return;

	while (menu_soundname[i]) {
		if (menusound[i]) {
			FreeSound(menusound[i]);
			menusound[i] = NULL;
		}
		i++;
	}

	sound_loaded = FALSE;
}
