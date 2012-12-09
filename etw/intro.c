#include "etw_locale.h"
#include "menu.h"
#include "SDL.h"
#include "anim.h"

void Intro(void)
{
    FILE *fh;

    if ((fh = fopen("intro/intro.anim"/*-*/, "rb"))) {
        char buffer[80];
        FILE *fh2;
        int t = RangeRand(NUMERO_INTRO);

        D(bug("Playing intro %ld...\n",t));

        sprintf(buffer,"intro/intro%lc.anim"/*-*/,'a'+t);

        if ((fh2 = fopen(buffer, "rb"))) {
            struct AnimInstData *a;
            unsigned int id;

            if ((a = LoadFrames(fh))) {
                if (!(id = MergeAnim(a,fh2))) {
                    struct FrameNode *fn=(struct FrameNode *)a->aid_FrameList.pHead;
                    int i;

                    fn->fn_Sample=menusound[1];
                    fn->fn_Rate=menusound[1]->Rate;
                    fn->fn_Volume=menusound[1]->Volume;
                    fn->fn_Loops=4;

                    if((fn=GetFrameNode(a,90))) {
                        fn->fn_Sample=menusound[2];
                        fn->fn_Rate=menusound[2]->Rate;
                        fn->fn_Volume=menusound[2]->Volume;
                        fn->fn_Loops=1;
                    }

                    if((fn=GetFrameNode(a,100))) {
                        fn->fn_Sample=menusound[1];
                        fn->fn_Rate=menusound[1]->Rate;
                        fn->fn_Volume=menusound[1]->Volume/4;
                        fn->fn_Loops=7;
                    }

                    if((fn=GetFrameNode(a,300))) {
                        fn->fn_Sample=menusound[1];
                        fn->fn_Rate=menusound[1]->Rate;
                        fn->fn_Volume=menusound[1]->Volume;
                        fn->fn_Loops=4;
                    }

                    // different sounds in different intros
                    switch(t) {
                        case 3:
                            fn=GetFrameNode(a,401);

                            i=1;

                            while(fn->fn_Node.mpNext) {
                                fn->Clock+=(20*i);

                                i++;

                                fn=(struct FrameNode *)fn->fn_Node.mpNext;
                            }
                            break;
                        case 2:
                            if((fn=GetFrameNode(a,410))) {
                                fn->fn_Loops=1;
                                fn->fn_Sample=menusound[0];
                                fn->fn_Rate=menusound[0]->Rate;
                                fn->fn_Volume=menusound[0]->Volume;
                            }
                            break;
                        case 4:
                            if((fn=GetFrameNode(a,423))) {
                                fn->fn_Loops=1;
                                fn->fn_Sample=menusound[17];
                                fn->fn_Rate=menusound[17]->Rate;
                                fn->fn_Volume=menusound[17]->Volume;
                            }                            
                            if((fn=GetFrameNode(a,429))) {
                                fn->fn_Loops=1;
                                fn->fn_Sample=menusound[15];
                                fn->fn_Rate=menusound[15]->Rate;
                                fn->fn_Volume=menusound[15]->Volume;
                            }                            
                            if((fn=GetFrameNode(a,445))) {
                                fn->fn_Loops=1;
                                fn->fn_Sample=menusound[16];
                                fn->fn_Rate=menusound[16]->Rate;
                                fn->fn_Volume=menusound[16]->Volume;
                            }                            
                            break;
                        case 5:
                            if((fn=GetFrameNode(a,405))) {
                                fn->fn_Loops=1;
                                fn->fn_Sample=menusound[14];
                                fn->fn_Rate=menusound[14]->Rate;
                                fn->fn_Volume=menusound[14]->Volume;
                            }                            
                            break;
                    }

                    DisplayAnim(a);

                    // wait one second
                    os_delay(50);

                }
                else   
                    D(bug("MergeAnim failed with code %ld\n",id));

                FreeFrames(a);
            }

            fclose(fh2);
        }
        fclose(fh);
    }        
}

void Outro(void)
{
    FILE *fh;

    StopMenuMusic();

    if ((fh = fopen("intro/outro.anim"/*-*/, "rb"))) {
        struct AnimInstData *a;
        unsigned int i;

        if ((a = LoadFrames(fh))) {
            PlayMenuMusic();

            for(i=0; i<16; i++)
                DisplayAnim(a);

            os_delay(100);

            FreeFrames(a);
        }

        LoadIFFPalette("gfx/eat16menu.col"/*-*/);
        ChangeMenu(current_menu);

        fclose(fh);
    }        
}

#define ENDCREDITS 645

struct Stage
{
    char *string[5];
    long int Tick;
};

struct Stage stage[]=
{
#ifdef MOBILE_VERSION
    {{NULL,"EAT THE WHISTLE"/*-*/,NULL,"MOBILE EDITION"/*-*/,NULL},0},
#else
    {{NULL,"EAT THE WHISTLE"/*-*/,NULL,"2012 EDITION"/*-*/,NULL},0},
#endif
    {{NULL,NULL,NULL,NULL,NULL},43},
    {{"CONCEPT"/*-*/,NULL,"DANIELE CARAMASCHI"/*-*/,"GABRIELE GRECO"/*-*/,NULL},50},
    {{NULL,NULL,NULL,NULL,NULL},95},
    {{"CODE"/*-*/,NULL,"GABRIELE GRECO"/*-*/,NULL,NULL},100},
    {{NULL,NULL,NULL,NULL,NULL},145},
    {{"GRAPHICS"/*-*/,NULL,"DANIELE CARAMASCHI"/*-*/,NULL,NULL},150},
    {{NULL,NULL,NULL,NULL,NULL},175},
    {{"INTRO"/*-*/,NULL,"DANIELE CARAMASCHI"/*-*/,NULL,NULL},180},
    {{NULL,NULL,NULL,NULL,NULL},205},
    {{"SOUND"/*-*/,NULL,"DANIELE CARAMASCHI"/*-*/,"GABRIELE GRECO"/*-*/,NULL},210},
    {{NULL,NULL,NULL,NULL,NULL},255},

#if 0
    {"ETW USES SDL"/*-*/,NULL,"WWW.LIBSDL.ORG"/*-*/,"FOR MORE INFO"/*-*/,NULL,210},
    {NULL,NULL,NULL,NULL,NULL,255},
#endif

    {{"BETATESTING"/*-*/,NULL,"ANDREA CAROLFI"/*-*/,"FRANCESCO DUTTO"/*-*/,"MAURIZIO FAGGIONI"/*-*/},405},
    {{NULL,NULL,NULL,NULL,NULL},445},
    {{NULL,"LORENZO MORSELLI"/*-*/,"THOMAS STEIDING"/*-*/,NULL,msg_81},450},
    {{NULL,NULL,NULL,NULL,NULL},505},
    {{"SPECIAL THANKS"/*-*/,NULL,"SARA CAPPUCCINI"/*-*/,"DAVIDE CARGIOLLI"/*-*/,"FAUSTO CENDERELLI"/*-*/},510},
    {{NULL,NULL,NULL,NULL,NULL},550},
    {{"CLAUDIA CHIARDO"/*-*/,"ELENA FACONTI"/*-*/,"PAOLA FROLA"/*-*/,"GIULIANO GRECO"/*-*/,"ANDREA MENCONI"/*-*/},555},
    {{NULL,NULL,NULL,NULL,NULL},595},
    {{"MICHELE PANELLA"/*-*/,"NICOLO' PANELLA"/*-*/,"MATTEO RATTI"/*-*/,"GRAZIANO VIVIANI"/*-*/,"GIANLUIGI ZUNINO"/*-*/},600},
    {{NULL,NULL,NULL,NULL,NULL},645},
    {{NULL,NULL,NULL,NULL,NULL},ENDCREDITS+2}, // Questo deve sempre rimanere, e' per riferimento
};

#define MAXSTRINGLEN 20

void ShowCredits(void)
{
    gfx_t *o;
    BOOL clean=TRUE;
    long int ticks=0,actual=0;
    int top_x=WINDOW_WIDTH/4-MAXSTRINGLEN*bigfont->width/2,
        top_y=WINDOW_HEIGHT/2-(bigfont->height+6)*3,
        width=(MAXSTRINGLEN*bigfont->width)+4,
        height=(bigfont->height+6)*6+4; // I +4 sono per sicurezza.

    setfont(bigfont);

// Serching in newgfx and then in menugfx...

    if(!(o=LoadGfxObject("newgfx/credits.gfx"/*-*/,Pens,NULL)))
        o=LoadGfxObject("menugfx/credits.gfx"/*-*/,Pens,NULL);

    if(o) {
        SDL_Event e;

        ScaleGfxObj(o,back);
        FreeGfxObj(o);

        memcpy(main_bitmap,back,WINDOW_WIDTH*WINDOW_HEIGHT);
        ScreenSwap();

        // vuoto la porta

        while(ticks<ENDCREDITS)        
        {    
            os_delay(5);

            if(SDL_PollEvent(&e))
            {
                switch(e.type)
                {
                    case SDL_MOUSEBUTTONUP:
                    case SDL_MOUSEBUTTONDOWN:
                        ticks=ENDCREDITS;
                        break;
                    case SDL_KEYDOWN:
                        ticks=ENDCREDITS;
                        break;
                }
            }
            ticks++;

            if(ticks>stage[actual].Tick)
            {
                int i,x,y;

                if(!clean)
                    bltchunkybitmap(back,top_x-2,top_y-2,
                            main_bitmap,top_x-2,top_y-2,
                            width,height,bitmap_width,bitmap_width);

// I -2 sono per compensare i +4 di prima!

                clean=TRUE;

                y=top_y+bigfont->height;

                for(i=0;i<5;i++)
                {
                    if(stage[actual].string[i])
                    {
                        int l=strlen(stage[actual].string[i]);

                        x=top_x+((MAXSTRINGLEN-l)*bigfont->width)/2;

                        ColorTextShadow(x,y,stage[actual].string[i],l,P_GIALLO);

                        clean=FALSE;
                    }

                    y+=(bigfont->height+6);
                }

                actual++;
                ScreenSwap();
            }
        }

        LoadBack();
        ChangeMenu(current_menu);
    }
}
