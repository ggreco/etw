#include "etw_locale.h"
#include "menu.h"
#include "os_defs.h"
#include "SDL.h"
#include <ctype.h>

#define OWN_GOAL 64

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

struct Squadra_Disk leftteam_dk,rightteam_dk;

BOOL CheckMaglie(UBYTE a,UBYTE b)
{
/*
	Ritorno false se il colore 0 delle due squadre e' uguale,
	se il colore 1 e' uguale o se le coppie 1->0 e 0->1 sono
	uguali
*/
	if( 	(teamlist[a].maglie[0].Colore0 == P_ROSSO0 &&
 		teamlist[b].maglie[0].Colore0 == P_ROSSO1	) ||
		(teamlist[b].maglie[0].Colore0 == P_ROSSO0 &&
 		teamlist[a].maglie[0].Colore0 == P_ROSSO1	)
	)
		return FALSE;

	return (BOOL)( (teamlist[a].maglie[0].Colore0 !=teamlist[b].maglie[0].Colore0 &&
			teamlist[a].maglie[0].Colore1 != teamlist[b].maglie[0].Colore1 ) ||
			teamlist[a].maglie[0].Colore0 == teamlist[b].maglie[1].Colore0

/*
	commentato per problemi
			&& !(teamlist[a].maglie[0].Colore1 == teamlist[b].maglie[0].Colore0 &&
			teamlist[a].maglie[0].Colore0 == teamlist[b].maglie[0].Colore1 &)
*/
			 );
}

extern void game_main(void);

BOOL StartGame(void)
{
	extern void *screen;
	int i;

	StopMenuMusic();

	D(bug("Prima di FreeMenuStuff()\n"));
	FreeMenuStuff();
	D(bug("Dopo FreeMenuStuff()\n"));

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

	remove(TEMP_DIR "thismatch"/*-*/);

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

extern UBYTE team_a,team_b;
extern BYTE player_type[4],role[4],current_field;
extern char shirt[2][24],palette[24],fieldname[24];
extern LONG time_length;
extern BOOL use_offside;
BYTE field;

WORD StartMatch(BYTE team1,BYTE team2)
{
	FILE *f;
	BOOL swapped=FALSE,team_swap=FALSE,control_swap=FALSE,nightgame=FALSE,random_strict=FALSE;
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
//	write_config(TEMP_DIR "thismatch"/*-*/);

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
			rightteam_dk.maglie[0]=rightteam_dk.maglie[1];
	}

	if(training) {
		rightteam_dk.maglie[0]=rightteam_dk.maglie[1];
    }
    else if (offside) {
        use_offside = TRUE;
    }
    else
        use_offside = FALSE;
    
	sprintf(shirt[0],"gfx/play%lc%lc%lc.obj"/*-*/,
			( (nightgame||arcade) ? 'n' : 'e'),
			( (field==8&&!arcade) ? 's' : 'r'),
			teamlist[team1].maglie[0].Tipo+'a');

	sprintf(shirt[1],"gfx/play%lc%lc%lc.obj"/*-*/,
			( (nightgame||arcade) ? 'n' : 'e'),
			( (field==8&&!arcade) ? 's' : 'r'),
			(training ? teamlist[team2].maglie[1].Tipo : teamlist[team2].maglie[0].Tipo)+'a');

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
	else
	{
		char c=field_type-1;

		if(!field_type)
			c=RangeRand(NUMERO_CAMPI);

		if(newpitches)
		{
			sprintf(fieldname,"newgfx/pitch%lc+.gfx"/*-*/,c+'a');
			sprintf(palette,"newgfx/eat32%s.col"/*-*/,palettes[t]);
		}
		else
		{
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
	if((f=fopen(RESULT_FILE, "r")))	{
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

			if(sscanf(buffer,"%d-%d",&gol_a,&gol_b)==2)	{
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

				mr[0].Testo=teamlist[team1].nome;
				mr[1].Testo=teamlist[team2].nome;

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
							struct Giocatore_Disk *g = NULL;

							if(num&OWN_GOAL)
							{
								og=1;
								t= t==team1 ? team2 : team1;
								num&=~(OWN_GOAL);
							}
		
							for(j=0;j<teamlist[t].NumeroGiocatori;j++)
								if(teamlist[t].giocatore[j].Numero==num)
									g=&teamlist[t].giocatore[j];
	
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

								l=sprintf(buf,"%s (%s%d)",g->Cognome, og ? "OG " : "" ,min);

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
						struct Giocatore_Disk *g=NULL;
	
						for(i=0;i<teamlist[t].NumeroGiocatori;i++)
							if(teamlist[t].giocatore[i].Numero==num)
								g=&teamlist[t].giocatore[i];
		
						if(g)
						{
							D(bug("Eseguo %lc sul giocatore n.%ld della squadra %ld\n"/*-*/,(LONG)op,num,t));
	
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
										g->Infortuni+=RangeRand(5)+1;
									else
										g->Infortuni+=RangeRand(10)+1;
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
	else 
	{
		D(bug("No menu change! Parent menu: %ld\n"/*-*/,parent_menu));
	}

	return risultato;
}

void LoadHigh(char *file)
{
	FILE *f,*f2,*f3;

    request("LoadHigh not yet implemented in portable version");

    return;

// XXX this needs to be reworked...
	if((f=fopen(file,"rb")))
	{
// XXX sizeof(struct MatchStatus)= 2056, occhio, se modifichiamo MatchStatus va modificato!
		UWORD len,highsize;
		int i,matchstatus_size= 2056; // era sizeof(struct MatchStatus)
		char *a;
		struct Squadra_Disk s;

// Gestione di t:thismatch

		if(!(f2=fopen(TEMP_DIR "thismatch"/*-*/,"w")))
		{
			fclose(f);
			return;
		}

		fread(&len,1,sizeof(WORD),f);

		SWAP_WORD(len);

		if(!(a=malloc(len)))
		{
			fclose(f);
			fclose(f2);
			return;
		}

		fread(a,len,1,f);

		for(i=0;i<len-10;i++)
		{
			if(	!strncmp(a+i,"width="/*-*/,6)	||
				!strncmp(a+i,"height="/*-*/,7)	||
				!strncmp(a+i,"workbench"/*-*/,9)||
				!strncmp(a+i,"screen="/*-*/,7)	||
				!strncmp(a+i,"window="/*-*/,7)	)
			{
				a[i]=';';
			}
		}

		fwrite(a,len,1,f2);

		free(a);

// Gestione delle squadre...

		if(!(f3=fopen(TEMP_DIR "leftteam"/*-*/,"wb")))
		{
			fclose(f);
			fclose(f2);
			return;
		}

		ReadTeam(f,&s);
		WriteTeam(f3,&s);

		fclose(f3);

		if(!(f3=fopen(TEMP_DIR "rightteam"/*-*/,"wb")))
		{
			fclose(f);
			fclose(f2);
			return;
		}

		ReadTeam(f,&s);
		WriteTeam(f3,&s);

		fclose(f3);

		fread(&highsize,sizeof(WORD),1,f);

		fprintf(f2,"highsize=%ld\n"/*-*/,(LONG)highsize);

		if(wb_game)
			fprintf(f2,"workbench\n"/*-*/);
		else if(display_id)
			fprintf(f2,"displayid=%ld\noverscan=%ld\n"/*-*/,display_id,overscan );

		if( (wb_game&&public_screen) || !public_screen)
			fprintf(f2,"width=%ld\nheight=%ld\n"/*-*/,WINDOW_WIDTH,WINDOW_HEIGHT);

		fclose(f2);

// Gestione del file di highlight vero e proprio...

		if(!(f2=fopen(TEMP_DIR "high"/*-*/,"wb")))
		{
			fclose(f);
			return;
		}

		len=highsize*MAX_PLAYERS*sizeof(ULONG)+matchstatus_size+sizeof(WORD);

		if(!(a=malloc(len)))
		{
			fclose(f);
			fclose(f2);
			return;
		}

		fread(a,len,1,f);
		fwrite(a,len,1,f2);
		fclose(f2);

		free(a);

		fclose(f);

		StartGame();

		ChangeMenu(current_menu);
	}
}

