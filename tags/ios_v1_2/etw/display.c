#include "etw_locale.h"
#include "menu.h"

gfx_t *last_obj=NULL;
BOOL first=TRUE;

void LoadPLogo(char *name)
{
    memset(main_bitmap,Pens[P_NERO],bitmap_width*bitmap_height);
    ScreenSwap();

    LoadGfxObjPalette(name);

    LoadMenuLogo(name);
}

void LoadMenuLogo(char *name)
{
    gfx_t *o;

    if(last_obj)
    {
        FreeGfxObj(last_obj);
        last_obj=NULL;
    }

    o=LoadGfxObject(name,Pens);

    if(o)
    {
        if(o->width==WINDOW_WIDTH && o->height==WINDOW_HEIGHT)
            BltGfxObj(o,0,0,main_bitmap,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,bitmap_width);
        else
        {
            struct MyScaleArgs scale;

            scale.SrcX=scale.SrcY=scale.DestX=scale.DestY=0;
            scale.SrcWidth=scale.SrcSpan=o->width;
            scale.SrcHeight=o->height;
            scale.Dest=main_bitmap;
            scale.DestSpan=bitmap_width;
            scale.Src=o->bmap;
            scale.DestWidth=WINDOW_WIDTH;
            scale.DestHeight=WINDOW_HEIGHT;

            D(bug("Before scaling (display.c): %ldx%ld\n",o->width,o->height));
            bitmapScale(&scale);
            D(bug("After scaling: %ldx%ld\n",scale.DestWidth,scale.DestHeight));

        }
        FreeGfxObj(o);
        ScreenSwap();
    }
}

void ScaleGfxObj(gfx_t *o, uint8_t *b)
{
    struct MyScaleArgs scale;

    scale.SrcX=scale.SrcY=scale.DestX=scale.DestY=0;
    scale.SrcWidth=scale.SrcSpan=o->width;
    scale.SrcHeight=o->height;
    scale.Dest=b;
    scale.Src=o->bmap;
    scale.DestSpan=scale.DestWidth=WINDOW_WIDTH;
    scale.DestHeight=WINDOW_HEIGHT;

    bitmapScale(&scale);
    D(bug("Dopo: %ldx%ld\n",scale.DestWidth,scale.DestHeight));
}

