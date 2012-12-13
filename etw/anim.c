#include "etw_locale.h"
/*
**
**  $VER: dispatch.c 1.12 (12.11.97)
**  anim.datatype 1.12
**
**  Dispatch routine for a DataTypes class
**
**  Written 1996/1997 by Roland 'Gizzy' Mainz
**  Original example source from David N. Junod
**
*/

#define mysprintf sprintf


#include "menu.h"
#include "anim.h"
#include <time.h>
#include "myiff.h"
#include "SDL.h"

#define AddTail MyAddTail
#define AddHead MyAddHead
#define RemTail MyRemTail
#define NewList MyNewList
#define Remove MyRemove

#define MAX(a,b) max(a,b)
#define MIN(a,b) min(a,b)
#define ABS(a) abs(a)

LONG LoadFrameNode(struct AnimInstData *aid,struct FrameNode *fn);

typedef struct
{
  UBYTE *PlanePtr;            /* max 8 BitPlane-Pointers */
  APTR  next;
}  PTR_RING;


/*****************************************************************************/

/* local prototypes */
static                 BOOL                 FreeAbleFrame( struct AnimInstData *, struct FrameNode * );
static                 struct FrameNode    *AllocFrameNode( void );
static                 struct FrameNode    *FindFrameNode( struct MyList *, ULONG );
static                 void                 CopyBitMap( struct BitMap *, struct BitMap * );
static                 void                 XCopyMem( APTR, APTR, ULONG );
static                 void                 ClearBitMap( struct BitMap * );
static                 void                 XORBitMaps( struct BitMap *, struct BitMap * );
static                 struct BitMap       *AllocBitMapPooled( ULONG, ULONG, ULONG);
static                 APTR                 AllocVecPooled( APTR, ULONG );
static                 void                 FreeVecPooled( APTR, APTR );
static                 LONG                 DrawDLTA( struct AnimInstData *, struct BitMap *, struct BitMap *, struct AnimHeader *, UBYTE *, ULONG );
static                 void                 DumpAnimHeader( struct AnimInstData *, ULONG, struct AnimHeader * );
static                 struct FrameNode    *GetPrevFrameNode( struct FrameNode *, ULONG );

void DeltaUnpack(struct BitMap *f_bm,void *f_dlta_adr,long f_mode);
void BodyToBitMap(struct BitMap *f_bm,struct BitMapHeader *bmh,UBYTE *f_body,LONG dltasize);
void MakeYTable(short f_BytesPerRow);

/*
extern __asm void decode_plane(register __a0 UBYTE *,register __a5 UBYTE *,
                register __a2 PLANEPTR,register __d2 LONG,
                register __a3 short *,register __d0 LONG);
*/

void C_BdyUnpack(UBYTE *,PTR_RING *,LONG, LONG, LONG);

struct BitMap *Temp = NULL;

short  g_ytable[128];

void FreeFrames(struct AnimInstData *a)
{
    register struct FrameNode *fn,*fnsucc;

    for(fn=(struct FrameNode *)a->aid_FrameList.pHead;
        fn->fn_Node.mpNext ; fn=fnsucc)
    {
            fnsucc=(struct FrameNode*)fn->fn_Node.mpNext;
            if (fn->fn_BitMap)
                free(fn->fn_BitMap);
            free(fn);
    }
    free(a);
}

struct FrameNode *GetFrameNode(struct AnimInstData *a,int n)
{
    register struct FrameNode *fn;

    for(fn=(struct FrameNode *)a->aid_FrameList.pHead;
        fn->fn_Node.mpNext && n>0; fn=(struct FrameNode *)fn->fn_Node.mpNext,n--);

    return fn;
}

void DisplayAnim(struct AnimInstData *a)
{
    LONG i=0,l=0,t,s=0;
    extern void blit_anim(struct BitMap *, SDL_Rect *);
    struct FrameNode *fn;
    struct BitMap *bm;
    uint32_t sclk = os_get_timer();
    int w = a->aid_BMH->bmh_Width, h = a->aid_BMH->bmh_Height;
    float dst_ratio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
          src_ratio = (float)w / (float)h;

    if (!create_anim_context(w, h)) {
        D(bug("Unable to create animation texture!\n"));
        return;
    }

    SDL_Rect srect = {0, 0, w, h};

    if (dst_ratio > src_ratio) {
        int wanted_height = ((float)w / dst_ratio);
        srect.h = wanted_height;
        srect.y = (h - wanted_height) / 2;
    }
    else if (dst_ratio < src_ratio) {
        int wanted_width = ((float)h / dst_ratio);
        srect.w = wanted_width;
        srect.x = (w - wanted_width) / 2;
    }

    CopyBitMap(((struct FrameNode *)a->aid_FrameList.pHead)->fn_BitMap, Temp);

    for(fn=(struct FrameNode *)a->aid_FrameList.pHead;
        fn->fn_Node.mpNext;fn=(struct FrameNode *)fn->fn_Node.mpNext) {
        if(!fn->fn_BitMap) {
            DeltaUnpack(Temp,fn->delta,0);
            bm=Temp;
        }
        else 
            bm=fn->fn_BitMap;

        l++;

        if(fn->fn_Sample) {
            fn->fn_Sample->Rate=fn->fn_Rate;
            fn->fn_Sample->Volume=fn->fn_Volume;
            fn->fn_Sample->Loops=fn->fn_Loops;
            PlayBackSound(fn->fn_Sample);
        }

        t = os_get_timer() - sclk;

        // frameskip is implemented here
        if (fn->fn_Node.mpNext->mpNext && t > ((struct FrameNode *)fn->fn_Node.mpNext)->Clock) {
            s++;
            continue;
/*
            fn=(struct FrameNode *)fn->fn_Node.pNext;
            DeltaUnpack(Temp,(LONG)fn->delta,0);
            fn->fn_BitMap=Temp;
*/
        }

        // this function convert the bitmap to the texture and display it
        blit_anim(bm, &srect);

        if( t < (fn->Clock - 19) ) {
            SDL_Delay(fn->Clock - 18 - t);
            i++;
        }

/*
        if( !(l&1) )
        {
            if( (ReadJoyPort(0)&JPF_BUTTON_RED) ||
                (ReadJoyPort(1)&&JPF_BUTTON_RED) )
                break;
        }
*/
    }

    t = os_get_timer();

    if ( (t - sclk) > 0) {
        D(bug("%ld waits on %ld frames. (Rate: %ld fps, skip: %ld)\n", i , l , l * 1000 / (t - sclk), s));
    }
    delete_anim_context();
}


LONG MergeAnim(struct AnimInstData *aid,FILE *fh)
{
      LONG error=0;
      BOOL new=IsListEmpty(&aid->aid_FrameList);

      struct IFFHandle *iff = NULL;

#define NUM_PROPCHUNKS (9L)

      if ((iff = AllocIFF())) {
          ULONG                pos        = 0UL,curframe=0L;      /* current file pos in IFF stream  */
          ULONG                animwidth  = 0UL,                /* anim width                      */
                               animheight = 0UL,                /* anim height                     */
                               animdepth  = 0UL;                /* anim depth                      */
          ULONG                timestamp  = 0UL;                /* timestamp                       */
          ULONG                minreltime = 1UL,                /* Maximum ah_RelTime value        */
                               maxreltime = 0UL;                /* Minimum ah_RelTime              */
          struct StoredProperty *bmhdprop       = NULL, /* ILBM BMHD (struct BitMapHeader)        */
                                *dpanprop       = NULL; /* DPaint DPAN chunk                      */
          const LONG propchunks[ (NUM_PROPCHUNKS * 2) ] = {
              ID_ILBM, ID_BMHD,
              ID_ILBM, ID_CAMG,
              ID_ILBM, ID_GRAB,
              ID_ILBM, ID_DPAN,
              ID_ILBM, ID_ANNO,
              ID_ILBM, ID_AUTH,
              ID_ILBM, ID_Copyright,
              ID_ILBM, ID_FVER,
              ID_ILBM, ID_NAME
          };

          iff->iff_Stream=fh;

          InitIFFasDOS(iff);

          if (!OpenIFF(iff, IFFF_READ)) {
              if ( !(error = PropChunks( iff, (int32_t *)propchunks, NUM_PROPCHUNKS )) )  {
#define NUM_STOPCHUNKS (5L)
                  const LONG stopchunks[ (NUM_STOPCHUNKS * 2) ] = {
                      ID_ILBM, ID_FORM,
                      ID_ILBM, ID_ANHD,
                      ID_ILBM, ID_CMAP,
                      ID_ILBM, ID_BODY,
                      ID_ILBM, ID_DLTA
                  };

                  if ( !(error = StopChunks( iff, (int32_t *)stopchunks, NUM_STOPCHUNKS )) ) {
                      struct FrameNode *fn         = NULL;

                      /* Scan IFF stream until an error or an EOF occurs */
                      for (;;) {
                          struct ContextNode *cn;

                          if ( (error = ParseIFF( iff, IFFPARSE_SCAN ) ))  {
                              /* EOF (End Of File) is no error here... */
                              if( error == IFFERR_EOF ) {
//                                  D(bug("End of file reached\n"));
                                  error = 0L;
                              }
                              break;
                          }

                          /* Get file position */
#if 0
                          if ( (cn = CurrentChunk( iff ) ))  {
                              pos = 0UL;

                              while ((cn = ParentChunk(cn)))
                                  pos += cn -> cn_Scan;
                          }
#else
                          pos = ftell(fh);
#endif
                          /* bmhd header loaded ? */
                          if ( bmhdprop == NULL ) {
                              if ( (bmhdprop = FindProp( iff, ID_ILBM, ID_BMHD ) )) {
                                  struct BitMapHeader *bmh=aid->aid_BMH;

                                  *bmh = *((struct BitMapHeader *)(bmhdprop -> sp_Data));

                                  bmh -> bmh_Width = SDL_SwapBE16(bmh->bmh_Width);
                                  bmh -> bmh_Height = SDL_SwapBE16(bmh->bmh_Height);
                                  animwidth  = bmh -> bmh_Width;
                                  animheight = bmh -> bmh_Height;
                                  animdepth  = bmh -> bmh_Depth;

                                  //                      availmem = AvailMem( MEMF_PUBLIC );
                                  if(IsListEmpty(&aid->aid_FrameList))
                                  {
                                      // Questa roba mi serve solo se creo, non nel merge!

                                      /* Create a seperate pool for frames:
                                       * (((width + 7) / 8) * height * depth + struct BitMapHeader + Padding) * 4 frames
                                       */
                                      MakeYTable(animwidth>>3);

                                      /* Shrink pool to a fitting size */
                                  }
                                  D(bug("Setting anim to %dx%d/%d\n", animwidth, animheight, animdepth));
                              }
                          }

                          /* grab loaded ? - TOLTO */

                          /* dpan loaded ? */
                          if( dpanprop == NULL )
                          {
                              if( (dpanprop = FindProp( iff, ID_ILBM, ID_DPAN )) )
                              {
                                  if( (aid -> aid_FPS) == 0UL )
                                  {
                                      struct DPAnimChunk *dpan = (struct DPAnimChunk *)(dpanprop -> sp_Data);

                                      dpan->dpan_FPS = SDL_SwapBE32(dpan->dpan_FPS);
                                      if( (dpan -> dpan_FPS) <= 60UL )
                                      {
                                          aid -> aid_FPS = dpan -> dpan_FPS;

                                          D(bug( "DPAN found, FPS set to %lu\n", (aid -> aid_FPS) ));
                                      }
                                      else
                                      {
                                          D(bug( "DPAN found, ignoring invalid FPS value %lu\n", (ULONG)(dpan -> dpan_FPS) ));
                                      }
                                  }
                              }
                          }

                          /* IFF ANNO found ? - TOLTO */
                          /* IFF AUTH found ? - TOLTO */
                          /* IFF (C) found ? -TOLTO */
                          /* IFF FVER found ? -TOLTO */
                          /* IFF NAME found ? -TOLTO */

                          if ( (cn = CurrentChunk( iff )) ) {
                              switch( (cn -> cn_ID) ) {
                                  case ID_FORM:
                                      {
                                          /* Create an prepare a new frame node */
                                          if ( (fn = AllocFrameNode()) ) {
                                              MyAddTail( &(aid -> aid_FrameList), (struct MyNode*)&(fn -> fn_Node) );

//                                              D(bug("Allocating frame %d\n", timestamp));

                                              fn -> fn_TimeStamp = timestamp++;

                                              // display time is in 1000th of seconds, so 20 ticks for frame at 50fps
                                              if(fn->fn_Node.mpPrev)
                                                  fn -> Clock = ((struct FrameNode *)fn->fn_Node.mpPrev)->Clock + 20;
                                              else
                                                  fn -> Clock = 20;

                                              fn -> fn_Frame     = fn -> fn_TimeStamp;

                                              fn -> fn_PrevFrame = fn;
                                              curframe++;
                                          }
                                          else /* can't alloc frame node */
                                              error = ERROR_NO_FREE_STORE;
                                      }
                                      break;

                                  case ID_ANHD:
                                      {
                                          if( fn )  {
                                              ULONG interleave;

                                              /* Read struct AnimHeader */
                                              error = ReadChunkBytes( iff, (&(fn -> fn_AH)), (LONG)sizeof( struct AnimHeader ) );
                                              if( error == (LONG)sizeof( struct AnimHeader ) )
                                                  error = 0L;
                                              else {
                                                  D(bug("Error reading AnimHeader!\n"));
                                              }

                                              fn->fn_AH.ah_Width = SDL_SwapBE16(fn->fn_AH.ah_Width);
                                              fn->fn_AH.ah_Height = SDL_SwapBE16(fn->fn_AH.ah_Height);
                                              fn->fn_AH.ah_Left = SDL_SwapBE16(fn->fn_AH.ah_Left);
                                              fn->fn_AH.ah_Top = SDL_SwapBE16(fn->fn_AH.ah_Top);
                                              fn->fn_AH.ah_RelTime = SDL_SwapBE32(fn->fn_AH.ah_RelTime);
                                              fn->fn_AH.ah_AbsTime = SDL_SwapBE32(fn->fn_AH.ah_AbsTime);
                                              fn->fn_AH.ah_Flags = SDL_SwapBE32(fn->fn_AH.ah_Flags);

                                              /* Info */
                                              // DumpAnimHeader( aid, (fn -> fn_TimeStamp), (&(fn -> fn_AH)) );

                                              /* Check if we have dynamic timing */
                                              maxreltime = MAX( maxreltime, (fn -> fn_AH . ah_RelTime) );
                                              minreltime = MIN( minreltime, (fn -> fn_AH . ah_RelTime) );

                                              interleave = (ULONG)(fn -> fn_AH . ah_Interleave);

                                              /* An interleave of 0 means two frames back */
                                              if( interleave == 0 )
                                              {
                                                  interleave = 2;
                                              }

                                              /* Get previous frame */
                                              fn -> fn_PrevFrame = GetPrevFrameNode( fn, interleave );
                                          }
                                      }
                                      break;

                                  case ID_CMAP:
                                      {
                                          if( fn ) {
                                              UBYTE *buff;

                                              /* Allocate buffer */
                                              if( (buff = (UBYTE *)malloc((cn -> cn_Size) + 16UL)) ) {
                                                  /* Load CMAP data */
                                                  error = ReadChunkBytes( iff, buff, (cn -> cn_Size) );

                                                  /* All read ? */
                                                  if( error == (cn -> cn_Size) )
                                                  {
                                                      error = 0L; /* Success ! */

                                                      if( timestamp == 1UL )
                                                      {
                                                          UBYTE *rgb=buff;
                                                          int rgb_nc=cn->cn_Size/3,i;

                                                          D(bug("Loading %d colors from anim CMAP\n", rgb_nc));
                                                          // Qui se voglio carico la palette
                                                          for( i = 0UL ; i < rgb_nc ; i++ )
                                                          {
                                                              int r = *rgb++;
                                                              int g = *rgb++;
                                                              int b = *rgb++;
                                                              os_set_color(i, r, g, b);  
                                                          }
                                                      }
                                                  }

                                                  free(buff );
                                              }
                                              else
                                              {
                                                  /* no load buff */
                                                  error = ERROR_NO_FREE_STORE;
                                              }
                                          }
                                      }
                                      break;

                                  case ID_BODY:
                                  case ID_DLTA:
                                      {
                                          if ( fn )  {
                                              /* Store position of DLTA (pos points to the DLTA ID) */
                                              fn -> fn_BMOffset = pos;
                                              fn -> fn_BMSize   = cn -> cn_Size;

                                              if ( (fn -> fn_BitMap) == NULL ) {
                                                  /* Preload frames only if requested or if this is the key frame (first frame of anim) */
                                                  if ( (aid -> aid_LoadAll) || ((fn -> fn_TimeStamp) == 0UL) || !new ) {
                                                      if( animwidth && animheight && animdepth ) {
                                                          if( (fn -> fn_BitMap = AllocBitMapPooled( animwidth, animheight, animdepth )) ) {
                                                              UBYTE *buff;

                                                              if(!new)
                                                              {
                                                                  new=TRUE;
                                                              }
                                                              else Temp=fn->fn_BitMap;

                                                              /* Allocate buffer */
                                                              if( (buff = (UBYTE *)malloc( (cn -> cn_Size) + 32UL )) )
                                                              {
                                                                  struct FrameNode *prevfn;

                                                                  /* Clear buffer to get rid of some problems with corrupted DLTAs */
                                                                  memset( (void *)buff, 0, (size_t)((cn -> cn_Size) + 31UL) );

                                                                  /* Get previous frame */
                                                                  prevfn = fn -> fn_PrevFrame;

                                                                  /* Load delta data */
                                                                  error = ReadChunkBytes( iff, buff, (cn -> cn_Size) );

                                                                  /* All bytes read ? */
                                                                  if( error == (cn -> cn_Size) )
                                                                  {
                                                                      error = DrawDLTA( aid, /*(prevfn -> fn_BitMap)*/ Temp, (fn -> fn_BitMap), (&(fn -> fn_AH)), buff, (cn -> cn_Size) );

//                                                                      D(bug("Processed keyframe %ld of %d bytes (offset: %d)\n",fn->fn_TimeStamp, cn -> cn_Size, pos));

                                                                      if( error ) {
                                                                          D(bug( "scan/load: dlta unpacking error %lu\n", error ));
                                                                      }
                                                                  }

                                                                  free( buff );
                                                              }
                                                              else /* no load buff */
                                                                  error = ERROR_NO_FREE_STORE;
                                                          }
                                                          else /* no bitmap */
                                                              error = ERROR_NO_FREE_STORE;
                                                      }
                                                      else /* no dimensions for bitmap (possibly a missing bmhd) */
                                                          error = ERROR_NOT_ENOUGH_DATA;
                                                  }
                                                  else {
                                                      if( (fn->delta = (UBYTE *)malloc(((cn -> cn_Size) + 32UL) )) ) {
                                                          error = ReadChunkBytes( iff, fn->delta, (cn -> cn_Size) );

                                                          if( error == (cn -> cn_Size) ) {
//                                                              D(bug("Storing frame %ld of %d bytes (offset %d)\n", fn->fn_TimeStamp, cn -> cn_Size, pos));
                                                              error=0;
                                                          }

                                                      }
                                                      else error = ERROR_NO_FREE_STORE;
                                                  }
                                              }
                                              else {
                                                  D(bug( "scan/load: bitmap already loaded\n" ));
                                              }
                                          }
                                      }
                                      break;
                              }
                          }
                        

                          /* on error: leave for-loop */
                          if( error )
                              break;
                      }
                  }
              }

              /* Check for required information */
              if( error == 0L )
              {
                  /* bmh information available  ? */
                  if( bmhdprop == NULL )
                  {
                      D(bug( "scan: no bmhd found\n" ));
                      error=1;
                  }
                  else
                  {
                      /* Any frames loaded ? */
                      if( timestamp == 0UL )
                      {
                          D(bug("Error, no frame loaded!"));
                          error=1;
                      }
                  }
              }

              /* Dynamic timing ? */
              if( (minreltime != maxreltime) && ((aid -> aid_NoDynamicTiming) == FALSE) )
              {
                  struct FrameNode *worknode,
                                   *nextnode;
                  ULONG             shift = 0UL,ts = 0UL;    
                   
                  if( minreltime == 0UL )
                  {
                      shift = 1UL;
                  }

                  D(bug( "using dynamic timing\n" ));

                  /* Renumber timestamps */

                  worknode = (struct FrameNode *)(aid -> aid_FrameList . pHead);

                  while ( (nextnode = (struct FrameNode *)(worknode -> fn_Node . mpNext)) )  {
                      ULONG duration = (worknode -> fn_AH . ah_RelTime) + shift - 1UL;

                      worknode -> fn_TimeStamp = ts;
                      worknode -> fn_Frame     = ts;
                      worknode -> fn_Duration  = duration;

                      ts += (duration + 1UL);

                      worknode = nextnode;
                  }
              }

              /* No FPS rate found ? */
              if ( (aid -> aid_FPS) == 0UL )  {
                  aid -> aid_FPS = 50UL; /* should be 60 (e.g. 1/60 sec per frame) */

                  D(bug( "Framerate not found, setting 50FPS...\n" ));
              }

              /* Infos */
              D(bug( "width %lu height %lu depth %lu frames %lu fps %lu\n",
                          animwidth,
                          animheight,
                          animdepth,
                          timestamp,
                          (aid -> aid_FPS) ));

              CloseIFF(iff);
          }
          else
              error = ERROR_REQUIRED_ARG_MISSING;
          
          FreeIFF(iff);
    }

    return error;
}

struct FrameNode *LoadFrame(struct AnimInstData *aid,ULONG timestamp)
{
    struct FrameNode *fn;

    /* Find frame by timestamp */

    if( (fn = FindFrameNode( (&(aid -> aid_FrameList)), timestamp )) )
        LoadFrameNode(aid,fn);


    return fn;
}

LONG LoadFrameNode(struct AnimInstData *aid,struct FrameNode *fn)
{
    LONG error = 0L;

    //          ObtainSemaphore( (&(aid -> aid_SigSem)) );

    if (fn) {
        aid -> aid_CurrFN = fn;

        /* Load bitmaps only if we don't cache the whole anim and
         * if we have a filehandle to load from (an empty object created using DTST_RAM don't have this)...
         */
        if ( ((aid -> aid_LoadAll) == FALSE) && (aid -> aid_FH) ) {
            /* If no bitmap is loaded, load it... */
            if ( (fn -> fn_BitMap) == NULL ) {
                if( (fn -> fn_BitMap = AllocBitMapPooled( aid->aid_BMH->bmh_Width, aid->aid_BMH->bmh_Height, aid ->aid_BMH->bmh_Depth))) {
                    struct FrameNode *worknode = fn;
                    ULONG             rollback = 0UL;
                    UBYTE            *buff;
                    ULONG             buffsize;

                    /* Buffer to fill. Below we try to read some more bytes
                     * (the size value is stored in worknode -> fn_LoadSize)
                     * (ANHD chunk (~68 bytes), maybe a CMAP) to save
                     * the Seek in the next cycle.
                     * This makes only much sense when doing async io (during playback)...
                     */

                    /* Not the last frame !
                     * Note that this code is replicated in the loop below !!
                     */

                    worknode -> fn_LoadSize = worknode -> fn_BMSize;

                    buffsize = worknode -> fn_LoadSize;

                    do
                    {
                        worknode = worknode -> fn_PrevFrame;
                        rollback++;

                        worknode -> fn_LoadSize = worknode -> fn_BMSize;

                        buffsize = MAX( buffsize, (worknode -> fn_LoadSize) );
                    } while( ((worknode -> fn_BitMap) == NULL) && ((worknode -> fn_TimeStamp) != 0UL) );

                    if( ((worknode -> fn_BitMap) == NULL) && ((worknode -> fn_TimeStamp) == 0UL) )
                    {
                        D(bug( "first frame without bitmap ... !\n" ));
                        ClearBitMap( (fn -> fn_BitMap) );
                    }

                    /* Alloc buffer for compressed frame (DLTA) data */
                    if( (buff = (UBYTE *)malloc( (buffsize + 32UL) )))   {
                        do {
                            ULONG current = rollback;

                            worknode = fn;

                            while( current-- )
                            {
                                worknode = worknode -> fn_PrevFrame;
                            }

                            if( (worknode -> fn_BitMap) && (worknode != fn) )
                            {
                                CopyBitMap( (worknode -> fn_BitMap), (fn -> fn_BitMap) );
                            }
                            else
                            {
                                LONG seekdist; /* seeking distance (later Seek result, if Seek'ed) */

                                seekdist = (((worknode -> fn_BMOffset) + 8UL) - (aid -> aid_CurrFilePos));

                                /* Seek needed ? */
                                if( seekdist != 0L )
                                {
                                    seekdist = fseek( (aid -> aid_FH), seekdist, SEEK_CUR );
                                }

                                /* "Seek" success ? */
                                if( seekdist != (-1L) )
                                {
                                    LONG bytesread;

                                    bytesread = fread( buff, 1, (worknode -> fn_LoadSize),(aid -> aid_FH) );

                                    /* No error during reading ? */
                                    if( (bytesread >= (worknode -> fn_BMSize)) && (bytesread != -1L) )
                                    {

                                        if( (error = DrawDLTA( aid, (fn -> fn_BitMap), (fn -> fn_BitMap), (&(worknode -> fn_AH)), buff, (worknode -> fn_BMSize) )) )
                                        {
                                            D(bug( "dlta unpacking error %lu\n", error ));
                                        }

                                        /* Bump file pos */
                                        aid -> aid_CurrFilePos = ((worknode -> fn_BMOffset) + 8UL) + bytesread;
                                    }
                                    else
                                    {
                                        /* Read error */

                                        /* Error, rewind stream */
                                        fseek( (aid -> aid_FH), 0L, SEEK_SET );
                                        aid -> aid_CurrFilePos = 0L;
                                    }

                                    worknode -> fn_LoadSize = 0UL; /* destroy that this value won't affect anything else */
                                }
                                else
                                {
                                    /* Seek error */
                                    D(bug("seek error\n"));
                                }
                            }
                        } while( rollback-- && (error == 0L) );

                        free( buff );
                    }
                    else
                    {
                        /* No memory for compressed frame data */
                        error = ERROR_NO_FREE_STORE;
                    }
                }
                else
                {
                    /* No memory for frame bitmap */
                    error = ERROR_NO_FREE_STORE;
                }
            }
        }

        /*
           alf -> alf_Frame    = fn -> fn_Frame;
           alf -> alf_Duration = fn -> fn_Duration;
           alf -> alf_UserData = (APTR)fn;
         */
        /* Links back to this FrameNode (used by ADTM_UNLOADFRAME) */

        /* Store bitmap information */
        /*
           alf -> alf_BitMap = fn -> fn_BitMap;
           alf -> alf_CMap   = fn -> fn_CMap;
         */

        fn -> fn_UseCount++;

        /* Is this node in the posted-free queue ? */
        if ( fn -> fn_PostedFree ) {
            MyRemove( (struct MyNode*) &(fn -> fn_PostedFreeNode) );
            fn -> fn_PostedFree = FALSE;
        }

    }
    else {
        /* no matching frame found */
        D(bug("Frame non trovato\n"));
    }

    //          ReleaseSemaphore( (&(aid -> aid_SigSem)) );

    return error;
}

void UnloadFrame(struct AnimInstData *aid,struct FrameNode *fn)
{
          /* Free bitmaps only if we don't cache the whole anim */
    struct FrameNode *fn2=fn;

          if( (aid -> aid_LoadAll) == FALSE )
          {
            struct MyMinNode *pfn;
            UWORD           i   = 10;

//            ObtainSemaphore( (&(aid -> aid_SigSem)) );

            if( fn )
            {
              if( (fn -> fn_UseCount) > 0 )
              {
                fn -> fn_UseCount--;

                /* Free an existing bitmap if it isn't in use and if it is NOT the first bitmap */
                if( ((fn -> fn_UseCount) == 0) && (fn -> fn_BitMap) && (fn != (struct FrameNode *)(aid -> aid_FrameList . pHead)) )
                {
                  if( FALSE /*FreeAbleFrame( aid, fn )*/ )
                  {
                    /* Is this node in the posted-free queue ? */
                    if( fn -> fn_PostedFree ) {
                      MyRemove( (struct MyNode *)(&(fn -> fn_PostedFreeNode)) );
                      fn -> fn_PostedFree = FALSE;

                      D( kprintf( "free posted 1 %lu\n", (fn -> fn_TimeStamp) ) );
                    }

                    free( (fn -> fn_BitMap) );
                    fn -> fn_BitMap = NULL;
                  }
                  else
                  {
                    if( (fn -> fn_PostedFree) == FALSE )
                    {
                      D( bug( "posted free %lu\n", (fn -> fn_TimeStamp) ) );

                      MyAddTail( &(aid -> aid_PostedFreeList), (struct MyNode *)(&(fn -> fn_PostedFreeNode)) );
                      fn -> fn_PostedFree = TRUE;
                    }
                  }
                }
              }
            }

            while( (pfn = (struct MyMinNode *)MyRemHead( &(aid -> aid_PostedFreeList)))  )  {
              fn = POSTEDFREENODE2FN( pfn );
              fn -> fn_PostedFree = FALSE;

              if( (fn -> fn_UseCount) == 0 )
              {
                if( FreeAbleFrame( aid, fn ) )
                {
                  D( kprintf( "free posted 2 %lu \n", (fn -> fn_TimeStamp) );

                  free( fn -> fn_BitMap );
                  fn -> fn_BitMap = NULL;
                }
                else
                {
                  MyAddTail(&(aid -> aid_PostedFreeList), (struct MyNode *)&(fn -> fn_PostedFreeNode)) ;
                  fn -> fn_PostedFree = TRUE;
                }

                /* Don't process the list twice */
                if( fn == fn2 )
                {
                  i = MIN( 1, i );

                  break;
                }

                if( i-- == 0 )
                {
                  D( kprintf( "pl overflow at %lu\n", fn2-> fn_TimeStamp) ) );

                  break;
                }
              }
            }

//            ReleaseSemaphore( (&(aid -> aid_SigSem)) );
          }

}


static BOOL FreeAbleFrame( struct AnimInstData *aid, struct FrameNode *fn )
{
    struct FrameNode *currfn = aid -> aid_CurrFN;

    /* Don't free the current nor the previous nor the next bitmap (to avoid problems with delta frames) */
    if( (fn == currfn) ||
        (fn == (struct FrameNode *)(currfn->fn_Node.mpNext)) ||
        (fn == (struct FrameNode *)(currfn->fn_Node.mpPrev)) )
    {
      return( FALSE );
    }

    if( ABS( ((LONG)(fn -> fn_TimeStamp)) - ((LONG)(currfn -> fn_TimeStamp)) ) < 5UL )
    {
      return( FALSE );
    }

    return( TRUE );
}

struct AnimInstData *LoadFrames( FILE *fh )
{
    struct AnimInstData *aid;
    LONG error = 0L;
    /* Create a memory pool for frame nodes and delta buffers */

    if ((aid=malloc(sizeof(struct AnimInstData)))) {
        // E' importante azzerarla!

        memset(aid,0,sizeof(struct AnimInstData));

        //    InitSemaphore( (&(aid -> aid_SigSem)) );
        MyNewList(&(aid -> aid_FrameList));
        MyNewList(&(aid -> aid_PostedFreeList));

        aid -> aid_FH = fh;
        aid -> aid_BMH = malloc(sizeof(struct BitMapHeader));
        error=MergeAnim(aid,fh);
    }


    /* Error codes below 0 are related to the IFFParse.library functions */
    if( error < 0L ) {
      D(bug( "iff error %ld\n", (long)error ));
    }

    if(error!=0) {
        D(bug("Errore: %ld\n",error));
    }

    return( aid );
}


static
struct FrameNode *AllocFrameNode(void )
{
    struct FrameNode *fn;

    if( (fn = (struct FrameNode *)malloc((ULONG)sizeof( struct FrameNode ) )) )
    {
      memset( fn, 0, sizeof( struct FrameNode ) );
    }

    return( fn );
}


static
struct FrameNode *FindFrameNode( struct MyList *fnl, ULONG timestamp )
{
    if( fnl )
    {
      struct FrameNode *worknode,
                       *nextnode,
                       *prevnode;

      prevnode = worknode = (struct FrameNode *)(fnl -> pHead);

      while( (nextnode = (struct FrameNode *)(worknode -> fn_Node .mpNext)) )
      {
        if( (worknode -> fn_TimeStamp) > timestamp )
          return( prevnode );

        prevnode = worknode;
        worknode = nextnode;
      }

      if( !IsListEmpty( fnl) )
        return( prevnode );
    }

    return( NULL );
}

/* Copy bm1 to bm2 */
static
void CopyBitMap( struct BitMap *bm1, struct BitMap *bm2 )
{
    ULONG  bpr1 = bm1 -> BytesPerRow;
    ULONG  bpr2 = bm2 -> BytesPerRow;

    /* Same bitmap layout ? */
    if( bpr1 == bpr2 )
    {
      /* Interleaved BitMap ? */
      if( ((bm1 -> Planes[ 1 ]) - (bm1 -> Planes[ 0 ])) == (bpr1 / (ULONG)(bm1 -> Depth)) )
      {
        ULONG planesize = bpr2 * (ULONG)(bm2 -> Rows);

        memcpy( (bm2 -> Planes[ 0 ]), (bm1 -> Planes[ 0 ]), planesize );
      }
      else
      {
        ULONG planesize = bpr2 * (ULONG)(bm2 -> Rows);
        UWORD i;

        for( i = 0U ; i < (bm2 -> Depth) ; i++ )
        {
          memcpy( (bm2 -> Planes[ i ]), (bm1 -> Planes[ i ]), planesize );
        }
      }
    }
    else
    {
      register UBYTE *src;
      register UBYTE *dst;
      register LONG   r;
      register LONG   p;
               ULONG  width = bm1 -> BytesPerRow;

      /* Interleaved BitMap ? */
      if( ((bm1 -> Planes[ 1 ]) - (bm1 -> Planes[ 0 ])) == (bpr1 / (ULONG)(bm1 -> Depth)) )
      {
        width /= (bm1 -> Depth);
      }

      for( p = bm1 -> Depth - 1 ; p >= 0 ; p-- )
      {
        src = (UBYTE *)bm1 -> Planes[ p ];
        dst = (UBYTE *)bm2 -> Planes[ p ];

        for( r = bm1 -> Rows - 1 ; r >= 0 ; r-- )
        {
          memcpy( dst, src, width );
          src += bpr1;
          dst += bpr2;
        }
      }
    }
}



static
void ClearBitMap( struct BitMap *bm )
{
    if( bm )
    {
      ULONG planesize = (ULONG)(bm -> BytesPerRow) * (ULONG)(bm -> Rows);
      UWORD i;

      for( i = 0U ; i < (bm -> Depth) ; i++ )
      {
        memset( (bm -> Planes[ i ]), 0, (size_t)planesize );
      }
    }
}


/* XOR Bitmaps op1 ^= op2 */
static
void XORBitMaps( struct BitMap *op1, struct BitMap *op2 )
{
    if( op1 && op2 )
    {
               ULONG  planesize = (ULONG)(op1 -> BytesPerRow) * (ULONG)(op1 -> Rows);
               ULONG  missing;
               ULONG  i;
      register ULONG  j;
      register ULONG *op1p, /* op1 planes */
                     *op2p; /* op2 planes */

      planesize = planesize / sizeof( ULONG ); /* op1p and op2p are ULONGs, not BYTES... */
      missing   = planesize % sizeof( ULONG ); /* missing bytes */

      for( i = 0U ; i < (op1 -> Depth) ; i++ )
      {
        j = planesize;

        op1p = (ULONG *)(op1 -> Planes[ i ]);
        op2p = (ULONG *)(op2 -> Planes[ i ]);

        while( j-- )
        {
          *op1p++ ^= *op2p++;
        }

        if( missing )
        {
          register UBYTE *op1px = (UBYTE *)op1p;
          register UBYTE *op2px = (UBYTE *)op2p;

          j = missing;

          while( j-- )
          {
            *op1px++ ^= *op2px++;
          }
        }
      }
    }
}

static
struct BitMap *AllocBitMapPooled( ULONG width, ULONG height, ULONG depth )
{
    struct BitMap *bm;
    ULONG          planesize,
                   moredepthsize,
                   size;

    planesize       = (ULONG)BITRASSIZE( ((width + 63UL) & ~63UL), height );
    moredepthsize   = (depth > 8UL)?((depth - 8UL) * sizeof( PLANEPTR )):(0UL);
    size            = ((ULONG)sizeof( struct BitMap )) + moredepthsize + (planesize * depth) + 31UL;

    if ( (bm = (struct BitMap *)malloc( size )) ) {
        UWORD    pl;
        PLANEPTR plane;

        bm->Depth = depth;
        bm->Rows  = height;
        bm->Flags = 0;
        bm->pad   = 0;
        bm->BytesPerRow = ((width + 15) >> 3) & ~0x1;
        //      InitBitMap( bm, depth, width, height );

        plane = ALIGN_QUADLONG( (PLANEPTR)(bm + 1) ); /* First plane follows struct BitMap */

        /* Set up plane data */
        pl = 0U;

        /* Set up plane ptrs */
        while( pl < depth ) {
            bm -> Planes[ pl ] = plane;
            plane = ALIGN_QUADLONG( (PLANEPTR)(((UBYTE *)plane) + planesize) );
            pl++;
        }

        /* Clear the remaining plane ptrs (up to 8 planes) */
        while ( pl < 8U ) {
            bm -> Planes[ pl ] = NULL;
            pl++;
        }
    }

    return( bm );
}


/*****************************************************************************/

static LONG DrawDLTA( struct AnimInstData *aid, struct BitMap *prevbm, struct BitMap *bm, struct AnimHeader *ah, UBYTE *dlta, ULONG dltasize )
{
    LONG error = 0L;

    if( bm && ah && dlta && dltasize )
    {
      struct BitMap       *unpackbm = bm,
                          *tempbm   = NULL;
      struct BitMapHeader *bmh      = aid -> aid_BMH;
      BOOL                 DoXOR;

      /* Handle acmpILBM, acmpXORILBM and acmpAnimJ explicitly */
      switch( ah -> ah_Operation )
      {
        case acmpILBM:    /*  0  */
        {
            /* unpack ILBM BODY */
//            return( LoadILBMBody( unpackbm, bmh, dlta, dltasize ) );

          BodyToBitMap(unpackbm,bmh,dlta,dltasize);          
          return 0;
        }

        case acmpXORILBM: /*  1  */
        {
            D(bug( "\adlta: acmpXORILBM disabled, call author immediately\n" ));
            return( ERROR_NOT_IMPLEMENTED );
        }

        case acmpAnimJ:   /* 'J' */
        {
            /* unpack ANIM-J  */
            D(bug( "\adlta: acmpanimjdelta disabled, call author immediately\n" ));
            return( ERROR_NOT_IMPLEMENTED );
//            return( unpackanimjdelta( dlta, dltasize, prevbm, bm ) );
        }
      }

      /* XOR ? */
      DoXOR = ((ah -> ah_Flags) & ahfXOR);

      /* Prepare XOR (see below) */
      if( DoXOR && prevbm )
      {
        if( prevbm == bm )
        {
          if( !(tempbm = AllocBitMapPooled( (ULONG)(aid -> aid_BMH -> bmh_Width), (ULONG)(aid -> aid_BMH -> bmh_Height), (ULONG)(aid -> aid_BMH -> bmh_Depth) )) )
          {
            return( ERROR_NO_FREE_STORE );
          }

          unpackbm = prevbm = tempbm;
        }

        ClearBitMap( unpackbm );
      }
      else
      {
        if( prevbm )
        {
          if( prevbm != bm )
          {
            CopyBitMap( prevbm, bm );
          }
        }
        else
        {
          ClearBitMap( bm );
        }
      }

      /* dispatch compression type, second attempt */
      switch( ah -> ah_Operation )
      {
        /* acmpILBM, acmpXORILBM and acmpAnimJ have been processed above */

        case acmpLongDelta:         /* 2 */
        {
//            error = unpacklongdelta( unpackbm, dlta, dltasize );
            D(bug( "\adlta: acmpanim2 disabled, call author immediately\n" ));
            return( ERROR_NOT_IMPLEMENTED );
        }
    break;

        case acmpShortDelta:        /* 3 */
        {
//          error = unpackshortdelta( unpackbm, dlta, dltasize );
            D(bug( "\adlta: acmpanim3 disabled, call author immediately\n" ));
            return( ERROR_NOT_IMPLEMENTED );
        }
            break;

        case acmpDelta:             /*  4 */
        {
#ifdef COMMENTED_OUT
            if( (ah -> ah_Flags) & ahfLongData )
            {
              error = unpackanim4longdelta( unpackbm, dlta, dltasize, (ah -> ah_Flags) );
            }
            else
            {
              error = unpackanim4worddelta( unpackbm, dlta, dltasize, (ah -> ah_Flags) );
            }
#else
            D(bug( "\adlta: acmpDelta disabled, call author (gisburn@w-specht.rhein-ruhr.de)\n"
                                   "immediately. If you are this FIRST user who send me a VALID IFF ANIM-4 compressed animation\n"
                                   "you'll get $10 US-Dollar !!\n THIS IS NO JOKE !!\n" ));
            error = ERROR_NOT_IMPLEMENTED;
#endif /* COMMENTED_OUT */
        }
            break;

        case acmpByteDelta:         /* 5 */
        case acmpStereoByteDelta:   /* 6 */
        {
//            error = unpackbytedelta( unpackbm, dlta, dltasize );
        DeltaUnpack(unpackbm, dlta,0);
        }
            break;

        case acmpAnim7:             /* 7 */
        {
            if( (ah -> ah_Flags) & ahfLongData )
            {
//              error = unpackanim7longdelta( unpackbm, dlta, dltasize );
        DeltaUnpack(unpackbm, dlta,2);
            }
            else
            {
//              error = unpackanim7worddelta( unpackbm, dlta, dltasize );
        DeltaUnpack(unpackbm, dlta,1);
            }
        }
            break;

        case acmpAnim8:             /* 8 */
        {
            if( (ah -> ah_Flags) & ahfLongData )
            {
//              error = unpackanim8longdelta( unpackbm, dlta, dltasize );
            D(bug( "\adlta: acmpanim8long disabled, call author immediately\n" ));
            return( ERROR_NOT_IMPLEMENTED );
            }
            else
            {
//              error = unpackanim8worddelta( unpackbm, dlta, dltasize );
            D(bug( "\adlta: acmpanim8word disabled, call author immediately\n" ));
            return( ERROR_NOT_IMPLEMENTED );
            }
        }
            break;

        default:                    /* 'l' */
        {
            D(bug( "\adlta: anim compression %ld not implemented yet\n", (long)(ah -> ah_Operation) ));

            error = ERROR_NOT_IMPLEMENTED;
        }
            break;
      }

      /* Handle XOR (see above) */
      if( DoXOR && prevbm )
      {
        XORBitMaps( bm, prevbm );
      }

      if( tempbm )
      {
        free( tempbm );
      }
    }

    return( error );
}


static void DumpAnimHeader( struct AnimInstData *aid, ULONG ti, struct AnimHeader *anhd )
{
    if ( anhd )  {
        D(bug( "%4lu: ", ti ));

        switch( anhd -> ah_Operation )  {
            case acmpILBM:              D(bug( "Operation ILBM" ));                  break;
            case acmpXORILBM:           D(bug( "Operation XORILBM" ));               break;
            case acmpLongDelta:         D(bug( "Operation LongDelta" ));             break;
            case acmpShortDelta:        D(bug( "Operation ShortDelta" ));            break;
            case acmpDelta:             D(bug( "Operation Delta" ));                 break;
            case acmpByteDelta:         D(bug( "Operation ByteDelta" ));             break;
            case acmpStereoByteDelta:   D(bug( "Operation StereoByteDelta" ));       break;
            case acmpAnim7:             D(bug( "Operation Anim7" ));                 break;
            case acmpAnim8:             D(bug( "Operation Anim8" ));                 break;
            case acmpAnimJ:             D(bug( "Operation AnimJ" ));                 break;
            default:                    D(bug( "Operation <unknown compression>" )); break;
        }

        D(bug( " AbsTime %3lu RelTime %3lu Interleave %3lu", (anhd -> ah_AbsTime), (anhd -> ah_RelTime), (ULONG)(anhd -> ah_Interleave) ));

        if( (anhd -> ah_Flags) & ahfLongData          ) D(bug( " LongData"          ));
        if( (anhd -> ah_Flags) & ahfXOR               ) D(bug( " XOR"               ));
        if( (anhd -> ah_Flags) & ahfOneInfoList       ) D(bug( " OneInfoList"       ));
        if( (anhd -> ah_Flags) & ahfRLC               ) D(bug( " RLC"               ));
        if( (anhd -> ah_Flags) & ahfVertical          ) D(bug( " Vertical"          ));
        if( (anhd -> ah_Flags) & ahfLongInfoOffsets   ) D(bug( " LongInfoOffsets"   ));

        D(bug( "\n" ));

    }
}


static
struct FrameNode *GetPrevFrameNode( struct FrameNode *currfn, ULONG interleave )
{
    struct FrameNode *worknode,
                     *prevnode;

    /* Get previous frame */
    worknode = currfn;

    while( (prevnode = (struct FrameNode *)(worknode -> fn_Node . mpPrev)) )
    {
      if( (interleave-- == 0U) || ((prevnode -> fn_Node . mpPrev) == NULL) )
      {
        break;
      }

      worknode = prevnode;
    }

    return( worknode );
}


void BodyToBitMap(struct BitMap *f_bm,struct BitMapHeader *bmh,UBYTE *f_body,LONG dltasize)
{
  register short i;
  short Depth;
  short Height;
  PTR_RING Ring[8+1];
  PTR_RING *RingPtr;

  Height = f_bm->Rows;
  Depth  = f_bm->Depth;


  /* Rinstruktur mit BitMapPointern versorgen */
  for(i = 0; i < Depth; i++)
  {
    Ring[i].PlanePtr = f_bm->Planes[i];
    Ring[i].next = (APTR)&Ring[i+1];
  }

  RingPtr = (PTR_RING *)&Ring[0];
  i = Height * Depth;  /* Zeilensumme aller Planes */

    /* take care of the extra stencilplane in the body chunk
     * (appears as extra plane)
     */

/*
  if(f_info->masking == 1)
  {
    Ring[Depth].PlanePtr = g_stencil;
    Ring[Depth].next = (APTR)&Ring[0];

    i += Height;  // add the lines from stencil  plane

    BdyUnpack(f_body,
          &Ring[0],
          (long)i,              
          (long)f_bm->BytesPerRow,
          (long)f_info->compression);

  }
  else
*/
  {
    Ring[Depth-1].next = (APTR)&Ring[0];
    C_BdyUnpack(f_body,
          &Ring[0],
          (long)i,                  /* Zeilensumme */
          (long)f_bm->BytesPerRow,
          (long)(dltasize!=(f_bm->BytesPerRow*i)) );
  }
}               /* end BodyToBitMap */


/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
   MakeYTable
    Setup a Multiplication Table for the
    skip opcode.
   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

void MakeYTable(short f_BytesPerRow)
{
   register short *pt;
   register short  acc, height;
   static   short  Old_BytesPerRow = 0;

   /* if no changes in Width we can re-use the previous Table */
   if (Old_BytesPerRow == f_BytesPerRow)  return;

   Old_BytesPerRow = f_BytesPerRow;

   pt  = &g_ytable[0];
   acc = 0;
   height = 128;    /* max. line skip range */
   while (height--)
   {
      *(pt++) = acc;
      acc += f_BytesPerRow;
   }
}               /* end MakeYTable */


/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
   DeltaUnpack
    Unpack a DLTA Chunk into the corresponding frame bitmap.
    This code handles:
    - Anim5 standard DLTA Chunks       (IFF standard)
    - Anim7 short and long DLTA Chunks (nonstandard format !!)

    The DLTA Chunk contains
    - 8 opcode list pointers (one for each plane) followed by
    - 8 corresponding data list pointers (only in Anim7) followd by
    - opcode and data
    Unused Bitplanes have a deltadata[i] == 0
   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

// f_mode:

/* 0 .. Anim5 Vert. Byte DLTA
 * 1 .. Anim7 Vert. Word DLTA
 * 2 .. Anim7 Vert. Long DLTA
 */

void decode_plane(uint8_t *opc, uint8_t *dta, uint8_t *plane, long bytesperrow, long fmode)
{
    int i, j, k;

    // the decoding is done column by column
    for (i = 0; i < bytesperrow; ++i) {
        uint8_t *dst = plane + i;

        // get the number of operations for this row
        int ops = *opc;
        opc++;
        for (j = 0; j < ops; ++j) {
            // if the high bit is set it's a "unique" run 
            if (*opc & 0x80) {
                int count = *opc & 0x7f;
                opc++;
                for (k = 0; k < count; ++k) {
                    *dst = *opc++;
                    dst += bytesperrow;
                }
            }
            else if (*opc == 0) { // if the byte is 0 we have to check the next byte for a "repeat" run
                opc++;
                int count = *opc;
                opc++;
                for (k = 0; k < count; ++k) {
                    *dst = *opc;
                    dst += bytesperrow;
                }
                opc++;
            }
            else { // otherwise is a "skip" run, just go down of "byte" rows
                int count = *opc;
                dst += (count * bytesperrow);
                opc++;
            }
        }
    }
}

void DeltaUnpack(struct BitMap *f_bm,void *f_dlta_adr,long f_mode)
{
    register unsigned char *opclist;
    unsigned char *dtalist;
    register short i;
    long          *deltadata;

    deltadata = (long *)(f_dlta_adr);
    /* Loop for max. 8 Bitplanes */

    if (f_mode != 0) {
        D(bug("Unpack mode %d not (yet?) supported!", f_mode));
        return;
    }

    for (i=0; i < 8; i++) {
        if (deltadata[i]) {
            opclist = (unsigned char *)deltadata + SDL_SwapBE32(deltadata[i]);
            dtalist = (unsigned char *)deltadata + SDL_SwapBE32(deltadata[i+8]);
            decode_plane(opclist,        /* in   (opc list)  */
                         dtalist,        /* dta  (data list) */
                         f_bm->Planes[i],  /* out  (bitplane)  */
                         (long)f_bm->BytesPerRow,
                         f_mode);
        }
    }
}               /* end DeltaUnpack */

void C_BdyUnpack( UBYTE *f_bdy, PTR_RING *f_ring_ptr,long f_row,long f_BytePerRow,long f_compress)
{
  register UBYTE  *WrPtr;       /* WritePointer to destination in BitMap */
  register short i, count;

  long ct_u, ct_r, sum_u, sum_r;


  ct_u = ct_r = sum_u = sum_r = 0;


  if (!f_compress)
  {
    /* do a 1:1 copy of each pixel Line */
    while(f_row--)
    {
      WrPtr = f_ring_ptr->PlanePtr;
      for(i=0; i < f_BytePerRow; i++) *(WrPtr++)  =  *(f_bdy++);
      f_ring_ptr->PlanePtr = WrPtr;
      f_ring_ptr = (PTR_RING *)f_ring_ptr->next;
    }
  }
  else
  {
    /* unpack (byte run method) */
    /* general loop for all pixel Lines (f_row) of all planes */
    while(f_row--)
    {
      WrPtr = f_ring_ptr->PlanePtr;
      count = f_BytePerRow;
      /* loop for all bytes in one pixelline (count) */
      while(count > 0)
      {
    i = *(f_bdy++);
    if(i > 127 )            /* next byte is repeated i times */
    {
      i =  257 - i;

          ct_r++;
          sum_r += i;
//          printf("r%3d ", (long)i);

      count -= i;
      while(i--)   *(WrPtr++) = *f_bdy;
      f_bdy++;
    }
    else                    /* transfer i uncompressed bytes */
    {
      i++;

          ct_u++;
          sum_u += i;
//          D(bug("u%3ld ", (long)i));


      count -= i;
      while(i--)  *(WrPtr++) = *(f_bdy++);
    }                               /* end if *f_bdy < 0 */
      }                                 /* end while */

//      D(bug("\n"));

      f_ring_ptr->PlanePtr = WrPtr;        /* store WritePosition in ringlist */
      f_ring_ptr = (PTR_RING *)f_ring_ptr->next;
    }           /* end for f_row */


          ct_u++;
          sum_u += i;
//   D(bug("\n\n u: %ld (%ld bytes)   r: %ld (%ld bytes)", ct_u, sum_u, ct_r, sum_r));


  }             /* end if compress */

}               /* end C_BdyUnpack */

