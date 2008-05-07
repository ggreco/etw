/*
Routine di controllo dei samples
*/
#define SOUND_C

#include "eat.h"
#include "preinclude.h"
#include "menu.h"

struct BufferInfo *BufferInfo;

struct SoundInfo *busy[AVAILABLE_CHANNELS + 2] = { 0 };

static int samplerate = 22050;

static int sound_started = 0;
static int sound_loaded = FALSE;

/* AC: I need it for sound conversion. */
static SDL_AudioSpec obt;

/*
 * Questa e' la callback di SDL per il play del suono, busy[i] e' il
 * puntatore a una lista di struct SoundInfo che indica i campioni 
 * che stanno venendo suonati sui vari canali. I campioni vengono
 * mixxati al momento sempre con volume massimo, in futuro la cosa
 * si potra' cambiare.
 */

void handle_sound(void *unused, Uint8 * stream, int len)
{
    int i, amount;

    if (!sound_loaded)
        return;

//  +2 is because one channel is reserved for speaker comment and another
//  one for chorus.
   
    for (i = 0; i < (AVAILABLE_CHANNELS + 2); i++) {
        amount = 0;

        if (busy[i]) {

            amount = busy[i]->Length - busy[i]->Offset;

            if (amount > len)
                amount = len;

            if (amount > 0) {
//              D(bug("Mixxo canale %ld, %ld bytes (st: %lx)\n",i,amount,stream));

				if (busy[i]->Flags & SOUND_DISK) {
					Uint8 buffer[BUFFER_SIZE];

					fread(buffer, amount, 1, (FILE *) busy[i]->SoundData);
					SDL_MixAudio(stream, buffer, amount,
								 SDL_MIX_MAXVOLUME);
				} else
					SDL_MixAudio(stream,
								 ((Uint8 *) busy[i]->SoundData) +
								 busy[i]->Offset, amount,
								 SDL_MIX_MAXVOLUME);

                busy[i]->Offset += amount;
            }
        }

/*
 * spostato all'esterno dell'if su busy[i] per evitare il blocco
 * dei suoni loopanti che capitava in certe situazioni!
 */
        if (i >= AVAILABLE_CHANNELS && 
                (!busy[i] || busy[i]->Offset >= busy[i]->Length) ) {

            if (i == AUDIO_CROWD) {
                if (use_crowd && game_start)
                    busy[i] = handle_crowd();
            } else {
                if (use_speaker && game_start)
                    busy[i] = handle_speaker();
            }

            if (busy[i]) // metto un goto per non ripetere due volte il codice.
                goto reinit_sound;
        }

/*
 * In questo loop gestisco i suoni in loop e l'eventuale stop
 * della riproduzione nel caso in cui il suono sia da riprodurre
 * una sola volta, la label mi serve in quanto uso il codice
 * all'interno anche per reinizializzare i dati relativi a
 * cori & commenti.
 */
           if (busy[i] && busy[i]->Offset >= busy[i]->Length) {                
            if (busy[i]->Flags & SOUND_LOOP) {
reinit_sound:
                busy[i]->Offset = 0;

                if (busy[i]->Flags & SOUND_DISK) {
                    fseek((FILE *) busy[i]->SoundData,
                          busy[i]->FileOffset, SEEK_SET);
                } else if (amount < len) {
                    len -= amount;
                    SDL_MixAudio(stream + amount, busy[i]->SoundData,
                                 len, SDL_MIX_MAXVOLUME);
                    busy[i]->Offset += len;
                }
            } else
                busy[i] = NULL;
        }
    }
}

void SetCrowd(int s)
{
    if (use_crowd && !no_sound) {
        SDL_LockAudio();

/*
        if(s<0)
            busy[AUDIO_CROWD]=NULL;
        else
*/
        if (busy[AUDIO_CROWD])
            busy[AUDIO_CROWD]->Offset = busy[AUDIO_CROWD]->Length - 2;
        else if (s >= 0) {
            extern int playing;

			playing = s;
			busy[AUDIO_CROWD] = sound[s];
            if (sound[s])
    			sound[s]->Offset = 0;
		}

        wanted_sound = s;
        SDL_UnlockAudio();
    }
}

int32_t __saveds ETW_DiskPlay(void)
{
    D(bug("Warning called UNIMPLEMENTED DiskPlay!!!\n"));

    return 0;
}

/*
 * Avvio la riproduzione di un suono, in pratica quello che fa questa
 * funzione e' bloccare il semaforo dell'audio in modo da essere sicuri
 * che la callback non venga eseguita e imposta nell'array dei canali
 * il suono che si vuole riprodurre.
 */

int PlayBackSound(struct SoundInfo * si)
{
    register int i;

    if (no_sound)
        return -1;

    for (i = 0; i < AVAILABLE_CHANNELS; i++) {
        if (!busy[i]) {
            SDL_LockAudio();
            busy[i] = si;

            if (si->Flags & SOUND_DISK)
                fseek((FILE *) si->SoundData, si->FileOffset, SEEK_SET);

            si->Offset = 0;
            SDL_UnlockAudio();
            return i;
        }
    }

    D(bug("All virtual channels busy!\n"));

    return -1;
}

void PlayIfNotPlaying(int s)
{
    register int i;
    struct SoundInfo *si = sound[s];

    for (i = 0; i < AVAILABLE_CHANNELS; i++)
        if (busy[i] == si)
            return;

    PlayBackSound(si);
}

#ifndef MACOSX
static void convert_sound(struct SoundInfo *s)
{
    uint8_t *buffer, *destsnd;
    size_t length;

    if (s->Flags & SOUND_DISK) 
        return;

    if (!(buffer = malloc(s->Rate))) {
        D(bug("Error in convert_sound: not enough memory!\n"));
        return;
    }

    MakeRef(buffer, s->Rate, samplerate);

    // Sto giochetto per evitare overflow

    length = (long)((((double)samplerate) / ((double)s->Rate)) * ((double)s->Length));

    if ((destsnd = malloc(length + 2048))) { // be sure about possible overflows
        register int l = s->Length, k = 0, t;
        register uint8_t *src = s->SoundData, *dst = destsnd;

        while (l--) {
            t = buffer[k++];

            while (t--)
                *dst++ = *src;

            src++;

            if (k == s->Rate)
                k = 0;
        }

        D(bug("Convert sound: source length %ld, planned length %ld, final length %ld\n",
                    s->Length, length, dst - destsnd));

        length = dst - destsnd;
    } else {
        D(bug("There is not enough free memory!\n"));
        return;
    }

    free(buffer);

    SDL_FreeWAV(s->SoundData); // a sound is never converted twice so I've not to make any check

    s->Flags |= SOUND_CONVERTED;
    s->Length = length;
    s->Rate = samplerate;
    s->SoundData = s->LeftData = destsnd;

}
#endif

BOOL InitSoundSystem(void)
{
    SDL_AudioSpec fmt;

    D(bug("Initializing audio channels...\n"));

    fmt.freq = samplerate;

/* AC: I don't know under other system (I think that it is a currently bug of the SDL version)
 * but under X, when using 8bit format, the audio is distorced.
 * --> Now, I'm using this value that works perfectly. <--
 */
// for linux the correct value is U8
// for OSX it seems S8
// for win32? (test)
//#ifdef MACOSX
#if 0
    fmt.format = AUDIO_U16;
#else
    fmt.format = AUDIO_U8; // signed o unsigned?!?!?
#endif
    fmt.samples = BUFFER_SIZE;
    fmt.callback = handle_sound;
    fmt.channels = 1;
    fmt.userdata = NULL;

    if (SDL_OpenAudio(&fmt, &obt) < 0) {
        D(bug("Unable to open audio: %s\n", SDL_GetError()));
        return FALSE;
    }

/*
 * MacOSX ha problemi a convertirsi l'audio da solo, al contrario 
 * winzozz, linux e amiga lo fanno bene. Tutti i campioni al momento
 * sono in AUDIO_S8, e macos non riesce a convertire verso tale formato,
 * la scelta e' tra cercare il bug in macos o passare tutti i campioni
 * in altri formati (ad esempio wav).
 */
    D(bug("AUDIO: Asking for %d/%d/%d/%d, obtained %d/%d/%d/%d\n",
                fmt.freq, fmt.format, fmt.samples, fmt.channels,
                obt.freq, obt.format, obt.samples, obt.channels));

#ifndef MACOSX
    if(obt.format & 0x10) {
        D(bug("*** TARGET needs 16bit audio, UNSUPPORTED, exiting ***"));
        return FALSE;
    }
#endif

    if(obt.freq != samplerate) {
        D(bug("*** Forcing frequency %d\n", obt.freq));
        samplerate = obt.freq;
    }
    
    sound_started = 1;
    return TRUE;
}

void FreeSoundSystem(void)
{
    SDL_CloseAudio();

    sound_started = 0;

    D(bug("Done.\n"));
}

void FreeSound(struct SoundInfo *s)
{   
    if (s->Flags & SOUND_CONVERTED)
        free(s->SoundData);
    else
        SDL_FreeWAV(s->SoundData);

    free(s);
}

struct SoundInfo *LoadSound(char const *Name)
{
    char buf[1024];
    BOOL loop = FALSE;
    Uint8 *buffer;
    Uint32 len;
    SDL_AudioSpec spec;

// this version of ETW doesn't support anymore DISK or FILE samples
    if (*Name == '+' || *Name == '-')
        Name++;

    if (*Name == '.') {
        Name++;
        loop = TRUE;
    }

    D(bug("Loading a WAV sample... [%s]", Name));

    sprintf(buf, GAME_DIR "%s", Name);

    if ( SDL_LoadWAV(buf, &spec, &buffer, &len)) {
        struct SoundInfo *s;
        
        if ( (s = (struct SoundInfo *) calloc(sizeof(struct SoundInfo), 1))) {
            s->Flags = SOUND_FAST;

            if (loop)
                s->Flags |=    SOUND_LOOP;

            s->Volume = 63; // unused
            s->Rate = spec.freq; // unused
            s->Length = len;
            s->SoundData = s->LeftData = buffer;

            D(bug(" length: %d rate: %d\n", len, spec.freq));

            /* AC: I don't know if this conversion is needed for the other system, but is strongly
             * necessary under OS X. ^_^
             */
#ifdef MACOSX
            /* 12/07/04 - If sound isn't started, obt is an invalid structure! */
            if(sound_started && (spec.format != obt.format || spec.freq != obt.freq))
            {
                SDL_AudioCVT  wav_cvt;
                
                D(bug("Convert sound: original format %x, needed format %x\n",
                    spec.format,obt.format));

                /* Build AudioCVT */
                if(SDL_BuildAudioCVT(&wav_cvt,
                        spec.format,spec.channels,spec.freq,
                        obt.format, obt.channels,obt.freq) != -1) 
                {
                    /* Setup for conversion */
                    if((wav_cvt.buf = malloc(len * wav_cvt.len_mult)) != NULL)
                    {
                        wav_cvt.len = len;
                        memcpy(wav_cvt.buf,buffer,len);

                        /* We can delete the original WAV data now */
                        SDL_FreeWAV(buffer);

                        /* And now we're ready to convert */
                        SDL_ConvertAudio(&wav_cvt);
                        s->Flags |= SOUND_CONVERTED;
                        s->Length = wav_cvt.len*wav_cvt.len_ratio;
                        s->SoundData = s->LeftData = wav_cvt.buf;
                    }            
                }
            }    
            /* The default is returning the unconverted sound? */
#else
            if (s->Rate < (samplerate-1000) || s->Rate > (samplerate+1000))
                convert_sound(s); // convert sample if needed
#endif            
            return s;
        }
        SDL_FreeWAV(buffer);
    }

    D(bug("\n*** Unable to load sample!\n"));

    return NULL;
}


BOOL CaricaSuoni(void)
{
    BOOL ok = TRUE;
    int i;

    for(i = 0; i < (AVAILABLE_CHANNELS + 2); i++)
        busy[i] = NULL;

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
    SDL_LockAudio();

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

    for(i = 0; i < (AVAILABLE_CHANNELS + 2); i++)
        busy[i] = NULL;

	SDL_UnlockAudio();
	D(bug("Ok.\n"));
}

BOOL SoundStarted(void)
{
    return sound_started;
}

void os_start_audio(void)
{
    if (!no_sound)
        SDL_PauseAudio(0);
}

void os_stop_audio(void)
{
    if (!no_sound)
        SDL_PauseAudio(1);
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

	for(i = 0; i < (AVAILABLE_CHANNELS + 2); i++)
		busy[i] = NULL;

    sound_loaded = TRUE;

    return TRUE;

}

void LiberaSuoniMenu(void)
{
    int i = 0;

    os_stop_audio();

    if(!sound_loaded)
        return;

	SDL_LockAudio();


	while (menu_soundname[i]) {
		if (menusound[i]) {
			FreeSound(menusound[i]);
			menusound[i] = NULL;
		}
		i++;
	}

    for(i = 0; i < (AVAILABLE_CHANNELS + 2); i++)
        busy[i] = NULL;

	SDL_UnlockAudio();

	sound_loaded = FALSE;
}
