#include "eat.h"
#include "preinclude.h"

// this code has been largely semplified removing double buffering since the loading
// of the samples is done in foreground and so DBuffering was unuseful.

extern struct SoundInfo *busy[];

struct SoundInfo *Cori[NUMERO_CORI];

LONG wanted_sound = FONDO, playing = -1, last_looped = FONDO;
WORD numero_loops = 0;

char corobuffer[] = ".crowd/crowd00.wav";

BOOL played = FALSE, crowd_loading = FALSE;

void init_crowd(void)
{
	played = FALSE;
	playing = -1;
	last_looped = FONDO;
	wanted_sound = FONDO;
	crowd_loading = FALSE;

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

		corobuffer[13] = MyRangeRand(9) + '1';	// Carico da coro 1 a coro 9

    	if(sound[FONDO])
			FreeSound(sound[FONDO]);
        
		sound[FONDO] = LoadSound(corobuffer);
	}

	busy[AUDIO_CROWD] = sound[FONDO];

    if (busy[AUDIO_CROWD])
    	busy[AUDIO_CROWD]->Offset = 0;
    D(else bug("*** busy[CROWD] NULL in init_crowd()!"));
}

struct SoundInfo *handle_crowd(void)
{
	BOOL newloop = FALSE;

	if (wanted_sound != playing) {
		if (wanted_sound < 0)
			return NULL;

		if (playing >= 0)
			if (sound[playing]->Flags & SOUND_LOOP)
				last_looped = playing;

		newloop = TRUE;
		playing = wanted_sound;
	}

	if (playing >= 0) {
  //            D(bug("CT: Inizio a playare il suono %ld!\n",playing));

		if (playing == FONDO)
			numero_loops--;

		if (numero_loops < 0) {
			if (audio2fast) {
				sound[FONDO] = Cori[MyRangeRand(NUMERO_CORI)];
				numero_loops = 128 / (sound[FONDO]->Length / 3600);
// i loop ora sono ricampionati a 20khz, 256 non andava piu' bene!

//	            D(bug("CT: Coro: %ld - %ld (l: %ld)\n",sound[FONDO]->SoundData,sound[FONDO]->Length,numero_loops));
			} else {
				int i = MyRangeRand(NUMERO_CORI) + 1;

				corobuffer[12] = (i / 10) + '0';
				corobuffer[13] = (i % 10) + '0';

                FreeSound(sound[FONDO]);

                sound[FONDO] = LoadSound(corobuffer);
                
				if (sound[FONDO]) {
    				convert_sound(sound[FONDO]);
					numero_loops =
						128 / (sound[FONDO]->Length / 3600);
				} else {
					playing = -1;
					return FALSE;
				}
			}
		}

		if (!(sound[playing]->Flags & SOUND_LOOP) && !newloop) {
			wanted_sound = playing = last_looped;
		}

		if (!(sound[playing]->Flags & SOUND_FAST)) {
//                      D(bug("CT: Suoni in chip o disco non supportati!\n"));
			return NULL;
		}

		return sound[playing];
	}
	return NULL;
}
