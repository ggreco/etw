#include "eat.h"

ULONG *r_controls[MAX_PLAYERS];
// UWORD actual_control[MAX_PLAYERS];

void EndReplay(void);

extern BOOL replay_onscreen;
extern int highsize;

UBYTE ReplaySet=0,smallcounter=0,old_tc,OriginalReplaySet,SetLimit,StartReplaySet;
BOOL replay_mode=FALSE,start_replay=FALSE,was_using_radar=FALSE,was_using_nosound=FALSE,
	was_using_result=FALSE,slow_motion=FALSE,mantieni_distanza=FALSE,
	no_record=FALSE,replay_looped=FALSE,full_replay=FALSE,time_stopped=FALSE;
UWORD real_counter,counter=0,CounterLimit;

extern BOOL draw_r;

struct Pos *arcade_buf;

struct MatchStatus
{
	struct Partita partita;
	struct Squadra squadra[2];
	UWORD ReplayCounter;
	WORD field_x,field_y;
	UBYTE need_release[MAX_PLAYERS];
};

struct MatchStatus *match,InitialStatus;
mytimer StartReplayTime,StopTimeVal;

void StopTime(void)
{
	if(!time_stopped&&!replay_mode)
	{
		StopTimeVal=Timer();
		time_stopped=TRUE;
	}
}

void RestartTime(void)
{
	if(time_stopped)
	{
		mytimer temp;

		temp=Timer();

		EndTime+=(temp-StopTimeVal);
		StartGameTime+=(temp-StopTimeVal);
		time_stopped=FALSE;
	}
}

void StoreReplay(UBYTE Set)
{			

// Arbitro e palla

	match[Set].partita=*p;

	match[Set].squadra[0]=*p->squadra[0];
	match[Set].squadra[1]=*p->squadra[1];


	match[Set].ReplayCounter=counter;
	match[Set].field_x=field_x;
	match[Set].field_y=field_y;

	memcpy(match[Set].need_release,need_release,sizeof(BYTE)*MAX_PLAYERS);

	if(arcade)
	{
		register LONG i;

// Moltiplico Set per il numero di bonus su schermo...

		Set*=MAX_ARCADE_ON_FIELD;

		for(i=0;i<MAX_ARCADE_ON_FIELD;i++)
		{
			arcade_buf[Set+i].x=bonus[i]->world_x;
			arcade_buf[Set+i].y=bonus[i]->world_y;
		}
	}

	if(Set==0&&first_kickoff)
		InitialStatus=match[0];
}

#define SQ_PTR (14+22)

// Mi servono 36 puntatori per ogni squadra, 11 per le immagini, 11 per nomi, 11 per cognomi e tre per cose varie.

void LoadReplay(UBYTE Set)
{
	register LONG i,j;
	APTR *a=NULL;

	i=0;

	RestartTime();

	StartReplayTime=Timer();

	if(!pl->Hide)
		RemAnimObj(pl->immagine);
			
	if(p->penalty_onscreen)
		RemAnimObj(p->extras);

	if(highlight)
	{
// Salvo tutti i puntatori...

		if(!(a=(APTR *)malloc(sizeof(APTR)*(SQ_PTR*2+6))))
		{
			quit_game=TRUE;
			return;
		}
	}

	while(object_list[i])
	{
		if(object_list[i]->OnScreen)
		{
			RemAnimObj(object_list[i]->immagine);
			object_list[i]->OnScreen=FALSE;
		}

		i++;
	}

	for(i=0;i<4;i++)
		pezzi_porte[i]->world_x=-1000;

	for(i=0;i<2;i++)
	{
/*
	Superfluo e' in object_list

		if(p->squadra[i]->portiere.OnScreen)
			RemAnimObj(p->squadra[i]->portiere.immagine);
*/
		
		if(p->squadra[i]->MarkerOnScreen)
			RemAnimObj(p->squadra[i]->Marker);

		if(a)
		{
			a[i*SQ_PTR+34]=p->squadra[i]->portiere.Nome;
			a[i*SQ_PTR+35]=p->squadra[i]->portiere.Cognome;

			a[i*SQ_PTR]=p->squadra[i]->portiere.immagine;
			a[i*SQ_PTR+11]=p->squadra[i]->tattica;
			a[i*SQ_PTR+12]=p->squadra[i]->Marker;
			a[i*SQ_PTR+13]=p->squadra[i]->NomeAttivo;
		}

		for(j=0;j<10;j++)
		{
			if(p->squadra[i]->giocatore[j].OnScreen)
				RemAnimObj(p->squadra[i]->giocatore[j].immagine);

			if(a)
			{
				a[j+i*SQ_PTR+1]=p->squadra[i]->giocatore[j].immagine;
				a[j+i*SQ_PTR+14]=p->squadra[i]->giocatore[j].Nome;
				a[j+i*SQ_PTR+24]=p->squadra[i]->giocatore[j].Cognome;
			}
		}

		*p->squadra[i]=match[Set].squadra[i];

		p->squadra[i]->portiere.OnScreen=FALSE;
		p->squadra[i]->MarkerOnScreen=FALSE;

		if(a)
		{
			char *c;
			ULONG d=(ULONG)p->squadra[i]->tattica,e;

			p->squadra[i]->portiere.immagine->node.mln_Succ=p->squadra[i]->portiere.immagine->node.mln_Pred=NULL;
			p->squadra[i]->portiere.squadra=p->squadra[i];
			p->squadra[i]->portiere.Nome=a[i*SQ_PTR+34];
			p->squadra[i]->portiere.Cognome=a[i*SQ_PTR+35];
			p->squadra[i]->portiere.immagine=a[i*SQ_PTR];

			p->squadra[i]->tattica=a[i*SQ_PTR+11];
			p->squadra[i]->Marker->node.mln_Succ=p->squadra[i]->Marker->node.mln_Pred=NULL;
			p->squadra[i]->Marker=a[i*SQ_PTR+12];
			p->squadra[i]->NomeAttivo=a[i*SQ_PTR+13];

			p->squadra[i]->attivo=&p->squadra[i]->giocatore[(ULONG)p->squadra[i]->attivo];

			c=p->squadra[i]->tattica->Name;

			e=(c[0]<<24)|(c[1]<<16)|(c[2]<<8)|c[4];

			if(e!=d)
			{
				char buffer[40];


				sprintf(buffer,"ETW-TCT:%lc%lc%lc-%lc",((d&0xff000000)>>24),((d&0x00ff0000)>>16),((d&0xff00)>>8),(d&0xff));

				if(!(p->squadra[i]->tattica=LoadTactic(buffer)))
				{
					quit_game=TRUE;
					p->squadra[i]->tattica=a[i*SQ_PTR+11];
				}
				else
					FreeTactic(p->squadra[i]->tattica);
			}
		}


		for(j=0;j<10;j++)
		{
			p->squadra[i]->giocatore[j].OnScreen=FALSE;

			if(a)
			{
				p->squadra[i]->giocatore[j].squadra=p->squadra[i];
				p->squadra[i]->giocatore[j].immagine->node.mln_Succ=p->squadra[i]->giocatore[j].immagine->node.mln_Pred=NULL;
				p->squadra[i]->giocatore[j].immagine=a[j+i*SQ_PTR+1];
				p->squadra[i]->giocatore[j].Nome=a[j+14+i*SQ_PTR];
				p->squadra[i]->giocatore[j].Cognome=a[j+24+i*SQ_PTR];
			}
		}

		PrintSmall(p->squadra[i]->NomeAttivo,p->squadra[i]->attivo->Cognome,p->squadra[i]->attivo->NameLen);
	}

//per debug
	old_tc=p->TabCounter;

	if(a)
	{
		a[SQ_PTR*2]=p->arbitro.immagine;
		a[SQ_PTR*2+1]=pl->immagine;
		a[SQ_PTR*2+2]=p->result;
		a[SQ_PTR*2+3]=p->extras;
		a[SQ_PTR*2+4]=p->squadra[0];
		a[SQ_PTR*2+5]=p->squadra[1];
	}

	*p=match[Set].partita;

	if(a)
	{
		p->arbitro.immagine->node.mln_Succ=p->arbitro.immagine->node.mln_Pred=NULL;
		p->palla.immagine->node.mln_Succ=p->palla.immagine->node.mln_Pred=NULL;
		p->extras->node.mln_Succ=p->extras->node.mln_Pred=NULL;
		p->arbitro.immagine=a[SQ_PTR*2];
		p->palla.immagine=a[SQ_PTR*2+1];
		p->result=a[SQ_PTR*2+2];
		p->extras=a[SQ_PTR*2+3];
		p->squadra[0]=a[SQ_PTR*2+4];
		p->squadra[1]=a[SQ_PTR*2+5];

		p->possesso= p->squadra[(LONG)p->possesso];

		if(p->player_injuried)
		{
			ULONG l=(ULONG)p->player_injuried;
			l--;

			p->player_injuried=( l>=11 ? &(p->squadra[1]->giocatore[l-11]) : &(p->squadra[0]->giocatore[l]) );
		}

		if(pl->sq_palla)
		{
			pl->sq_palla= p->squadra[ ((ULONG)pl->sq_palla)-1];
		}

		if(pl->gioc_palla)
		{
			ULONG l=(ULONG)pl->gioc_palla;

			l--;

			pl->gioc_palla = ( l>=11 ? &(p->squadra[1]->giocatore[l-11]) : &(p->squadra[0]->giocatore[l]) );
		}
	}

// Era questa la causa del bug!!!

	p->arbitro.OnScreen=FALSE;

	if(!pl->Hide)
	{
		AddAnimObj(pl->immagine,10,10,0);
	}

	if(p->penalty_onscreen)
		AddAnimObj(p->extras,0,0,(p->marker_x>640 ? 1 : 0 ));

	real_counter=counter;
	counter=match[Set].ReplayCounter;
	field_x=match[Set].field_x;
	field_y=match[Set].field_y;
	memcpy(need_release,match[Set].need_release,sizeof(BYTE)*MAX_PLAYERS);


	D(bug("Replay: start %ld - end %ld (startset %ld) (TC: %ld->%ld)\n",counter,real_counter,Set,old_tc,p->TabCounter));

	if(arcade)
	{
		Set*=MAX_ARCADE_ON_FIELD;

		for(i=0;i<MAX_ARCADE_ON_FIELD;i++)
		{
			bonus[i]->world_x=arcade_buf[Set+i].x;
			bonus[i]->world_y=arcade_buf[Set+i].y;
		}
	}

	if(highlight)
		free(a);
// rilascio tutti i puntatori...		


}

void StartReplay(void)
{
	if(highlight)
	{
		LoadReplay(0);
	}
	else if(full_replay)
	{
		OriginalReplaySet=ReplaySet+1;
		StoreReplay(OriginalReplaySet);
		LoadReplay(0);
		StartReplaySet=0;
		full_replay=FALSE;
	}
	else if(SetLimit==1)
	{
		OriginalReplaySet=ReplaySet^1;
		StoreReplay(OriginalReplaySet);
		StartReplaySet=ReplaySet;
		LoadReplay(ReplaySet);
	}
	else
	{
		OriginalReplaySet=ReplaySet+1;

		if(OriginalReplaySet>SetLimit)
			OriginalReplaySet=0;

		StoreReplay(OriginalReplaySet);

// Ho allungato il replay di un set

		if(ReplaySet>2)
		{
			StartReplaySet=ReplaySet-3;
			LoadReplay(StartReplaySet);
		}
		else if(replay_looped)
		{
			StartReplaySet=SetLimit-2+ReplaySet;
			LoadReplay(StartReplaySet);
		}
		else
		{
			StartReplaySet=0;
			LoadReplay(0);
		}
	}

	if(!full_replay)
	{
		if(detail_level&USA_RADAR)
		{
			detail_level&=~USA_RADAR;
			was_using_radar=TRUE;
		}
		else was_using_radar=FALSE;

		if(detail_level&USA_RISULTATO)
		{
			detail_level&=~USA_RISULTATO;
			was_using_result=TRUE;
		}
		else was_using_result=FALSE;

		if(!no_sound)
		{
			SetCrowd(REPLAY);
			was_using_nosound=FALSE;
			no_sound=TRUE;
		}
		else was_using_nosound=TRUE;
	}

	start_replay=FALSE;
	replay_mode=TRUE;
	draw_r=TRUE;

	if(highlight)
	{
		counter=0;
		real_counter=highsize;
	}
}

void HandleReplay(void)
{
	if(!allow_replay)
		return;

	if(start_replay)
	{
		StartReplay();
	}
	else if(replay_mode)
	{
		smallcounter++;

		if(smallcounter>15)
		{
			smallcounter=0;

			draw_r= ( (draw_r==TRUE) ? FALSE : TRUE );
		}

		if(MyReadPort0(0)&JPF_BUTTON_RED ||
			MyReadPort1(1)&JPF_BUTTON_RED )
		{
			counter=real_counter;

			EndReplay();
			LoadReplay(OriginalReplaySet);
		}

		if(counter>=real_counter)
		{
			if(highlight)
			{
				if(!was_using_nosound)
					no_sound=FALSE;

				quit_game=TRUE;
				replay_mode=FALSE;
			}
			else
				EndReplay();
		}

	}
	else /* if(!no_record) */
	{
		if((counter&0xff)==0)
		{
			StoreReplay(ReplaySet);

			ReplaySet++;

			if(ReplaySet>SetLimit)
			{
				D(bug("Replay looped!\n"));
				replay_looped=TRUE;
				ReplaySet=0;
			}
		}
	}

	counter++;

	if(counter>CounterLimit)
	{
		D(bug("Replay looped!\n"));
		replay_looped=TRUE;
		counter=0;
	}
}

void EndReplay(void)
{
	mytimer EndReplayTime;

	EndReplayTime=Timer();


	D(bug("Fine replay (%ld) - TabCounter: %ld\n",counter,p->TabCounter));


	if(old_tc!=p->TabCounter)
	{
		D(bug("Errore nel replay, ne ripristino la coerenza...\n"));
		LoadReplay(OriginalReplaySet);
	}

	EndTime+=(EndReplayTime-StartReplayTime);
	StartGameTime+=(EndReplayTime-StartReplayTime);

	if(replay_onscreen)
	{
		replay_onscreen=FALSE;
		RemAnimObj(replay);
	}

	if(was_using_radar)
		detail_level|=USA_RADAR;

	if(was_using_result)
		detail_level|=USA_RISULTATO;
		
	if(!was_using_nosound)
	{
		no_sound=FALSE;

		SetCrowd(FONDO);
	}

	if(slow_motion)
	{
		slow_motion=FALSE;
		MY_CLOCKS_PER_SEC_50>>=2;
	}

	replay_mode=FALSE;

	p->flash_mode=TRUE;
	p->flash_pos=0;
}


void SaveReplay(void)
{
	FILE *f;
	char buffer[16]="t:replay.001";

	if(real_counter<=match[StartReplaySet].ReplayCounter)
		return;

	while(f=fopen(buffer,"r"))
	{
		fclose(f);
		buffer[11]++;

		if(buffer[11]>'9')
		{
			buffer[11]='0';
			buffer[10]++;

			if(buffer[10]>'9')
			{
				buffer[10]='0';
				buffer[9]++;
			}
		}
	}

	if(f=fopen(buffer,"wb"))
	{
		int i;
		FILE *f2;
		APTR a;
		WORD temp;
		long lf;
		struct MatchStatus *m;
		struct Squadra_Disk s;


// la prima parte del file viene letta da "etw

		if(!(f2=fopen(CONFIG_FILE,"rb")))
		{
			fclose(f);
			return;
		}

		fseek(f2,0,SEEK_END);
		lf=ftell(f2);
		fseek(f2,0,SEEK_SET);
		
		if(!(a=malloc(lf)))
		{
			fclose(f);
			fclose(f2);
			return;
		}
// da rivedere la endianness!

		fread(a,lf,1,f2);
		temp=lf;
		SWAP_WORD(temp);
		fwrite(&temp,sizeof(WORD),1,f);
		fwrite(a,lf,1,f);

		free(a);
	
		fclose(f2);

		for(i=0;i<2;i++)
		{
			if(!(f2=OpenTeam(team_name[i])))
			{
				fclose(f);
				return;
			}

			fread(&s,sizeof(struct Squadra_Disk),1,f2);
			fwrite(&s,sizeof(struct Squadra_Disk),1,f);

			fclose(f2);
		}

		temp=real_counter-match[StartReplaySet].ReplayCounter+1;
		SWAP_WORD(temp);
		fwrite(&temp,sizeof(WORD),1,f);

		D(bug("Salvo da %ld a %ld (Set %ld), %ld frames\n",
			match[StartReplaySet].ReplayCounter,real_counter,StartReplaySet,temp));

// Da qui viene letto da "game"

		if(m=malloc(sizeof(struct MatchStatus)))
		{
			SWAP_WORD(swaps);
			fwrite(&swaps,sizeof(WORD),1,f);
			SWAP_WORD(swaps);

			*m=match[StartReplaySet];

			if(m->partita.player_injuried)
				m->partita.player_injuried=(Giocatore *)(1+m->partita.player_injuried->SNum*11+m->partita.player_injuried->GNum);

			m->partita.possesso=(Squadra *)(m->partita.possesso==p->squadra[0] ? 0 : 1);
			
			if(m->partita.palla.sq_palla)
				m->partita.palla.sq_palla=(Squadra *)(m->partita.palla.sq_palla==p->squadra[0] ? 1 : 2);

			for(i=0;i<2;i++)
			{
				char *c=m->squadra[i].tattica->Name;

				m->squadra[i].attivo=(Giocatore *)((int)m->squadra[i].attivo->GNum);

				m->squadra[i].tattica=(struct Tactic *)( (c[0]<<24)|(c[1]<<16)|(c[2]<<8)|c[4]);
			}

			if(m->partita.palla.gioc_palla)
				m->partita.palla.gioc_palla=(Giocatore *)(m->partita.palla.gioc_palla->SNum*11+m->partita.palla.gioc_palla->GNum+1);

			fwrite(m,sizeof(struct MatchStatus),1,f);

			for(i=0;i<MAX_PLAYERS;i++)
				fwrite(&r_controls[i][m->ReplayCounter],sizeof(ULONG),temp,f);

// Qui devo aggiungere il salvataggio delle formazioni.

			PlayBackSound(sound[DOG]);

			free(m);
		}

		fclose(f);
	}
}

BOOL AllocReplayBuffers(void)
{
	int i,size=128,fast=os_avail_mem();

	if(fast<100000)
		size=2;
	else if(fast<1000000)
		size=8;
	else if(fast<2000000)
		size=64;
	else if(fast>4000000)
		size=256;

	if(highlight)
	{
		D(bug("Setto i buffer del replay per l'highlight...\n"));
		size=(highsize/256)+3; //per sicurezza
	}

	CounterLimit=(UWORD)(size*256-1);
	SetLimit=size-1;

	for(i=0;i<MAX_PLAYERS;i++)
	{
		if(!(r_controls[i]=malloc(size*256*sizeof(LONG))))
		{
			D(bug("Non ho memoria per allocare i r_controls buffers!\n"));
			return FALSE;
		}
	}

	if(!(match=malloc(size*sizeof(struct MatchStatus))))
	{
		D(bug("Non ho memoria per allocare i match buffer!\n"));
		return FALSE;
	}

	if(arcade)
	{
		if(!(arcade_buf=malloc(size*sizeof(struct Pos)*MAX_ARCADE_ON_FIELD)))
		{
			D(bug("Non ho memoria per allocare gli arcade buffer!\n"));
			return FALSE;
		}
	}

	D(bug("Replay configurato: SetLimit:%ld CounterLimit:%ld\n",(LONG)SetLimit,(LONG)CounterLimit));
	return TRUE;
}

void FreeReplayBuffers(void)
{
	int i;

	for(i=0;i<MAX_PLAYERS;i++)
		if(r_controls[i])
			free(r_controls[i]);

	if(match)
		free(match);

	if(arcade && arcade_buf)
		free(arcade_buf);
}

void LoadHighlight(void)
{
	FILE *fh;

	if(fh=fopen(HIGH_FILE,"rb"))
	{
		WORD temp;
		int i;

		D(bug("Carico un highlight...\n"));

		fread(&temp,sizeof(WORD),1,fh);
		SWAP_WORD(temp);

		fread(&match[0],sizeof(struct MatchStatus),1,fh);

// Da mettere apposto l'endianness...

		for(i=0;i<MAX_PLAYERS;i++)
			fread(r_controls[i],highsize*sizeof(ULONG),1,fh);

// Idem...

		fclose(fh);

		for(i=0;i<temp;i++)
			SwapTeams();

		start_replay=TRUE;
	}
	else
	{
		D(bug("Unable to open the highlight file!\n"));
		quit_game=TRUE;
	}
}


void RestartGame(void)
{
	StartGameTime=Timer();
	EndTime=StartGameTime+time_length*MY_CLOCKS_PER_SEC;

	match[0]=InitialStatus;

	LoadReplay(0);

	first_kickoff=TRUE;
	first_half=TRUE;
	starting_team=MyRangeRand(2);
	p->squadra[starting_team]->Possesso=1;
	p->squadra[starting_team^1]->Possesso=0;
}
