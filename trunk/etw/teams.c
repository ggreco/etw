#include "etw_locale.h"
#include <ctype.h>
#include "menu.h"

UBYTE colore_team[3]={P_GRIGIO0,P_ROSSO0,P_BLU2};
UBYTE highlight_team[3]={P_BIANCO,P_ROSSO1,P_BLU1},totale_giornate;

BYTE teamarray[64],competition=MENU_TEAMS,turno=0,actual_team=-1;
BYTE controllo[64],numero_squadre,ruolo[64],ppv=3,pps=0,ppp=1;
char teamfile[128];

BOOL saved=TRUE,random_draw=FALSE,special=FALSE;
char division=0,scontri=0,i_scontri=0;

char career_file[128]="\0";

struct Campionato_Disk campionato;
struct Squadra_Disk *teamlist=NULL;
struct Manager_Disk manager;

struct DatiSquadra_Disk DatiCampionato[64];
struct Controlled_Disk *giocatori[64];

struct Match turni[64][32];

char *empty=" "/*-*/;

void ReadTeam(FILE *fh, struct Squadra_Disk *s)
{
	int i;

	fread(&s->disponibilita,sizeof(long),1,fh);
	SWAP_LONG(s->disponibilita);
	fread(&s->NumeroGiocatori,sizeof(char),1,fh);
	fread(&s->NumeroPortieri,sizeof(char),1,fh);
	fread(&s->Nazione,sizeof(char),1,fh);
	fread(&s->Flags,sizeof(char),1,fh);

	for(i=0;i<2;i++)
	{
		fread(&s->maglie[i].Tipo,sizeof(char),1,fh);
		fread(&s->maglie[i].Colore0,sizeof(char),1,fh);
		fread(&s->maglie[i].Colore1,sizeof(char),1,fh);
		fread(&s->maglie[i].Colore2,sizeof(char),1,fh);
	}

	for(i=0;i<3;i++)
		fread(&s->Tattiche[i],sizeof(char),16,fh);

	fread(s->nome,sizeof(char),52,fh);
	fread(s->allenatore,sizeof(char),52,fh);


	// le squadre han sempre 3 portieri e 21 giocatori perche' sono files di dimensione fissa!


	for(i=0;i<3;i++)
	{
		fread(s->portiere[i].Nome,sizeof(char),20,fh);
		fread(s->portiere[i].Cognome,sizeof(char),20,fh);
		fread(&s->portiere[i].valore,sizeof(long),1,fh);
		SWAP_LONG(s->portiere[i].valore);
		fread(&s->portiere[i].Numero,sizeof(char),1,fh);
		fread(&s->portiere[i].Velocita,sizeof(char),1,fh);
		fread(&s->portiere[i].Parata,sizeof(char),1,fh);
		fread(&s->portiere[i].Attenzione,sizeof(char),1,fh);
		fread(&s->portiere[i].Nazionalita,sizeof(char),1,fh);
		fread(&s->portiere[i].Eta,sizeof(char),1,fh);
		fread(&s->portiere[i].Infortuni,sizeof(char),1,fh);
		fread(&s->portiere[i].Flags,sizeof(char),1,fh);
	}

	for(i=0;i<21;i++)
	{
		fread(s->giocatore[i].Nome,sizeof(char),20,fh);
		fread(s->giocatore[i].Cognome,sizeof(char),20,fh);
		fread(&s->giocatore[i].valore,sizeof(long),1,fh);
		SWAP_LONG(s->giocatore[i].valore);
		fread(&s->giocatore[i].Numero,sizeof(char),1,fh);
		fread(&s->giocatore[i].Velocita,sizeof(char),1,fh);
		fread(&s->giocatore[i].Contrasto,sizeof(char),1,fh);
		fread(&s->giocatore[i].Tiro,sizeof(char),1,fh);
		fread(&s->giocatore[i].Durata,sizeof(char),1,fh);
		fread(&s->giocatore[i].Resistenza,sizeof(char),1,fh);
		fread(&s->giocatore[i].Prontezza,sizeof(char),1,fh);
		fread(&s->giocatore[i].Nazionalita,sizeof(char),1,fh);
		fread(&s->giocatore[i].Creativita,sizeof(char),1,fh);
		fread(&s->giocatore[i].Tecnica,sizeof(char),1,fh);
		fread(&s->giocatore[i].Eta,sizeof(char),1,fh);
		fread(&s->giocatore[i].Infortuni,sizeof(char),1,fh);
		fread(&s->giocatore[i].Ammonizioni,sizeof(char),1,fh);
		fread(&s->giocatore[i].Posizioni,sizeof(char),1,fh);
	}
}


void WriteTeam(FILE *fh, struct Squadra_Disk *s)
{
	int i;

	SWAP_LONG(s->disponibilita);
	fwrite(&s->disponibilita,sizeof(long),1,fh);
	SWAP_LONG(s->disponibilita);
	fwrite(&s->NumeroGiocatori,sizeof(char),1,fh);
	fwrite(&s->NumeroPortieri,sizeof(char),1,fh);
	fwrite(&s->Nazione,sizeof(char),1,fh);
	fwrite(&s->Flags,sizeof(char),1,fh);

	for(i=0;i<2;i++)
	{
		fwrite(&s->maglie[i].Tipo,sizeof(char),1,fh);
		fwrite(&s->maglie[i].Colore0,sizeof(char),1,fh);
		fwrite(&s->maglie[i].Colore1,sizeof(char),1,fh);
		fwrite(&s->maglie[i].Colore2,sizeof(char),1,fh);
	}

	for(i=0;i<3;i++)
		fwrite(&s->Tattiche[i],sizeof(char),16,fh);

	fwrite(s->nome,sizeof(char),52,fh);
	fwrite(s->allenatore,sizeof(char),52,fh);

	for(i=0;i<3;i++)
	{
		fwrite(s->portiere[i].Nome,sizeof(char),20,fh);
		fwrite(s->portiere[i].Cognome,sizeof(char),20,fh);
		SWAP_LONG(s->portiere[i].valore);
		fwrite(&s->portiere[i].valore,sizeof(long),1,fh);
		SWAP_LONG(s->portiere[i].valore);
		fwrite(&s->portiere[i].Numero,sizeof(char),1,fh);
		fwrite(&s->portiere[i].Velocita,sizeof(char),1,fh);
		fwrite(&s->portiere[i].Parata,sizeof(char),1,fh);
		fwrite(&s->portiere[i].Attenzione,sizeof(char),1,fh);
		fwrite(&s->portiere[i].Nazionalita,sizeof(char),1,fh);
		fwrite(&s->portiere[i].Eta,sizeof(char),1,fh);
		fwrite(&s->portiere[i].Infortuni,sizeof(char),1,fh);
		fwrite(&s->portiere[i].Flags,sizeof(char),1,fh);
	}


	// portieri e giocatori da scrivere son sempre fissi


	for(i=0;i<21;i++)
	{
		fwrite(s->giocatore[i].Nome,sizeof(char),20,fh);
		fwrite(s->giocatore[i].Cognome,sizeof(char),20,fh);
		SWAP_LONG(s->giocatore[i].valore);
		fwrite(&s->giocatore[i].valore,sizeof(long),1,fh);
		SWAP_LONG(s->giocatore[i].valore);
		fwrite(&s->giocatore[i].Numero,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Velocita,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Contrasto,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Tiro,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Durata,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Resistenza,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Prontezza,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Nazionalita,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Creativita,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Tecnica,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Eta,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Infortuni,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Ammonizioni,sizeof(char),1,fh);
		fwrite(&s->giocatore[i].Posizioni,sizeof(char),1,fh);
	}
}

WORD PlayMatch(BYTE a, BYTE b)
{
	if(controllo[a]>=0 || controllo[b]>=0)
		return StartMatch(a,b);
	else
		return ComputerMatch(a,b);

	final=FALSE;
}

void ViewEliminazioneDiretta(int n)
{
	int i;

	for(i=0;i<n;i++)
	{
		mp[i*4].Colore=colore_team[controllo[teamarray[i*2]]+1];
		mp[i*4].Highlight=highlight_team[controllo[teamarray[i*2]]+1];
		mp[i*4+2].Colore=colore_team[controllo[teamarray[i*2+1]]+1];
		mp[i*4+2].Highlight=highlight_team[controllo[teamarray[i*2+1]]+1];
		mp[i*4].Testo=teamlist[teamarray[i*2]].nome;
		mp[i*4+1].Testo="-"/*-*/;
		mp[i*4+2].Testo=teamlist[teamarray[i*2+1]].nome;

		if(mp[i*4+3].Testo)
		{
			free(mp[i*4+3].Testo);
			mp[i*4+3].Testo=NULL;
		}
	}
	ClearMatches(n);
}

void EliminazioneDiretta(int n)
{
	int i;

	nopari=TRUE;

	for(i=0;i<n;i++)
	{
		BYTE a=teamarray[i*2],b=teamarray[i*2+1];
		WORD risultato;

		risultato=PlayMatch(a,b);

		mp[i*4+3].Testo=strdup(ElaboraRisultato(a,b,risultato));

		if((risultato>>8)>(risultato&0xff))
		{
			teamarray[i]=b;
			teamarray[i+n*2]=a;
		}
		else
		{
			teamarray[i]=a;
			teamarray[i+n*2]=b;
		}
	}

// Riordino le squadre...

	for(i=0;i<n;i++)
		teamarray[i+n]=teamarray[i+n*2];

	nopari=FALSE;
}

void SetupSpecialEvent(struct Bottone *b)
{
	int i;

	special=TRUE;

	if(competition!=MENU_TEAMS)
	{
		b->ID=competition;
		return;
	}

	switch(campionato.Tipo)
	{
		case CAMP_CUP:
			competition=MENU_MATCHES;
			b->ID=MENU_TEAM_SELECTION;
			wanted_number=campionato.NumeroSquadre;
			menu[MENU_TEAM_SELECTION].Bottone[64].Testo=msg_0;
			menu[MENU_TEAM_SELECTION].Bottone[66].Testo=NULL;
			i_scontri=scontri=1; // campionato.Scontri; Non ancora supportato
			break;
		case CAMP_LEAGUE:
			competition=MENU_LEAGUE;
			b->ID=MENU_TEAM_SELECTION;

			if(!division)
				wanted_number=campionato.NumeroSquadre;
			else
			{
				wanted_number=0;

// Attualmente permetto solo di giocare partite di serie A.

				for(i=0;i<campionato.NumeroSquadre;i++)
				{
					if(teamlist[i].Flags==0)
						wanted_number++;
				}
				D(bug("Campionato con %ld squadre\n",wanted_number));
			}
			ppv=campionato.Vittoria;
			ppp=campionato.Pareggio;
			pps=campionato.Sconfitta;

			i_scontri=scontri=campionato.Scontri;
			menu[MENU_TEAM_SELECTION].Bottone[64].Testo=msg_0;
			menu[MENU_TEAM_SELECTION].Bottone[66].Testo=NULL;
			break;
		case CAMP_WORLDCUP:
			menu[MENU_TEAM_SELECTION].Bottone[64].Testo=msg_1;
			menu[MENU_TEAM_SELECTION].Bottone[66].Testo=msg_2;

			competition=MENU_WORLD_CUP;
			b->ID=MENU_TEAM_SELECTION;
			wanted_number=32;
			ppv=3;
			pps=0;
			ppp=1;
			break;
		case CAMP_CHAMPIONSLEAGUE:
			ppv=3;
			pps=0;
			ppp=1;
			wanted_number=64;
			break;
	}

	ClearMatches(0);

	selected_number=0;

	i=0;

	while(selected_number<wanted_number&&i<64)
	{
		while(!teamselection[i].Testo&&i<64)
			i++;

		if(competition==MENU_LEAGUE&&division)
			while(teamlist[-teamselection[i].ID-1].Flags)
				i++;

		selected_number++;

		teamselection[i].Colore=COLORE_COMPUTER;
		i++;
	}

	for(i=0;i<64;i++)
		controllo[i]=-1;

	if(selected_number<wanted_number)
	{
		D(bug("Errore nelle squadre, non posso selezionarne %ld!\n"/*-*/,wanted_number));
	}
}

BOOL LoadMenuTactic(char *name,struct Tactic *t)
{
	int i,j,k;
	FILE *fh;

	if(!(fh=fopen(name,"rb")))
		return FALSE;

	fread(&t->NameLen,1,sizeof(char),fh);
	fseek(fh,t->NameLen+1,SEEK_SET);

	for(i=0;i<2;i++)
		for(j=0;j<PLAYERS;j++)
			for(k=0;k<(SECTORS+SPECIALS);k++)
			{
				fread(&t->Position[i][j][k].x,1,sizeof(WORD),fh);
				fread(&t->Position[i][j][k].y,1,sizeof(WORD),fh);
				SWAP_WORD(t->Position[i][j][k].x);
				SWAP_WORD(t->Position[i][j][k].y);
			}

	fclose(fh);

	return TRUE;
}

void DisplayTactic(int xs,int ys)
{
// Il campo e'	108*156, le tattiche 10240,4352

	char buffer[120];
	struct Tactic t;

	if(actual_team<0)
		return;

	sprintf(buffer,"tct/%s",teamlist[actual_team].Tattiche[0]);

	ys+=menu[MENU_TEAM_SETTINGS].Y;
	xs+=menu[MENU_TEAM_SETTINGS].X;

	setfont(smallfont);

	if(LoadMenuTactic(buffer,&t))
	{
		int i,x,y,l;

		for(i=0;i<PLAYERS;i++)
		{
			if(i<8)
				l=1;
			else l=2;

			y=145-t.Position[0][i][GOALKICK].x*156/10240;
			x=t.Position[0][i][GOALKICK].y*100/4352-18;

			BltAnimObj(symbols,main_bitmap,17+controllo[actual_team],x+xs,y+ys,bitmap_width);

			TextShadow(x+xs+(10-l*smallfont->width)/2+2-l,
				y+ys+smallfont->height+1,numero[i+1],l);
		}
	}
}

void MakeLeague(int ns)
{
	if(ns>20||ns<3)
		return;

	numero_squadre=ns;

// Se le squadre sono dispari ne aggiungo sempre una che e' in effetti la
// squadra a riposo.

	if( ((numero_squadre>>1)<<1) != numero_squadre )
	{
		teamarray[numero_squadre]=FAKE_TEAM;
		numero_squadre++;
	}

	totale_giornate=numero_squadre-1;
	turno=0;


	
	switch((numero_squadre-4)/2)
	{
		case 0:
			{
				extern struct Match camp4[3][2];
				int i,j;

				for(i=0;i<3;i++)
					for(j=0;j<2;j++)
						turni[i][j]=camp4[i][j];
			}
			break;
		case 1:
			{
				extern struct Match camp6[5][3];
				int i,j;

				for(i=0;i<5;i++)
					for(j=0;j<3;j++)
						turni[i][j]=camp6[i][j];
			}
			break;
		case 2:
			{
				extern struct Match camp8[7][4];
				int i,j;

				for(i=0;i<7;i++)
					for(j=0;j<4;j++)
						turni[i][j]=camp8[i][j];
			}
			break;
		case 3:
			{
				extern struct Match camp10[9][5];
				int i,j;

				for(i=0;i<9;i++)
					for(j=0;j<5;j++)
						turni[i][j]=camp10[i][j];
			}
			break;
		case 4:
			{
				extern struct Match camp12[11][6];
				int i,j;

				for(i=0;i<11;i++)
					for(j=0;j<6;j++)
						turni[i][j]=camp12[i][j];
			}
			break;
		case 5:
			{
				extern struct Match camp14[13][7];
				int i,j;

				for(i=0;i<13;i++)
					for(j=0;j<7;j++)
						turni[i][j]=camp14[i][j];
			}
			break;
		case 6:
			{
				extern struct Match camp16[15][8];
				int i,j;

				for(i=0;i<15;i++)
					for(j=0;j<8;j++)
						turni[i][j]=camp16[i][j];
			}
			break;
		case 7:
			{
				extern struct Match camp18[17][9];
				int i,j;

				for(i=0;i<17;i++)
					for(j=0;j<9;j++)
						turni[i][j]=camp18[i][j];
			}
			break;
		case 8:
			{
				extern struct Match camp20[19][10];
				int i,j;

				for(i=0;i<19;i++)
					for(j=0;j<10;j++)
						turni[i][j]=camp20[i][j];
			}
			break;

	}

// debug

#ifndef DEBUG_DISABLED
	{
		int i,j;

		for(i=0;i<totale_giornate;i++)
		{
			D(bug("Giornata %ld\n",i+1));

			for(j=0;j<(numero_squadre/2);j++)
				D(bug("%2ld-%2ld\n",turni[i][j].t1,turni[i][j].t2));

			D(bug(".\n"));
		}
	}
#endif

}

void AddName(struct Giocatore_Disk *g,int posizione)
{
	char namebuffer[60];
	char *c=namebuffer,*d=g->Cognome;

	*c++='-';


	if(*g->Nome)
	{
		if(*g->Cognome)
		{
			*c++=*g->Nome;
			*c++='.';
		}
		else
			strcpy(c,g->Nome);
	}


	while(*d)
	{
		*c++=toupper(*d);
		d++;
	}

	*c=0;

	if(teamsettings[posizione*2+1].Testo)
		free(teamsettings[posizione*2+1].Testo);

	teamsettings[posizione*2+1].Testo=strdup(namebuffer);


}

void SetPlayerStatus(int posizione,char infortuni,char ammonizioni,long v)
{
	int i;

	if(pannelli[posizione*3].Testo!=NULL &&
		pannelli[posizione*3].Testo != empty)
		free(pannelli[posizione*3].Testo);

	if(infortuni>0)
	{
		char t=infortuni;

		if(t>3)
			t=3;

		D(bug("Imposto %ld infortunio per %ld\n",t,posizione));

		pannelli[posizione*3].Testo=malloc(2);

		pannelli[posizione*3].Testo[0]=12+t;
		
	}
	else if(ammonizioni>0)
	{
		char t=ammonizioni;

		if(t>3)
			t=3;

		D(bug("Imposto %ld ammonizioni per %ld\n",t,posizione));

		pannelli[posizione*3].Testo=malloc(2);

		pannelli[posizione*3].Testo[0]=4+t;
	}
	else
	{
		pannelli[posizione*3].Testo=empty;
	}

	if(pannelli[posizione*3+2].Testo==NULL ||
		pannelli[posizione*3+2].Testo == empty)
		pannelli[posizione*3+2].Testo=malloc(6);

	pannelli[posizione*3+2].Testo[5]=13;

// Deve dipendere dal valore!

	if(v<1)
	{
		pannelli[posizione*3+2].Testo[0]=' ';
		pannelli[posizione*3+2].Testo[1]=0;
		return;
	}
	
	v--;

	if(v>9)
		v=9;

//	D(bug("Valore: %ld -> %ld\n",valore,v));

	for(i=0;i<10;i+=2)
	{
		if(i<=v)
			pannelli[posizione*3+2].Testo[i/2]= ( (i<v) ? i/2 : (i/2 + 8) );
		else
			pannelli[posizione*3+2].Testo[i/2]=13;
	}
}

void AddGiocatore(struct Giocatore_Disk *g,int posizione)
{
	char buffer[8];

	AddName(g,posizione);

	*buffer=0;
	
	if(g->Posizioni&P_DIFESA)
	{
		strcat(buffer,msg_3);
	}
	if(g->Posizioni&P_CENTRO)
	{
		strcat(buffer,msg_4);
	}
	if(g->Posizioni&P_ATTACCO)
	{
		strcat(buffer,msg_5);
	}

	if(pannelli[posizione*3+1].Testo&&pannelli[posizione*3+1].Testo!=empty)
		free(pannelli[posizione*3+1].Testo);

	pannelli[posizione*3+1].Testo=strdup(buffer);

	SetPlayerStatus(posizione,g->Infortuni,g->Ammonizioni,
		(((g->Tiro+g->Contrasto+g->Velocita*2+g->Tecnica+g->Creativita-2*6+3)*10)/7)/6 );
}

void SetTeamSettings(WORD team)
{
	int i,k;

	actual_team=team;

	for(i=0;i<11;i++)
	{
		teamsettings[i*2+1].Colore=
			teamsettings[i*2].Colore=pannelli[i*3].Colore=colore_team[controllo[team]+1];

		teamsettings[i*2+1].Highlight=
			teamsettings[i*2].Highlight=pannelli[i*3].Highlight=highlight_team[controllo[team]+1];
	}

	for(i=11;i<16;i++)
	{
		teamsettings[i*2+1].Colore=
			teamsettings[i*2].Colore=pannelli[i*3].Colore=highlight_team[controllo[team]+1];

		teamsettings[i*2+1].Highlight=
			teamsettings[i*2].Highlight=pannelli[i*3].Highlight=colore_team[controllo[team]+1];
	}

	for(i=0,k=1;i<min(15,teamlist[team].NumeroGiocatori);i++)
	{
		if(k==11)
		{
			k++;
		}

		AddGiocatore(&teamlist[team].giocatore[i],k);

		k++;
	}

	for(i=0;i<min(17,teamlist[team].NumeroGiocatori+teamlist[team].NumeroPortieri);i++)
	{
		teamsettings[i*2].Testo=numero[i];
		teamsettings[i*2].ID=i;
//		pannelli[i*3].Testo=empty;
	}

	if(teamlist[team].NumeroGiocatori<15)
	{
		if(k==11)
			k++;

		for(i=k;i<17;i++)
		{
			if(teamsettings[i*2+1].Testo)
				free(teamsettings[i*2+1].Testo);

			teamsettings[i*2+1].Testo=NULL;
			teamsettings[i*2].Testo=NULL;

			if(pannelli[i*3].Testo&&pannelli[i*3].Testo!=empty)
				free(pannelli[i*3].Testo);

			if(pannelli[i*3+1].Testo&&pannelli[i*3+1].Testo!=empty)
				free(pannelli[i*3+1].Testo);

			if(pannelli[i*3+2].Testo&&pannelli[i*3+2].Testo!=empty)
				free(pannelli[i*3+2].Testo);

			pannelli[i*3+1].Testo=pannelli[i*3].Testo=pannelli[i*3+2].Testo=NULL;
		}
	}

	for(i=0;i<6;i++)
	{
		if(!stricmp(teamsettings[34+i].Testo,teamlist[team].Tattiche[0]))
		{
			teamsettings[34+i].Colore=COLORE_TATTICA_SELEZIONATA;
		}
		else teamsettings[34+i].Colore=COLORE_TATTICA_NON_SELEZIONATA;
	}

	AddName((struct Giocatore_Disk *)&teamlist[team].portiere[0],0);
	SetPlayerStatus(0,teamlist[team].portiere[0].Infortuni,0,(((teamlist[team].portiere[0].Parata*2+teamlist[team].portiere[0].Attenzione-2*3+2)*10)/7)/3);

	if(teamlist[team].NumeroPortieri<2)
	{
		if(teamsettings[11*2+1].Testo)
			free(teamsettings[11*2+1].Testo);

		if(pannelli[11*3].Testo&&pannelli[11*3].Testo!=empty)
			free(pannelli[11*3].Testo);

		if(pannelli[11*3+1].Testo&&pannelli[11*3+1].Testo!=empty)
			free(pannelli[11*3+1].Testo);

		if(pannelli[11*3+2].Testo&&pannelli[11*3+2].Testo!=empty)
			free(pannelli[11*3+2].Testo);

		pannelli[11*3+2].Testo=pannelli[11*3+1].Testo=pannelli[11*3].Testo=NULL;
		teamsettings[11*2].Testo=teamsettings[11*2+1].Testo=NULL;
	}
	else
	{
		AddName((struct Giocatore_Disk *)&teamlist[team].portiere[1],11);
		SetPlayerStatus(11,teamlist[team].portiere[1].Infortuni,0,
						(teamlist[team].portiere[1].Parata*2+teamlist[team].portiere[1].Attenzione+2)/3);
	}

	if(game_start)
		teamsettings[42].Testo=msg_0;
	else
		teamsettings[42].Testo=msg_6;
}

void SetTeamSelection(void)
{
	int righe,start,i;

	righe=campionato.NumeroSquadre/TS_COLONNE;

	if((righe*TS_COLONNE)<campionato.NumeroSquadre)
		righe++;

	start=TS_RIGHE/2-righe/2;

	for(i=0;i<TS_RIGHE*TS_COLONNE;i++)
	{
		teamselection[i].Testo=NULL;
		teamselection[i].Colore=COLORE_UNSELECTED;
		teamselection[i].ID=-65;
	}

	for(i=0;i<campionato.NumeroSquadre;i++)
	{
		teamselection[i+start*TS_COLONNE].ID=-i-1;
		teamselection[i+start*TS_COLONNE].Testo=teamlist[i].nome;
	}
}

void LoadLeague(void)
{
	competition=current_menu;

	LoadTeams(career_file);

// Bottoni di 4 deve essere sempre il bottone SAVE.

	if(competition==MENU_WORLD_CUP)
		special=TRUE;
/*
	if(menu[current_menu].Bottone[4].Testo==NULL)
	{
		menu[current_menu].Bottone[4].Testo="SAVE";
		ChangeMenu(current_menu);
	}
*/
}

void SaveLeague(void)
{
	SaveTeams(career_file);

	saved=TRUE;
}

void SaveTeams(char *name)
{
	FILE *fh;
	int i;

	/* AC: Secondo me l'opzione è sbagliata. Direi che serve wb :) */
	//if(fh=fopen(name,"rb"))
	if(fh=fopen(name,"wb"))
	{
		campionato.NumeroSquadre--;

		fwrite(&campionato,sizeof(struct Campionato_Disk),1,fh);

		campionato.NumeroSquadre++;

		for(i=0;i<campionato.NumeroSquadre;i++)
			WriteTeam(fh,&teamlist[i]);

		if(competition!=MENU_TEAMS)
		{
			fwrite(DatiCampionato,sizeof(struct DatiSquadra_Disk)*campionato.NumeroSquadre,1,fh);

			for(i=0;i<campionato.NumeroSquadre;i++)
			{
				if(DatiCampionato[i].Controllata)
				{
					fwrite(giocatori[i],sizeof(struct Controlled_Disk),1,fh);

					if(giocatori[i]->ManagerType)
						fwrite(&manager,sizeof(struct Manager_Disk),1,fh);
				}
			}

			
			fwrite(&competition,sizeof(competition),1,fh);
			fwrite(&numero_squadre,sizeof(numero_squadre),1,fh);

// Per ricavarmi correttamente i_scontri uso un patch su turno

			if(i_scontri>scontri)
			{
				turno+=(numero_squadre-1)*(i_scontri-scontri);
			}

			fwrite(&turno,sizeof(turno),1,fh);

			if(i_scontri>scontri)
			{
				turno-=(numero_squadre-1)*(i_scontri-scontri);
			}

// Per non distruggere il comportamento attuale uso un patch su totale_giornate...

			if(scontri>1)
			{
				totale_giornate+=(numero_squadre-1)*(scontri-1);
			}

			fwrite(&totale_giornate,sizeof(totale_giornate),1,fh);

			if(scontri>1)
			{
				totale_giornate-=(numero_squadre-1)*(scontri-1);
			}

			fwrite(teamarray,sizeof(teamarray),1,fh);
			fwrite(controllo,sizeof(controllo),1,fh);

			if(competition==MENU_WORLD_CUP)
			{
				extern BYTE start_groups[8][4],groups[8][4];
				
				for(i=0;i<8;i++)
				{
					fwrite(start_groups[i],4,sizeof(BYTE),fh);
				}
			}
			else if(competition==MENU_LEAGUE)
			{
				fwrite(league_pos,sizeof(league_pos),1,fh);
			}
			
		}
		fclose(fh);
	}
}

void LoadTeams(char *name)
{
	FILE *fh;
	struct Campionato_Disk temp;

	if(teamfile!=name)
	strcpy(teamfile,name);

	temp=campionato;

	if(fh=fopen(name,"rb"))
	{
		BOOL ok=FALSE;

		D(bug("Carico le squadre da %s...\n",name));

		if(fread(&campionato,1,sizeof(struct Campionato_Disk),fh)==sizeof(struct Campionato_Disk))
		{
			struct Squadra_Disk *teamold;
			D(bug("Campionato: %s V:%ld-P:%ld-L:%ld-S:%ld\n",campionato.Nome,campionato.Vittoria,
					campionato.Pareggio, campionato.Sconfitta,campionato.NumeroSquadre+1));

			campionato.NumeroSquadre++;

			menu[MENU_SIMULATION].Bottone[0].Testo=(*campionato.Nome!=0) ? campionato.Nome : NULL;

			{
				int i=0;

				while(campionato.Nome[i])
				{
					campionato.Nome[i]=toupper(campionato.Nome[i]);
					i++;
				}
			}

			teamold=teamlist;

			if(teamlist=malloc(campionato.NumeroSquadre*sizeof(struct Squadra_Disk)) )
			{
				int i;
				char *s;

				for(i=0;i<campionato.NumeroSquadre;i++)
				{
					ReadTeam(fh,&teamlist[i]);
					
					s=teamlist[i].nome;

					while(*s)
					{
						*s=toupper(*s);
						s++;
					}

					if(division<teamlist[i].Flags)
						division=teamlist[i].Flags;

					s=teamlist[i].allenatore;

					while(*s)
					{
						*s=toupper(*s);
						s++;
					}
				}

				if(i==campionato.NumeroSquadre)
				{
					ok=TRUE;

					if(teamold)
						free(teamold);

					SetTeamSelection();
				}
				else
				{
					free(teamlist);
					teamlist=teamold;
				}

				if(competition!=MENU_TEAMS)
				{
					if(fread(DatiCampionato,sizeof(struct DatiSquadra_Disk)*campionato.NumeroSquadre,1,fh)==sizeof(struct DatiSquadra_Disk)*campionato.NumeroSquadre)
					{
						for(i=0;i<campionato.NumeroSquadre;i++)
						{
							if(DatiCampionato[i].Controllata)
							{
								if(!giocatori[i])
									giocatori[i]=malloc( sizeof(struct Controlled_Disk) ); 

								if(giocatori[i])
								{
									fread(giocatori[i],sizeof(struct Controlled_Disk),1,fh);

									if(giocatori[i]->ManagerType)
										fread(&manager,sizeof(struct Manager_Disk),1,fh);
								}
							}
						}
					}

					/* AC: C'e' un problema qui. i valori di numero_squadre e turno
					 * o sono salvati male o sono caricati male, sono entrambi a zero!
					 * Indaghero'.
					 */
					fread(&competition,sizeof(competition),1,fh);
					fread(&numero_squadre,sizeof(numero_squadre),1,fh);

					if(competition==MENU_LEAGUE)
						MakeLeague(numero_squadre);

// Mi serve sapere inizialmente quanti scontri avevo...

					i_scontri=1;

					fread(&turno,sizeof(turno),1,fh);

					while(turno>=numero_squadre)
					{
						/* AC: Al primo ciclo del while, numero_squadre vale 0 
						 * e turno 1.
						 * Dopo questa istruzione turno vale 2 perch - * - = +
						 * Se numero_squadre  vero che deve valere zero, allora
						 * si pu˜ usare turno += (numero_squadre-1)
						 * altrimenti non va.
						turno-=(numero_squadre-1);*/
						if(numero_squadre != 0)
							turno-=(numero_squadre-1);
						else
							turno+=(numero_squadre-1);
						i_scontri++;
					}

					scontri=1;

					fread(&totale_giornate,sizeof(totale_giornate),1,fh);

// Patch per gestire correttamente il caricamento di un campionato con + di uno scontro!

					while(totale_giornate>=numero_squadre)
					{
						/* AC: Stesso discorso del while precedente */
						if(numero_squadre != 0)
							totale_giornate-=(numero_squadre-1);
						else
							totale_giornate+=(numero_squadre-1);
						scontri++;
					}

					if(i_scontri>1&&i_scontri!=scontri)
					{
// Significa che devo fare uno swap delle squadre...
						InvertiSquadre();
					}

					fread(teamarray,sizeof(teamarray),1,fh);
					fread(controllo,sizeof(controllo),1,fh);

					if(competition==MENU_WORLD_CUP)
					{
						extern BYTE start_groups[8][4],groups[8][4];
						
						for(i=0;i<8;i++)
						{
							fread(start_groups[i],sizeof(BYTE),4,fh);

							memcpy(groups[i],start_groups[i],4*sizeof(BYTE));
						}

						GroupsUpdate();
					}
					else if(competition==MENU_LEAGUE)
					{
						fread(league_pos,sizeof(league_pos),1,fh);

						InitTable();
						UpdateLeagueTable();
						ChangeMenu(MENU_LEAGUE);
					}
				}
			}
		}

		if(!ok)
			campionato=temp;

		fclose(fh);
	}
	D(else bug("Non posso caricare %s!\n",name));
}
