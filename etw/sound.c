/*
Routine di controllo dei samples
*/
#define SOUND_C

#include "eat.h"
#include "myiff.h"
#include "8svx.h"

#include "preinclude.h"
#include "menu.h"

#define ID_CHAN MAKE_ID('C','H','A','N')	/* Sound channel information. */

/* Maximum replay volume. */

#define MAX_VOLUME      64

/* Channel definitions. */

#define SAMPLE_LEFT     2
#define SAMPLE_RIGHT    4
#define SAMPLE_STEREO   6

/* A fixed-point value, 16 bits to the left of
* the point and 16 to the right. A Fixed is a
* number of 2**16ths, i.e. 65536ths.
*/

/* Unity = Fixed 1.0 = maximum volume */

#define Unity           0x10000L

/* Sample compression modes. */

#define sCmpNone        0
#define sCmpFibDelta    1

struct BufferInfo *BufferInfo;

struct SoundInfo *busy[AVAILABLE_CHANNELS + 2] = { 0 };

static int samplerate = 44100; // era 44100
static int signed_samples = 0;

static int sound_started = 0;
static int sound_loaded = FALSE;

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

// il +2 e' xche' riservo un canale per il commento e uno per il tifo
    
	for (i = 0; i < (AVAILABLE_CHANNELS + 2); i++) {
        amount = 0;

		if (busy[i]) {

			amount = busy[i]->Length - busy[i]->Offset;

			if (amount > len)
				amount = len;

			if (amount > 0) {
//              D(bug("Mixxo canale %ld, %ld bytes (st: %lx)\n",i,amount,stream));

				if (busy[i]->Flags & SOUND_DISK) {
					char buffer[BUFFER_SIZE];

					fread(buffer, amount, 1, (FILE *) busy[i]->SoundData);
					SDL_MixAudio(stream, buffer, amount,
								 SDL_MIX_MAXVOLUME);
				} else
					SDL_MixAudio(stream,
								 ((char *) busy[i]->SoundData) +
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
			sound[s]->Offset = 0;
		}

		wanted_sound = s;
		SDL_UnlockAudio();
	}
}

LONG __saveds ETW_DiskPlay(void)
{
	D(bug("Attenzione chiamata DiskPlay!!!\n"));

	return 0;
}

/*
 * Avvio la riproduzione di un suono, in pratica quello che fa questa
 * funzione e' bloccare il semaforo dell'audio in modo da essere sicuri
 * che la callback non venga eseguita e imposta nell'array dei canali
 * il suono che si vuole riprodurre.
 */

LONG PlayBackSound(struct SoundInfo * si)
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

	D(bug("Canali tutti occupati!\n"));

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


void convert_sound(struct SoundInfo *s)
{
	UBYTE *buffer, *destsnd;
	long length;

    if(!signed_samples) // se questo e' definito devo far SEMPRE la conversione
    	if (s->Flags & SOUND_DISK || 
                (s->Rate > (samplerate-2000) && s->Rate < (samplerate+2000)))
		    return;

	if (!(buffer = malloc(s->Rate))) {
		D(bug("Errore nella conversione del suono!\n"));
		return;
	}

	MakeRef(buffer, s->Rate, samplerate);

	// Sto giochetto per evitare overflow

	length = (long)((((double)samplerate)/ ((double)s->Rate)) * ((double)s->Length));

	if (!(destsnd = malloc(length + 512))) {
		D(bug("Non c'e' memoria!\n"));
		return;
	}

	{
		register int l = s->Length, k = 0, t;
		register unsigned char *src = s->SoundData, *dst = destsnd;

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
	}

	free(buffer);
	free(s->SoundData);

    s->Length = length;
	s->Rate = samplerate;
	s->SoundData = s->LeftData = destsnd;

// converto i campioni da signed ad unsigned e viceversa
    if(signed_samples) {
        while(length--)
            *destsnd++ ^= 0x80;
    }
}

BOOL InitSoundSystem(void)
{
	SDL_AudioSpec fmt, obt;

	D(bug("Inizializzo i canali audio...\n"));

	fmt.freq = samplerate;
#ifdef LINUX
	fmt.format = AUDIO_U8; // su linux mi serve U8, su MacOSX probabilmente S8
#else
	fmt.format = AUDIO_S8;
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
    D(bug("AUDIO: Richiesti %d/%d/%d/%d, ottenuti %d/%d/%d/%d\n",
                fmt.freq, fmt.format, fmt.samples, fmt.channels,
                obt.freq, obt.format, obt.samples, obt.channels));

    if(obt.format & 0x10) {
        D(bug("*** Il TARGET richiede audio a 16bit, NON supportato, esco ***"));
        return FALSE;
    }
    if(obt.freq != samplerate) {
        D(bug("*** Forzo la frequenza a %d\n", obt.freq));
        samplerate = obt.freq;
    }

// non sono tanto sicuro di questa modifica... sembra che per alcuni campioni
// vada bene, per altri no...
#ifndef LINUX
    if(obt.format != fmt.format) {
        D(bug("*** Forzo lo swap del segno dei campioni.\n"));
        signed_samples = 1;
    }
    else
        signed_samples = 0;
#else
    signed_samples = 1;
#endif
    
	sound_started = 1;
	return TRUE;
}

struct SoundInfo *LoadRAWSample(char *name, int rate)
{
	FILE *fh;
	struct SoundInfo *s;
	BOOL loop = FALSE, disk = FALSE;

	// Rimosso il codice di supporto dei vari tipi di suoni...

	if (*name == '+' || *name == '-') {
		name++;
	}

	if (*name == '.') {
		name++;
		loop = TRUE;
	}

	D(bug("Carico un suono RAW... [%s]\n", name));

	if ((fh = fopen(name, "rb"))) {
		if ((s = malloc(sizeof(struct SoundInfo)))) {
			if (loop)
				s->Flags = SOUND_LOOP;
			else
				s->Flags = 0L;

			fseek(fh, 0, SEEK_END);
			s->Length = ftell(fh);
			fseek(fh, 0, SEEK_SET);

			s->Rate = rate;
			s->Volume = 60 << 10;

			if (disk) {
				s->Flags |= SOUND_DISK;
				s->SoundData = (APTR) fh;
				s->Offset = 0;
				s->FileOffset = 0;
				return s;
			} else if ((s->SoundData = malloc(s->Length))) {
				s->Flags |= SOUND_FAST;
				fread(s->SoundData, s->Length, 1, fh);

				fclose(fh);
				return s;
			}

			free(s);
		}
		fclose(fh);
	}

	return NULL;
}

void FreeSoundSystem(void)
{
	SDL_CloseAudio();

	sound_started = 0;

	D(bug("Fatto\n"));
}


/* DeltaUnpack(UBYTE *Src,ULONG Size,BYTE *Dst):
*
*      Unpack Fibonacci-delta-encoded data.
*/

static void DeltaUnpack(UBYTE * Src, ULONG Size, BYTE * Dst)
{
	static BYTE CodeToDelta[16] =
		{ -34, -21, -13, -8, -5, -3, -2, -1, 0, 1, 2, 3, 5, 8, 13, 21 };

	BYTE Value = (BYTE) Src[1];
	UBYTE Code;

	/* Skip the header information. */

	Src += 2;
	Size -= 2;

	/* Run down the chunk... */

	while (Size--) {
		Code = *Src++;

		/* Add the top nibble delta. */

		Value += CodeToDelta[Code >> 4];

		*Dst++ = Value;

		/* Add the bottom nibble delta. */

		Value += CodeToDelta[Code & 0xF];

		*Dst++ = Value;
	}
}

/* FreeSound(struct SoundInfo *SoundInfo):
*
*      Free sound handle and associated data.
*/

void FreeSound(struct SoundInfo *SoundInfo)
{
	if (SoundInfo->Flags & SOUND_DISK)
		fclose((FILE *) SoundInfo->SoundData);
	else
		free(SoundInfo->SoundData);

	free(SoundInfo);
}

/* LoadSound(STRPTR Name):
*
*      Load a sound file from disk.
*/

struct SoundInfo *LoadSound(STRPTR Name)
{
	struct SoundInfo *SoundInfo = NULL;
	struct IFFHandle *Handle;
	BOOL rawmode = FALSE, loop = FALSE, disk = FALSE;

	if (*Name == '+' || *Name == '-') {
#if 0
		if (*Name == '-' && !audio2fast)
			disk = TRUE;
#endif
		Name++;
	}

	if (*Name == '.') {
		Name++;
		loop = TRUE;
	}

	D(bug("Carico un suono IFF... [%s]\n", Name));
	/* Allocate IFF handle for reading. */

	if ((Handle = AllocIFF())) {
		/* Open a standard DOS stream. */

		if ((Handle->iff_Stream = fopen(Name, "rb"))) {
			char buff[4];

			/* Say it's a DOS stream. */

			fread(buff, 1, 4, Handle->iff_Stream);

			if (!strnicmp(buff, "FORM", 4)) {
				fseek(Handle->iff_Stream, 0, SEEK_SET);

				InitIFFasDOS(Handle);

				/* Open the file for reading. */

				if (!OpenIFF(Handle, IFFF_READ)) {
					ULONG SoundStops[3 * 2] = {
						ID_8SVX, ID_VHDR,
						ID_8SVX, ID_CHAN,
						ID_8SVX, ID_BODY
					};

					/* Mark the chunks to stop at. */

					if (!StopChunks(Handle, (LONG *) SoundStops, 3)) {
						struct ContextNode *Chunk;
						Voice8Header Header;
						UBYTE Compression = 0;
						ULONG Channel = SAMPLE_STEREO;
						BYTE SingleChannel = TRUE;

						/* Clear the voice header. */

						memset(&Header, 0, sizeof(Voice8Header));

						/* Scan for data... */

						while (!ParseIFF(Handle, IFFPARSE_SCAN)) {
							Chunk = CurrentChunk(Handle);

							/* What did we find? */

							switch (Chunk->cn_ID) {
								/* Looks like the basic voice header. */

							case ID_VHDR:

								/* Read the header. */

								if (ReadChunkRecords
									(Handle, &Header,
									 min(Chunk->cn_Size,
										 sizeof(Voice8Header)), 1) == 1) {
									/* Compression type supported? */
									SWAP_WORD(Header.samplesPerSec);
									SWAP_LONG(Header.oneShotHiSamples);
									SWAP_LONG(Header.repeatHiSamples);
									SWAP_LONG(Header.samplesPerHiCycle);
									SWAP_LONG(Header.volume);
									if (Header.sCompression == sCmpNone
										|| Header.sCompression ==
										sCmpFibDelta) {
										/* Allocate the sound handle. */

										if ( (SoundInfo =
											(struct SoundInfo *)
											malloc(sizeof
												   (struct SoundInfo)))) {
											/* Install the rate, volume and length. */
											if (!disk)
												SoundInfo->Flags =
													SOUND_FAST;
											else
												SoundInfo->Flags =
													SOUND_DISK;

											if (loop)
												SoundInfo->Flags |=
													SOUND_LOOP;

											SoundInfo->Volume =
												(((Header.volume *
												   MAX_VOLUME) /
												  Unity) << 10) - 1;
											SoundInfo->Rate =
												Header.samplesPerSec;
											SoundInfo->Length =
												Header.
												oneShotHiSamples ? Header.
												oneShotHiSamples : Header.
												repeatHiSamples;

											/* Remember compression mode. */

											Compression =
												Header.sCompression;
										}
									}
								}

								break;

								/* Looks like sound channel information. */

							case ID_CHAN:

								/* Do we have a handle to manage it? */

								if (SoundInfo) {
									/* Read the channel information. */

									if (ReadChunkRecords
										(Handle, &Channel,
										 min(Chunk->cn_Size,
											 sizeof(ULONG)), 1) == 1) {
										/* Stereo sound file? */

										SWAP_LONG(Channel);

										if (Channel == SAMPLE_STEREO)
											SingleChannel = FALSE;
										else if (Channel != SAMPLE_LEFT
												 && Channel !=
												 SAMPLE_RIGHT)
											D(bug
											  ("** Attenzione tipo di CHAN non supportato %d!!!\n",
											   Channel));
									} else {
										free(SoundInfo);

										SoundInfo = NULL;
									}
								}

								break;

								/* Looks like the sound body data. */

							case ID_BODY:

								/* Do we have sound handle to manage it? */

								if (SoundInfo) {
									BYTE Success = FALSE;

									/* Uncompressed raw data? */

									if (Compression == sCmpNone) {
										ULONG Length = min(Chunk->cn_Size,
														   SoundInfo->
														   Length);

										/* Allocate a buffer. */
										SoundInfo->Length = Length;

										if (disk) {
											SoundInfo->SoundData =
												(APTR) Handle->iff_Stream;
											SoundInfo->FileOffset =
												ftell(Handle->iff_Stream);
											CloseIFF(Handle);
											FreeIFF(Handle);
											D(bug
											  ("Offset per il playing da disco: %ld\n",
											   SoundInfo->Offset));
											return SoundInfo;
										} else if ( (SoundInfo->SoundData =
												   malloc(Length)) ) {
											/* Read the data. */

											if (ReadChunkRecords
												(Handle,
												 SoundInfo->SoundData,
												 Length, 1) == 1) {
												/* Store the actual length. */

												SoundInfo->Length = Length;

												Success = TRUE;
											} else
												free(SoundInfo->SoundData);
										}
									} else if (!disk) {
										ULONG Length = Chunk->cn_Size;
										UBYTE *TempBuffer;

										/* Allocate a temporary decompression buffer. */

										if ( (TempBuffer =
											(UBYTE *) malloc(Length)) ) {
											/* Read the compressed data. */

											if (ReadChunkRecords
												(Handle, TempBuffer,
												 Length, 1) == 1) {
												/* Allocate space for the uncompressed data. */

												if ( (SoundInfo->SoundData =
													malloc((Length -
															2) * 2)) ) {
													/* Stereo sound file? */

													if (!SingleChannel
														&& Channel ==
														SAMPLE_STEREO) {
														UBYTE *Data =
															SoundInfo->
															SoundData;

														/* Unpack the stereo sound. */

														DeltaUnpack
															(TempBuffer,
															 Length / 2,
															 Data);
														DeltaUnpack
															(TempBuffer +
															 (Length / 2),
															 Length / 2,
															 Data +
															 (Length - 4));

														/* Remember the sound length. */

														SoundInfo->Length =
															(Length -
															 4) * 2;
													} else {
														/* Unpack the mono sound. */

														DeltaUnpack
															(TempBuffer,
															 Length,
															 SoundInfo->
															 SoundData);

														/* Remember the sound length. */

														SoundInfo->Length =
															(Length -
															 2) * 2;
													}

													Success = TRUE;
												}
											}

											free(TempBuffer);
										}
									} else {
										D(bug
										  ("** Suoni con compressione non utilizzabili da disco!\n"));
										Success = FALSE;
									}

									if (!Success) {
										free(SoundInfo);

										SoundInfo = NULL;
									}
								}

								break;
							}
						}

						/* Did we get what we wanted? */

						if (SoundInfo) {
							/* Any sound data allocated? */

							if (SoundInfo->SoundData) {
								UBYTE *Data = SoundInfo->SoundData;

								/* Which kind of sound file did
								   * we read?
								 */

								switch (Channel) {
									/* Left channel only. */

								case SAMPLE_LEFT:

									SoundInfo->LeftData = Data;
									break;

									/* Right channel only. */

								case SAMPLE_RIGHT:

									SoundInfo->RightData = Data;
									break;

									/* Two stereo channels. */

								case SAMPLE_STEREO:

									/* One sound mapped to two voices. */

									if (SingleChannel)
										SoundInfo->LeftData =
											SoundInfo->RightData = Data;
									else {
										/* Split the voice data. */

										SoundInfo->Length =
											SoundInfo->Length / 2;

										SoundInfo->LeftData = Data;
										SoundInfo->RightData =
											Data + SoundInfo->Length;
									}

									break;
								}
							} else {
								free(SoundInfo);

								SoundInfo = NULL;
							}
						}
					}
					CloseIFF(Handle);
				}
			} else
				rawmode = TRUE;

			fclose(Handle->iff_Stream);
		}
		FreeIFF(Handle);
	}

	/* Successful? */

	if (!SoundInfo && !rawmode)
		D(bug("*** Attenzione non posso caricare il suono!\n"));

	if (!rawmode)
		return (SoundInfo);
	else
		return LoadRAWSample(Name, 10000);
}


BOOL CaricaSuoni(void)
{
	BOOL ok = TRUE;
	int i;

	for(i = 0; i < (AVAILABLE_CHANNELS + 2); i++)
		busy[i] = NULL;

	for (i = 0; i < (NUMERO_SUONI + 4); i++)
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
			D(bug("Non trovo %s!\n", soundname[i]));
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

	sound[NUMERO_SUONI] = NULL;


	for (i = 0; i < NUMERO_SUONI; i++)
		if (sound[i] && sound[i]->Rate != samplerate)
			convert_sound(sound[i]);

	// Trucchi per risparmiare memoria su alcuni suoni!

    for(i = (NUMERO_SUONI + 1) ; i< (NUMERO_SUONI+4); i++) {
   	    sound[i] = malloc(sizeof(struct SoundInfo));
    }

	sound[NUMERO_SUONI + 2]->SoundData =
		sound[NUMERO_SUONI + 1]->SoundData = sound[SHOOT]->SoundData;
	sound[NUMERO_SUONI + 2]->Length = sound[NUMERO_SUONI + 1]->Length =
		sound[SHOOT]->Length;
	sound[NUMERO_SUONI + 2]->Rate = sound[NUMERO_SUONI + 1]->Rate =
		sound[SHOOT]->Rate;
	sound[NUMERO_SUONI + 1]->Volume = 5;
	sound[NUMERO_SUONI + 2]->Volume = 30;

	sound[NUMERO_SUONI + 1]->Volume <<= 10;
	sound[NUMERO_SUONI + 2]->Volume <<= 10;

	sound[NUMERO_SUONI + 1]->Flags = sound[NUMERO_SUONI + 2]->Flags =
		SOUND_NORMAL;


	return TRUE;

}


void LiberaSuoni(void)
{
	int i = 0;

	os_stop_audio();

	D(bug("Libero i suoni...\n"));

	while (soundname[i]) {
		if (sound[i])
			FreeSound(sound[i]);
		i++;
	}

	if (sound[NUMERO_SUONI + 1])
		free(sound[NUMERO_SUONI + 1]);

	if (sound[NUMERO_SUONI + 2])
		free(sound[NUMERO_SUONI + 2]);

	for(i = 0; i < (AVAILABLE_CHANNELS + 2); i++)
		busy[i] = NULL;

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
			D(bug("Non trovo %s!\n", menu_soundname[i]));
		}

		convert_sound(menusound[i]);

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

	for(i = 0; i < (AVAILABLE_CHANNELS + 2); i++)
		busy[i] = NULL;

	sound_loaded = FALSE;
}
