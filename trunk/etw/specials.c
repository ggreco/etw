#include "etw_locale.h"
#include "menu.h"
#ifdef WIN
#include "highdirent.h"
#elif SOLARIS_X86
#include <dirent.h>
#else
#include <sys/dir.h>
#endif

#include "SDL.h"

extern void MenuResizing(int,int);
extern char joycfg_buttons[2][8];

int actual_joystick=0;

char *buttons[]=
{
	"BUTTON 1",
	"BUTTON 2",
	"BUTTON 3",
	"BUTTON 4",
	"BUTTON 5",
	"BUTTON 6",
	"BUTTON 7",
	"BUTTON 8",
	"BUTTON 9",
	"BUTTON 10",
	"BUTTON 11",
	"BUTTON 12",
	"BUTTON 13",
};

LONG jingle=-1;
extern struct GfxMenu *actual_menu;
extern int FIXED_SCALING_WIDTH,FIXED_SCALING_HEIGHT;

BOOL make_setup=FALSE,game_start=FALSE,can_modify=TRUE,savehigh=FALSE,
	triple=FALSE,chunky_version=FALSE,use_gfx_scaling=FALSE;
BYTE selected_number=0,wanted_number=0,duration=1,field_type=0,daytime=0;
char *enabled=msg_7,*disabled=msg_8;
extern struct SoundInfo *busy[];
extern int os_videook(int,int);

BYTE arcade_sequence[]={0,1,2,3,4,5,6,7,8,9,10};

BYTE mondiali[]={8,4,2,2},current_resolution=0,current_scaling=0;

char *resolutions[]=
{
	"320x200",
	"320X240",
	"400X300",
	"640x400",
	"640X480",
	"800X600",
	NULL
};


void UpdateJoyCfg(int joy);

void init_joy_config(void)
{
	int i,k;

	for(i=0;i<2;i++)
	{
		for(k=0;k<8;k++)
			joycfg_buttons[i][k]=k;
	}
}

char *scaling_resolutions[]=
{
	"320X240",
	"320X256",
	"356X288",
	"400X300",
	NULL
};

char *daytimes[]=
{
	msg_2,
	msg_9,
	msg_10,
	NULL
};

char *field_types[]=
{
	msg_2,
	"A"/*-*/,
	"B"/*-*/,
	"C"/*-*/,
	"D"/*-*/,
	"E"/*-*/,
	"F"/*-*/,
	NULL,
};

void SetCurrentResolution(void)
{
	if(WINDOW_WIDTH<=360)
	{
		if(WINDOW_HEIGHT<210)
			current_resolution=0;
		else
			current_resolution=1;
	}
	else if(WINDOW_WIDTH<=450)
	{
		current_resolution=2;
	}
	else if(WINDOW_WIDTH<700)
	{
		if(WINDOW_HEIGHT<410)
			current_resolution=3;
		else
			current_resolution=4;
	}
	else
		current_resolution=5;
}

char *radar_options[]=
{
	msg_11,
	msg_12,
	msg_13,
	msg_14,
	msg_15,
	msg_16,
	msg_17,
	msg_18,
	msg_19,
	msg_20,
	msg_21,
	msg_22,
	NULL,
};

char *field_options[]=
{
	msg_2,
	msg_23,
	msg_24,
	msg_25,
	msg_26,
	msg_27,
	msg_28,
	msg_29,
	msg_30,
	NULL,
};

char *time_options[]=
{
	"1"/*-*/,
	"2"/*-*/,
	"3"/*-*/,
	"5"/*-*/,
	"10"/*-*/,
	"20"/*-*/,
	"45"/*-*/,
	NULL,
};

// Mischio le squadre!

void RandomDraw(int n)
{
	int i=RangeRand(50),k,s1,s2;
	BYTE temp;

	for(k=0;k<i;k++)
	{
		s1=RangeRand(n);
		s2=RangeRand(n);

		if(s1==s2)
			continue;

// controllo e' fisso sulle squadre, quindi non serve swapparlo!

		temp=teamarray[s1];
		teamarray[s1]=teamarray[s2];
		teamarray[s2]=temp;
	}
}

void InvertiSquadre(void)
{
	int i,k;
	UBYTE t;

	for(i=0;i<totale_giornate;i++)
	{
		for(k=0;k<((numero_squadre+1)/2);k++)
		{
			t=turni[i][k].t1;
			turni[i][k].t1=turni[i][k].t2;
			turni[i][k].t2=t;
		}
	}
}

void ClearMatches(int k)
{
	int i,j;

	for(i=k*4;i<64;i+=4)
	{
		mp[i].Colore=P_VERDE1;
		mp[i].Highlight=P_VERDE0;

		if(mp[i+3].Testo)
			free(mp[i+3].Testo);

		for(j=0;j<4;j++)
			mp[i+j].Testo=NULL;
	}
}

void ClearSelection(void)
{
	int i;

	selected_number=0;

	if(!arcade_teams)
	{
		for(i=0;i<64;i++)
			teamselection[i].Colore=COLORE_UNSELECTED;
	}
	else
	{
		for(i=0;i<ARCADE_TEAMS;i++)
			asb[i].Colore=asb[i].Highlight=COLORE_UNSELECTED;
	}
}

void NewTurn(void)
{
	register int i,j;

	turno++;

	for(i=0;i<campionato.NumeroSquadre;i++)
	{
		for(j=0;j<teamlist[i].NumeroGiocatori;j++)
		{
			if(teamlist[i].giocatore[j].Infortuni>0)
				teamlist[i].giocatore[j].Infortuni--;

			if(teamlist[i].giocatore[j].Ammonizioni>1)
				teamlist[i].giocatore[j].Ammonizioni-=2;
		}
	}
}

BOOL CanContinue(void)
{
  if(wanted_number==0)
  {
	int i;

	for(i=1;i<7;i++)
	{
		if(selected_number==(1<<i))
			return TRUE;
	}

	return FALSE;
  }

  return (BOOL) ( (wanted_number>0&&selected_number==wanted_number) ||
			  (wanted_number<0&&selected_number>=(-wanted_number) ) );
}

BOOL ReturnFalse(WORD bottone)
{
	return FALSE;
}

BOOL TeamSelection(WORD bottone)
{
	static BYTE selected=-1;
	static BOOL team1_selected=FALSE,team2_selected=FALSE;
	struct Bottone *b;


	if(bottone<0)
		return TRUE;

	b=&actual_menu->Bottone[bottone];

	if(bottone<64)
	{
		struct Bottone *b2=&actual_menu->Bottone[64];
		struct Bottone *b3=&actual_menu->Bottone[66];

		if(selected>=0)
			MyRestoreBack();
		
		selected=bottone;

		if(competition!=MENU_WORLD_CUP||wanted_number!=32)
		{
			if(b->Colore!=COLORE_COMPUTER)
			{
				PrintShadow(FixedScaledX(1),FixedScaledY(210),b->Testo,strlen(b->Testo),bigfont);

				if(!b3->Testo)
				{
					b3->Testo=msg_31;
					RedrawBottone(b3,b3->Colore);
				}
			}
			else
			{
				b3->Testo=NULL;
				CancellaBottone(b3);
			}
		}

		if(b->Colore==COLORE_UNSELECTED)
		{
			selected_number++;

			controllo[-b->ID-1]=0;

			if(!team1_selected || wanted_number>2 || wanted_number<=0 || selected_number>2)
			{
				team1_selected=TRUE;
				controllo[-b->ID-1]=1;
				b->Colore=COLORE_SQUADRA_A;
			}
			else
			{
				b->Colore=COLORE_SQUADRA_B;			
				team2_selected=TRUE;
				controllo[-b->ID-1]=0;
			}
		}
		else if(b->Colore==COLORE_COMPUTER)
		{
			b->Colore=COLORE_UNSELECTED;
			selected_number--;
		}
		else if(b->Colore==COLORE_SQUADRA_A && (!team2_selected || wanted_number>2 || wanted_number<=0 || selected_number>2) )
		{
			team1_selected=FALSE;
			controllo[-b->ID-1]=0;
			b->Colore=COLORE_SQUADRA_B;
		}
		else
		{
			if(b->Colore==COLORE_SQUADRA_A)
				team1_selected=FALSE;
			else
				team2_selected=FALSE;

			b->Colore=COLORE_COMPUTER;
			controllo[-b->ID-1]=-1;
		}

		RedrawBottone(b,b->Colore);

		if( CanContinue() )
		{
			if(!b2->Testo)
			{
				if(competition!=MENU_WORLD_CUP||wanted_number!=32)
					b2->Testo=msg_0;
				else
				{
					b2->Testo=msg_1;
					b3->Testo=msg_2;
					RedrawBottone(b3,b3->Colore);

				}

				RedrawBottone(b2,b2->Colore);
			}
		}
		else if(b2->Testo)
		{
			b2->Testo=NULL;
			CancellaBottone(b2);

			if(competition==MENU_WORLD_CUP&&wanted_number==32)
			{
				b3->Testo=NULL;
				CancellaBottone(b3);
			}
		}
	}
	else if(bottone==66&&(competition!=MENU_WORLD_CUP||wanted_number!=32))
	{
		if(selected>-1)
		{
			can_modify=FALSE;
			SetTeamSettings( -actual_menu->Bottone[selected].ID-1 );
			can_modify=TRUE;

			ChangeMenu(MENU_TEAM_SETTINGS);
		}
	}
	else if(bottone==64 || (bottone==66 && competition==MENU_WORLD_CUP) )
	{
		int i,j;

		team1_selected=FALSE;
		team2_selected=FALSE;

		for(i=0,j=0;i<64;i++)
			if(actual_menu->Bottone[i].Colore!=COLORE_UNSELECTED)
			{
/*
	Questo e' disabilitato perche' controllo e' fisso sulla squadra!

				if(actual_menu->Bottone[i].Colore==COLORE_SQUADRA_A)
					controllo[j]=0;
				else if(actual_menu->Bottone[i].Colore==COLORE_SQUADRA_B)
					controllo[j]=1;
				else
					controllo[j]=-1;
*/
				teamarray[j]=(-actual_menu->Bottone[i].ID)-1;

				j++;
			}

		if(j!=selected_number)
			D(bug("Attenzione, selezionato un numero errato di squadre! (%ld invece di %ld)\n",j,selected_number));

		actual_menu->Bottone[64].Testo=actual_menu->Bottone[66].Testo=NULL;

		if(friendly||training) {
			goto singlematch;
		}
		else if(competition==MENU_WORLD_CUP&&wanted_number==32) {
			if(bottone==64)
				random_draw=FALSE;
			else
				random_draw=TRUE;

			if(GroupsClear()) {
				GroupsUpdate();
				ChangeMenu(MENU_WORLD_CUP);
			}
		}
		else if(competition==MENU_MATCHES) {
// coppa
			RandomDraw(selected_number);

			numero_squadre=selected_number;

			menu[MENU_MATCHES].Titolo=msg_32;
			ViewEliminazioneDiretta(numero_squadre/2);
			mb[0].ID=MENU_MATCHES;
			ChangeMenu(MENU_MATCHES);
		}
		else if(competition==MENU_LEAGUE) {
// campionato
			if(j>20) {
				request(msg_178);
				j=20;
			}

			RandomDraw(j);
			MakeLeague(j);

			InitTable();
			UpdateLeagueTable();

			ChangeMenu(MENU_LEAGUE);
		}
		else {
singlematch:

			team1_selected=FALSE;
			team2_selected=FALSE;

			if(j==1&&training)  {
				StartMatch(teamarray[0],teamarray[0]);
			}
			else if(j==2) {
				StartMatch(teamarray[0],teamarray[1]);
			}
			else if(network_game) {
				if( (network_player=connect_server(network_server,teamarray[0])) )	{
					BYTE team=teamarray[0]!=0 ? 0 : 1;
					controllo[team]=-1;
					StartMatch(teamarray[0],team);
				}
			}
			else {
				D(bug("Carriera non ancora implementata!"/*-*/));
			}
		}

		// Da finire!
	}
	else if(bottone==65&&b->ID>=0) {
		team1_selected=team2_selected=FALSE;

		selected=-1;

		actual_menu->Bottone[66].Testo=actual_menu->Bottone[64].Testo=NULL;

		special=FALSE;
		competition=MENU_TEAMS;

		ClearSelection();

		ChangeMenu(b->ID);
	}

	return TRUE;
}

BOOL JoyCfg(WORD bottone)
{
	if(bottone>=(actual_menu->NumeroBottoni-1)&&actual_menu->Bottone[bottone].ID>=0)
		ChangeMenu(actual_menu->Bottone[bottone].ID);
	else
	{
		char *temp;
		int k;

		bottone/=2;

		temp=actual_menu->Bottone[bottone*2].Testo;

		actual_menu->Bottone[bottone*2].Testo="WAITING BUTTON...";

		RedrawBottone(&actual_menu->Bottone[bottone*2],actual_menu->Bottone[bottone*2].Highlight);

		ScreenSwap();

		k=os_get_joy_button(actual_joystick);

		if(k>=0)
		{
			actual_menu->Bottone[bottone*2+1].Testo=buttons[k];
			joycfg_buttons[actual_joystick][bottone]=k;
			RedrawBottone(&actual_menu->Bottone[bottone*2+1],actual_menu->Bottone[bottone*2+1].Colore);
		}

		actual_menu->Bottone[bottone*2].Testo=temp;

		RedrawBottone(&actual_menu->Bottone[bottone*2],actual_menu->Bottone[bottone*2].Colore);
		ScreenSwap();
	}

	return TRUE;
}

BOOL KeyCfg(WORD bottone)
{
	ChangeMenu(actual_menu->Bottone[bottone].ID);
	return TRUE;
}

BOOL ArcadeTeamSelection(WORD bottone)
{
	static BYTE selected=-1;
	static BOOL team1_selected=FALSE,team2_selected=FALSE;
	struct Bottone *b;


	if(bottone<0)
		return TRUE;

	b=&actual_menu->Bottone[bottone];

	if(bottone<ARCADE_TEAMS)
	{
		struct Bottone *b2=&actual_menu->Bottone[ARCADE_TEAMS];
		struct Bottone *b3=&actual_menu->Bottone[ARCADE_TEAMS+2];

		if(selected>=0)
			MyRestoreBack();

		if(menu_music&&music_playing)
			StopMenuMusic();

		if(jingle>=0&&(selected!=bottone||b->Colore==COLORE_COMPUTER) )
		{
			D(bug("Interrompo il canale %ld\n",jingle));
			SDL_LockAudio();
// codice che blocca il sample.
			busy[jingle]=NULL;
			SDL_UnlockAudio();
			jingle=-1;
		}

		if(selected!=bottone&&b->Colore!=COLORE_COMPUTER&&!no_sound)
		{
			jingle=PlayBackSound(menusound[FIRST_ARCADE+b->ID]);
			D(bug("Playo il suono %ld sul canale %ld\n",FIRST_ARCADE+b->ID,jingle));
		}

		selected=bottone;

		if(b->Colore!=COLORE_COMPUTER)
		{
			int l=strlen(teamlist[b->ID].nome);


			PrintShadow((WINDOW_WIDTH-l*bigfont->width)/2,FixedScaledY(120),teamlist[b->ID].nome,l,bigfont);

			if(!b3->Testo)
			{
				b3->Testo=msg_31;
				RedrawBottone(b3,b3->Colore);
			}
			else
			{
				b3->Testo=NULL;
				CancellaBottone(b3);
			}
		}

		if(b->Colore==COLORE_UNSELECTED)
		{
			selected_number++;

			controllo[b->ID]=0;

			if(!team1_selected || wanted_number>2 || wanted_number<=0 || selected_number>2)
			{
				team1_selected=TRUE;
				controllo[b->ID]=1;
				b->Colore=COLORE_SQUADRA_A;
			}
			else
			{
				b->Colore=COLORE_SQUADRA_B;
				team2_selected=TRUE;
				controllo[b->ID]=0;
			}			
		}
		else if(b->Colore==COLORE_COMPUTER)
		{
			b->Colore=COLORE_UNSELECTED;
			selected_number--;
		}
		else if(b->Colore==COLORE_SQUADRA_A && (!team2_selected || wanted_number>2 || wanted_number<=0 || selected_number>2) )
		{
			team1_selected=FALSE;
			controllo[b->ID]=0;
			b->Colore=COLORE_SQUADRA_B;
		}
		else
		{
			if(b->Colore==COLORE_SQUADRA_A)
				team1_selected=FALSE;
			else
				team2_selected=FALSE;

			b->Colore=COLORE_COMPUTER;
			controllo[b->ID]=-1;
		}

		b->Highlight=b->Colore;
		RedrawBottone(b,b->Colore);

		if( CanContinue() )
		{
			if(!b2->Testo)
			{
				b2->Testo=msg_0;

				RedrawBottone(b2,b2->Colore);
			}
		}
		else if(b2->Testo)
		{
			b2->Testo=NULL;
			CancellaBottone(b2);
		}
	}
	else if(bottone==ARCADE_TEAMS+2)
	{
		if(selected>-1&&selected<ARCADE_TEAMS)
		{
			if(jingle>=0)
			{
				D(bug("Interrompo il canale %ld\n",jingle));
				SDL_LockAudio();
// codice che blocca il sample.
				busy[jingle]=NULL;
				SDL_UnlockAudio();
				jingle=-1;
			}

			can_modify=FALSE;
			SetTeamSettings( actual_menu->Bottone[selected].ID );
			can_modify=TRUE;

			ChangeMenu(MENU_TEAM_SETTINGS);
		}
	}
	else if(bottone==ARCADE_TEAMS )
	{
		int i,j;

		D(bug("Selezionato continue\n"));

		team1_selected=FALSE;
		team2_selected=FALSE;

		if(jingle>=0)
		{
			D(bug("Interrompo il canale %ld\n",jingle));
			SDL_LockAudio();
// codice che blocca il sample.
			busy[jingle]=NULL;
			SDL_UnlockAudio();
			jingle=-1;
		}

		for(i=0,j=0;i<ARCADE_TEAMS;i++)
			if(actual_menu->Bottone[i].Colore!=COLORE_UNSELECTED)
			{
/*
				if(actual_menu->Bottone[i].Colore==COLORE_SQUADRA_A)
					controllo[j]=0;
				else if(actual_menu->Bottone[i].Colore==COLORE_SQUADRA_B)
					controllo[j]=1;
				else
					controllo[j]=-1;
*/
				teamarray[j]=actual_menu->Bottone[i].ID;
				D(bug("Squadra %ld selezionata\n",teamarray[j]));
				j++;
			}

		if(j!=selected_number)
			D(bug("Attenzione, selezionato un numero errato di squadre! (%ld invece di %ld)\n",j,selected_number));

		if(friendly||training)
		{
			goto friendlymatch;
		}
		else if(competition==MENU_MATCHES)
		{
			actual_menu->Bottone[ARCADE_TEAMS].Testo=actual_menu->Bottone[ARCADE_TEAMS+2].Testo=NULL;

			PlayMenuMusic();

			numero_squadre=selected_number;

			menu[MENU_MATCHES].Titolo=msg_33;
			ViewEliminazioneDiretta(numero_squadre/2);
			mb[0].ID=MENU_MATCHES;
			ChangeMenu(MENU_MATCHES);
		}
		else if(competition==MENU_CHALLENGE)
		{
// Da fare!
			turno=0;
			cb[0].ID=MENU_CHALLENGE;
			SetupMatches();
			ChangeMenu(MENU_CHALLENGE);
		}
		else
		{
friendlymatch:
			D(bug("Inizio amichevole/allenamento...\n"));
			actual_menu->Bottone[ARCADE_TEAMS].Testo=actual_menu->Bottone[ARCADE_TEAMS+2].Testo=NULL;

			PlayMenuMusic();

			team1_selected=FALSE;
			team2_selected=FALSE;

			if(j==1&&training)
			{
				StartMatch(teamarray[0],teamarray[0]);
			}
			else if(j==2)
			{
				D(bug("->Entro in startmatch!\n"));
				StartMatch(teamarray[0],teamarray[1]);
			}
			else
			{
				D(bug("Carriera non ancora implementata!"/*-*/));
			}
		}

	}
	else if(bottone==(ARCADE_TEAMS+1)&&b->ID>=0)
	{
		team1_selected=team2_selected=FALSE;

		selected=-1;

		competition=MENU_TEAMS;

		if(jingle>=0)
		{
			D(bug("Interrompo il canale %ld\n",jingle));
			SDL_LockAudio();
// codice che blocca il sample.
			busy[jingle]=NULL;
			SDL_UnlockAudio();
			jingle=-1;
		}

		actual_menu->Bottone[ARCADE_TEAMS].Testo=actual_menu->Bottone[ARCADE_TEAMS+2].Testo=NULL;

		ClearSelection();

		PlayMenuMusic();

		ChangeMenu(b->ID);
	}

	return TRUE;
}

BOOL TeamSettings(WORD bottone)
{
	struct Bottone *b;
	static int sel1=-1;

	if(bottone<0)
		return TRUE;

	b=&actual_menu->Bottone[bottone];

	if(bottone==42)
	{
		sel1=-1;

		if(!game_start)
			ChangeMenu(b->ID);
		else
			return FALSE;
	}
	else if(bottone==43)
	{
// "Default" Da implementare!		
	}
	else if(controllo[actual_team]>=0&&can_modify)
	{
		if(bottone<34)
		{
			if(bottone==32)
			{
				if(teamlist[actual_team].NumeroGiocatori>15)
				{
					int i;

					if(teamlist[actual_team].NumeroGiocatori>(b->ID-1))
					{
						AddGiocatore(&teamlist[actual_team].giocatore[b->ID-1],16);
						b->ID++;
					}
					else
					{
						b->ID=16;
						AddGiocatore(&teamlist[actual_team].giocatore[14],16);
					}

					if(sel1==16)
						sel1=-1;

					RedrawBottone(&actual_menu->Bottone[33],actual_menu->Bottone[33].Colore);

					for(i=0;i<3;i++)
						RedrawBottone(&pannelli[48+i],pannelli[48+i].Colore);
				}
			}
			else
			{
				struct Bottone *b2;
				int pos,selected=bottone/2;

				b2=&actual_menu->Bottone[selected*2+1];

				if(sel1>=0)
				{

					if(sel1==selected)
					{
						if(!ruolo[actual_team] || ruolo[actual_team]!=selected )
							RedrawBottone(b2,b2->Colore);
						else
							RedrawBottone(b2,P_GIALLO);
					}
					else
					{
						struct Bottone *b3=&actual_menu->Bottone[sel1*2+1];
						int i;

						pos=selected;

						if( (pos==0&&sel1==11) || (sel1==0&&pos==11) )
						{
							struct Portiere_Disk p;

							p=teamlist[actual_team].portiere[1];
							teamlist[actual_team].portiere[1]=teamlist[actual_team].portiere[0];
							teamlist[actual_team].portiere[0]=p;

							AddName((struct Giocatore_Disk *)&teamlist[actual_team].portiere[0],0);
							SetPlayerStatus(0,teamlist[actual_team].portiere[0].Infortuni,0,
								(((teamlist[actual_team].portiere[0].Parata*2+teamlist[actual_team].portiere[0].Attenzione-2*3+2)*10)/7)/3);
							AddName((struct Giocatore_Disk *)&teamlist[actual_team].portiere[1],11);
							
							SetPlayerStatus(11,teamlist[actual_team].portiere[1].Infortuni,0,
								(((teamlist[actual_team].portiere[0].Parata*2+teamlist[actual_team].portiere[1].Attenzione-2*3+2)*10)/7)/3);
							RedrawBottone(b2,b2->Colore);

							for(i=0;i<3;i++)
							{
								RedrawBottone(&pannelli[33+i],pannelli[33+i].Colore);
								RedrawBottone(&pannelli[i],pannelli[i].Colore);
							}
						}
						else if(pos!=0 && pos!=11 &&sel1!=0 &&sel1!=11)
						{
							struct Giocatore_Disk g;

							int pos2;

							if(selected==16)
							{
								pos=actual_menu->Bottone[32].ID-2;
							}
							else
							{
								pos--;

								if(pos>10)
									pos--;
							}

							g=teamlist[actual_team].giocatore[pos];

							if(sel1==16)
								pos2=actual_menu->Bottone[32].ID-2;
							else
							{
								pos2=sel1-1;

								if(pos2>10)
									pos2--;
							}

							teamlist[actual_team].giocatore[pos]=teamlist[actual_team].giocatore[pos2];
							teamlist[actual_team].giocatore[pos2]=g;
							AddGiocatore(&teamlist[actual_team].giocatore[pos],selected);
							AddGiocatore(&teamlist[actual_team].giocatore[pos2],sel1);

							if(!ruolo[actual_team] || ruolo[actual_team]!=selected )
								RedrawBottone(b2,b2->Colore);
							else
								RedrawBottone(b2,P_GIALLO);

							for(i=0;i<3;i++)
							{
								RedrawBottone(&pannelli[selected*3+i],pannelli[selected*3+i].Colore);
								RedrawBottone(&pannelli[sel1*3+i],pannelli[sel1*3+i].Colore);
							}
						}

						if(!ruolo[actual_team] || ruolo[actual_team]!=sel1 )
							RedrawBottone(b3,b3->Colore);
						else
							RedrawBottone(b3,P_GIALLO);

					}					
					sel1=-1;
				}
				else
				{
					sel1=selected;
					RedrawBottone(b2,b2->Highlight);
				}
				ScreenSwap();
			}
		}
		else if(bottone<40||bottone==41)
		{
			if(b->Colore!=COLORE_TATTICA_SELEZIONATA)
changetactic:
			{
				int i;

				strcpy(teamlist[actual_team].Tattiche[0],b->Testo);
				bltchunkybitmap(back,actual_menu->X,actual_menu->Y,main_bitmap,
					actual_menu->X,actual_menu->Y,108,156,bitmap_width,bitmap_width);
				BltAnimObj(logos,main_bitmap,actual_menu->Immagine,actual_menu->X,actual_menu->Y,bitmap_width);
				DisplayTactic(0,0);

				for(i=0;i<9;i++)
				{
					if(teamsettings[34+i].Colore==COLORE_TATTICA_SELEZIONATA)
					{
						teamsettings[34+i].Colore=COLORE_TATTICA_NON_SELEZIONATA;
						RedrawBottone(&teamsettings[34+i],COLORE_TATTICA_NON_SELEZIONATA);
					}
				}

				b->Colore=COLORE_TATTICA_SELEZIONATA;

				RedrawBottone(b,COLORE_TATTICA_SELEZIONATA);
				ScreenSwap();
			}
		}
		else if(bottone==40)
		{
			freq.Title=msg_34;
			freq.Dir="tct/"/*-*/;

			if(AslRequest(&freq))
			{
				char *c;

				b=&actual_menu->Bottone[41];

				if(b->Testo)
					free(b->Testo);

				c=freq.File+strlen(freq.File)-1;

				while(*c!='/' && *c!='\\' && c>freq.File)
					c--;

				b->Testo=strdup(c);

				goto changetactic;
			}
		}
	}

	return TRUE;
}

BOOL GamePrefs(WORD bottone)
{
	if(bottone==(actual_menu->NumeroBottoni-1)&&actual_menu->Bottone[bottone].ID>=0)
	{
		ChangeMenu(actual_menu->Bottone[bottone].ID);
		return TRUE;
	}

	bottone=((bottone>>1)<<1)+1;

	switch(bottone)
	{
		case 1:
			duration++;

			if(!time_options[duration])
				duration=0;

			actual_menu->Bottone[bottone].Testo=time_options[duration];

			t_l=atol(time_options[duration]);
			break;
		case 3:
			field++;

			if(!field_options[field])
				field=0;

			actual_menu->Bottone[bottone].Testo=field_options[field];

			if(field>=7)
			{
				actual_menu->Bottone[20].Testo=actual_menu->Bottone[21].Testo=NULL;
				CancellaBottone(&actual_menu->Bottone[21]);
				CancellaBottone(&actual_menu->Bottone[20]);
			}
			else if(!actual_menu->Bottone[20].Testo)
			{
				field_type=0;
				actual_menu->Bottone[20].Testo=msg_35;
				actual_menu->Bottone[21].Testo=field_types[0];
				RedrawBottone(&actual_menu->Bottone[20],actual_menu->Bottone[20].Colore);
				RedrawBottone(&actual_menu->Bottone[21],actual_menu->Bottone[21].Colore);
			}
			break;
		case 5:
			strictness++;

			if(strictness>10)
				strictness=0;

			if(strictness==10)
				actual_menu->Bottone[bottone].Testo=msg_2;	
			else
				actual_menu->Bottone[bottone].Testo=numero[strictness];

			break;
		case 7:
		case 9:
		case 11:
		case 13:
		case 25:
			if(actual_menu->Bottone[bottone].Testo==enabled)
				actual_menu->Bottone[bottone].Testo=disabled;
			else
				actual_menu->Bottone[bottone].Testo=enabled;

			switch(bottone)
			{
				case 7:
					injuries = injuries ? FALSE : TRUE;
					break;
				case 9:
					substitutions = substitutions ? FALSE : TRUE;
					break;
				case 11:
					bookings = bookings ? FALSE : TRUE;
					break;
				case 13:
					use_replay= use_replay ? FALSE : TRUE;
					break;
				case 25:
					offside= offside ? FALSE : TRUE;
					break;
			}

			break;
		case 15:
			if(free_longpass)
			{
				free_longpass=FALSE;
				actual_menu->Bottone[bottone].Testo=msg_36;
			}
			else
			{
				free_longpass=TRUE;
				actual_menu->Bottone[bottone].Testo=msg_37;
			}
			break;
		case 21:
			field_type++;

			if(!field_types[field_type])
				field_type=0;

			actual_menu->Bottone[bottone].Testo=field_types[field_type];
			break;
		case 17:
			daytime++;

			if(!daytimes[daytime])
				daytime=0;

			actual_menu->Bottone[bottone].Testo=daytimes[daytime];
			break;
		case 19:
			if(golden_gol)
			{
				golden_gol=FALSE;
				actual_menu->Bottone[bottone].Testo=disabled;
			}
			else
			{
				golden_gol=TRUE;
				actual_menu->Bottone[bottone].Testo=enabled;
			}
			break;
		case 23:
			if(newchange)
			{
				newchange=FALSE;
				actual_menu->Bottone[bottone].Testo="CLASSIC";
			}
			else
			{
				newchange=TRUE;
				actual_menu->Bottone[bottone].Testo="ETW";
			}
			break;
		default:
			D(bug("Errore, opzione (%ld) non prevista!\n"/*-*/,bottone));
			return FALSE;
	}

	RedrawBottone(&actual_menu->Bottone[bottone],actual_menu->Bottone[bottone].Colore);

	return TRUE;
}

BOOL AudioPrefs(WORD bottone)
{
	if(bottone==(actual_menu->NumeroBottoni-1)&&actual_menu->Bottone[bottone].ID>=0)
	{
		ChangeMenu(actual_menu->Bottone[bottone].ID);
		return TRUE;
	}

	bottone=((bottone>>1)<<1)+1;

	switch(bottone)
    {
        case 1:
            /* AC: Quando il programma parte con no_sound = true, ma le opzioni
             * vengono modificate a no_sound = false, i suoni dei men non sono
             * caricati e la PlayBackSound viene chiamata con un elemento NULL
             * ( successo nei men arcade). 
             * Secondo me bisogna chiamare la CaricaSuoniMenu.
             */
            no_sound = no_sound ? FALSE : TRUE;

            /* Se il suono  off, libero i suoni, altrimenti li carico */
            if(no_sound)
            {
                LiberaSuoniMenu();
                FreeSoundSystem();
            }
            else
            {
                /* AC: sicuramente ci vogliono dei controlli */
                InitSoundSystem();
                CaricaSuoniMenu();
            }    

            /* AC: Credo serva anche questa. */
            os_start_audio();

            break;
        case 3:
            if(use_crowd)
                use_crowd=FALSE;
            else
            {
                use_speaker=FALSE;
                use_crowd=TRUE;
            }
#ifdef CD_VERSION
            DeleteAudio2Fast();
#endif
            break;
        case 5:
#ifdef CD_VERSION
            if(use_speaker)
                use_speaker=FALSE;
            else
            {
                use_speaker=TRUE;
                use_crowd=FALSE;
            }
            DeleteAudio2Fast();
#else
            request(msg_38);
#endif

            break;
        case 7:
#ifdef CD_VERSION
            menu_music= menu_music ? FALSE : TRUE;

            if(menu_music==FALSE)
                StopMenuMusic();
            else
                PlayMenuMusic();
#else
            request(msg_39);
#endif
            break;
        case 9:
#ifdef CD_VERSION

            audio_to_fast = audio_to_fast ? FALSE : TRUE;

#else
            request(msg_39);
#endif
            break;
    }

	UpdatePrefs(MENU_AUDIO_PREFS);

	RedrawBottone(&actual_menu->Bottone[bottone],actual_menu->Bottone[bottone].Colore);

	switch(bottone)
	{
		case 5:
			RedrawBottone(&actual_menu->Bottone[3],actual_menu->Bottone[3].Colore);
			break;
		case 3:
			RedrawBottone(&actual_menu->Bottone[5],actual_menu->Bottone[5].Colore);
			break;
	}

	return TRUE;
}

BOOL SystemPrefs(WORD bottone)
{
	if(bottone>=(actual_menu->NumeroBottoni-3)&&actual_menu->Bottone[bottone].ID>=0)
	{
		if(bottone==(actual_menu->NumeroBottoni-3))
		{
			if(!os_check_joy(0))
			{
				request("NO JOYSTICK FOUND");
				return TRUE;
			}
			else UpdateJoyCfg(actual_joystick);
		}

		ChangeMenu(actual_menu->Bottone[bottone].ID);
		return TRUE;
	}

	bottone=((bottone>>1)<<1)+1;

	switch(bottone)
	{
		case 1:
			break;
		case 3:
			chunky_version= chunky_version ? FALSE : TRUE;
			break;
		case 5:
		case 7:
			control[(bottone-5)/2]++;


			if(!controls[control[(bottone-5)/2]])
				control[(bottone-5)/2]=0;

			if(control[(bottone-5)/2]<CTRL_KEY_1)
			{
				if(!os_check_joy(0))
					control[(bottone-5)/2]=CTRL_KEY_1;
			}

			actual_menu->Bottone[bottone].Testo=controls[control[(bottone-5)/2]];
			break;

		case 9:
			break;
	}

	UpdatePrefs(MENU_SYSTEM_PREFS);

	RedrawBottone(&actual_menu->Bottone[bottone],actual_menu->Bottone[1].Colore);

	return TRUE;
}

BOOL VideoPrefs(WORD bottone)
{
	if(bottone==(actual_menu->NumeroBottoni-1)&&actual_menu->Bottone[bottone].ID>=0)
	{
		ChangeMenu(actual_menu->Bottone[bottone].ID);
		return TRUE;
	}

	bottone=((bottone>>1)<<1)+1;

	switch(bottone)
	{
		case 1:
			id_change=TRUE;

			if(wb_game)
			{
				wb_game=FALSE;
				/* AC: se sono su schermo, disattivo lo scaling */
				use_gfx_scaling = FALSE;

				MenuResizing(atol(resolutions[current_resolution]),atol(resolutions[current_resolution]+4));

				/* AC: Da come vengono ripristinati in seguito, credo non debbano essere
				 * eliminati questi bottoni che sono Scaling e Buffering.
				 */
				//CancellaBottone(&actual_menu->Bottone[16]);
				//CancellaBottone(&actual_menu->Bottone[17]);
				CancellaBottone(&actual_menu->Bottone[18]);
				CancellaBottone(&actual_menu->Bottone[19]);
				
				/* AC: Lo stesso dicasi per questi */
				//CancellaBottone(&actual_menu->Bottone[20]);
				//CancellaBottone(&actual_menu->Bottone[21]);
			}
			else
			{
				wb_game=TRUE;	

				MenuResizing(atol(resolutions[current_resolution]),atol(resolutions[current_resolution]+4));

				RedrawBottone(&actual_menu->Bottone[16],actual_menu->Bottone[0].Colore);
				RedrawBottone(&actual_menu->Bottone[17],actual_menu->Bottone[1].Colore);
				RedrawBottone(&actual_menu->Bottone[20],actual_menu->Bottone[0].Colore);
				RedrawBottone(&actual_menu->Bottone[21],actual_menu->Bottone[1].Colore);

				if(use_gfx_scaling)
				{
					RedrawBottone(&actual_menu->Bottone[18],actual_menu->Bottone[0].Colore);
					RedrawBottone(&actual_menu->Bottone[19],actual_menu->Bottone[1].Colore);
				}
			}
#if 0
			{
			    struct ScreenModeRequester *Req;

			    if(Req=AllocAslRequest(ASL_ScreenModeRequest,NULL))
			    {
				if(!AslRequestTags(Req,ASLSM_InitialDisplayWidth,WINDOW_WIDTH,
					ASLSM_InitialDisplayHeight,WINDOW_HEIGHT,
					ASLSM_InitialDisplayDepth,4,
					ASLSM_InitialOverscanType,OSCAN_TEXT,
					ASLSM_MinWidth,300,
					ASLSM_MinHeight,180,
					ASLSM_MaxWidth,1280,
					ASLSM_MaxHeight,1024,
					ASLSM_InitialDisplayID,LORES_KEY,
					ASLSM_Screen,screen,
					ASLSM_DoHeight,TRUE,
					ASLSM_DoWidth,TRUE,
					ASLSM_DoOverscanType,TRUE,
					TAG_DONE))
				{
					if(wb_game)
					{
						actual_menu->Bottone[bottone].Testo=msg_40;
						wb_game=FALSE;
						display_id=INVALID_ID;
					}
					else
					{
						actual_menu->Bottone[bottone].Testo="WORKBENCH"/*-*/;
						wb_game=TRUE;
					}
				}
				else
				{
					char *c;

					display_id=Req->sm_DisplayID;
					wb_game=FALSE;
					overscan=Req->sm_OverscanType;
					wanted_width=Req->sm_DisplayWidth;
					wanted_height=Req->sm_DisplayHeight;

					GetDisplayInfoData(FindDisplayInfo(display_id),(UBYTE *)&ni,sizeof(struct NameInfo),DTAG_NAME,NULL);

					c=ni.Name;

					while(*c)
					{
						*c=toupper(*c);
						c++;
					}

					actual_menu->Bottone[bottone].Testo=ni.Name;
				}

				FreeAslRequest(Req);
			    }
			}
#endif
		break;
		case 3:
			if(detail_level&USA_RADAR)
			{
				radar_position++;

				if(radar_position>=12)
				{
					detail_level&= ~USA_RADAR;
					radar_position=-1;
					actual_menu->Bottone[bottone].Testo=disabled;
				}
				else actual_menu->Bottone[bottone].Testo=radar_options[radar_position];
			}
			else
			{
				radar_position=0;
				detail_level|=USA_RADAR;

				actual_menu->Bottone[bottone].Testo=radar_options[radar_position];
			}
			break;
		case 7:
			if(detail_level&USA_ARBITRO)
				detail_level&= ~USA_ARBITRO;
			else
				detail_level|=USA_ARBITRO;
			break;
		case 5:
			if(detail_level&USA_RISULTATO)
				detail_level&= ~USA_RISULTATO;
			else
				detail_level|=USA_RISULTATO;
			break;
		case 9:
			if(detail_level&(USA_POLIZIOTTI|USA_FOTOGRAFI))
				detail_level&= ~(USA_POLIZIOTTI|USA_FOTOGRAFI);
			else
				detail_level|=(USA_POLIZIOTTI|USA_FOTOGRAFI);
			break;
		case 11:
			if(detail_level&USA_NOMI)
				detail_level&= ~USA_NOMI;
			else
				detail_level|=USA_NOMI;
			break;
		case 13:
			nointro= (nointro) ? FALSE : TRUE;
			break;					
		case 15:
			if(detail_level&USA_GUARDALINEE)
				detail_level&= ~USA_GUARDALINEE;
			else
				detail_level|=USA_GUARDALINEE;
			break;
		case 17:
			if(!use_gfx_scaling)
			{
				if(wb_game)
				{
					actual_menu->Bottone[18].Testo="SCALING RES";
					actual_menu->Bottone[19].Testo=scaling_resolutions[current_scaling];
					RedrawBottone(&actual_menu->Bottone[19],actual_menu->Bottone[1].Colore);
					RedrawBottone(&actual_menu->Bottone[18],actual_menu->Bottone[0].Colore);
					use_gfx_scaling=TRUE;
				}
				else
					request("You can enable scaling only\nif you play in a window!");
			}
			else
			{
				use_gfx_scaling=FALSE;
				CancellaBottone(&actual_menu->Bottone[18]);
				CancellaBottone(&actual_menu->Bottone[19]);
			}
			break;
		case 19:
            {
                char buffer[20];
                
    			current_scaling++;
			
	    		if(!scaling_resolutions[current_scaling])
		    		current_scaling=0;
                
				// AC: Un piccolo refuso :-)
    			//scaling_resolutions[current_scaling][3]=0;

                strncpy(buffer, scaling_resolutions[current_scaling], 3);

                buffer[3] = 0;
                
                FIXED_SCALING_WIDTH=atol(buffer);
		    	FIXED_SCALING_HEIGHT=atol(scaling_resolutions[current_scaling]+4);
            }
			break;
		case 21:
			if(force_single)
			{
// Uso il DBuffering
				force_single=FALSE;
				triple=FALSE;
			}
			else if(triple)
			{
// Uso il Single buffering
				triple=FALSE;
				force_single=TRUE;
			}
			else
			{
// Uso il triple buffering
				force_single=FALSE;
				triple=TRUE;
			}
			break;
		case 23:
			if(newpitches)
			{
				newpitches=FALSE;

				if(!wb_game)
					id_change=TRUE;
			}
			else if(CheckNewPitches())
			{
				if(!wb_game)
					id_change=TRUE;

				newpitches=TRUE;
			}
			break;
		case 25:
			do
			{
				current_resolution++;

				if(!resolutions[current_resolution])
					current_resolution=0;

				wanted_width=atol(resolutions[current_resolution]);
				wanted_height=atol(resolutions[current_resolution]+4);	
			}
			while (!os_videook(wanted_width,wanted_height));

			MenuResizing(wanted_width,wanted_height);

			break;
	}

	UpdatePrefs(MENU_VIDEO_PREFS);

	RedrawBottone(&actual_menu->Bottone[bottone],actual_menu->Bottone[1].Colore);

	return TRUE;
}

void UpdatePrefs(BYTE set)
{
  struct GfxMenu *m=&menu[set];

  switch(set)
  {
    case MENU_GAME_PREFS:
	m->Bottone[1].Testo=time_options[duration];
	m->Bottone[3].Testo=field_options[field];

	if(field<7)
	{
		m->Bottone[20].Testo=msg_35;
		m->Bottone[21].Testo=field_types[field_type];
	}
	else
		m->Bottone[20].Testo=m->Bottone[21].Testo=NULL;

	if(strictness==10)
		m->Bottone[5].Testo=msg_2;
	else
		m->Bottone[5].Testo=numero[strictness];

	if(injuries)
		m->Bottone[7].Testo=enabled;
	else
		m->Bottone[7].Testo=disabled;

	if(substitutions)
		m->Bottone[9].Testo=enabled;
	else
		m->Bottone[9].Testo=disabled;

	if(bookings)
		m->Bottone[11].Testo=enabled;
	else
		m->Bottone[11].Testo=disabled;

	if(use_replay)
		m->Bottone[13].Testo=enabled;
	else
		m->Bottone[13].Testo=disabled;

	if(!free_longpass)
		m->Bottone[15].Testo=msg_36;
	else
		m->Bottone[15].Testo=msg_37;

	m->Bottone[17].Testo=daytimes[daytime];

	if(golden_gol)
		m->Bottone[19].Testo=enabled;
	else
		m->Bottone[19].Testo=disabled;

	if(newchange)
		m->Bottone[23].Testo="ETW";
	else
		m->Bottone[23].Testo="CLASSIC";

	if(offside)
		m->Bottone[25].Testo=enabled;
	else
		m->Bottone[25].Testo=disabled;

	break;

    case MENU_AUDIO_PREFS:
	if(!no_sound)
		m->Bottone[1].Testo=enabled;
	else
		m->Bottone[1].Testo=disabled;

	if(use_crowd)
		m->Bottone[3].Testo=enabled;
	else
		m->Bottone[3].Testo=disabled;

	if(use_speaker)
		m->Bottone[5].Testo=enabled;
	else
		m->Bottone[5].Testo=disabled;

	if(menu_music)
		m->Bottone[7].Testo=enabled;
	else
		m->Bottone[7].Testo=disabled;

	if(audio_to_fast)
		m->Bottone[9].Testo=enabled;
	else
		m->Bottone[9].Testo=disabled;

	if(!use_ahi)
		m->Bottone[11].Testo=disabled;
	else
		m->Bottone[11].Testo=enabled;
	break;
    case MENU_VIDEO_PREFS:
	if(wb_game)
		m->Bottone[1].Testo="WINDOW"/*-*/;
	else
		m->Bottone[1].Testo="FULLSCREEN"/*-*/;
	
	if(detail_level&USA_RADAR)
		m->Bottone[3].Testo=radar_options[radar_position]; 
	else
		m->Bottone[3].Testo=disabled;

	if(detail_level&USA_RISULTATO)
		m->Bottone[5].Testo=enabled;
	else
		m->Bottone[5].Testo=disabled;

	if(detail_level&USA_ARBITRO)
		m->Bottone[7].Testo=enabled;
	else
		m->Bottone[7].Testo=disabled;

	if(detail_level&USA_POLIZIOTTI)
		m->Bottone[9].Testo=enabled;
	else
		m->Bottone[9].Testo=disabled;

	if(detail_level&USA_NOMI)
		m->Bottone[11].Testo=enabled;
	else
		m->Bottone[11].Testo=disabled;
	if(nointro)
		m->Bottone[13].Testo=disabled;
	else
		m->Bottone[13].Testo=enabled;

	if(detail_level&USA_GUARDALINEE)
		m->Bottone[15].Testo=enabled;
	else
		m->Bottone[15].Testo=disabled;
	if(use_gfx_scaling)
		m->Bottone[17].Testo=enabled;
	else
		m->Bottone[17].Testo=disabled;

	if(triple)
		m->Bottone[21].Testo="TRIPLE";
	else if(force_single)
		m->Bottone[21].Testo="SINGLE";
	else
		m->Bottone[21].Testo="DOUBLE";

	if(newpitches)
		m->Bottone[23].Testo=enabled;
	else
		m->Bottone[23].Testo=disabled;

	m->Bottone[25].Testo=resolutions[current_resolution];

	if(use_gfx_scaling)
	{
		m->Bottone[18].Testo="SCALING RES";
		m->Bottone[19].Testo=scaling_resolutions[current_scaling];
	}
	else
	{
		m->Bottone[18].Testo=NULL;
		m->Bottone[19].Testo=NULL;
	}
	break;

    case MENU_SYSTEM_PREFS:

	if(chunky_version)
		m->Bottone[3].Testo=enabled;
	else
		m->Bottone[3].Testo=disabled;
		
	m->Bottone[5].Testo=controls[control[0]];
	m->Bottone[7].Testo=controls[control[1]];

	if(use_ahi)
		m->Bottone[9].Testo=enabled;
	else
		m->Bottone[9].Testo=disabled;
	break;
  }
}

void SetupMatches(void)
{
	make_setup=FALSE;

	switch(competition)
	{
		case MENU_CHALLENGE:

// Azzero il controllo per tutti tranne la squadra in uso...

			if(turno==0)
			{
				int i;

				cp[6].Testo=NULL;

				for(i=0;i<ARCADE_TEAMS+1;i++)
					if(i!=*teamarray)
						controllo[i]=-1;
			}

			if(*teamarray==arcade_sequence[turno])
				turno++;

			cb[0].ID=MENU_CHALLENGE;

			if(turno<10)
			{
				struct Squadra_Disk *s=&teamlist[arcade_sequence[turno]];
				int i;

				menu[MENU_CHALLENGE].Titolo=msg_41;

				for(i=0;i<s->NumeroPortieri;i++)
				{
					s->portiere[i].Parata=min(9,s->portiere[i].Parata+(turno+1)/2);
					s->portiere[i].Velocita=min(9,s->portiere[i].Velocita+(turno+1)/2);
					s->portiere[i].Attenzione=min(9,s->portiere[i].Attenzione+(turno+1)/2);
				}

				for(i=0;i<s->NumeroGiocatori;i++)
				{
					s->giocatore[i].Velocita=min(9,s->giocatore[i].Velocita+(turno+1)/2);
					s->giocatore[i].Tiro=min(9,s->giocatore[i].Tiro+(turno+1)/2);
					s->giocatore[i].Contrasto=min(9,s->giocatore[i].Contrasto+(turno+1)/2);
					s->giocatore[i].Prontezza=min(9,s->giocatore[i].Prontezza+(turno+1)/2);
					s->giocatore[i].Tecnica=min(9,s->giocatore[i].Tecnica+(turno+1)/2);
				}
			}
			else if(turno==10)
				menu[MENU_CHALLENGE].Titolo=msg_42;
			else
			{
// Aggiungere qui la schermata finale dell'arcade.

#ifdef CD_VERSION
				Outro();
#else
				ShowCredits();
#endif
				arcade_score+=500; // Bonus finale

				AddScore(*teamarray);
				LoadTeams("teams/arcade"/*-*/);
				turno=0;
				competition=MENU_TEAMS;
				cb[0].ID=MENU_ARCADE;
			}
// Squadra A
// AC: Ancora quella stranezza delle stringhe globabili inmodificabili... ma c'è
// qualche opzione di compilazione?
// -fwritable-strings
			cp[0].Testo[1]=*teamarray;
			cp[4].Colore=cp[0].Colore=cp[0].Highlight=cp[2].Colore=colore_team[controllo[*teamarray]+1];
			cp[2].Testo=teamlist[*teamarray].nome;
			cp[4].Highlight=cp[2].Highlight=highlight_team[controllo[*teamarray]+1];

			jingle=PlayBackSound(menusound[FIRST_ARCADE+arcade_sequence[turno]]);
			D(bug("Playo il suono %ld sul canale %ld...\n",FIRST_ARCADE+arcade_sequence[turno],jingle));

			if(cp[4].Testo)
			{
				free(cp[4].Testo);
				cp[4].Testo=NULL;
				cp[5].Testo=NULL;
			}
// Squadra B
// AC: Ancora quella stranezza delle stringhe globabili inmodificabili... ma c'è
// qualche opzione di compilazione?
			cp[1].Testo[1]=arcade_sequence[turno];
			cp[3].Testo=teamlist[arcade_sequence[turno]].nome;
			cp[5].Colore=cp[1].Colore=cp[1].Highlight=cp[3].Colore=colore_team[0];
			cp[5].Highlight=cp[3].Highlight=highlight_team[0];

			break;
		case MENU_MATCHES:
			mb[0].ID=MENU_MATCHES;

			if(arcade)
				menu[MENU_MATCHES].Titolo=msg_33;
			else
				menu[MENU_MATCHES].Titolo=msg_43;

			ViewEliminazioneDiretta(numero_squadre/2);
			break;
		case MENU_LEAGUE:
			menu[MENU_MATCHES].Titolo=msg_44;

			if(turno<totale_giornate)
			{
				BYTE a,b;
				int k;

				for(k=0;k<numero_squadre/2;k++)
				{
					a=teamarray[turni[turno][k].t1-1];
					b=teamarray[turni[turno][k].t2-1];

					ClearMatches((numero_squadre+1)/2);

					if(a==FAKE_TEAM)
					{
						a=b;
						b=FAKE_TEAM;
					}

					if(b==FAKE_TEAM)
					{
						mp[k*4].Testo=teamlist[a].nome;
						mp[k*4].Colore=colore_team[controllo[a]+1];
						mp[k*4].Highlight=highlight_team[controllo[a]+1];
						mp[k*4+1].Testo=NULL;
						mp[k*4+2].Testo="NO GAME"/*-*/;
						mp[k*4+2].Colore=colore_team[0];
						mp[k*4+2].Highlight=highlight_team[0];

					}
					else
					{
						mp[k*4].Colore=colore_team[controllo[a]+1];
						mp[k*4].Highlight=highlight_team[controllo[a]+1];
						mp[k*4+2].Colore=colore_team[controllo[b]+1];
						mp[k*4+2].Highlight=highlight_team[controllo[b]+1];

						mp[k*4].Testo=teamlist[a].nome;
						mp[k*4+1].Testo="-"/*-*/;
						mp[k*4+2].Testo=teamlist[b].nome;
					}

					if(mp[k*4+3].Testo)
					{
						free(mp[k*4+3].Testo);
						mp[k*4+3].Testo=NULL;
					}
				}

				if(turno<totale_giornate)
					mb[0].ID=MENU_MATCHES;
				else if(scontri<2)
				{
					mb[0].ID=MENU_LEAGUE;
					turno=0;
					competition=MENU_TEAMS;
				}
				else 
				{
					mb[0].ID=MENU_MATCHES;
					InvertiSquadre();
					turno=0;
					scontri--;
				}
			}
			else
			{
					mb[0].ID=MENU_SIMULATION;
					competition=MENU_TEAMS;
					turno=0;
			}
			break;
		case MENU_WORLD_CUP:
			if(turno<3)
			{
				int i,j,k=0;
				extern struct Match camp4[3][2];
				extern BYTE start_groups[8][4];

				
				menu[MENU_MATCHES].Titolo= ( turno==0 ? msg_45 : (turno==1 ? msg_46 : msg_47) );

				for(i=0;i<8;i++)
				{
					for(j=0;j<2;j++)
					{
						mp[k*4].Testo=teamlist[start_groups[i][camp4[turno][j].t1-1]].nome;
						mp[k*4+1].Testo="-"/*-*/;
						mp[k*4+2].Testo=teamlist[start_groups[i][camp4[turno][j].t2-1]].nome;
						mp[k*4].Colore=colore_team[controllo[start_groups[i][camp4[turno][j].t1-1]]+1];
						mp[k*4].Highlight=highlight_team[controllo[start_groups[i][camp4[turno][j].t1-1]]+1];
						mp[k*4+2].Colore=colore_team[controllo[start_groups[i][camp4[turno][j].t2-1]]+1];
						mp[k*4+2].Highlight=highlight_team[controllo[start_groups[i][camp4[turno][j].t2-1]]+1];

						if(mp[k*4+3].Testo)
						{
							free(mp[k*4+3].Testo);
							mp[k*4+3].Testo=NULL;
						}

						k++;
					}
				}
				mb[0].ID=MENU_MATCHES;
			}
			else if(turno<7)
			{
				ViewEliminazioneDiretta(mondiali[turno-3]);


				if(turno!=6)
					menu[MENU_MATCHES].Titolo=msg_48;
				else
				{	
					int i;

					menu[MENU_MATCHES].Titolo=msg_49;

					for(i=0;i<4;i++)
					{
						mp[5*4+i].Testo=mp[1*4+i].Testo;
						mp[2*4+i].Testo=mp[i].Testo;
						mp[2*4+i].Colore=mp[i].Colore;
						mp[2*4+i].Highlight=mp[i].Highlight;
						mp[5*4+i].Colore=mp[4+i].Colore;
						mp[5*4+i].Highlight=mp[4+i].Highlight;
						mp[i].Testo=NULL;
						mp[4+i].Testo=NULL;
						mp[3*4+i].Testo=NULL;
						mp[4*4+i].Testo=NULL;
					}

					mp[4].Testo=msg_50;
					mp[4].Colore=P_GIALLO;
					mp[4].Highlight=P_BIANCO;

					mp[4*4].Testo=msg_51;
					mp[4*4].Colore=P_GIALLO;
					mp[4*4].Highlight=P_BIANCO;
				}

				mb[0].ID=MENU_MATCHES;
			}

			break;
	}
}

void PlayMatches(void)
{
	int i;

	switch(competition)
	{
		case MENU_CHALLENGE:
			{
				WORD result;
				char *c;

				if(jingle>=0)
				{
					D(bug("Interrompo il canale %ld\n",jingle));
					SDL_LockAudio();
// codice che blocca il sample.
					busy[jingle]=NULL;
					SDL_UnlockAudio();
					jingle=-1;
				}

				if(turno==10)
					final=TRUE;

				result=PlayMatch(*teamarray,arcade_sequence[turno]);
	
				
				cp[2].Testo=teamlist[*teamarray].nome;
				cp[3].Testo=teamlist[arcade_sequence[turno]].nome;

				c=cp[4].Testo=strdup(ElaboraRisultato(*teamarray,arcade_sequence[turno],result));

				while(*c!='-')
					c++;

				*c=0;

				c++;

				cp[5].Testo=c;

				make_setup=TRUE;

				arcade_score-=( (result>>8) *10);
				arcade_score+=((result&0xff)*15);

				if((result&0xff)>(result>>8))
				{
					cb[0].ID=MENU_CHALLENGE;
					arcade_score+=turno*20;
					turno++;
				}
				else
				{
					if(turno>0)
						AddScore(*teamarray);
					turno=0;
					competition=MENU_TEAMS;					
					LoadTeams("teams/arcade"/*-*/); // Ricarico le squadre originali...
					cp[2].Testo=teamlist[*teamarray].nome;
					cp[3].Testo=teamlist[arcade_sequence[turno]].nome;
					cp[6].Testo=msg_52;
					cb[0].ID=MENU_ARCADE;
				}
			}
			break;
		case MENU_MATCHES:
			if(arcade)
				menu[MENU_MATCHES].Titolo=msg_53;
			else
				menu[MENU_MATCHES].Titolo=msg_54;

			if(numero_squadre==2)
				final=TRUE;

			EliminazioneDiretta(numero_squadre/2);

			numero_squadre/=2;

			if(numero_squadre>1)
			{
				NewTurn();
				make_setup=TRUE;

			}
			else
			{
				if(!arcade)
					mb[0].ID=MENU_SIMULATION;
				else
					mb[0].ID=MENU_ARCADE;

				turno=0;
				competition=MENU_TEAMS;
			}
			break;
		case MENU_LEAGUE:
			menu[MENU_MATCHES].Titolo=msg_55;

			if(turno<totale_giornate)
			{
				WORD risultato;
				BYTE a,b;
				int k;

				nopari=FALSE;

				for(k=0;k<numero_squadre/2;k++)
				{
					a=teamarray[turni[turno][k].t1-1];
					b=teamarray[turni[turno][k].t2-1];

					if(a!=FAKE_TEAM&&b!=FAKE_TEAM)
					{
						risultato=PlayMatch(a,b);
						mp[k*4+3].Testo=strdup(ElaboraRisultato(a,b,risultato));
					}
				}

				NewTurn();
				UpdateLeagueTable();
				mb[0].ID=MENU_LEAGUE;

				if(turno==totale_giornate)
				{
					turno=0;

					if(scontri<2)
					{
						lb[0].ID=MENU_LEAGUE;
						competition=MENU_TEAMS;
					}
					else
					{
						scontri--;
						InvertiSquadre();
					}
				}
			}
			break;
		case MENU_WORLD_CUP:
			menu[MENU_MATCHES].Titolo=msg_56;

			if(turno<3)
			{
				int j,k=0;
				BYTE a,b;
				WORD risultato;
				extern struct Match camp4[3][2];
				extern BYTE start_groups[8][4],groups[8][4];

				for(i=0;i<8;i++)
				{
					for(j=0;j<2;j++)
					{
						a=start_groups[i][camp4[turno][j].t1-1];
						b=start_groups[i][camp4[turno][j].t2-1];

						risultato=PlayMatch(a,b);

						mp[k*4+3].Testo=strdup(ElaboraRisultato(a,b,risultato));

						k++;
					}
				}

				NewTurn();
				GroupsUpdate();
				mb[0].ID=MENU_WORLD_CUP;

// Creo l'array da usare per l'eliminazione diretta

				if(turno==3)
				{
					for(i=0;i<8;i++)
					{
						teamarray[i*2]=groups[i][0];
						teamarray[i*2+1]=groups[7-i][1];
					}
				}
			}
			else if(turno<7)
			{
				if(turno==7)
					final=TRUE;

				EliminazioneDiretta(mondiali[turno-3]);
				make_setup=TRUE;
				NewTurn();

				if(turno==7)
				{
					BYTE temp=teamarray[2];

					teamarray[2]=teamarray[1];
					teamarray[1]=temp;

					mb[0].ID=MENU_WORLD_CUP_END;

					mp[5*4+3].Testo=mp[4+3].Testo;
					mp[2*4+3].Testo=mp[3].Testo;
					mp[3].Testo=mp[4+3].Testo=NULL;

					for(i=0;i<4;i++)
					{
						wcfp[i].Testo=teamlist[teamarray[i]].nome;
						wcfp[i].Colore=colore_team[controllo[teamarray[i]]+1];
						wcfp[i].Highlight=highlight_team[controllo[teamarray[i]]+1];
					}

					turno=0;
					competition=MENU_TEAMS;
				}
			}
			else
			{
				D(bug("Qui non dovrei arrivarci! (ottavo turno mondiale)\n"/*-*/));
			}
			break;
	}
}

void FreeHighSelection(void)
{
	register int i;

	for(i=0;i<64;i++)
		if(hl[i].Testo)
			free(hl[i].Testo);
}

BOOL HighSelection(WORD bottone)
{
	struct Bottone *b;

	if(bottone<0)
		return TRUE;

	b=&actual_menu->Bottone[bottone];

	if (bottone == 64)	{
		FreeHighSelection();
		ChangeMenu(MENU_HIGHLIGHT);
	}
	else {
		char buffer[32];

		strcpy(buffer,TEMP_DIR "replay."/*-*/);
		strcat(buffer,b->Testo);

		if (!savehigh)	{
			D(bug("Carico %s...\n",b->Testo));

			LoadHigh(buffer);
		}
		else {
			FILE *fh;

			if ((fh=fopen(buffer,"rb")))	{
				
				LONG l;

				freq.Title="Save highlight...";
				freq.Save=TRUE;

				if (AslRequest(&freq)) {
					char buffer[130];
					APTR a;

					fseek(fh,0,SEEK_END);
					l=fseek(fh,0,SEEK_SET);

					strcpy(buffer,freq.File);

					if ((a=malloc(l))) {
						FILE *f2;

						fread(a,1,l,fh);

						if ((f2=fopen(buffer,"wb"))) {
							fwrite(a,1,l,f2);
							fclose(f2);
						}
						free(a);
					}
				}

				fclose(fh);
			}
		}
	}

	return TRUE;
}

void SetHighSelection(void)
{
	register int i,n=0;
	char *highs[64];
	int righe,start;
	DIR *lock;

	D(bug("Scan della dir " TEMP_DIR "...\n"));

	for(i=0;i<64;i++)
		hl[i].Testo=NULL;

	if ((lock=opendir(TEMP_DIR))) {
		struct dirent *ent;

		while( (ent=readdir(lock))!=NULL) {
			if(!strnicmp(ent->d_name,"replay."/*-*/,7))	{
				highs[n]=strdup(ent->d_name+7);
				n++;
			}
		}
		closedir(lock);
	}

	righe=n/TS_COLONNE;

	if((righe*TS_COLONNE)<n)
		righe++;

	start=TS_RIGHE/2-righe/2;

	for(i=0;i<n;i++) {
		hl[i+start*TS_COLONNE].ID=i;
		hl[i+start*TS_COLONNE].Testo=highs[i];
	}
}

void UpdateJoyCfg(int joy)
{
	extern struct Bottone joycfg_bottoni[];
	int i;

	for(i=0;i<7;i++)
	{
		joycfg_bottoni[i*2+1].Testo=buttons[joycfg_buttons[joy][i]];
	}
}
