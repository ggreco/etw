#include "eat.h"
#include "preinclude.h"

void * handle = NULL;
uint8_t * bmap = NULL, * original_bm;

SDL_Window *screen=NULL;
extern int Colors;
extern BOOL use_width,use_height,wb_game,use_direct;
static SDL_Texture *screen_texture = NULL;
static SDL_Renderer *renderer = NULL;

SDL_Color SDL_palette[256];
static uint16_t palette16[256];
static BOOL texture32bit = FALSE;

#include "anim.h"


static void blitBitmap16(struct BitMap *src, uint16_t *dst)
{
    int x, i, current_bit;
    uint8_t *plane[8];
    for (i = 0; i < src->Depth; ++i)
        plane[i] = (uint8_t*)src->Planes[i];

    for (x = 0; x < (src->BytesPerRow * src->Rows); ++x) {
        for(current_bit = 128; current_bit > 0; current_bit >>= 1) {
            uint8_t source_color = 0;

            for(i = 0; i < src->Depth; i++)
                if(plane[i][x] & current_bit)
                    source_color |= (1 << i);

            *(dst++) = palette16[source_color];
        }
    }
}

static void blitBitmap32(struct BitMap *src, uint32_t *dst)
{
    int x, i, current_bit;
    uint8_t *plane[8];
    for (i = 0; i < src->Depth; ++i)
        plane[i] = (uint8_t*)src->Planes[i];

    for (x = 0; x < (src->BytesPerRow * src->Rows); ++x) {
        for(current_bit = 128; current_bit > 0; current_bit >>= 1) {
            uint8_t source_color = 0;

            for(i = 0; i < src->Depth; i++)
                if(plane[i][x] & current_bit)
                    source_color |= (1 << i);

            *(dst++) = *((uint32_t*)&(SDL_palette[source_color]));
        }
    }
}

static void blitScreen16(uint8_t *src, uint16_t *dst, uint32_t width, uint32_t height)
{
    int x, y;

    for (y = height; y > 0; y--)
        for (x = width; x > 0; x--)
            *(dst++) = palette16[*src++];
}

static void blitScreen32(uint8_t *src, uint32_t *dst, uint32_t width, uint32_t height)
{
    int x, y;

    for (y = height; y > 0; y--)
        for (x = width; x > 0; x--)
            *(dst++) = *((uint32_t*)&(SDL_palette[*src++]));
}

#ifdef MOBILE_VERSION

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
    int i, n = SDL_GetNumDisplayModes(0);

    for (i = 0; i < n; ++i) {
        SDL_DisplayMode mode;
        SDL_GetDisplayMode(0, i, &mode);
        D(bug("Got resolution %dx%d\n", mode.w, mode.h));
        if (mode.w > mode.h) {
            WINDOW_WIDTH = mode.w;
            WINDOW_HEIGHT = mode.h;

            if (mode.w % 2)
                WINDOW_WIDTH++;
            if (mode.h % 2)
                WINDOW_HEIGHT++;
            return;
        }
    }
    D(bug("Unable to find a valid iOS resolution!\n"));
}
#endif




BOOL ResizeWindow(int w, int h)
{
    extern Uint8 *back;

    SDL_SetWindowSize(screen, w, h);
    SDL_RenderSetLogicalSize(renderer, w, h);

    texture32bit = FALSE;
    SDL_DestroyTexture(screen_texture);
    screen_texture = SDL_CreateTexture(renderer,
                                       SDL_PIXELFORMAT_RGB565,
                                       SDL_TEXTUREACCESS_STREAMING, w, h);

    free(main_bitmap);
    free(back);
    bitmap_width = WINDOW_WIDTH = w;
    bitmap_height = WINDOW_HEIGHT = h;

    main_bitmap = malloc(WINDOW_WIDTH * WINDOW_HEIGHT);
    back = malloc(WINDOW_WIDTH * WINDOW_HEIGHT);

    if (!main_bitmap || !back) {
        // We should free main_bitmap and back here?
        return FALSE;
    }
    return TRUE;
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

    

    if(!scaling)
    {
        if(!(newbm=malloc(WINDOW_WIDTH*WINDOW_HEIGHT)))
        {
            // Da aggiungere il  controresize della finestra
            D(bug("Failed malloc of resized bitmap!\n"));
            ClipX=WINDOW_WIDTH=old_width;
            ClipY=WINDOW_HEIGHT=old_height;
            return;
        }
        
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
    D(bug("Begin close_graphics()!\n"));

    if (screen_texture)
        SDL_DestroyTexture(screen_texture);

    if (renderer)
        SDL_DestroyRenderer(renderer);

    if (screen)
        SDL_DestroyWindow(screen);

    FreeGraphics();
    D(bug("End close_graphics()!\n"));
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

void SetTitle(const char *title)
{
    SDL_SetWindowTitle(screen, title);
}

void OpenTheScreen(void)
{
#ifdef MOBILE_VERSION
    extern void initialize_dpi();
    initialize_dpi();
    // ios devices only permit touch controls
    control[0] = CTRL_TOUCH; 
    control[1] = CTRL_TOUCH;

    set_resolution();

    screen = SDL_CreateWindow("ETW"/*-*/, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN);

    wb_game = FALSE;
    scaling = NULL;

    if (WINDOW_WIDTH > 640 ||
        WINDOW_HEIGHT > 320) {
        extern double display_width_inches, display_height_inches;
        // we need linear since we don't use a multiple of resolution
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        double dpi = sqrt(display_height_inches * display_height_inches + display_width_inches * display_width_inches);
        if (dpi <= 4.1) {
            WINDOW_WIDTH = 256 * WINDOW_WIDTH / WINDOW_HEIGHT;
            WINDOW_HEIGHT = 256;
        }
        else if (dpi <= 5.5) {
            WINDOW_WIDTH = 320 * WINDOW_WIDTH / WINDOW_HEIGHT;
            WINDOW_HEIGHT = 320;
        }
        else if (dpi < 7.0 ){
            WINDOW_WIDTH = 480 * WINDOW_WIDTH / WINDOW_HEIGHT;
            WINDOW_HEIGHT = 480;
        } else {
            WINDOW_WIDTH = 512 * WINDOW_WIDTH / WINDOW_HEIGHT;
            WINDOW_HEIGHT = 512;
        }
        WINDOW_WIDTH -= (WINDOW_WIDTH % 2); // was % 4, check with different HW
        D(bug("Display size in inches: %gx%g (%g), software destination size: %dx%d\n",
                    display_width_inches, display_height_inches, dpi, WINDOW_WIDTH, WINDOW_HEIGHT));
    }
#else
    if(wb_game) 
        screen = SDL_CreateWindow("ETW"/*-*/, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    else
        screen = SDL_CreateWindow("ETW"/*-*/, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN);
#endif

    if(screen) {
        renderer = SDL_CreateRenderer(screen, -1, 0);

        if (!renderer) {
            D(bug("Error creating SDL renderer: %s\n", SDL_GetError()));
        }

#ifdef MOBILE_VERSION
        SDL_ShowCursor(0);
#else
        SetTitle("Eat The Whistle " ETW_VERSION);
#endif

        texture32bit = FALSE;
        SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
        screen_texture = SDL_CreateTexture(renderer,
                SDL_PIXELFORMAT_RGB565,
                SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
//        SDL_SetTextureBlendMode(screen_texture, SDL_BLENDMODE_BLEND);
        if (!screen_texture) {
            D(bug("Error creating screen texture!\n"));
        }

        alloc_bitmap();
    }

    // enable drop of files on the game window
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    D(bug("Opened window size %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT));
}

static SDL_Texture *anim_texture = NULL;

int create_anim_context(int width, int height)
{
    anim_texture = SDL_CreateTexture(renderer,
                                     SDL_PIXELFORMAT_RGB565,
                                     SDL_TEXTUREACCESS_STREAMING, width, height);

    return anim_texture != NULL;
}

void delete_anim_context()
{
    if (anim_texture) {
        SDL_DestroyTexture(anim_texture);
        anim_texture = NULL;
    }
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


    D(bug("Loading %d colors from %d...\n", colornum, first));

    for(i = 0; i < colornum; i++)
    {
        int r = pal[1 + i *3] >> 27,
            g = pal[1 + i *3 + 1] >> 26,
            b = pal[1 + i *3 + 2] >> 27;

        palette16[i + first] = (r << 11) | (g << 5) | b;
        SDL_palette[i + first].b = pal[1 + i *3] >> 24;
        SDL_palette[i + first].g = pal[1 + i *3 + 1] >> 24;
        SDL_palette[i + first].r = pal[1 + i *3 + 2] >> 24;

        SDL_palette[i + first].a = SDL_ALPHA_OPAQUE;

        // doing half-colors:
        if (!first && colornum <= 32) {
            r = (r*2/3);  g = (g*2/3);  b = (b*2/3);
            SDL_palette[224 + i].r =  (pal[1 + i *3] >> 24) * 2 / 3;
            SDL_palette[224 + i].g =  (pal[1 + i *3 + 1] >> 24) * 2 / 3;
            SDL_palette[224 + i].b = (pal[1 + i *3 + 2] >> 24) * 2 / 3;
            palette16[224 + i] = (r << 11) | (g  << 5) | b;
        }
    }
}

void os_set_color(int i, int r, int g, int b)
{
    SDL_palette[i].b = r;
    SDL_palette[i].g = g;
    SDL_palette[i].r = b;
    r >>= 3;
    g >>= 2;
    b >>= 3;

    palette16[i] = (r << 11) | (g << 5) | b;

    SDL_palette[i].a = SDL_ALPHA_OPAQUE;

}

void ScreenSwap(void)
{
    extern BOOL reqqing;
    void *pixels;
    int pitch;
    uint8_t *src = main_bitmap;
    uint32_t width = bitmap_width, height = bitmap_height;

    if(!SDL_LockTexture(screen_texture, NULL, &pixels, &pitch)) {
        SDL_RenderClear(renderer);
        if (scaling) {
            bitmapFastScale(scaling);
            src = scaling->Dest;
            height = scaling->DestHeight;
            width = scaling->DestWidth;
        }

        if (texture32bit)
            blitScreen32(src, pixels, width, height);
        else
            blitScreen16(src, pixels, width, height);
    
        SDL_UnlockTexture(screen_texture);
        SDL_RenderCopy(renderer, screen_texture, NULL, NULL);

        if (use_touch)
            if (!pause_mode ||
                (pause_mode && replay_mode))
            draw_touch();

        SDL_RenderPresent(renderer);
        
        if (!game_start || reqqing) {
            SDL_Delay(40); // let's wait at least a frame
            SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    } else
        D(bug("Failed texture locking!"));
    
    SDL_PumpEvents();
}

void blit_anim(struct BitMap *src, SDL_Rect *dest)
{
    void *pixels;
    int pitch;

    if (!anim_texture)
        return;

    if(!SDL_LockTexture(anim_texture, NULL, &pixels, &pitch)) {
        int width = (src->BytesPerRow*8);

        if (texture32bit)
            blitBitmap32(src, pixels);
        else
            blitBitmap16(src, pixels);

        SDL_UnlockTexture(anim_texture);

        SDL_RenderCopy(renderer, anim_texture, dest, NULL);

        SDL_RenderPresent(renderer);
    }
    SDL_PumpEvents();
}

int os_videook(int x, int y)
{
    return 1;
    /* TODO SDL_VideoModeOK(x,y,8,SDL_SWSURFACE | (wb_game ? SDL_RESIZABLE : SDL_FULLSCREEN) ); */
}

void FullScreen(BOOL active)
{
    SDL_SetWindowFullscreen(screen, active ? SDL_WINDOW_FULLSCREEN : 0);
}
