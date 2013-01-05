#include "eat.h"
#include "preinclude.h"

static Mix_Music *crowd_effect = NULL;

// this code has been largely semplified removing double buffering since the loading
// of the samples is done in foreground and so DBuffering was unuseful.

static char *CrowdName(int i)
{
    static char corobuffer[] = "crowd/crowd00.wav";

    corobuffer[11] = (i / 10) + '0';
    corobuffer[12] = (i % 10) + '0';

    return corobuffer;
}

static char *RandomCrowdName(void)
{
    return CrowdName(MyRangeRand(NUMERO_CORI) + 1);
}

void free_crowd(void)
{
    if (use_crowd && crowd_effect) {
        Mix_HaltMusic();
        Mix_HookMusicFinished(NULL);
        Mix_FreeMusic(crowd_effect);
        crowd_effect = NULL;
    }
}


static void change_crowd(void)
{
    if (crowd_effect)
        Mix_FreeMusic(crowd_effect);
    if ((crowd_effect = Mix_LoadMUS(RandomCrowdName())))
        Mix_PlayMusic(crowd_effect, MyRangeRand(10) + 4);
    else {
        D(bug("Unable to load new crowd sample!\n"));
    }
}

void init_crowd(void)
{
    Mix_HookMusicFinished(change_crowd);

    change_crowd();
}
