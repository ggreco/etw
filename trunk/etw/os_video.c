#include "eat.h"
#include "preinclude.h"

void * handle = NULL;
uint8_t * bmap = NULL, * original_bm;

SDL_Window *screen=NULL;
SDL_Event lastevent;
extern int Colors;
extern BOOL use_width,use_height,wb_game,use_direct;
static SDL_Texture *screen_texture = NULL;
static SDL_Renderer *renderer = NULL;

SDL_Color SDL_palette[256];
static uint16_t palette16[256];

static void blitScreen16(uint16_t *dst)
{
    uint8_t *src = main_bitmap;    
    int x, y;

    for (y = bitmap_height; y > 0; y--)
    	for (x = bitmap_width; x > 0; x--)
			*(dst++) = palette16[*src++];
}

static void blitScreen32(uint32_t *dst)
{
    uint8_t *src = main_bitmap;    
    int x, y;

    for (y = bitmap_height; y > 0; y--)
    	for (x = bitmap_width; x > 0; x--)
			*(dst++) = *((uint32_t*)&(SDL_palette[*src++]));
}

#ifdef IPHONE


typedef struct ResInfo {
    int w, h;
} ResInfo;

static ResInfo ressize[3] = {
    {480, 320},
    {568, 320},
    {1024,768}
};

static void blitScreen32x2(uint32_t *dst)
{
    uint8_t *src = main_bitmap;
    uint32_t *dst2 = dst + bitmap_width * 2;
    int x, y;
    
    for (y = bitmap_height; y > 0; y--) {
    	for (x = bitmap_width; x > 0; x--) {
            uint32_t *col = (uint32_t *)&(SDL_palette[*src++]);
			*(dst++) = *col;
            *(dst++) = *col;
			*(dst2++) = *col;
            *(dst2++) = *col;
        }
        dst += bitmap_width * 2;
        dst2 += bitmap_width * 2;
    }
}

void set_resolution()
{
    int i, j, n = SDL_GetNumDisplayModes(0);
    
    for (i = 0; i < n; ++i) {
        SDL_DisplayMode mode;
        SDL_GetDisplayMode(0, i, &mode);
        for (j = 0; j < 3; ++j) {
            if (mode.w == ressize[j].w &&
                mode.h == ressize[j].h) {
                WINDOW_WIDTH = mode.w;
                WINDOW_HEIGHT = mode.h;
                return;
            }
        }
    }
    D(bug("Unable to find a valid iOS resolution!"));
}
#endif

void AdjustSDLPalette(void)
{
    int i;

    for(i=0;i<32;i++) {
        int r = (palette16[i] >> 11) * 2 / 3,
            g = ((palette16[i] >> 6) & 0x3f) * 2 / 3,
            b = (palette16[i] & 0x1f) * 2 / 3;

        palette16[224 + i] = (r << 11) | (g << 5) | b;
        SDL_palette[224+i].r=SDL_palette[i].r*2/3;
        SDL_palette[224+i].g=SDL_palette[i].g*2/3;
        SDL_palette[224+i].b=SDL_palette[i].b*2/3;
    }
}

void ResizeWindow(int w, int h)
{
    SDL_SetWindowSize(screen, w, h);
}

void ResizeWin(SDL_Event *event)
{
    uint8_t *newbm;
    int old_width = WINDOW_WIDTH, old_height = WINDOW_HEIGHT;
    int w, h;
    
    SDL_GetWindowSize(screen, &w, &h);
        
// WINDOW_WIDTH e WINDOW_HEIGHT sono copiati qui;

//    os_resize(event);


    ClipX=WINDOW_WIDTH=w;
    ClipY=WINDOW_HEIGHT=h;

    if(!(newbm=malloc(WINDOW_WIDTH*WINDOW_HEIGHT)))
    {
// Da aggiungere il  controresize della finestra
        D(bug("Fallita la malloc!\n"));
        ClipX=WINDOW_WIDTH=old_width;
        ClipY=WINDOW_HEIGHT=old_height;
        return;
    }

    if(!scaling)
    {
        bltchunkybitmap(main_bitmap,0,0,newbm,0,0,min(WINDOW_WIDTH,old_width),min(WINDOW_HEIGHT,old_height),bitmap_width,WINDOW_WIDTH);
        free(main_bitmap);
        main_bitmap=newbm;
        bitmap_width=WINDOW_WIDTH;
        bitmap_height=WINDOW_HEIGHT;
    }
    else
    {
        scaling->DestWidth=WINDOW_WIDTH;
        scaling->DestSpan=WINDOW_WIDTH;
        scaling->DestHeight=WINDOW_HEIGHT;

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

    D(bug("*** Resize %dx%d!\n",WINDOW_WIDTH,WINDOW_HEIGHT));
}

void close_graphics(void)
{
    D(bug("Inizio FreeGraphics()!\n"));

    FreeGraphics();
    D(bug("Fine FreeGraphics()!\n"));
}

BOOL alloc_bitmap(void)
{
    if (main_bitmap)
        free(main_bitmap);
    
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

    palette16[color] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
//    D(bug("Eseguita setcolor(%ld, r%ld,g%ld,b%ld)\n",color,r,g,b));
}

void SetTitle(const char *title)
{
    SDL_SetWindowTitle(screen, title);
}

void OpenTheScreen(void)
{
    force_single = TRUE;
    double_buffering = FALSE;
#ifdef IPHONE
    set_resolution();
    
    screen = SDL_CreateWindow("ETW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN);
    
    wb_game = FALSE;
    scaling = NULL;
#else
    if(wb_game) 
        screen =SDL_CreateWindow("ETW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    else
        screen = SDL_CreateWindow("ETW", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN);
#endif

    if(screen) {
        renderer = SDL_CreateRenderer(screen, -1, 0);
        
        if(!wb_game)
            SDL_ShowCursor(0);
        else
            SetTitle("Eat The Whistle " ETW_VERSION);

        D(bug("Fine InitAnimSystem!\n"));
        SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
        screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
//        SDL_SetTextureBlendMode(screen_texture, SDL_BLENDMODE_BLEND);
        alloc_bitmap();
    }
    D(bug("Opened window size %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT));
}

void os_wait(void)
{
    SDL_WaitEvent(&lastevent);
}

int os_get_screen_width(void)
{
    int w;
    SDL_GetWindowSize(screen, &w, NULL);
    return w;
}

int os_get_screen_height(void)
{
    int h;
    SDL_GetWindowSize(screen, NULL, &h);
    return h;
}

int os_get_innerwidth(void)
{
    return os_get_screen_width();
}

int os_get_innerheight(void)
{
    return os_get_screen_height();
}

void os_load_palette(uint32_t *pal)
{
    int colornum, first, i;
    colornum = pal[0] >> 16;
    first = pal[0] & 0xffff;

    if(colornum > 256)
        return;

    for(i = 0; i < colornum; i++)
    {
#if 1 
        int r = pal[1 + i *3] >> 27,
            g = pal[1 + i *3 + 1] >> 26,
            b = pal[1 + i *3 + 2] >> 27;

        palette16[i + first] = (r << 11) | (g << 5) | b;
        SDL_palette[i + first].b = pal[1 + i *3] >> 24;
        SDL_palette[i + first].g = pal[1 + i *3 + 1] >> 24;
        SDL_palette[i + first].r = pal[1 + i *3 + 2] >> 24;
#else
        SDL_palette[i + first].r = pal[1 + i *3] >> 24;
        SDL_palette[i + first].g = pal[1 + i *3 + 1] >> 24;
        SDL_palette[i + first].b = pal[1 + i *3 + 2] >> 24;
#endif
        SDL_palette[i + first].unused = SDL_ALPHA_OPAQUE;
    }
}

void ScreenSwap(void)
{
    void *pixels;
    int pitch;
    
    if(!SDL_LockTexture(screen_texture, NULL, &pixels, &pitch)) {
        if (pitch == bitmap_width * 2)
            blitScreen16(pixels);
        else if (pitch == bitmap_width * 4)
            blitScreen32(pixels);
        else {
            D(bug("Unsupported pitch: %d (width %d)\n", pitch, bitmap_width));
        }

        SDL_UnlockTexture(screen_texture);
        // sdl_flip fall back in SDL_UpdateRect if we are single buffer
        SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
}

int os_videook(int x, int y)
{
    return 1;
    /* TODO SDL_VideoModeOK(x,y,8,SDL_SWSURFACE | (wb_game ? SDL_RESIZABLE : SDL_FULLSCREEN) ); */
}

