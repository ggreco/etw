#include "etw_locale.h"
#include "menu.h"
#include "os_defs.h"
#include "SDL.h"
#include <ctype.h>

#define OWN_GOAL 64


extern uint8_t team_a, team_b;
extern int8_t player_type[4], role[4], current_field;
extern char shirt[2][24], palette[24], fieldname[24];
extern long int time_length;
extern BOOL use_offside, highlight;
int8_t field;
extern int highsize, matchstatus_size;

char *palettes[]= 
{
    "",
    "dry"/*-*/,
    "hard"/*-*/,
    "soft"/*-*/,
    "frozen"/*-*/,
    "muddy"/*-*/,
};

BOOL ETW_running=FALSE;

struct team_disk leftteam_dk,rightteam_dk;

BOOL CheckMaglie(UBYTE a,UBYTE b)
{
/*
    Ritorno false se il colore 0 delle due squadre e' uguale,
    se il colore 1 e' uguale o se le coppie 1->0 e 0->1 sono
    uguali
*/
    if(     (teamlist[a].jerseys[0].color0 == P_ROSSO0 &&
         teamlist[b].jerseys[0].color0 == P_ROSSO1    ) ||
        (teamlist[b].jerseys[0].color0 == P_ROSSO0 &&
         teamlist[a].jerseys[0].color0 == P_ROSSO1    )
    )
        return FALSE;

    return (BOOL)( (teamlist[a].jerseys[0].color0 !=teamlist[b].jerseys[0].color0 &&
            teamlist[a].jerseys[0].color1 != teamlist[b].jerseys[0].color1 ) ||
            teamlist[a].jerseys[0].color0 == teamlist[b].jerseys[1].color0

/*
    commentato per problemi
            && !(teamlist[a].jerseys[0].color1 == teamlist[b].jerseys[0].color0 &&
            teamlist[a].jerseys[0].color0 == teamlist[b].jerseys[0].color1 &)
*/
             );
}

extern void game_main(void);

#include "files.h"

void WriteGameConfig(FILE *f)
{
    fwrite_u8(use_offside, f);
    fwrite_u8(training, f); 
    fwrite_u8(arcade, f); 
    fwrite_u8(arcade_teams, f); 
    fwrite_u8(free_longpass, f);

    fwrite_u8(injuries, f);
    fwrite_u8(bookings, f);
    fwrite_u8(substitutions, f);
    fwrite_u8(newchange, f);
    fwrite_u8(newpitches, f);

    fwrite_u32(detail_level, f);

    fwrite_u8(team_a, f);
    fwrite_u8(team_b, f);
    fwrite_u8(field, f);
    fwrite_u8(strictness, f);
    fwrite_data(&player_type, sizeof(player_type), f);
    fwrite_u8(current_field, f);

    fwrite_data(&shirt, sizeof(shirt), f);

    fwrite_data(&palette, sizeof(palette), f);
    fwrite_data(&fieldname, sizeof(fieldname), f);

    fwrite_u32(time_length, f);
}

void ReadGameConfig(FILE *f)
{
    use_offside = fread_u8(f);
    training = fread_u8(f); 
    arcade = fread_u8(f); 
    arcade_teams = fread_u8(f); 
    free_longpass = fread_u8(f);
    
    injuries = fread_u8(f);
    bookings = fread_u8(f);
    substitutions = fread_u8(f);
    newchange = fread_u8(f);
    newpitches = fread_u8(f);

    detail_level = fread_u32(f);
    
    team_a = fread_u8(f);
    team_b = fread_u8(f);
    field = fread_u8(f);
    strictness = fread_u8(f);
    fread_data(&player_type, sizeof(player_type), f);
    current_field = fread_u8(f);

    fread_data(&shirt, sizeof(shirt), f);

    fread_data(&palette, sizeof(palette), f);
    fread_data(&fieldname, sizeof(fieldname), f);

    time_length = fread_u32(f);
}

BOOL StartGame(void)
{
    char buf[1024];
    extern void *screen;
    int i;

    StopMenuMusic();

    D(bug("Before FreeMenuStuff()\n"));
    FreeMenuStuff();
    D(bug("After FreeMenuStuff()\n"));

    for(i=0;i<ARCADE_TEAMS+1;i++)
        arcade_gfx[i]=NULL;

    os_stop_audio();

    game_start=TRUE;

    game_main();

    if(network_game) {
        network_game=FALSE;
        free_network();
    }

    {
        SDL_Event e;

        while(SDL_PollEvent(&e));
    }

    game_start = FALSE;

    snprintf(buf, 1024, "%sthismatch", TEMP_DIR);
#ifdef __CODEGUARD__
    if(access(buf,0) != -1)
#endif
    remove(buf/*-*/);

    WINDOW_WIDTH=wanted_width;
    WINDOW_HEIGHT=wanted_height;

    OpenMenuScreen();

    SDL_ShowCursor(1);

    if(!screen || !(LoadMenuStuff())) {
        request("Unable to load the menu datas!");
        return FALSE;
    }

    os_start_audio();

    if(arcade_teams)
        LoadArcadeGfx();

    ClearSelection();

    return TRUE;
}

WORD StartMatch(BYTE team1,BYTE team2)
{
    FILE *f;
    BOOL /*swapped=FALSE,*/ team_swap=FALSE,control_swap=FALSE,nightgame=FALSE,random_strict=FALSE;
    WORD risultato=-1,parent_menu;
    int t;

    parent_menu=current_menu;

// temporanea, solo x evitare l'apertura della squadra!
    if(network_game)
        controllo[team2]=-1;

    if(!training&&!network_game) {
        if(RangeRand(2)) {
            UBYTE temp;

            temp=team1;
            team1=team2;
            team2=temp;
            team_swap=TRUE;
        }

        if(controllo[team2]>=0 && controllo[team1]==controllo[team2]) {
            control_swap=TRUE;

            controllo[team2]--;

            if(controllo[team2]<0)
                controllo[team2]=1;
        }
    }
    else if(network_game && network_player->num) {
        UBYTE temp=team1;

        D(bug("Player got team 1, swapping teams\n"));

        team1=team2;
        team2=temp;
        team_swap=TRUE;
    }

    if( (daytime==0 && RangeRand(2)==0 ) || daytime==2 )
            nightgame=TRUE;

    if(controllo[team1]>=0&&!arcade_teams) {
        SetTeamSettings(team1, TRUE);
        ChangeMenu(MENU_TEAM_SETTINGS);
        while(HandleMenuIDCMP());
    }

    if(controllo[team2]>=0&&!training&&!arcade_teams) {
        SetTeamSettings(team2, TRUE);
        ChangeMenu(MENU_TEAM_SETTINGS);
        while(HandleMenuIDCMP());
    }

    window_opened=FALSE;
//    write_config(TEMP_DIR "thismatch"/*-*/);

    if(arcade_teams)
    {
        extern BYTE arcade_team[];

        arcade_team[0]=team1;
        arcade_team[1]=team2;
    }

    team_a=team1;
    team_b=team2;

/*
 * Questo codice si occupa di inviare la squadra e ricevere
 * l'altra, notare la specularita' a seconda del controller
 * che si e' deciso di utilizzare.
 */
    if(!network_game || network_player->num == 0) {
        if(network_game && !SendTeam(team1))
                return -1;

        leftteam_dk=teamlist[team1];
    }
    else if(network_game && !ReceiveTeam(&leftteam_dk))
            return -1;

    if(!network_game || network_player->num == 1) {
        if(network_game && !SendTeam(team2))
                return -1;

        rightteam_dk=teamlist[team2];
    }
    else if(network_game && !ReceiveTeam(&rightteam_dk))
            return -1;

    if(!training && !network_game)
    {
        if(!CheckMaglie(team1,team2))
            rightteam_dk.jerseys[0]=rightteam_dk.jerseys[1];
    }

    use_offside = FALSE;
    
    if(training) {
        rightteam_dk.jerseys[0]=rightteam_dk.jerseys[1];
    }
    else if (offside)
        use_offside = TRUE;
    
    sprintf(shirt[0],"gfx/play%lc%lc%lc.obj"/*-*/,
            ( (nightgame||arcade) ? 'n' : 'e'),
            ( (field==8&&!arcade) ? 's' : 'r'),
            teamlist[team1].jerseys[0].type+'a');

    sprintf(shirt[1],"gfx/play%lc%lc%lc.obj"/*-*/,
            ( (nightgame||arcade) ? 'n' : 'e'),
            ( (field==8&&!arcade) ? 's' : 'r'),
            (training ? teamlist[team2].jerseys[1].type : teamlist[team2].jerseys[0].type)+'a');

    if(!field)
        t=RangeRand(6);
    else
        t=field-1;

    current_field= (arcade ? 0 : t);

    if(arcade)
    {
        if(!final&&!warp)
        {
            sprintf(fieldname,"gfx/arcade.gfx"/*-*/);
            sprintf(palette,"gfx/eat16arcade.col"/*-*/);
        }
        else
        {
            sprintf(fieldname,"gfx/a_final.gfx"/*-*/);
            sprintf(palette,"gfx/eat16arcade.col"/*-*/);
        }
    }
    else if(field>=7)
    {
        if(field==7)
        {
            if(newpitches)
            {
                sprintf(fieldname,"newgfx/pitchwet+.gfx"/*-*/);
                sprintf(palette,"newgfx/eat32wet.col"/*-*/);
            }
            else
            {
                sprintf(fieldname,"gfx/pitchwet.gfx"/*-*/);
                sprintf(palette,"gfx/eat16wet.col"/*-*/);
            }
        }
        else
        {
            if(newpitches)
            {
                sprintf(fieldname,"newgfx/pitchsnow+.gfx"/*-*/);
                sprintf(palette,"newgfx/eat32snow.col"/*-*/);
            }
            else
            {
                sprintf(fieldname,"gfx/pitchsnow.gfx"/*-*/);
                sprintf(palette,"gfx/eat16snow.col"/*-*/);
            }
        }
    }
    else {
        char c=field_type-1;

        if(!field_type)
            c=RangeRand(NUMERO_CAMPI);

        if(newpitches)    {
            sprintf(fieldname,"newgfx/pitch%lc+.gfx"/*-*/,c+'a');
            sprintf(palette,"newgfx/eat32%s.col"/*-*/,palettes[t]);
        }
        else {
            sprintf(fieldname,"gfx/pitch%lc.gfx"/*-*/,c+'a');
            sprintf(palette,"gfx/eat16%s.col"/*-*/,palettes[t]);
        }
    }

    if(!network_game) {
        if(ruolo[team1])
            role[0]=ruolo[team1];
        if(ruolo[team2])
            role[1]=ruolo[team2];
    }

    if(strictness==10) {
        random_strict=TRUE;
        strictness=RangeRand(10);
    }

    if(use_gfx_scaling)
        use_scaling=TRUE;

#ifndef DEMOVERSION
    t=t_l;

    if(arcade&&competition==MENU_CHALLENGE)
        t=3;
    else if(training)
        t=20;
#else
    t=1
#endif

    t_l=t;

    time_length=t*60;

    if(use_speaker)
    {
        extern char spk_basename[64];
        FILE *l;
        char buffer[100];

        strcpy(buffer,"talk/"/*-*/);
        strcat(buffer,localename);
        strcat(buffer,".spk"/*-*/);
    
        if(!(l=fopen(buffer,"r"/*-*/)))
            strcpy(localename,"english"/*-*/);
        else
            fclose(l);

        sprintf(spk_basename,"talk/%s"/*-*/,localename);

        D(bug("Setto il commento a %s\n"/*-*/,localename));
    }

    oldwidth=WINDOW_WIDTH;
    oldheight=WINDOW_HEIGHT;

    // XXX not yet completed the correct association of player and controls in network play.


    if(network_game) {
        final = TRUE;
        training = FALSE;

        if(network_player->num)
            controllo[team1]=controllo[team2]^1;
        else
            controllo[team2]=controllo[team1]^1;
    }

    player_type[0]=controllo[team1];
    player_type[1]=(!training ? controllo[team2] : -1);


    D(bug("Tipo giocatore 0: %d\nTipo giocatore 1: %d\n", player_type[0], player_type[1]));
    
    if(!StartGame())
    {
        request(msg_84);
        use_scaling=FALSE;
        warp=FALSE;
        final=FALSE;
        friendly=FALSE;
        ChangeMenu(0);
        return -1;
    }

    if(random_strict)
        strictness=10;

    if(control_swap)
        controllo[team2]=controllo[team1];

    warp=FALSE;
    final=FALSE;
    friendly=FALSE;
    use_scaling=FALSE;

    /* AC: Stranamente non veniva usata la define. */ 
    if((f=fopen(RESULT_FILE, "r")))    {
        char buffer[20];
        int i,l;

// Qui devo gestire il risultato
        
        fgets(buffer,19,f);

        if(!strnicmp(buffer,"error",5))
        {
            request(msg_84);
        }
        else if(!strnicmp(buffer,"break",5))
        {
            if( (controllo[team1]>=0 && controllo[team2]>=0) ||
                (controllo[team1]<0 && controllo[team2]<0)  )
                risultato=0; // Se si gioca in due o in 0 il risultato e' 0 a 0.
            else if( (controllo[team1]<0&&team_swap==FALSE) ||
                 (controllo[team2]<0&&team_swap==TRUE) )
                risultato=5;  // 5 a 0
            else
                risultato=(5<<8); // 0 a 5
        }
        else
        {
            int gol_a=0,gol_b=0;

            if(sscanf(buffer,"%d-%d",&gol_a,&gol_b)==2)    {
                D(bug("Result before swap %d-%d\n", gol_a, gol_b));
                
                if(team_swap != arcade) { // this is the fix for the arcade match bug
                    int s;

                    s=gol_a;
                    gol_a=gol_b;
                    gol_b=s;
                }

                D(bug("Result after swap %d-%d\n", gol_a, gol_b));
                
                risultato=gol_a | (gol_b<<8); 
            }

            if(!arcade_teams&&!training) {
                int yl=FixedScaledY(125);
                int yr;
                char *c=buffer,*d;

                yr=yl;

// Codice per ricavarmi due stringhe che abbiano i punteggi delle due squadre.
                while(*c!='-')
                    c++;

                *c=0;
                c++;
                d=c;

                while(*d>' ')
                    d++;
                *d=0;


                mr[2].Colore=mr[0].Colore=colore_team[controllo[team1]+1];
                mr[2].Highlight=mr[0].Highlight=highlight_team[controllo[team1]+1];
                mr[1].Colore=mr[3].Colore=colore_team[controllo[team2]+1];
                mr[1].Highlight=mr[3].Highlight=highlight_team[controllo[team2]+1];

                mr[0].Testo=teamlist[team1].name;
                mr[1].Testo=teamlist[team2].name;

                mr[2].Testo=buffer;
                mr[3].Testo=c;

                ChangeMenu(MENU_MATCH_RESULT);

                for(i=0;i<(gol_a+gol_b);i++)
                {
                    int team,num,min;

                    if(fgets(buffer,19,f))
                    {
                        if(!strnicmp(buffer,"penalties"/*-*/,9))
                        {
                            int reti=gol_a+gol_b;

                            fgets(buffer,19,f);

                            D(bug("Partita finita ai rigori, reti originali: %ld, %ld ai rigori!\n",reti,atol(buffer)));

                            reti-=atol(buffer);
                            gol_a=reti;
                            gol_b=0;

                            fgets(buffer,19,f);
                        }

                        if(sscanf(buffer,"%d %d %d",&team,&num,&min)==3)
                        {
                            int t= (team==0 ? team1 : team2),og=0,j;
                            struct player_disk *g = NULL;

                            if(num&OWN_GOAL)
                            {
                                og=1;
                                t= t==team1 ? team2 : team1;
                                num&=~(OWN_GOAL);
                            }
        
                            for(j=0;j<teamlist[t].nplayers;j++)
                                if(teamlist[t].players[j].number==num)
                                    g=&teamlist[t].players[j];
    
                            if(g)
                            {
                                int x=mr[0].X1,*y=&yl;
                                char buf[40];

                                if(WINDOW_HEIGHT>350&&WINDOW_WIDTH>430)
                                    setfont(bigfont);
                                else
                                    setfont(smallfont);

                                if( (t==team2&&!og) || (t==team1&&og))
                                {
                                    x=mr[1].X1;
                                    y=&yr;
                                }

                                l=sprintf(buf,"%s (%s%d)",g->surname, og ? "OG " : "" ,min);

                                d=buf;

                                while(*d)
                                {
                                    *d=toupper(*d);
                                    d++;
                                }

                                D(bug("Gol %ld: %s (%ld,%ld)\n",i,buf,x,*y));

                                if(*y<(WINDOW_HEIGHT-bigfont->height))
                                {
                                    TextShadow(x,*y,buf,l);
                                }

                                if(WINDOW_HEIGHT>350&&WINDOW_WIDTH>430)
                                    *y+=(bigfont->height+2);
                                else
                                    *y+=smallfont->height;

                                *y+=3;
                            }
                            else
                                D(bug("Giocatore inesistente!\n"));
                        }
                        else
                        {
                            D(bug("Errore nel parsing dei marcatori!\n (sscanf)"/*-*/));
                        }
                    }
                    else
                    {
                        D(bug("Errore nel parsing dei marcatori (FGets)!\n"/*-*/));
                    }
    
                }

                /* AC: Non comparivano i nomi dei marcatori. */
                ScreenSwap();

                while(HandleMenuIDCMP());
    
                while(fgets(buffer,19,f))
                {
                    int num,team;
                    char op;
    
                    if(sscanf(buffer,"%d %d %c"/*-*/,&team,&num,&op)==3)
                    {
                        int t= (team==0 ? team1 : team2);
                        struct player_disk *g=NULL;
    
                        for(i=0;i<teamlist[t].nplayers;i++)
                            if(teamlist[t].players[i].number==num)
                                g=&teamlist[t].players[i];
        
                        if(g)
                        {
                            D(bug("Eseguo %lc sul giocatore n.%d della squadra %d\n"/*-*/,(long int)op,num,t));
    
                            switch(op)
                            {
                                case 'e':
                                    g->Ammonizioni=4;
                                    break;
                                
                                case 'a':
                                    g->Ammonizioni++;
    
                                    if(g->Ammonizioni>1)
                                        g->Ammonizioni=4;
                                    break;
        
                                case 'i':
                                    if(competition==MENU_WORLD_CUP||competition==MENU_MATCHES)
                                        g->injury+=RangeRand(5)+1;
                                    else
                                        g->injury+=RangeRand(10)+1;
                                    break;
        
                                default:
                                    D(bug("Operazione non definita...\n"/*-*/));
                            }
                        }
                        else
                        {
                            D(bug("Comando su giocatore inesistente!!!\n"/*-*/));
                        }
                    }
                    else
                    {
                        D(bug("Errore nel parsing di infortuni/ammonizioni!\n"/*-*/));
                    }
                }
            }
        }

        fclose(f);
    }

    if(parent_menu==MENU_ARCADE_SELECTION||parent_menu==MENU_TEAM_SELECTION)
        ChangeMenu(parent_menu);
    else {
        D(bug("No menu change! Parent menu: %ld\n"/*-*/,parent_menu));
    }

    return risultato;
}

void LoadHigh(char *file)
{
    char buf[1024];
    FILE *f,*f2;

    if((f=fopen(file,"rb"))) {
        ReadGameConfig(f);

// team handling...

        ReadTeam(f, &leftteam_dk);
        ReadTeam(f, &rightteam_dk);

        highsize = fread_u16(f);
        
// Here we handle the REAL highlight...

        snprintf(buf, 1024, "%shigh", TEMP_DIR);
        if(!(f2=fopen(buf/*-*/,"wb"))) {
            fclose(f);
            return;
        }

        while(!feof(f)) {
            unsigned char c = fgetc(f);
            fputc(c, f2);
        }
        
        fclose(f2);
        fclose(f);
        
        highlight = TRUE;

        StartGame();

        highlight = FALSE;

        ChangeMenu(current_menu);
    }
}

