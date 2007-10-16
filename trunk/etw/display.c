#include "etw_locale.h"
#include "menu.h"

gfx_t *last_obj=NULL;
BOOL first=TRUE;

void LoadPLogo(char *name)
{
    memset(main_bitmap,Pens[P_NERO],bitmap_width*bitmap_height);
    ScreenSwap();

    if(!use_remapping)
    {
        if(!first)
            FreeIFFPalette();
        else
            first=FALSE;

        LoadGfxObjPalette(name);
    }

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

    if(!use_remapping)
        o=LoadGfxObject(name,Pens,NULL);
    else
        o=LoadGfxObject(name,NULL,NULL);

    if(o)
    {
        if(o->width==WINDOW_WIDTH&&o->height==WINDOW_HEIGHT)
        {
        BltGfxObj(o,0,0,main_bitmap,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,bitmap_width);
        }
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

        if(!use_remapping)
            FreeGfxObj(o);
        else
            last_obj=o;
        }
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

#if 0

#define GRD_STEP 30

void GriddedWipe(UBYTE Type,struct BitMap *source)
{
    /* Wipes the screen in a gridded fashion (horizontal and verticle 
        gradual wipe combination) */
    
    /*********************************************************************/
    /*                                                             */
    /* Inputs:                                                     */
    /*                                                             */
    /* Type     - Type of wipe. 1 = Draw images, else = Draw black */
    /*                                                             */
    /*********************************************************************/   
    
    register int IndexX,IndexY,IndexZ;
    int TX=win->BorderLeft,TY=win->BorderTop;
    register struct RastPort *dest = public_screen ?  win->RPort : &screen->RastPort;

    /* This loop increases the difference between two adjoining wipe lines
        by one each time to give a smooth gradual effect */

    SetAPen(dest,Pens[P_NERO]);

    for (IndexZ = 0;IndexZ < GRD_STEP;IndexZ += 2)
    {
        /* As two loops cannot be used at once, the y coordinate must be
            set differently */
        IndexY = IndexZ;
        
        /* This loop creates the actual wipe effect by creating a step from 
            the left of the wipe area to the right of the wipe area. offset
            by the gradual loop previous */
        for (IndexX = IndexZ;IndexX < 320;IndexX += GRD_STEP)
        {   
            /* Draw graphics on the destination bitmap */
            if (Type == 1)
            {
                /* Picture wipe */
                BltBitMapRastPort(source,IndexX,0,dest,TX+IndexX,TY,2,256,0xc0);
                BltBitMapRastPort(source,0,IndexY,dest,TX,TY+IndexY,320,2,0xc0);
            }
            else
            {
                RectFill(dest,TX + IndexX,TY,TX + IndexX + 1,TY +255);
                RectFill(dest,TX,TY + IndexY,TX + 319,TY +IndexY + 1);
            }

            /* Increase the y coordinate to have the same effect as if it were
                part of a loop */
            IndexY += GRD_STEP;
            
            /* Check to see if the y coordinate has exceeded the height 
                specified and sort it out if it is by setting it to the top */

            if (IndexY >= 254)
                IndexY = 0;
            
            /* End of the horizontal loop */
        }
        
        /* End of the gradual loop */
    }
}
#endif
