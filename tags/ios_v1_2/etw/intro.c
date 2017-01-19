#include "etw_locale.h"
#include "menu.h"
#include "SDL.h"
#include "anim.h"

void Intro(void)
{
    FILE *fh;

    if ((fh = os_open("intro/intro.anim"/*-*/, "rb"))) {
        char buffer[80];
        FILE *fh2;
        int t = RangeRand(NUMERO_INTRO);

        D(bug("Playing intro %ld...\n",t));

        sprintf(buffer,"intro/intro%lc.anim"/*-*/,'a'+t);

        if ((fh2 = os_open(buffer, "rb"))) {
            struct AnimInstData *a;
            unsigned int id;

            if ((a = LoadFrames(fh))) {
                if (!(id = MergeAnim(a,fh2))) {
                    struct FrameNode *fn=(struct FrameNode *)a->aid_FrameList.pHead;
                    int i;

                    fn->fn_Sample=menusound[1];
                    fn->fn_Loops=4;

                    if((fn=GetFrameNode(a,90))) {
                        fn->fn_Sample=menusound[2];
                        fn->fn_Loops=0;
                    }

                    if((fn=GetFrameNode(a,100))) {
                        fn->fn_Sample=menusound[1];
                        fn->fn_Loops=7;
                    }

                    if((fn=GetFrameNode(a,300))) {
                        fn->fn_Sample=menusound[1];
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
                                fn->fn_Loops=0;
                                fn->fn_Sample=menusound[0];
                            }
                            break;
                        case 4:
                            if((fn=GetFrameNode(a,423))) {
                                fn->fn_Loops=0;
                                fn->fn_Sample=menusound[17];
                            }                            
                            if((fn=GetFrameNode(a,429))) {
                                fn->fn_Loops=0;
                                fn->fn_Sample=menusound[15];
                            }                            
                            if((fn=GetFrameNode(a,445))) {
                                fn->fn_Loops=0;
                                fn->fn_Sample=menusound[16];
                            }                            
                            break;
                        case 5:
                            if((fn=GetFrameNode(a,405))) {
                                fn->fn_Loops=0;
                                fn->fn_Sample=menusound[14];
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

    if ((fh = os_open("intro/outro.anim"/*-*/, "rb"))) {
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

#define ENDCREDITS 580

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
    {{"MUSIC"/*-*/,NULL,"NOISY TUBE BY HORRORPEN", "BLUEBEAT 1 BY ERH", NULL},260},
    {{NULL,NULL,NULL,NULL,NULL},290},
    {{"ADDITIONAL CODE", NULL, "ANDREA CAROLFI", NULL, NULL}, 295},
    {NULL,NULL,NULL,NULL,NULL,320},
    {"ETW USES SDL"/*-*/,NULL,"WWW.LIBSDL.ORG"/*-*/,"FOR MORE INFO"/*-*/,NULL,325},
    {NULL,NULL,NULL,NULL,NULL,340},
    {{"BETATESTING"/*-*/,NULL,"ALEXANDRA BENVENUTI"/*-*/,NULL, NULL},345},
    {NULL,NULL,NULL,NULL,NULL,360},
    {{"BETATESTING"/*-*/,NULL,"ANDREA CAROLFI"/*-*/,"FRANCESCO DUTTO"/*-*/,"MAURIZIO FAGGIONI"/*-*/},365},
    {{NULL,NULL,NULL,NULL,NULL},395},
    {{NULL,"LORENZO MORSELLI"/*-*/,"THOMAS STEIDING"/*-*/,NULL,msg_81},400},
    {{NULL,NULL,NULL,NULL,NULL},435},
    {{"SPECIAL THANKS"/*-*/,NULL,"SARA CAPPUCCINI"/*-*/,"DAVIDE CARGIOLLI"/*-*/,"FAUSTO CENDERELLI"/*-*/},440},
    {{NULL,NULL,NULL,NULL,NULL},470},
    {{"CLAUDIA CHIARDO"/*-*/,"ELENA FACONTI"/*-*/,"PAOLA FROLA"/*-*/,"GIULIANO GRECO"/*-*/,"ANDREA MENCONI"/*-*/},475},
    {{NULL,NULL,NULL,NULL,NULL},505},
    {{"MICHELE PANELLA"/*-*/,"NICOLO' PANELLA"/*-*/,"MATTEO RATTI"/*-*/,"GRAZIANO VIVIANI"/*-*/,"GIANLUIGI ZUNINO"/*-*/},510},
    {{NULL,NULL,NULL,NULL,NULL},555},
    {{NULL,NULL,NULL,NULL,NULL},ENDCREDITS+2}, // Questo deve sempre rimanere, e' per riferimento
};

#define MAXSTRINGLEN 20

static long int actual=0, started;
static BOOL clean=TRUE;

BOOL credits_iteration()
{
    int top_x=WINDOW_WIDTH/4-MAXSTRINGLEN*bigfont->width/2,
        top_y=WINDOW_HEIGHT/2-(bigfont->height+6)*3;
    
    long int ticks = (os_get_timer() - started) / 100;
    
    if (ticks < ENDCREDITS) {
        SDL_Event e;
        
        while (SDL_PollEvent(&e)) {
            
            switch(e.type) {
                case SDL_FINGERDOWN:
                case SDL_FINGERUP:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_KEYDOWN:
                    LoadBack();
                    ChangeMenu(current_menu);
                    return FALSE;
            }
        }
        
        if(ticks > stage[actual].Tick) {
            int i,x,y;
            
            if(!clean) {
                bltchunkybitmap(back,0,0,
                                main_bitmap,0,0,
                                WINDOW_WIDTH,WINDOW_HEIGHT,bitmap_width,bitmap_width);
                        
                clean = TRUE;
            }
            
            y=top_y+bigfont->height;
            
            for (i = 0; i < 5; i++) {
                if (stage[actual].string[i]) {
                    int l=(int)strlen(stage[actual].string[i]);
                    
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
    else {
        add_achievement("9_credits", 100.0);
    
        LoadBack();
        ChangeMenu(current_menu);
        return FALSE;
    }

    return TRUE;
}

void ShowCredits(void)
{
    gfx_t *o;
    
    // Serching in newgfx and then in menugfx...
    
    if(!(o=LoadGfxObject("newgfx/credits.gfx"/*-*/,Pens)))
        o=LoadGfxObject("menugfx/credits.gfx"/*-*/,Pens);
    
    
    if(o) {
        setfont(bigfont);
        
        os_flush_events();
        
        ScaleGfxObj(o, back);
        FreeGfxObj(o);
        
        memcpy(main_bitmap,back,WINDOW_WIDTH*WINDOW_HEIGHT);
        ScreenSwap();
        actual = 0;
        clean = TRUE;
        started = os_get_timer();

        while (credits_iteration()) 
            os_delay(5);

        os_flush_events();
    }
}

