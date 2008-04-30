#include "eat.h"
#include "preinclude.h"

// this code has been largely semplified removing double buffering since the loading
// of the samples is done in foreground and so DBuffering was unuseful.

extern struct SoundInfo *busy[];

static struct SoundInfo *Cori[NUMERO_CORI] = {0}; // initialize everything to zero

int wanted_sound = FONDO, playing = -1;

static int last_looped = FONDO, numero_loops = 0;

static char *CrowdName(int i)
{
    static char corobuffer[] = ".crowd/crowd00.wav";    

    corobuffer[12] = (i / 10) + '0';
    corobuffer[13] = (i % 10) + '0';

    return corobuffer;
}

static char *RandomCrowdName()
{
    return CrowdName(MyRangeRand(NUMERO_CORI) + 1);
}

static BOOL played = FALSE;

void free_crowd()
{
    if (use_crowd && audio2fast) {
        int i;

        for (i = 0; i < (NUMERO_CORI - 1); i++) {
            if (Cori[i])
                FreeSound(Cori[i]);
        }
        sound[FONDO] = Cori[NUMERO_CORI - 1];
    }
}

// preload crowd samples, may help on slower machines.
BOOL crowd2memory()
{
    int i;
    BOOL ok = TRUE;

    for (i = 0; i < NUMERO_CORI; i++) {
        if (!(Cori[i] = LoadSound(CrowdName(i + 1)))) {
            D(bug("Error loading crowd %ld!\n", i + 1));
            ok = FALSE;
            break;
        }
    }

    if (!ok) {
        int l;

        for (l = 0; l < i; l++)
            FreeSound(Cori[l]);
    }
    D( else bug("Crowd preload completed!\n"));

    return ok;
}

void init_crowd(void)
{
    played = FALSE;
    playing = -1;
    last_looped = FONDO;
    wanted_sound = FONDO;

    if (audio2fast) {
        D(bug("A2F: init crowd...\n"));
        FreeSound(sound[FONDO]);
        sound[FONDO] = Cori[MyRangeRand(NUMERO_CORI)];
        numero_loops = 128 / (sound[FONDO]->Length / 3600);
        D(bug
          ("CT: Using audio2fast L: %ld B: %lx NL: %ld...\n",
           sound[FONDO]->Length, sound[FONDO]->SoundData, numero_loops));
    } else {
        numero_loops = -1;

        if(sound[FONDO])
            FreeSound(sound[FONDO]);
        
        sound[FONDO] = LoadSound(RandomCrowdName());
    }

    busy[AUDIO_CROWD] = sound[FONDO];

    if (busy[AUDIO_CROWD])
        busy[AUDIO_CROWD]->Offset = 0;
    D(else bug("*** busy[CROWD] NULL in init_crowd()!"));
}

struct SoundInfo *handle_crowd(void)
{
    BOOL newloop = FALSE;

    if (highlight)
        return NULL;

    if (wanted_sound != playing) {
        if (wanted_sound < 0)
            return NULL;

		if (playing >= 0 && sound[playing])
			if (sound[playing]->Flags & SOUND_LOOP)
				last_looped = playing;

        newloop = TRUE;
        playing = wanted_sound;
    }

    if (playing >= 0) {
        if (playing == FONDO)
            numero_loops--;

        if (numero_loops < 0) {
            if (audio2fast) {
                sound[FONDO] = Cori[MyRangeRand(NUMERO_CORI)];
                numero_loops = 128 / (sound[FONDO]->Length / 3600);
            } else {
                FreeSound(sound[FONDO]);

                sound[FONDO] = LoadSound(RandomCrowdName());
                
                if (sound[FONDO]) {
                    numero_loops = 128 / (sound[FONDO]->Length / 3600);
                } else {
                    playing = -1;
                    return FALSE;
                }
            }
        }

        if (!(sound[playing]->Flags & SOUND_LOOP) && !newloop) {
            wanted_sound = playing = last_looped;
        }

        return sound[playing];
    }
    return NULL;
}
