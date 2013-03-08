/* Includes di Eat The Whistle */

// #define USE_BUILTIN_MATH

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if 0
#include <exec/exec.h>
#include <proto/dos.h>
#include <proto/asl.h>
#include <proto/timer.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/diskfont.h>
// #include <proto/iffparse.h>
#include <proto/gadtools.h>
#include <proto/lowlevel.h>

#include <dos/dos.h>
/*
#include <dos/exall.h>
#include <dos/rdargs.h>
*/
#include <dos/dosextens.h>
#include <dos/dostags.h>
/*
#include <rexx/rxslib.h>
#include <rexx/storage.h>
#include <workbench/startup.h>
*/
#include <intuition/intuition.h>
/*
#include <intuition/classes.h>
#include <intuition/icclass.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
*/
#include <graphics/gfx.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <graphics/scale.h>
#include <graphics/view.h>
#include <graphics/rpattr.h>
#include <graphics/videocontrol.h>

#undef GetOutlinePen

#include <graphics/gfxmacros.h>
#include <hardware/blit.h>
#include <libraries/commodities.h>
#include <libraries/locale.h>
#include <libraries/gadtools.h>
#include <libraries/lowlevel.h>
#include <libraries/asl.h>
#include <utility/tagitem.h>
#include <clib/alib_protos.h>
// #include <libraries/iffparse.h>

#ifdef __PPC__
    #include <powerup/ppclib/interface.h>
    #include <powerup/ppclib/message.h>
    #include <powerup/ppclib/tasks.h>
    #include <powerup/ppclib/memory.h>
    #include <powerup/gcclib/powerup_protos.h>
#endif
#else
#include "SDL.h"
#include "SDL_timer.h"
#include "SDL_audio.h"
#include "SDL_events.h"

#endif
