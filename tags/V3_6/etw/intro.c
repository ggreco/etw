#include "etw_locale.h"
#include "menu.h"
#include "SDL.h"

#if 0

void Intro(void)
{
    BPTR fh;

    if(fh=Open("intro/intro.anim"/*-*/,MODE_OLDFILE))
    {
        char buffer[20];
        BPTR fh2;
        long int t=RangeRand(NUMERO_INTRO);

        D(bug("Playo la intro %ld...\n",t));

        sprintf(buffer,"intro/intro%lc.anim"/*-*/,'a'+t);

        if(fh2=Open(buffer,MODE_OLDFILE))
        {
        struct AnimInstData *a;
        struct Screen *scr=NULL;
        unsigned int id;

        if(    WINDOW_WIDTH!=320    ||
            WINDOW_HEIGHT<256    ||
            public_screen    )
        {
            id=BestModeID(BIDTAG_DesiredWidth,320,BIDTAG_DesiredHeight,256,BIDTAG_Depth,4,
                BIDTAG_NominalWidth,320,BIDTAG_NominalHeight,256,
                BIDTAG_DIPFMustHave,DIPF_IS_DBUFFER,
                TAG_DONE);

            if(id!=INVALID_ID)
                scr=OpenScreenTags(NULL,
                    SA_Behind,TRUE,
                    SA_Width,320,
                    SA_Height,256,
                    SA_Interleaved,FALSE,
                    SA_Depth,4,
                    SA_Quiet,TRUE,
                    SA_DisplayID,id,
                    TAG_DONE);
        }
        else
            scr=screen;

        if(scr)
        {
            struct RastPort *OldRP=CurrentRP;
            struct Screen *OldScr=screen;

            screen=scr;

            if(a=LoadFrames(fh))
            {
                if(!(id=MergeAnim(a,fh2)))
                {
                    register struct FrameNode *fn=(struct FrameNode *)a->aid_FrameList.mlh_Head;
                    int i;

                    CurrentRP=&scr->RastPort;

                    fn->fn_Sample=sound[1];
                    fn->fn_Rate=sound[1]->Rate;
                    fn->fn_Volume=sound[1]->Volume;
                    fn->fn_Loops=4;

                    if(fn=GetFrameNode(a,90))
                    {
                        fn->fn_Sample=sound[2];
                        fn->fn_Rate=sound[2]->Rate;
                        fn->fn_Volume=sound[2]->Volume;
                        fn->fn_Loops=1;
                    }

                    if(fn=GetFrameNode(a,100))
                    {
                        fn->fn_Sample=sound[1];
                        fn->fn_Rate=sound[1]->Rate;
                        fn->fn_Volume=sound[1]->Volume/4;
                        fn->fn_Loops=7;
                    }

                    if(fn=GetFrameNode(a,300))
                    {
                        fn->fn_Sample=sound[1];
                        fn->fn_Rate=sound[1]->Rate;
                        fn->fn_Volume=sound[1]->Volume;
                        fn->fn_Loops=4;
                    }

                    switch(t)
                    {
                        case 3:
                            fn=GetFrameNode(a,401);

                            i=1;

                            while(fn->fn_Node.mln_Succ)
                            {
                                fn->Clock+=(20*i);

                                i++;

                                fn=(struct FrameNode *)fn->fn_Node.mln_Succ;
                            }
                            break;
                        case 2:
                            if(fn=GetFrameNode(a,410))
                            {
                                fn->fn_Loops=1;
                                fn->fn_Sample=sound[0];
                                fn->fn_Rate=sound[0]->Rate;
                                fn->fn_Volume=sound[0]->Volume;
                            }
                            break;
#ifdef CD_VERSION
                        case 4:
                            if(fn=GetFrameNode(a,423))
                            {
                                fn->fn_Loops=1;
                                fn->fn_Sample=sound[17];
                                fn->fn_Rate=sound[17]->Rate;
                                fn->fn_Volume=sound[17]->Volume;
                            }                            
                            if(fn=GetFrameNode(a,429))
                            {
                                fn->fn_Loops=1;
                                fn->fn_Sample=sound[15];
                                fn->fn_Rate=sound[15]->Rate;
                                fn->fn_Volume=sound[15]->Volume;
                            }                            
                            if(fn=GetFrameNode(a,445))
                            {
                                fn->fn_Loops=1;
                                fn->fn_Sample=sound[16];
                                fn->fn_Rate=sound[16]->Rate;
                                fn->fn_Volume=sound[16]->Volume;
                            }                            
                            break;
                        case 5:
                            if(fn=GetFrameNode(a,405))
                            {
                                fn->fn_Loops=1;
                                fn->fn_Sample=sound[14];
                                fn->fn_Rate=sound[14]->Rate;
                                fn->fn_Volume=sound[14]->Volume;
                            }                            
                            break;
#endif
                    }

                    if(scr!=OldScr)
                        ScreenToFront(scr);

                    AllocAnimScreenBuffers();

                    DisplayAnim(a);

                    FreeAnimScreenBuffers();

                    Delay(50);

                    CurrentRP=OldRP;
                }
                else    D(bug("MergeAnim Fallita con codice %ld\n",id));
            
                FreeFrames(a);
            }

            screen=OldScr;

            if(scr!=OldScr)
                CloseScreen(scr);
        }

        Close(fh2);
        }
        Close(fh);
    }        
}

#ifdef CD_VERSION

void Outro(void)
{
    BPTR fh;

    StopMenuMusic();

    if(fh=Open("intro/outro.anim"/*-*/,MODE_OLDFILE))
    {
        struct AnimInstData *a;
        struct Screen *scr=NULL;
        unsigned int id,i;

        if(    WINDOW_WIDTH!=320    ||
            WINDOW_HEIGHT<256    ||
            public_screen    )
        {
            id=BestModeID(BIDTAG_DesiredWidth,320,BIDTAG_DesiredHeight,256,BIDTAG_Depth,4,
                BIDTAG_NominalWidth,320,BIDTAG_NominalHeight,256,
                BIDTAG_DIPFMustHave,DIPF_IS_DBUFFER,
                TAG_DONE);

            if(id!=INVALID_ID)
                scr=OpenScreenTags(NULL,
                    SA_Behind,TRUE,
                    SA_Width,320,
                    SA_Height,256,
                    SA_Interleaved,FALSE,
                    SA_Depth,4,
                    SA_Quiet,TRUE,
                    SA_DisplayID,id,
                    TAG_DONE);
        }
        else
        {
            FreeIFFPalette(screen);
            scr=screen;
        }

        if(scr)
        {
            struct RastPort *OldRP=CurrentRP;
            struct Screen *OldScr=screen;

            screen=scr;

            if(a=LoadFrames(fh))
            {
                PlayMenuMusic();

                CurrentRP=&scr->RastPort;

                if(scr!=OldScr)
                    ScreenToFront(scr);

                AllocAnimScreenBuffers();

                for(i=0;i<16;i++)
                    DisplayAnim(a);

                FreeAnimScreenBuffers();

                Delay(50);

                CurrentRP=OldRP;
            
                FreeFrames(a);
            }

            screen=OldScr;

            if(scr!=OldScr)
                CloseScreen(scr);
            else
            {
                LoadIFFPalette("gfx/eat16menu.col"/*-*/);
                ChangeMenu(current_menu);
            }
        }

        Close(fh);
    }        
}

#endif

#else
void Intro(void)
{
}

void Outro(void)
{
}

#endif

#define ENDCREDITS 645

struct Stage
{
    char *string[5];
    long int Tick;
};

struct Stage stage[]=
{
    {{NULL,"EAT THE WHISTLE"/*-*/,NULL,"2001 EDITION"/*-*/,NULL},0},
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

    {{NULL,"SPOKEN"/*-*/,NULL,"COMMENT"/*-*/,NULL},260},
    {{NULL,NULL,NULL,NULL,NULL},290},
    {{NULL,"ENGLISH"/*-*/,NULL, "JOHN TAYLOR"/*-*/,NULL},295},
    {{NULL,NULL,NULL,NULL,NULL},330},
    {{NULL,"DEUTSCH"/*-*/,NULL, "THILO ETTWEIN"/*-*/,NULL},335},
    {{NULL,NULL,NULL,NULL,NULL},370},
    {{NULL,"ITALIANO"/*-*/,NULL,"GASPARE GRECO"/*-*/,NULL},375},
    {{NULL,NULL,NULL,NULL,NULL},400},
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
