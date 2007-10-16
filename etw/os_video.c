#include "eat.h"
#include "preinclude.h"

void * handle = NULL;
uint8_t * bmap = NULL, * original_bm;

SDL_Surface *screen=NULL;
SDL_Event lastevent;
extern int screen_depth;
extern int Colors;
extern BOOL use_width,use_height,wb_game,use_direct;
BOOL triple_buffering=FALSE,wpa8=FALSE,overscan=0;

BOOL os_create_dbuffer(void)
{
    return FALSE;
}

SDL_Color SDL_palette[256];

void AdjustSDLPalette(void)
{
    int i;

    for(i=0;i<32;i++) {
        SDL_palette[224+i].r=SDL_palette[i].r*2/3;
        SDL_palette[224+i].g=SDL_palette[i].g*2/3;
        SDL_palette[224+i].b=SDL_palette[i].b*2/3;
    }
    SDL_SetColors(screen,SDL_palette,0,256);
}

void ResizeWin(SDL_Event *event)
{
    SDL_Surface *temp = screen;
    uint8_t *newbm;
    int old_width = WINDOW_WIDTH, old_height = WINDOW_HEIGHT, nw;

// WINDOW_WIDTH e WINDOW_HEIGHT sono copiati qui;

//    os_resize(event);

    if(!(screen=SDL_SetVideoMode(event->resize.w,event->resize.h,8,SDL_SWSURFACE|SDL_RESIZABLE)))
    {
        screen=temp;
        return;
    }

    ClipX=WINDOW_WIDTH=event->resize.w;
    ClipY=WINDOW_HEIGHT=event->resize.h;

    nw=screen->pitch;

    if(!(newbm=malloc(nw*WINDOW_HEIGHT)))
    {
// Da aggiungere il  controresize della finestra
        D(bug("Fallita la malloc!\n"));
        ClipX=WINDOW_WIDTH=old_width;
        ClipY=WINDOW_HEIGHT=old_height;
        return;
    }

    SDL_SetColors(screen,SDL_palette,0,256);

    if(!scaling)
    {
        bltchunkybitmap(main_bitmap,0,0,newbm,0,0,min(WINDOW_WIDTH,old_width),min(WINDOW_HEIGHT,old_height),bitmap_width,nw);
        free(main_bitmap);
        main_bitmap=newbm;
        bitmap_width=nw;
        bitmap_height=WINDOW_HEIGHT;
    }
    else
    {
        extern void MakeRef(uint8_t *, int, int);

//        free(scaling->Dest);

//        scaling->Dest=new;
        scaling->DestWidth=WINDOW_WIDTH;
        scaling->DestSpan=nw;
        scaling->DestHeight=WINDOW_HEIGHT;
        MakeRef(scaling->XRef,FIXED_SCALING_WIDTH,WINDOW_WIDTH);
        MakeRef(scaling->YRef,FIXED_SCALING_HEIGHT,WINDOW_HEIGHT);

        ClipX=WINDOW_WIDTH=old_width;
        ClipY=WINDOW_HEIGHT=old_height;

        return;
    }

    D(bug("New bitmap %ld x %ld\n",bitmap_width,bitmap_height));

    o_limit=WINDOW_WIDTH*2/5;
    e_limit=WINDOW_WIDTH-o_limit;
    n_limit=WINDOW_HEIGHT*2/5;
    s_limit=WINDOW_HEIGHT-n_limit;
    field_y_limit=FIELD_HEIGHT-WINDOW_HEIGHT-1;
    field_x_limit=FIELD_WIDTH-WINDOW_WIDTH-1;

    if(field_x>field_x_limit)
        field_x=field_x_limit;

    if(field_y>field_y_limit)
        field_y=field_y_limit;

    ResizeRadar();

    D(bug("Resize %ld %ld!\n",WINDOW_WIDTH,WINDOW_HEIGHT));
}

void close_graphics(void)
{
    D(bug("Inizio FreeGraphics()!\n"));

    FreeGraphics();
    D(bug("Fine FreeGraphics()!\n"));

    if(public_screen&&Colors>0)
    {
        int i;

        D(bug("Libero i colori!\n"));

        for(i=0;i<Colors;i++)
            release_pen(Pens[i]);
    }
    else if(!screen_opened)
    {
        FreeIFFPalette();
    }
}

BOOL window_open(void)
{
    bitmap_width=WINDOW_WIDTH;
    bitmap_height=WINDOW_HEIGHT;

    if(!(main_bitmap=malloc(bitmap_width*WINDOW_HEIGHT)))
    {
        D(bug("Non ho memoria per la bitmap!\n"));

        return FALSE;
    }

    original_bm=main_bitmap;

    return TRUE;
}


void os_set_color(int color,int r ,int g, int b)
{
    SDL_Color c;
    c.b=(unsigned char)b;
    c.g=(unsigned char)g;
    c.r=(unsigned char)r;
    SDL_palette[color]=c; // copio x il resize dopo!

    SDL_SetColors(screen,&c,color,1);
//    D(bug("Eseguita setcolor(%ld, r%ld,g%ld,b%ld)\n",color,r,g,b));
}

long obtain_pen(char r, char b, char g)
{
    static int mypen=0;

    return mypen++;
}

void release_pen(long p)
{
}

void lock_pen(int p)
{
}

void OpenTheScreen(void)
{
#ifndef WINCE
    if(wb_game) {
        screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 8, SDL_SWSURFACE|SDL_RESIZABLE);   

        use_remapping=FALSE;
        force_single=TRUE;
        double_buffering=FALSE;
        public_screen=FALSE;
        // metto due false qui x evitare problemi
    }
    else {
        // osx fullscreen is already double buffered by default
#ifndef __APPLE__
        if(!force_single)
            double_buffering=TRUE;
        else
#endif
            double_buffering=FALSE;

        if(double_buffering)
            screen =SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 8, SDL_HWSURFACE|SDL_FULLSCREEN|SDL_DOUBLEBUF);
        else
            screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 8, SDL_HWSURFACE|SDL_FULLSCREEN); 
    }

    if(screen) {
        screen_depth=8;

        if(!wb_game)
            SDL_ShowCursor(0);

        SDL_WM_SetCaption("Eat The Whistle " ETW_VERSION,"ETW");

        D(bug("Fine InitAnimSystem!\n"));
    }
#else
    screen_depth = 8;
    force_single = TRUE; double_buffering = FALSE; wb_game = FALSE;
    public_screen = FALSE; use_remapping = FALSE;
    scaling = FALSE;
    screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 8, SDL_SWSURFACE|SDL_FULLSCREEN);
#endif
}

void os_set_window_frame(void)
{
    WINDOW_WIDTH=min(screen->w,FIELD_WIDTH);
/*
 non serve +

    if(WINDOW_WIDTH!=((WINDOW_WIDTH>>5)<<5))
        WINDOW_WIDTH=(WINDOW_WIDTH>>5)<<5;
*/
    WINDOW_HEIGHT=min(screen->h,FIELD_HEIGHT);
}

void os_wait(void)
{
    SDL_WaitEvent(&lastevent);
}

int os_get_screen_width(void)
{
    return screen->w;
}

int os_get_screen_height(void)
{
    return screen->h;
}

int os_get_innerwidth(void)
{
    return screen->w;
}

int os_get_innerheight(void)
{
    return screen->h;
}

void os_load_palette(uint32_t *palette)
{
    int colornum, first, i;
    colornum = palette[0] >> 16;
    first = palette[0] & 0xffff;

    if(colornum > 256)
        return;

    for(i = 0; i < colornum; i++)
    {
        SDL_palette[i + first].r = palette[1 + i *3] >> 24;
        SDL_palette[i + first].g = palette[1 + i *3 + 1] >> 24;
        SDL_palette[i + first].b = palette[1 + i *3 + 2] >> 24;
    }

    SDL_SetColors(screen, &SDL_palette[first], first, colornum);
}

void ScreenSwap(void)
{
    if(os_lock_bitmap())
    {
        if(scaling)
        {
            scaling->Dest=screen->pixels;
            bitmapFastScale(scaling);
        }
        else if(screen->pitch==bitmap_width)
            memcpy(screen->pixels,main_bitmap,bitmap_width*bitmap_height);
        else
        {
            uint8_t *src = main_bitmap, *dest = screen->pixels;
            int i;

            for(i=bitmap_height;i;--i)
            {
                memcpy(dest,src,bitmap_width);
                src += bitmap_width;
                dest += screen->pitch;
            }
        }
    
        os_unlock_bitmap();

        if(double_buffering)
            SDL_Flip(screen);
        else
            SDL_UpdateRect(screen,0,0,0,0);
    }
}


void os_free_dbuffer(void)
{
}

int os_videook(int x, int y)
{
    return SDL_VideoModeOK(x,y,8,SDL_SWSURFACE | (wb_game ? SDL_RESIZABLE : SDL_FULLSCREEN) );
}

BOOL os_lock_bitmap(void)
{    
    if ( SDL_MUSTLOCK(screen) )
          if(SDL_LockSurface(screen)<0)
              return FALSE;
    return TRUE;
}

void os_unlock_bitmap(void)
{
    if (SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);
}
