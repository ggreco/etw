#include "eat.h"

#define NUM_ARCADE_FRAMES 10

WORD arcade_frame_sequence[NUM_ARCADE_FRAMES]={0,1,1,2,2,2,2,2,1,1};

struct Pos bonus_pos[]=
{
	{1100*8,90*8},
	{100*8,180*8},
	{300*8,250*8},
	{500*8,310*8},
	{700*8,380*8},
	{900*8,460*8},
};

void RemoveArcadeEffect(Giocatore *g,UBYTE effect)
{
	switch(effect)
	{
		case ARCADE_SPEED:
			g->Velocita=g->OldStat;
			if(g==pl->gioc_palla)
				g->Velocita-=3;
			break;
		case ARCADE_FREEZE:
			{
				int i;

				g->squadra->ArcadeEffect=NESSUN_COMANDO;

				for(i=0;i<10;i++)
				{
					if(g->squadra->giocatore[i].ArcadeEffect==ARCADE_FREEZE)
					{
						g->squadra->giocatore[i].Comando=NESSUN_COMANDO;
						g->squadra->giocatore[i].ArcadeEffect=NESSUN_COMANDO;
					}
				}
			}
			break;
	}
	g->ArcadeEffect=NESSUN_COMANDO;
}

void AddArcadeEffect(Giocatore *g,UBYTE effect)
{
	if(g->ArcadeEffect)
		RemoveArcadeEffect(g,g->ArcadeEffect);

	switch(effect)
	{
		case ARCADE_GLUE:
			g->ArcadeCounter=400; // 8 secondi di durata;
			g->ArcadeEffect=effect;	
			break;
		case ARCADE_SPEED:
			g->OldStat=g->Velocita;
			if(g==pl->gioc_palla)
				g->OldStat+=3;

			g->Velocita=9;
			g->ArcadeCounter=500; // 10 secondi di durata;
			g->ArcadeEffect=effect;				
			break;
		case ARCADE_CANNON:
			g->ArcadeCounter=500; // 10 secondi di durata;
			g->ArcadeEffect=effect;
			break;
		case ARCADE_FREEZE:
			{
				int i;
				Squadra *s=p->squadra[g->SNum^1];

				s->ArcadeEffect=ARCADE_FREEZE;
				s->ArcadeCounter=400; // 8 secondi di durata.

				for(i=0;i<10;i++)
				{
					if(s->giocatore[i].AnimType!=GIOCATORE_ESPULSO)
					{
						s->giocatore[i].Comando=STAI_FERMO;
						s->giocatore[i].ArcadeEffect=ARCADE_FREEZE;
						s->giocatore[i].ArcadeCounter=300;
						s->giocatore[i].ActualSpeed=0;
					}
				}
			}
			break;
	}
}

void GetArcadeEffect(Giocatore *g,Oggetto *o)
{
	WORD k=(o->AnimType-20)/3;

	switch(k)
	{
		case 0:
		case 1:			
		case 3:
			PlayBackSound(sound[FIRST_ARCADE_SOUND+k]);
		case 2: // Per il cannone niente suono
			AddArcadeEffect(g,k+1);
			break;
		default:
			D(bug("Errore, ArcadeObject inesistente!\n"));
	}

	o->world_x=-1000;
	p->arcade_on_field--;
}

void HandleArcade(void)
{
	register int i;

// Gestisco le animazioni...

	for(i=0;i<MAX_ARCADE_ON_FIELD;i++)
	{
		if(bonus[i]->OnScreen)
		{
/*
			bonus[i]->FrameLen--;

			if(bonus[i]->FrameLen<=0)
			{
				bonus[i]->FrameLen=3;
*/
				bonus[i]->AnimFrame++;

				if(bonus[i]->AnimFrame>=NUM_ARCADE_FRAMES)
				{
					bonus[i]->AnimFrame=0;
				}

				ChangeAnimObj(bonus[i]->immagine,bonus[i]->AnimType+arcade_frame_sequence[bonus[i]->AnimFrame]);

/*			}
*/
		}
	}

// Gestisco il repop...

	p->arcade_counter++;

	if(p->arcade_counter>ARCADE_REPOP_TIME)
	{
		p->arcade_counter=0;

		if(p->arcade_on_field<MAX_ARCADE_ON_FIELD)
		{
			for(;;)
			{
				i=GetTable();

				if(i<MAX_ARCADE_ON_FIELD && bonus[i]->world_x<0)
				{
					PlayBackSound(sound[AS_REPOP]);
					p->arcade_on_field++;

					bonus[i]->world_x=bonus_pos[GetTable()].x;
					bonus[i]->world_y=bonus_pos[GetTable()].y;
					break;
				}
			}
		}
	}
}
