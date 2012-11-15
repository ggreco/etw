#ifndef SOUND_H

#define SOUND_H

#ifndef MENU
    #include "suoni.h"
#endif

    /* Double buffering chunk size. */

#ifndef __AROS__
#define BUFFER_SIZE    2048
#else
#define BUFFER_SIZE    4096
#endif

    /* The voice header. */

struct BufferInfo
{
    UBYTE    Buffer[BUFFER_SIZE];
};

    /* Sound replay information. */



#define SOUND_NORMAL 1
#define SOUND_FAST   2
#define SOUND_DISK   4
#define SOUND_LOOP   8
#define SOUND_CONVERTED 16

struct SoundInfo
{
    ULONG    Rate, Length, Volume,    Flags;
    // ULONG Loops; ???

    ULONG    Offset;
    void    *SoundData;

    void    *LeftData, *RightData;
    long    FileOffset;
};

extern void os_start_audio(void);
extern void os_stop_audio(void);
extern void ReplaySound(struct SoundInfo *SoundInfo);
extern void FreeSound(struct SoundInfo *SoundInfo);
extern struct SoundInfo *LoadSound(char const *Name);
extern int PlayBackSound(struct SoundInfo *si);
extern struct SoundInfo *LoadRAWSample(char const *name, int rate);
extern BOOL InitSoundSystem(void);
extern void FreeSoundSystem(void);
extern BOOL CaricaSuoniMenu(void);
extern void LiberaSuoniMenu(void);
extern BOOL SoundStarted(void);
#endif
