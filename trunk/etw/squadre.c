#include "eat.h"
#ifndef WIN
#ifdef SOLARIS_X86
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#else
#include "highdirent.h"
#endif

char team_name[2][16]={"brazil","england"};
int result_width,scivolate_modificate=0;
GuardaLinee *guardalinee;

UBYTE cols[2][4],NumeroTattiche=0,TotaleRiserve[2];
BOOL teams_swapped=FALSE,has_black[2]={FALSE,FALSE};
WORD swaps=0;
char *Tattiche[32];

struct Giocatore_Disk Riserve[2][12];

AnimObj *giocatore[2]={NULL,NULL},*portiere=NULL,*ports,*arcade_anim;
AnimObj *g_neri[2]={NULL,NULL};
Oggetto *pezzi_porte[4];
Oggetto *bonus[MAX_ARCADE_ON_FIELD];

char snum,divisa;

extern struct Pos portieri[2][2][SECTORS+SPECIALS];

BOOL NumeroDiverso(struct Squadra *s,char n)
{
	register int i;

	for(i=0;i<10;i++)
			if(s->giocatore[i].Numero==n)
					return FALSE;

	return TRUE;
}

void RimuoviGuardalinee(void)
{
	if((detail_level&USA_GUARDALINEE)
        && guardalinee)	{
		D(bug("Removing linesmen...\n"));

		RimuoviLista((Oggetto *)&guardalinee[1]);
		RimuoviLista((Oggetto *)&guardalinee[0]);
		FreeAnimObj(guardalinee[1].immagine);
		FreeAnimObj(guardalinee[0].immagine);
        free(guardalinee);
        guardalinee = NULL;
	}
}

void ChangeImmagine(Giocatore *g,AnimObj *o)
{
	WORD x,y,f,b;
	BOOL g_nero=IsBlack(g);

	x=g->immagine->x_pos;
	y=g->immagine->y_pos;
	f=g->immagine->current_frame;
	b=g->immagine->bottom;

// Devo usare forbid/permit per evitare rendering indesiderati!

	if(g->immagine->Flags&AOBJ_CLONED)
	{
		D(bug("Changing skin colors, simple case!\n"));

		FreeAnimObj(g->immagine);
		g->immagine=CloneAnimObj(o);
	}
	else
	{
		Squadra *s=g->squadra;
		Giocatore *g2=NULL;
		int i;

		for(i=0;i<10;i++)
		{
			if(i==g->GNum)
				continue;

			if(g_nero && IsBlack(&s->giocatore[i]))
			{
				g2=&s->giocatore[i];
				break;
			}
			else if(!g_nero && !IsBlack(&s->giocatore[i]))
			{
				g2=&s->giocatore[i];
				break;
			}
		}

		if(g2)
		{
			D(bug("Changing skin colors, trick case A!\n"));
			g->immagine->x_pos=g2->immagine->x_pos;
			g->immagine->y_pos=g2->immagine->y_pos;
			g->immagine->current_frame=g2->immagine->current_frame;
			g->immagine->bottom=g2->immagine->bottom;

			FreeAnimObj(g2->immagine);

			g2->immagine=g->immagine;
			g->immagine=CloneAnimObj(o);
		}
		else
		{
// G e' l'unico nero (o bianco), quindi lo rimuovo!
			int snum=g->SNum;

			D(bug("Changing skin colors, trick case B!\n"));

			if(teams_swapped)
				snum^=1;

			FreeAnimObj(g->immagine);
			has_black[snum]=FALSE;

			g->immagine=CloneAnimObj(o);
		}
	}

	g->immagine->x_pos=x;
	g->immagine->y_pos=y;
	g->immagine->current_frame=f;
	g->immagine->bottom=b;
}

void SwapTeams(void)
{
	extern UBYTE ReplaySet;
	extern char golrig[2];
	char c;
	struct Squadra *s;
	int i,j;
	UBYTE tmp;
	void (*Temp)(int);

	D(bug("Inverting teams in pitch...\n"));

	swaps++;

	s=p->squadra[0];
	p->squadra[0]=p->squadra[1];
	p->squadra[1]=s;

	for(j=0;j<2;j++) {
		p->squadra[j]->portiere.SNum=j;

		for(i=0;i<10;i++)
			p->squadra[j]->giocatore[i].SNum=j;
	}

	
	Temp=HandleSquadra0;
	HandleSquadra0=HandleSquadra1;
	HandleSquadra1=Temp;
	InvertTactic(p->squadra[0]->tattica);
	InvertTactic(p->squadra[1]->tattica);

	counter=0;
	replay_looped=FALSE;
	ReplaySet=0;
	full_replay=FALSE;
	first_kickoff=TRUE;

	MakeResult();

	tmp=team_a;
	team_a=team_b;
	team_b=tmp;

	c=golrig[0];
	golrig[0]=golrig[1];
	golrig[1]=c;

	for(i=0;i<12;i++) {
		struct Giocatore_Disk temp;

		temp=Riserve[0][i];
		Riserve[0][i]=Riserve[1][i];

		Riserve[1][i]=temp;
	}

	tmp=TotaleRiserve[0];
	TotaleRiserve[0]=TotaleRiserve[1];
	TotaleRiserve[1]=tmp;

	teams_swapped^=1;
}

void CheckPelle(void)
{
	int i,j,neri;

	if(arcade_teams)
	{
		if(team_a==3)
			RemapAnimObjColor(p->squadra[0]->giocatore[9].immagine,Pens[P_ARANCIO1],Pens[P_GIALLO]);
		else if(team_b==3)
			RemapAnimObjColor(p->squadra[1]->giocatore[9].immagine,Pens[P_ARANCIO1],Pens[P_GIALLO]);
	}


	/* AC: I put Progress() out of the for, so the number of progres block are 20 and not 22,
	 * in order to have a plain result from the division between window width and number of
	 * blocks
	 */ 
	Progress();
	
	for(i=0;i<2;i++)
	{
		struct Squadra *s=p->squadra[i];

		D(bug("Check skin color team %ld\n",i));
		neri=0;

		for(j=0;j<10;j++)
		{
			if(IsBlack(&s->giocatore[j]))
				neri++;
		}

// if all the players are black remap every player
		if( neri>9 ) {
			has_black[i]=FALSE;

			RemapAnimObjColor(s->giocatore[9].immagine,Pens[P_ROSSO2],Pens[P_NERO]);
			RemapAnimObjColor(s->giocatore[9].immagine,Pens[P_ARANCIO1],Pens[P_ROSSO2]);
		}
		else if(neri>0)	{
			AnimObj *o;

			if ((o=CopyAnimObj(s->giocatore[9].immagine)))	{
				BOOL fatti=FALSE;
				has_black[i]=TRUE;

				RemapAnimObjColor(o,Pens[P_ROSSO2],Pens[P_NERO]);
				RemapAnimObjColor(o,Pens[P_ARANCIO1],Pens[P_ROSSO2]);

				for(j=0;j<9;j++)
				{
					if(IsBlack(&s->giocatore[j]))
					{
						FreeAnimObj(s->giocatore[j].immagine);

						if(!fatti)
						{
							s->giocatore[j].immagine=o;
							fatti=TRUE;
						}
						else
						{
							s->giocatore[j].immagine=CloneAnimObj(o);
						}
					}
				}

				if(IsBlack(&s->giocatore[9]))
				{
					j=8;

					while(IsBlack(&s->giocatore[j]))
						j--;

					FreeAnimObj(s->giocatore[j].immagine);
					s->giocatore[j].immagine=s->giocatore[9].immagine;
					s->giocatore[9].immagine=CloneAnimObj(o);
				}				
				g_neri[i]=o;
			}
		}

		D(bug("%ld black skinned players.\n",neri));
	}
}

void MakeName(Giocatore *g,struct Giocatore_Disk *gd)
{
	int j,l=strlen(gd->Nome)+strlen(gd->Cognome)+1;

	g->Nome=malloc(l+1);

	strcpy(g->Nome,gd->Nome);
	strcat(g->Nome," ");
		
	strcat(g->Nome,gd->Cognome);
	g->NameLen=strlen(gd->Cognome);

	g->Cognome=g->Nome+1+strlen(gd->Nome);

	for(j=0;j<l;j++)
	{
		g->Nome[j]=toupper(g->Nome[j]);
	}
}

void ChangePlayer(struct Giocatore_Disk *src,Giocatore *dest)
{
// Devo curarmi del cambio di colore della pelle...
	int snum=dest->SNum;

	if(teams_swapped)
		snum^=1;

	if(has_black[snum])
	{
		if( IsBlack(dest) && !IsBlack(src) )
		{
// E' entrato un giocatore bianco al posto di un nero...
			struct Squadra *s=dest->squadra;
			int i;

			for(i=0;i<10;i++)
			{
				if(!IsBlack(&s->giocatore[i]))
				{
					ChangeImmagine(dest,s->giocatore[i].immagine);
					break;
				}
			}
		}
		else if( IsBlack(src) && !IsBlack(dest))
		{
// E' entrato un giocatore nero al posto di un bianco...
			struct Squadra *s=dest->squadra;
			int i;

			for(i=0;i<10;i++)
			{
				if(IsBlack(&s->giocatore[i]))
				{
					ChangeImmagine(dest,s->giocatore[i].immagine);
					break;
				}
			}
		}
	}

	dest->Numero=src->Numero;
	dest->Velocita=src->Velocita;
	dest->Contrasto=src->Contrasto;
	dest->Tiro=src->Tiro;
	dest->Durata=src->Durata;
	dest->Resistenza=src->Resistenza;
	dest->Prontezza=src->Prontezza;
	dest->Creativita=src->Creativita;
	dest->Tecnica=src->Tecnica;
	dest->Posizioni=src->Posizioni;
	
    free(dest->Nome);
	MakeName(dest,src);

	dest->Posizioni=src->Posizioni;
}

void ScanTactics(void)
{
	DIR *dir;
	struct dirent *ent;

	D(bug("Scan of dir ETW-TCT:...\n"));

	/*AC: I think that NumeroTattiche must be resetted to zero here... ^_^ */
	NumeroTattiche = 0;

	if ((dir=opendir(TCT_DIR))) {
		while( (ent=readdir(dir))!=NULL) {
			if( *ent->d_name == '.' )
				continue;

			Tattiche[NumeroTattiche]=strdup(ent->d_name);
			NumeroTattiche++;
		}
		closedir(dir);
	}
	D(bug("%ld available tactics.\n",NumeroTattiche));
}

BOOL InizializzaOggetti(Partita *p)
{
	int i;
	Oggetto *o;
	BOOL ok=TRUE;

// Inizializzo il portiere di destra...

	switch(current_field)
	{
		case 5:
			// La lasciamo cosi'. Originariamente la rallentavo...
			break;
		case 6:
		case 7:
			scivolate_modificate=1;
			Animation[GIOCATORE_SCIVOLATA].FrameLen+=2;
			break;
		case 4:
			scivolate_modificate=1;
			Animation[GIOCATORE_SCIVOLATA].FrameLen+=3;
			break;
	}

	for(i=0;i<2;i++)
	{
		int j;
		struct Pos temp;

		if(arcade) {
			for(j=0;j<(SECTORS+SPECIALS);j++)
				portieri[1][i][j].x+=64;
		}

		for(j=0;j<(SECTORS+SPECIALS);j++)
			portieri[0][i][j].x=((FIELD_WIDTH-10)*8)-portieri[1][i][j].x;

		for(j=0;j<3;j++)
		{
			temp=portieri[0][i][j*4];
			portieri[0][i][j*4]=portieri[0][i][j*4+3];
			portieri[0][i][j*4+3]=temp;
			temp=portieri[0][i][j*4+1];
			portieri[0][i][j*4+1]=portieri[0][i][j*4+2];
			portieri[0][i][j*4+2]=temp;
		}
	}

// Inizializzo i bonus arcade...

	if (arcade)	{
		for (i=0;i<MAX_ARCADE_ON_FIELD;i++)	{
			if ((o=malloc(sizeof(Oggetto)))) {
				o->world_x=-1000;

				o->ObjectType=TIPO_BONUS;
				o->AnimFrame=0;
				o->AnimType=20+i*3;
				o->OnScreen=FALSE;
			
				o->world_y=0;

				if (!(o->immagine=CloneAnimObj(p->squadra[0]->Marker)))	{
					D(bug("Non c'e' piu' memoria per gli animobj!\n"));
					ok=FALSE;
					break;
				}
			
				AggiungiLista(o);

				bonus[i]=o;
			}
		}
	}

// Carico le porte

	for(i=0;i<4;i++) {
		if ((o=malloc(sizeof(Oggetto)))) {
			o->world_x=-1000;

			o->ObjectType=TIPO_PORTA;
			o->AnimFrame=0;
			o->AnimType=i;
			o->OnScreen=FALSE;

			if(!arcade)	{
				if(i!=3) {
					if(!(o->immagine=CloneAnimObj(ports))) {
						D(bug("Non c'e' piu' memoria per gli animobj!\n"));
						ok=FALSE;
						break;
					}
				}
				else
					o->immagine=ports;
			
				if(i==0||i==2) {
					o->immagine->Flags|=AOBJ_OVER;
					o->world_y=219*8+7;
				}
				else
					o->world_y=263*8+7;

				AggiungiLista(o);
			}

			pezzi_porte[i]=o;
		}
		else {
			D(bug("Not enough free memory!\n"));
			ok=FALSE;
			break;
		}
	}

	if(ok==FALSE) {
		for(i=0;i<totale_lista;i++) {
			if ((o=object_list[i])) {
				if(o->ObjectType>TIPO_ARBITRO)
				{
					FreeAnimObj(o->immagine);
					free(o);
				}
				object_list[i]=NULL;
				totale_lista--;
			}
		}
	}

	Progress();

	return ok;
}

void LiberaSquadra(Squadra *s)
{
	int i;

	D(bug("Freeing team...\n"));

	if(s->Marker)
		FreeAnimObj(s->Marker);

	FreeAnimObj(s->portiere.immagine);
	free(s->portiere.Nome);

	free(s->NomeAttivo);
	FreeTactic(s->tattica);

	for(i=0;i<10;i++)
	{
		free(s->giocatore[i].Nome);
		FreeAnimObj(s->giocatore[i].immagine);
	}

	giocatore[(int)s->giocatore[0].SNum]=NULL;
	portiere=NULL;

	free(s);

	D(bug("Ok.\n"));
}

void DisponiPortiere(Squadra *s,int settore,BOOL possesso)
{

	s->portiere.world_x=portieri[(int)s->portiere.SNum][possesso][settore].x;
	s->portiere.world_y=portieri[(int)s->portiere.SNum][possesso][settore].y;

	if(settore==GOALKICK)
	{
		if(possesso)
		{
			if(pl->world_y>CENTROCAMPO_Y)
			{
				s->portiere.world_y=((346-14)*8);

				if(s==p->squadra[1])
				{
					s->portiere.world_x=(70*8);
				}
				else	s->portiere.world_x=(1196*8);
			}
			else
			{
				s->portiere.world_y=((198-14)*8);

				if(s==p->squadra[1])
				{
					s->portiere.world_x=(80*8);
				}
				else	s->portiere.world_x=(1186*8);
			}
		}
	}
}

void DisponiSquadra(Squadra *s,int settore,BOOL possesso)
{
	int i;

	possesso = (possesso ? 1 : 0);

	if(s->ArcadeEffect)
		RemoveArcadeEffect(&s->giocatore[0],s->ArcadeEffect);

	for(i=0;i<10;i++)
	{

// Per debug, causo un enforcer hit se ho problemi

	    if(s->giocatore[i].AnimType!=GIOCATORE_ESPULSO&&s->giocatore[i].Comando!=STAI_FERMO&&s->giocatore[i].Comando!=ESCI_CAMPO)
	    {
		if(s->giocatore[i].world_x<0 ||
			s->giocatore[i].world_x>10400 ||
			s->giocatore[i].Velocita<3 ||
			s->giocatore[i].Direzione>7 ||
			s->giocatore[i].Direzione<0 ||
			s->giocatore[i].GNum!=i ||
			s->giocatore[i].ActualSpeed<0 ||
			s->giocatore[i].ActualSpeed>3 )
		{
//			char *a=NULL;

			D(bug(" *** Player %ld of team %ld with internal problems!\n",i+2,s->giocatore[i].SNum));
//			a[0]=1;
		}

		s->giocatore[i].world_x=s->tattica->Position[possesso][i][settore].x;
		s->giocatore[i].world_y=s->tattica->Position[possesso][i][settore].y;

// Metto questa roba per vedere se risolvo il problema dell'omino ballerino!

		s->giocatore[i].ArcadeEffect=NESSUN_COMANDO;
		s->giocatore[i].Comando=NESSUN_COMANDO;
		s->giocatore[i].Special=FALSE;
		s->giocatore[i].FrameLen=10;
		s->giocatore[i].AnimType=GIOCATORE_RESPIRA;
		s->giocatore[i].AnimFrame=0;
		s->giocatore[i].ActualSpeed=0;
		s->giocatore[i].Direzione=FindDirection(s->giocatore[i].world_x,s->giocatore[i].world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
	    }
	    else
	    {
		D(bug("Skipping player %ld of team %ld(A:%ld,S:%ld,C:%ld)\n",i+2,s->giocatore[0].SNum,
			s->giocatore[i].AnimType,s->giocatore[i].Special,s->giocatore[i].Comando));
	    }
	}

	DisponiPortiere(s,settore,possesso);
}

FILE *OpenTeam(char *Nome)
{
	char path[100];
	FILE *fh;

	strcpy(path,TEMP_DIR);
	strcat(path,Nome);

	if(!(fh=fopen(path,"rb")))
	{
		strcpy(path,TEAMS_DIR);
		strcat(path,Nome);

		if(!(fh=fopen(path,"rb")))
		{
			return NULL;
		}
	}

	return fh;
}

void ReadSquadra(FILE *fh, struct Squadra_Disk *s)
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

	for(i=0;i<s->NumeroPortieri;i++)
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

	for(i=0;i<s->NumeroGiocatori;i++)
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

Squadra *CreaSquadra(int num)
{
	char path[100];
	Squadra *s;
	extern struct Squadra_Disk leftteam_dk,rightteam_dk;
	struct Squadra_Disk sd;
#if 0
	FILE *fh;
#endif
	int i;

	if(!(s=calloc(1,sizeof(struct Squadra))))
		return NULL;

#if 0
	if(!(fh=OpenTeam(Nome)))
	{
		D(bug("Non trovo %s!\n",Nome));
		free(s);
		return NULL;
	}
	ReadSquadra(fh,&sd);
	
	fclose(fh);

#else
	if(num)
		sd=rightteam_dk;
	else
		sd=leftteam_dk;
#endif

//	fread(&sd,sizeof(struct Squadra_Disk),1,fh); prima la leggevo cosi'...


	if(portiere)
	{
		if(!(portiere=CloneAnimObj(portiere)))
			return NULL;
	}
	else
	{
		if(!(portiere=LoadAnimObject((current_field==7 ? "gfx/portsnow.obj" : "gfx/portiere.obj"),Pens)))
			return NULL;
	}


	for(i=0;i<strlen(sd.nome);i++)
	{
		s->Nome[i]=toupper(sd.nome[i]);
	}

	s->Nome[i]=0;

	s->giocatore[9].immagine=giocatore[snum];

	for(i=0;i<9;i++)
	{
		if(!(s->giocatore[i].immagine=CloneAnimObj(giocatore[snum])))
		{
			free(s);
			return NULL;
		}
	}

	strcpy(path,TCT_DIR);
	strcat(path,sd.Tattiche[0]);

	if(!(s->tattica=LoadTactic(path)))
	{
		D(bug("Non trovo la tattica: %s!\n",path));
		free(s);
		return NULL;
	}

	Progress();

	if(!(s->NomeAttivo=malloc(CHAR_X*MAX_LEN*CHAR_Y)))
	{
		FreeTactic(s->tattica);
		free(s);
		return NULL;
	}

	cols[snum][0]=sd.maglie[0].Colore0;
	cols[snum][1]=sd.maglie[0].Colore1;
	cols[snum][2]=sd.maglie[0].Colore2;

	if(sd.maglie[0].Colore0!=COLORE_STANDARD_MAGLIE)
		RemapAnimObjColor(giocatore[snum],Pens[COLORE_STANDARD_MAGLIE],Pens[sd.maglie[0].Colore0]);
	if(sd.maglie[0].Colore1!=COLORE_STANDARD_CALZONI)
		RemapAnimObjColor(giocatore[snum],Pens[COLORE_STANDARD_CALZONI],Pens[sd.maglie[0].Colore1]);
	if(sd.maglie[0].Colore2!=COLORE_STANDARD_AUSILIARIO&&sd.maglie[0].Tipo>0)
		RemapAnimObjColor(giocatore[snum],Pens[COLORE_STANDARD_AUSILIARIO],Pens[sd.maglie[0].Colore2]);

// Copio i dati dei giocatori.

	if(player_type[snum]<0)
	{
		for(i=0;i<sd.NumeroGiocatori;i++)
		{
			if(sd.giocatore[i].Velocita<5)
				sd.giocatore[i].Velocita++;
			if(sd.giocatore[i].Velocita<7)
				sd.giocatore[i].Velocita++;

			if(sd.giocatore[i].Contrasto<5)
				sd.giocatore[i].Contrasto++;
			if(sd.giocatore[i].Contrasto<7)
				sd.giocatore[i].Contrasto++;
			
			if(sd.giocatore[i].Prontezza<5)
				sd.giocatore[i].Prontezza++;
			if(sd.giocatore[i].Prontezza<7)
				sd.giocatore[i].Prontezza++;
		}
	}

	for(i=0;i<10;i++)
	{
// Copio le 10 stats del giocatore

		if(sd.giocatore[i].Ammonizioni>1)
		{
			int k=10;

			while(k<sd.NumeroGiocatori&&(sd.giocatore[k].Ammonizioni>1||sd.giocatore[k].Infortuni>2))
				k++;

			if(k==sd.NumeroGiocatori)
			{
				DoSpecialAnim( (&s->giocatore[i]),GIOCATORE_ESPULSO);
				s->giocatore[i].Comando=STAI_FERMO;
				s->giocatore[i].world_x=CENTROCAMPO_X;
				s->giocatore[i].world_y=(FIELD_HEIGHT-30)*8;
			}
			else
			{
				struct Giocatore_Disk temp=sd.giocatore[i];

				sd.giocatore[i]=sd.giocatore[k-1];
				sd.giocatore[k-1]=temp;
			}
		}

		if(sd.giocatore[i].Infortuni>0)
		{
			char t=sd.giocatore[i].Infortuni;

			if(t>3)
				t=3;
	
			t*=2;

			sd.giocatore[i].Velocita=max(1,sd.giocatore[i].Velocita-t);
			sd.giocatore[i].Durata=max(1,sd.giocatore[i].Durata-t);
			sd.giocatore[i].Contrasto=max(1,sd.giocatore[i].Contrasto-t);
			sd.giocatore[i].Tiro=max(1,sd.giocatore[i].Tiro-t);
		}

#ifdef OLD_VERSION
		memcpy(&s->giocatore[i].Numero,&sd.giocatore[i].Numero,sizeof(char)*10);
		s->giocatore[i].Posizioni=sd.giocatore[i].Posizioni;
#else
		s->giocatore[i].Numero=sd.giocatore[i].Numero;
		s->giocatore[i].Velocita=sd.giocatore[i].Velocita;
		s->giocatore[i].Contrasto=sd.giocatore[i].Contrasto;
		s->giocatore[i].Tiro=sd.giocatore[i].Tiro;
		s->giocatore[i].Durata=sd.giocatore[i].Durata;
		s->giocatore[i].Resistenza=sd.giocatore[i].Resistenza;
		s->giocatore[i].Prontezza=sd.giocatore[i].Prontezza;
		s->giocatore[i].Creativita=sd.giocatore[i].Creativita;
		s->giocatore[i].Tecnica=sd.giocatore[i].Tecnica;
		s->giocatore[i].Posizioni=sd.giocatore[i].Posizioni;
#endif
		MakeName(&s->giocatore[i],&sd.giocatore[i]);


		s->giocatore[i].GNum=i;
		s->giocatore[i].SNum=snum;
		s->giocatore[i].ObjectType=TIPO_GIOCATORE;
		s->giocatore[i].squadra=s;
	}

	TotaleRiserve[snum]=s->NumeroRiserve=sd.NumeroGiocatori-10;

	for(i=10;i<sd.NumeroGiocatori;i++)
	{
		int k=0;

		Riserve[snum][i-10]=sd.giocatore[i];

		while(Riserve[snum][i-10].Cognome[k])
		{
				Riserve[snum][i-10].Cognome[k]=toupper(Riserve[snum][i-10].Cognome[k]);
				k++;
		}
	}

	s->portiere.immagine=portiere;
	s->portiere.SNum=snum;
	s->portiere.ObjectType=TIPO_PORTIERE;
	s->portiere.squadra=s;
	s->portiere.Numero=sd.portiere[0].Numero;
	s->portiere.Parata=sd.portiere[0].Parata;
	s->portiere.Attenzione=sd.portiere[0].Attenzione;
	s->portiere.velocita=sd.portiere[0].Velocita;
	
// Rinforzo i portieri computerizzati, non definitivo

	if(player_type[snum]<0)
	{
		if(s->portiere.Parata<5)
			s->portiere.Parata+=3;
		else if(s->portiere.Parata<6)
			s->portiere.Parata+=2;
		else if(s->portiere.Parata<7)
			s->portiere.Parata+=1;

		if(s->portiere.Attenzione<5)
			s->portiere.Attenzione+=3;
		else if(s->portiere.Attenzione<6)
			s->portiere.Attenzione+=2;
		else if(s->portiere.Attenzione<7)
			s->portiere.Attenzione+=1;

		D(bug("Enhanced goalkeeper %ld, P: %ld (%ld) - A: %ld (%ld)\n",snum,
			s->portiere.Parata,sd.portiere[0].Parata,
			s->portiere.Attenzione,sd.portiere[0].Attenzione));
	}

	MakeName((Giocatore *)&s->portiere,(struct Giocatore_Disk *)&sd.portiere[0]);
	AggiungiLista((Oggetto *) &s->portiere);

	if(role[snum])
	{
		s->attivo=&s->giocatore[role[snum]-1];
		s->attivo->Controlled=TRUE;
		s->gioco_ruolo=TRUE;
	}
	else
	{
		s->attivo=&s->giocatore[8];
		s->attivo->Controlled=TRUE;
	}

	PrintSmall(s->NomeAttivo,s->attivo->Cognome,s->attivo->NameLen);

	return s;
}

// Okkio che sta cosa va fatta solo una volta.

void patch_anim_datas(void)
{
	static int datas_patched=0;
	int i;

	if(datas_patched)
		return;

	for(i=0;i<=GIOCATORE_DISPERAZIONE_OVEST;i++)
		Animation[i].FrameLen*=2;

	for(i=0;i<=ARBITRO_CARTELLINO;i++)
		ArbAnim[i].FrameLen*=2;

	for(i=0;i<=GL_FUORI_0;i++)
		GLAnim[i].FrameLen*=2;

	for(i=0;i<=PORTIERE_PRERIGORE;i++)
		PortAnim[i].FrameLen*=2;
		
	datas_patched=1;
}

Partita *SetupSquadre(void)
{
	int i;

// Questo non servira' piu' quando correggero' i dati...

	patch_anim_datas();

	for(i=0;i<DIMENSIONI_LISTA_OGGETTI;i++)
		object_list[i]=NULL;

	if(!(p=calloc(1,sizeof(struct Partita))))
		return NULL;

	Progress();

	if(!(p->palla.immagine=LoadAnimObject( current_field==7 ? "gfx/ballsnow.obj" : "gfx/ball.obj",Pens)))
	{
		free(p);
		return NULL;
	}

	ScanTactics();

	Progress();

	snum=0;

	if(!(giocatore[0]=LoadAnimObject(shirt[0],Pens)))
	{
		free(p);
		return NULL;
	}

#ifdef DEMOVERSION
	LoadPLogo("gfx/hurricane");
	os_delay(110);
#endif
	Progress();

	cols[0][3]=shirt[0][strlen(shirt[0])-5];

	if(!stricmp(shirt[0],shirt[1]))
	{
		D(bug("Cloning shirt of team 0...\n"));

		cols[1][3]=cols[0][3];

		if(!(giocatore[1]=CopyAnimObj(giocatore[0])))
		{
			free(p);
			return NULL;
		}
	}
	else
	{
		AnimObj *obj;

		D(bug("Loading %s for team 1 shirt...\n",shirt[1]));

		cols[1][3]=shirt[1][strlen(shirt[1])-5];

		if(!(obj=giocatore[1]=LoadAnimObject(shirt[1],Pens)))
		{
			free(p);
			return NULL;
		}


// Occhio, potrebbe causare problemi!
	}

	Progress();
#ifdef DEMOVERSION
	LoadPLogo("gfx/etwlogo");
	os_delay(50);
#endif

	if(!(p->squadra[0]=CreaSquadra(0)))
	{
		free(p);
		return NULL;
	}

	snum=1;

#ifdef DEMOVERSION
	LoadPLogo("gfx/control.gfx");
	os_delay(100);
#endif

	if(!(p->squadra[1]=CreaSquadra(1)))
	{
		LiberaSquadra(p->squadra[0]);
		free(p);
		return NULL;
	}


	if(!(p->squadra[0]->Marker=LoadAnimObject( (arcade ? "gfx/arcade.obj" : "gfx/marker.obj"),Pens)))
	{
		LiberaSquadra(p->squadra[1]);
		LiberaSquadra(p->squadra[0]);
		free(p);
		return NULL;
	}

	Progress();

	if(!(p->squadra[1]->Marker=CloneAnimObj(p->squadra[0]->Marker)))
	{
		LiberaSquadra(p->squadra[0]);
		LiberaSquadra(p->squadra[1]);
		free(p);
		return NULL;
	}
	
// La squadra 1 attacca da destra a sinistra.

	InvertTactic(p->squadra[0]->tattica);

/*  Setto a tutte e due le squadre lo stesso joystick per evitare problemi
    nelle rimesse e nei corner (non ancora implementati per il computer,
    con il quale andrebbe usato -1 come joystick)
 */


// Ricordarsi che i portieri sono inversi rispetto alle squadre come piazzamento!!!

	p->squadra[0]->Joystick=1;
	p->squadra[1]->Joystick=1;

	if(!(p->extras=LoadAnimObject("gfx/extras.obj",Pens)))
	{
		LiberaSquadra(p->squadra[0]);
		LiberaSquadra(p->squadra[1]);
		free(p);
		return NULL;
	}

	Progress();

	if(detail_level&USA_ARBITRO)
	{
		if(!(p->arbitro.immagine=LoadAnimObject( (current_field==7 ? "gfx/arbisnow.obj" : "gfx/arbitro.obj"),Pens))) {
// Non riesco a caricare l'arbitro... Lo disabilito.

			detail_level&=~USA_ARBITRO;
		}
		else {
			if(detail_level&USA_ARBITRO)
				AggiungiLista((Oggetto *)&p->arbitro);

			p->arbitro.ObjectType=TIPO_ARBITRO;
		}

		Progress();
	}

	if(detail_level&USA_GUARDALINEE)
	{
        if (!(guardalinee = calloc(2, sizeof(GuardaLinee)))) {
			detail_level&=~USA_GUARDALINEE;
        }
        else if(!(guardalinee[0].immagine=LoadAnimObject( 
                        (current_field==7 ? NEWGFX_DIR "gls.obj" : 
                         NEWGFX_DIR "gl.obj") ,Pens)))	{
// Non riesco a caricare i guardalinee... Li disabilito.
            free(guardalinee);
			detail_level&=~USA_GUARDALINEE;
		}
		else {
			if ((guardalinee[1].immagine=CloneAnimObj(guardalinee[0].immagine))) {
				int k;

				for(k=0;k<2;k++) {
					AggiungiLista((Oggetto *)&guardalinee[k]);
					guardalinee[k].ObjectType=TIPO_GUARDALINEE;
					guardalinee[k].OnScreen=FALSE;
					guardalinee[k].world_x=CENTROCAMPO_X;

					if(k) {
						guardalinee[k].world_y=(RIMESSA_Y_S-20);
						guardalinee[k].Direzione=6;
					}
					else {
						guardalinee[k].Direzione=2;
						guardalinee[k].world_y=(RIMESSA_Y_N-140);
					}
				}
			}
			else {
				FreeAnimObj(guardalinee[0].immagine);
                free(guardalinee);
				detail_level&=~USA_GUARDALINEE;
			}
		}		
		Progress();
	}

	if(detail_level&USA_RISULTATO) {
		result_width=(strlen(p->squadra[0]->Nome)+strlen(p->squadra[1]->Nome)+9)*VS_CHAR_X;

		if(!(p->result=malloc( result_width * (VS_CHAR_Y+1))))
		{
			D(bug("Unable to allocate result bitmap... disabled.\n"));
			detail_level&=(~USA_RISULTATO);
		}
		else
		{
			rectfill(p->result,0,0,result_width-1, VS_CHAR_Y,Pens[P_VERDE2],result_width);
			MakeResult();
		}
	}

	if(!(InizializzaOggetti(p)))
	{
		if(detail_level&USA_RISULTATO)
			free(p->result);

		if(detail_level&USA_ARBITRO)
			RimuoviLista((Oggetto *)&p->arbitro);

		RimuoviGuardalinee();
		LiberaSquadra(p->squadra[0]);
		LiberaSquadra(p->squadra[1]);
		free(p);
		return NULL;
	}

	CheckPelle();


	p->arbitro.OnScreen=FALSE;
	p->arbitro.cattiveria=strictness;
	p->arbitro.Tick=0;
	pl=&p->palla;

	if(penalties||free_kicks)
	{
		detail_level&=(~USA_RADAR);
		p->arbitro.Comando=FISCHIA_PREPUNIZIONE;
	}
	else
	{
		p->arbitro.world_x=CENTROCAMPO_X;
		p->arbitro.world_y=(340*8);
		p->arbitro.Comando=FISCHIA_RIPRESA;

		pl->settore=KICKOFF;

		starting_team=MyRangeRand(2);

		pl->sq_palla=p->squadra[starting_team];
		p->squadra[starting_team]->Possesso=1;
		p->squadra[starting_team^1]->Possesso=0;
	
		pl->world_x=CENTROCAMPO_X;
		pl->world_y=CENTROCAMPO_Y;
		pl->InGioco=FALSE;
	}
/*
	DisponiSquadra(p->squadra[0],KICKOFF,TRUE);
	DisponiSquadra(p->squadra[1],KICKOFF,FALSE);
*/
	D(bug("GK 0: P:%ld A:%ld\nGK 1: P:%ld A:%ld\n",
		p->squadra[0]->portiere.Parata,	p->squadra[0]->portiere.Attenzione,
		p->squadra[1]->portiere.Parata,	p->squadra[1]->portiere.Attenzione));

	return p;
}

void LiberaPartita(Partita *p)
{
	int i;

	D(bug("Freeing match datas...\n"));


    if (arcade) { // restore the original goalkeeper position
        int j;
            
        for(i=0;i<2;i++)
            for(j=0;j<(SECTORS+SPECIALS);j++)
                portieri[1][i][j].x -= 64;
    }

	RimuoviGuardalinee();

	if(scivolate_modificate) {
		scivolate_modificate=0;

		switch(current_field)
		{
			case 6:
			case 7:
				Animation[GIOCATORE_SCIVOLATA].FrameLen-=2;
				break;
			case 4:
				Animation[GIOCATORE_SCIVOLATA].FrameLen-=3;
				break;
		}
	}

	for(i=0;i<totale_lista;i++)	{
		if(object_list[i])	{
// Arbitro e portieri vengono liberati qui

			if(object_list[i]->ObjectType > TIPO_ARBITRO)	{
                if (object_list[i]->ObjectType == TIPO_GUARDALINEE) {
                    D(bug("Warning, linesmen still in list, skipping!"));
                    continue;
                }
				FreeAnimObj(object_list[i]->immagine);
				free(object_list[i]);
			}

			object_list[i]=NULL;
		}
	}

	totale_lista=0;

	LiberaSquadra(p->squadra[1]);
	LiberaSquadra(p->squadra[0]);

	if(detail_level&USA_ARBITRO)
		FreeAnimObj(p->arbitro.immagine);

	FreeAnimObj(p->palla.immagine);

// Questo controllo prima mancava!!!!

	if(detail_level&USA_RISULTATO && p->result)
		free(p->result);
    
	free(p);

// these should be reinint on game quit!
    has_black[0] = has_black[1] = FALSE;
    NumeroTattiche = 0; swaps = 0;
    teams_swapped = FALSE;
    
	D(bug("Ok.\n"));
}

void MakeResult(void)
{
	char buffer[100];

	if(!p->result)
		return;

	p->result_len=sprintf(buffer," %s %ld-%ld %s",p->squadra[1]->Nome,(LONG)p->squadra[1]->Reti,(LONG)p->squadra[0]->Reti,p->squadra[0]->Nome);

	rectfill(p->result,0,0,result_width-1, VS_CHAR_Y,Pens[P_VERDE2],result_width);

	PrintVerySmall(p->result,buffer,p->result_len,result_width);

	p->result_len++;
}
