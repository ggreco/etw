#include "eat.h"
#include "files.h"

ULONG *r_controls[MAX_PLAYERS];
// UWORD actual_control[MAX_PLAYERS];

void EndReplay(void);

extern int highsize;

UBYTE ReplaySet=0,smallcounter=0,old_tc,OriginalReplaySet,SetLimit,StartReplaySet;
BOOL replay_mode=FALSE,start_replay=FALSE,was_using_radar=FALSE,was_using_nosound=FALSE,
	was_using_result=FALSE,slow_motion=FALSE,mantieni_distanza=FALSE,
	no_record=FALSE,replay_looped=FALSE,full_replay=FALSE,time_stopped=FALSE;
UWORD real_counter,counter=0,CounterLimit;

extern BOOL draw_r;
extern void WriteGameConfig(FILE *);
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
int matchstatus_size = sizeof(struct MatchStatus);

static void ReadATeam(FILE *f, Squadra *s)
{
    int i;
    
    READ_DATA(s->TPossesso, f);
    READ_DATA(s->Reti, f);
    READ_DATA(s->Falli, f);
    READ_DATA(s->Ammonizioni, f);
    READ_DATA(s->Espulsioni, f);
    READ_DATA(s->Tiri, f);
    READ_DATA(s->Rigori, f);
    READ_DATA(s->Corner, f);
    READ_DATA(s->Sostituzioni, f);
    READ_DATA(s->Possesso, f);
    READ_DATA(s->Schema, f);
    READ_DATA(s->Joystick, f);
    
    READ_LONG(s->tattica, f);
    READ_LONG(s->TempoPossesso, f);
    READ_LONG(s->attivo, f);

// goalkeeper
    READ_WORD(s->portiere.world_x, f); 
    READ_WORD(s->portiere.world_y, f); 
    READ_BOOL(s->portiere.OnScreen, f); 
    READ_WORD(s->portiere.AnimType, f); 
    READ_WORD(s->portiere.AnimFrame, f); 
    READ_DATA(s->portiere.ObjectType, f); 
    READ_DATA(s->portiere.Direzione, f); 
    READ_DATA(s->portiere.ActualSpeed, f); 
    READ_DATA(s->portiere.FrameLen, f); 
    READ_WORD(s->portiere.Tick, f); 
    READ_LONG(s->portiere.squadra, f); 
    READ_DATA(s->portiere.NameLen, f); 
    READ_DATA(s->portiere.SNum, f); 
    READ_BOOL(s->portiere.Ammonito, f); 
    READ_BOOL(s->portiere.Special, f); 
    READ_BOOL(s->portiere.FirePressed, f); 
    READ_WORD(s->portiere.SpecialData, f); 
    READ_WORD(s->portiere.TimePress, f); 
    READ_DATA(s->portiere.Numero, f); 
    READ_DATA(s->portiere.velocita, f); 
    READ_DATA(s->portiere.Parata, f); 
    READ_DATA(s->portiere.Attenzione, f); 

// field players
    for (i = 0; i < 10; i++) {
        Giocatore *g = &s->giocatore[i];

        READ_WORD(g->world_x, f);
        READ_WORD(g->world_y, f);
        READ_BOOL(g->OnScreen, f);
        READ_WORD(g->AnimType, f);
        READ_WORD(g->AnimFrame, f);
        READ_DATA(g->ObjectType , f);
        READ_DATA(g->Direzione , f);
        READ_DATA(g->ActualSpeed , f);
        READ_DATA(g->FrameLen , f);
        READ_WORD(g->Tick, f);
        READ_LONG(g->squadra, f);
        READ_DATA(g->NameLen , f);
        READ_DATA(g->GNum , f);
        READ_BOOL(g->Ammonito, f);
        READ_BOOL(g->Special, f);
        READ_BOOL(g->FirePressed, f);
        READ_WORD(g->SpecialData, f);
        READ_WORD(g->TimePress, f);
        READ_DATA(g->Numero , f);
        READ_DATA(g->Velocita , f);
        READ_DATA(g->Contrasto , f);
        READ_DATA(g->Tiro , f);
        READ_DATA(g->Durata , f);
        READ_DATA(g->Resistenza , f);
        READ_DATA(g->Prontezza , f);
        READ_DATA(g->settore , f);
        READ_DATA(g->Creativita , f);
        READ_DATA(g->Tecnica , f);
        READ_DATA(g->Posizioni , f);
        READ_DATA(g->SNum , f);
        READ_BOOL(g->Controlled, f);
        READ_BOOL(g->Marker, f);
        READ_WORD(g->WaitForControl, f);
        READ_DATA(g->Comando , f);
        READ_DATA(g->Argomento , f);
        READ_DATA(g->CA , f);
        READ_DATA(g->OldStat , f);
        READ_DATA(g->ArcadeEffect, f);
        READ_WORD(g->ArcadeCounter, f);
    }
    
// other generic team datas
    READ_WORD(s->Marker_X, f);
    READ_WORD(s->Marker_Y, f);
    READ_WORD(s->MarkerFrame, f);

    READ_BOOL(s->MarkerOnScreen, f);
    READ_BOOL(s->gioco_ruolo, f);
    READ_DATA(s->Nome, f);
    READ_BOOL(s->MarkerRed, f);
    READ_DATA(s->ArcadeEffect, f);
    READ_DATA(s->NumeroRiserve, f);
    READ_WORD(s->ArcadeCounter, f);
}

void ReadMatch(FILE *f, struct MatchStatus *m)
{
    int i;
// write ball related data 
    READ_LONG(m->partita.palla.gioc_palla, f);
    READ_LONG(m->partita.palla.sq_palla, f);
    READ_WORD(m->partita.palla.world_x, f);
    READ_WORD(m->partita.palla.world_y, f);
    READ_WORD(m->partita.palla.delta_x, f);
    READ_WORD(m->partita.palla.delta_y, f);
    READ_BOOL(m->partita.palla.InGioco, f);
    READ_BOOL(m->partita.palla.Hide, f);
    READ_DATA(m->partita.palla.ToTheTop, f);
    READ_DATA(m->partita.palla.ThisQuota, f);
    READ_DATA(m->partita.palla.Stage, f);
    READ_DATA(m->partita.palla.TipoTiro, f);
    READ_DATA(m->partita.palla.ActualFrame, f);
    READ_DATA(m->partita.palla.MaxQuota, f);
    READ_DATA(m->partita.palla.SpeedUp, f);
    READ_DATA(m->partita.palla.Rimbalzi, f);
    READ_DATA(m->partita.palla.velocita, f);
    READ_DATA(m->partita.palla.Direzione, f);
    READ_DATA(m->partita.palla.quota, f);
    READ_DATA(m->partita.palla.settore, f);

// write refree related datas
    READ_WORD(m->partita.arbitro.world_x, f);    
    READ_WORD(m->partita.arbitro.world_y, f);    
    READ_WORD(m->partita.arbitro.AnimType, f);    
    READ_WORD(m->partita.arbitro.AnimFrame, f);    
    READ_WORD(m->partita.arbitro.Tick, f);    
    READ_WORD(m->partita.arbitro.Argomento, f);    
    READ_BOOL(m->partita.arbitro.OnScreen, f);
    READ_BOOL(m->partita.arbitro.Special, f);
    READ_DATA(m->partita.arbitro.ObjectType, f);
    READ_DATA(m->partita.arbitro.Direzione, f);
    READ_DATA(m->partita.arbitro.ActualSpeed, f);
    READ_DATA(m->partita.arbitro.FrameLen, f);
    READ_DATA(m->partita.arbitro.NameLen, f);
    READ_DATA(m->partita.arbitro.Comando, f);
    READ_DATA(m->partita.arbitro.velocita, f);
    READ_DATA(m->partita.arbitro.abilita, f);
    READ_DATA(m->partita.arbitro.recupero, f);
    READ_DATA(m->partita.arbitro.cattiveria, f);
    
// write generic datas
    READ_LONG(m->partita.TempoPassato, f);
    READ_LONG(m->partita.show_panel, f);
    READ_LONG(m->partita.show_time, f);
    READ_LONG(m->partita.possesso, f);
    READ_LONG(m->partita.player_injuried, f);
    READ_WORD(m->partita.check_sector, f);

    for (i = 0; i < SHOT_LENGTH; i++)
        READ_WORD(m->partita.shotheight[i], f);
    
    READ_WORD(m->partita.flash_pos, f);
    READ_BOOL(m->partita.goal, f);
    READ_BOOL(m->partita.sopra_rete, f);
    READ_BOOL(m->partita.flash_mode, f);
    READ_BOOL(m->partita.doing_shot, f);
    READ_BOOL(m->partita.mantieni_distanza, f);
    READ_BOOL(m->partita.penalty_onscreen, f);

    READ_WORD(m->partita.arcade_counter, f);
    READ_WORD(m->partita.marker_x, f);
    READ_WORD(m->partita.marker_y, f);
    READ_WORD(m->partita.penalty_counter, f);
    READ_WORD(m->partita.adder, f);

    READ_DATA(m->partita.TabCounter, f);
    READ_DATA(m->partita.result_len, f);
    READ_DATA(m->partita.last_touch, f);
    READ_DATA(m->partita.arcade_on_field, f);
    READ_DATA(m->partita.RiservaAttuale, f);
    
    ReadATeam(f,&m->squadra[0]);
    ReadATeam(f,&m->squadra[1]);
    
    READ_WORD(m->ReplayCounter, f);
    
    READ_WORD(m->field_x, f);
    READ_WORD(m->field_y, f);

    READ_DATA(m->need_release, f);
}

static void WriteATeam(FILE *f, Squadra *s)
{
    int i;
    
    WRITE_DATA(s->TPossesso, f);
    WRITE_DATA(s->Reti, f);
    WRITE_DATA(s->Falli, f);
    WRITE_DATA(s->Ammonizioni, f);
    WRITE_DATA(s->Espulsioni, f);
    WRITE_DATA(s->Tiri, f);
    WRITE_DATA(s->Rigori, f);
    WRITE_DATA(s->Corner, f);
    WRITE_DATA(s->Sostituzioni, f);
    WRITE_DATA(s->Possesso, f);
    WRITE_DATA(s->Schema, f);
    WRITE_DATA(s->Joystick, f);
    
    WRITE_LONG(((LONG)s->tattica), f);
    WRITE_LONG(s->TempoPossesso, f);
    WRITE_LONG(((LONG)s->attivo), f);

// goalkeeper
    WRITE_WORD(s->portiere.world_x, f); 
    WRITE_WORD(s->portiere.world_y, f); 
    WRITE_BOOL(s->portiere.OnScreen, f); 
    WRITE_WORD(s->portiere.AnimType, f); 
    WRITE_WORD(s->portiere.AnimFrame, f); 
    WRITE_DATA(s->portiere.ObjectType, f); 
    WRITE_DATA(s->portiere.Direzione, f); 
    WRITE_DATA(s->portiere.ActualSpeed, f); 
    WRITE_DATA(s->portiere.FrameLen, f); 
    WRITE_WORD(s->portiere.Tick, f); 
    WRITE_LONG(((LONG)s->portiere.squadra), f); 
    WRITE_DATA(s->portiere.NameLen, f); 
    WRITE_DATA(s->portiere.SNum, f); 
    WRITE_BOOL(s->portiere.Ammonito, f); 
    WRITE_BOOL(s->portiere.Special, f); 
    WRITE_BOOL(s->portiere.FirePressed, f); 
    WRITE_WORD(s->portiere.SpecialData, f); 
    WRITE_WORD(s->portiere.TimePress, f); 
    WRITE_DATA(s->portiere.Numero, f); 
    WRITE_DATA(s->portiere.velocita, f); 
    WRITE_DATA(s->portiere.Parata, f); 
    WRITE_DATA(s->portiere.Attenzione, f); 

// field players
    for (i = 0; i < 10; i++) {
        Giocatore *g = &s->giocatore[i];

        WRITE_WORD(g->world_x, f);
        WRITE_WORD(g->world_y, f);
        WRITE_BOOL(g->OnScreen, f);
        WRITE_WORD(g->AnimType, f);
        WRITE_WORD(g->AnimFrame, f);
        WRITE_DATA(g->ObjectType , f);
        WRITE_DATA(g->Direzione , f);
        WRITE_DATA(g->ActualSpeed , f);
        WRITE_DATA(g->FrameLen , f);
        WRITE_WORD(g->Tick, f);
        WRITE_LONG(((LONG)g->squadra), f);
        WRITE_DATA(g->NameLen , f);
        WRITE_DATA(g->GNum , f);
        WRITE_BOOL(g->Ammonito, f);
        WRITE_BOOL(g->Special, f);
        WRITE_BOOL(g->FirePressed, f);
        WRITE_WORD(g->SpecialData, f);
        WRITE_WORD(g->TimePress, f);
        WRITE_DATA(g->Numero , f);
        WRITE_DATA(g->Velocita , f);
        WRITE_DATA(g->Contrasto , f);
        WRITE_DATA(g->Tiro , f);
        WRITE_DATA(g->Durata , f);
        WRITE_DATA(g->Resistenza , f);
        WRITE_DATA(g->Prontezza , f);
        WRITE_DATA(g->settore , f);
        WRITE_DATA(g->Creativita , f);
        WRITE_DATA(g->Tecnica , f);
        WRITE_DATA(g->Posizioni , f);
        WRITE_DATA(g->SNum , f);
        WRITE_BOOL(g->Controlled, f);
        WRITE_BOOL(g->Marker, f);
        WRITE_WORD(g->WaitForControl, f);
        WRITE_DATA(g->Comando , f);
        WRITE_DATA(g->Argomento , f);
        WRITE_DATA(g->CA , f);
        WRITE_DATA(g->OldStat , f);
        WRITE_DATA(g->ArcadeEffect, f);
        WRITE_WORD(g->ArcadeCounter, f);
    }
    
// other generic team datas
    WRITE_WORD(s->Marker_X, f);
    WRITE_WORD(s->Marker_Y, f);
    WRITE_WORD(s->MarkerFrame, f);

    WRITE_BOOL(s->MarkerOnScreen, f);
    WRITE_BOOL(s->gioco_ruolo, f);
    WRITE_DATA(s->Nome, f);
    WRITE_BOOL(s->MarkerRed, f);
    WRITE_DATA(s->ArcadeEffect, f);
    WRITE_DATA(s->NumeroRiserve, f);
    WRITE_WORD(s->ArcadeCounter, f);
}

void WriteMatch(FILE *f, struct MatchStatus *m)
{
    int i;
// write ball related data 
    WRITE_LONG(((LONG)m->partita.palla.gioc_palla), f);
    WRITE_LONG(((LONG)m->partita.palla.sq_palla), f);
    WRITE_WORD(m->partita.palla.world_x, f);
    WRITE_WORD(m->partita.palla.world_y, f);
    WRITE_WORD(m->partita.palla.delta_x, f);
    WRITE_WORD(m->partita.palla.delta_y, f);
    WRITE_BOOL(m->partita.palla.InGioco, f);
    WRITE_BOOL(m->partita.palla.Hide, f);
    WRITE_DATA(m->partita.palla.ToTheTop, f);
    WRITE_DATA(m->partita.palla.ThisQuota, f);
    WRITE_DATA(m->partita.palla.Stage, f);
    WRITE_DATA(m->partita.palla.TipoTiro, f);
    WRITE_DATA(m->partita.palla.ActualFrame, f);
    WRITE_DATA(m->partita.palla.MaxQuota, f);
    WRITE_DATA(m->partita.palla.SpeedUp, f);
    WRITE_DATA(m->partita.palla.Rimbalzi, f);
    WRITE_DATA(m->partita.palla.velocita, f);
    WRITE_DATA(m->partita.palla.Direzione, f);
    WRITE_DATA(m->partita.palla.quota, f);
    WRITE_DATA(m->partita.palla.settore, f);

// write refree related datas
    WRITE_WORD(m->partita.arbitro.world_x, f);    
    WRITE_WORD(m->partita.arbitro.world_y, f);    
    WRITE_WORD(m->partita.arbitro.AnimType, f);    
    WRITE_WORD(m->partita.arbitro.AnimFrame, f);    
    WRITE_WORD(m->partita.arbitro.Tick, f);    
    WRITE_WORD(m->partita.arbitro.Argomento, f);    
    WRITE_BOOL(m->partita.arbitro.OnScreen, f);
    WRITE_BOOL(m->partita.arbitro.Special, f);
    WRITE_DATA(m->partita.arbitro.ObjectType, f);
    WRITE_DATA(m->partita.arbitro.Direzione, f);
    WRITE_DATA(m->partita.arbitro.ActualSpeed, f);
    WRITE_DATA(m->partita.arbitro.FrameLen, f);
    WRITE_DATA(m->partita.arbitro.NameLen, f);
    WRITE_DATA(m->partita.arbitro.Comando, f);
    WRITE_DATA(m->partita.arbitro.velocita, f);
    WRITE_DATA(m->partita.arbitro.abilita, f);
    WRITE_DATA(m->partita.arbitro.recupero, f);
    WRITE_DATA(m->partita.arbitro.cattiveria, f);
    
// write generic datas
    WRITE_LONG(m->partita.TempoPassato, f);
    WRITE_LONG(m->partita.show_panel, f);
    WRITE_LONG(m->partita.show_time, f);
    WRITE_LONG(((LONG)m->partita.possesso), f);
    WRITE_LONG(((LONG)m->partita.player_injuried), f);
    WRITE_WORD(m->partita.check_sector, f);

    for (i = 0; i < SHOT_LENGTH; i++)
        WRITE_WORD(m->partita.shotheight[i], f);
    
    WRITE_WORD(m->partita.flash_pos, f);
    WRITE_BOOL(m->partita.goal, f);
    WRITE_BOOL(m->partita.sopra_rete, f);
    WRITE_BOOL(m->partita.flash_mode, f);
    WRITE_BOOL(m->partita.doing_shot, f);
    WRITE_BOOL(m->partita.mantieni_distanza, f);
    WRITE_BOOL(m->partita.penalty_onscreen, f);

    WRITE_WORD(m->partita.arcade_counter, f);
    WRITE_WORD(m->partita.marker_x, f);
    WRITE_WORD(m->partita.marker_y, f);
    WRITE_WORD(m->partita.penalty_counter, f);
    WRITE_WORD(m->partita.adder, f);

    WRITE_DATA(m->partita.TabCounter, f);
    WRITE_DATA(m->partita.result_len, f);
    WRITE_DATA(m->partita.last_touch, f);
    WRITE_DATA(m->partita.arcade_on_field, f);
    WRITE_DATA(m->partita.RiservaAttuale, f);
    
    WriteATeam(f, &m->squadra[0]);
    WriteATeam(f, &m->squadra[1]);

    WRITE_WORD(m->ReplayCounter, f);
    WRITE_WORD(m->field_x, f);
    WRITE_WORD(m->field_y, f);

    WRITE_DATA(m->need_release, f);
}

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
    register LONG i;

// Arbitro e palla

	match[Set].partita=*p;

	match[Set].squadra[0]=*p->squadra[0];
	match[Set].squadra[1]=*p->squadra[1];


	match[Set].ReplayCounter=counter;
	match[Set].field_x=field_x;
	match[Set].field_y=field_y;

    for (i = 0; i < MAX_PLAYERS; i++)
    	match[Set].need_release[i] = need_release[i];

	if(arcade) {

// Moltiplico Set per il numero di bonus su schermo...

		Set*=MAX_ARCADE_ON_FIELD;

		for(i=0;i<MAX_ARCADE_ON_FIELD;i++) {
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
 
    p->penalty_onscreen = FALSE;

	if (highlight) {
// Salvo tutti i puntatori...

		if(!(a=(APTR *)malloc(sizeof(APTR)*(SQ_PTR*2+6))))
		{
			quit_game=TRUE;
			return;
		}
	}

	while (object_list[i]) {
		if (object_list[i]->OnScreen) {
			RemAnimObj(object_list[i]->immagine);
    	    object_list[i]->OnScreen=FALSE;
		}

		i++;
	}

	for(i=0;i<4;i++)
		pezzi_porte[i]->world_x=-1000;

	for(i=0;i<2;i++) {

        if (p->squadra[i]->MarkerOnScreen)
            RemAnimObj(p->squadra[i]->Marker);

        if (a) {
            a[i*SQ_PTR]    = p->squadra[i]->portiere.immagine;
            a[i*SQ_PTR+11] = p->squadra[i]->tattica;
            a[i*SQ_PTR+12] = p->squadra[i]->Marker;
            a[i*SQ_PTR+13] = p->squadra[i]->NomeAttivo;
            a[i*SQ_PTR+34] = p->squadra[i]->portiere.Nome;
            a[i*SQ_PTR+35] = p->squadra[i]->portiere.Cognome;
        }
        
        for (j=0; j<10; j++) {
            if (p->squadra[i]->giocatore[j].OnScreen)
                RemAnimObj(p->squadra[i]->giocatore[j].immagine);

            if (a) {
                a[j+i*SQ_PTR+1]  = p->squadra[i]->giocatore[j].immagine;
                a[j+i*SQ_PTR+14] = p->squadra[i]->giocatore[j].Nome;
                a[j+i*SQ_PTR+24] = p->squadra[i]->giocatore[j].Cognome;
            }
        }

        // copy over the playing team the saved team
        *p->squadra[i] = match[Set].squadra[i];
		p->squadra[i]->portiere.OnScreen=FALSE;
		p->squadra[i]->MarkerOnScreen=FALSE;

        if (a) {
            char *c;
            ULONG d = (ULONG)p->squadra[i]->tattica, e;
// first fix the pointers then the reference INSIDE them!
            p->squadra[i]->portiere.immagine = a[i*SQ_PTR];
            p->squadra[i]->Marker     = a[i*SQ_PTR+12];

            p->squadra[i]->portiere.immagine->node.mln_Succ = 
                p->squadra[i]->portiere.immagine->node.mln_Pred= NULL;

            p->squadra[i]->portiere.squadra  = p->squadra[i];
            p->squadra[i]->portiere.Nome     = a[i*SQ_PTR+34];
            p->squadra[i]->portiere.Cognome  = a[i*SQ_PTR+35];

            p->squadra[i]->tattica = a[i*SQ_PTR+11];
            p->squadra[i]->Marker->node.mln_Succ = 
                p->squadra[i]->Marker->node.mln_Pred=NULL;


            p->squadra[i]->NomeAttivo = a[i*SQ_PTR+13];

            p->squadra[i]->attivo = &p->squadra[i]->giocatore[(ULONG)p->squadra[i]->attivo];

            c = p->squadra[i]->tattica->Name;

            e=(c[0]<<24)|(c[1]<<16)|(c[2]<<8)|c[4];

            if (e != d) {
                char buffer[40];

                sprintf(buffer, "tct/%c%c%c-%c",
                        (char) ((d&0xff000000)>>24),
                        (char) ((d&0x00ff0000)>>16),
                        (char) ((d&0xff00)>>8),
                        (char) (d&0xff) );

                if(!(p->squadra[i]->tattica=LoadTactic(buffer))) {
                    quit_game=TRUE;
                    p->squadra[i]->tattica=a[i*SQ_PTR+11];
                }
                else
                    FreeTactic(p->squadra[i]->tattica);
            }

        }

        for(j=0;j<10;j++) {
            p->squadra[i]->giocatore[j].OnScreen = FALSE;

            if (a) {
                p->squadra[i]->giocatore[j].squadra  = p->squadra[i];
                p->squadra[i]->giocatore[j].immagine = a[j+i*SQ_PTR+1];
                p->squadra[i]->giocatore[j].Nome     = a[j+14+i*SQ_PTR];
                p->squadra[i]->giocatore[j].Cognome  = a[j+24+i*SQ_PTR];

                p->squadra[i]->giocatore[j].immagine->node.mln_Succ = 
                   p->squadra[i]->giocatore[j].immagine->node.mln_Pred = NULL;
            }
        }
        
        PrintSmall(p->squadra[i]->NomeAttivo,
                   p->squadra[i]->attivo->Cognome,
                   p->squadra[i]->attivo->NameLen);
    }

//per debug
	old_tc=p->TabCounter;

    if (a) {
		a[SQ_PTR*2]   = p->arbitro.immagine;
		a[SQ_PTR*2+1] = pl->immagine;
		a[SQ_PTR*2+2] = p->result;
		a[SQ_PTR*2+3] = p->extras;
		a[SQ_PTR*2+4] = p->squadra[0];
		a[SQ_PTR*2+5] = p->squadra[1];
    }
    
	*p=match[Set].partita;

    if (a) {

        p->arbitro.immagine = a[SQ_PTR*2];
        p->palla.immagine   = a[SQ_PTR*2+1];
        p->result           = a[SQ_PTR*2+2];
        p->extras           = a[SQ_PTR*2+3];
        p->squadra[0]       = a[SQ_PTR*2+4];
        p->squadra[1]       = a[SQ_PTR*2+5];
    
        p->arbitro.immagine->node.mln_Succ=p->arbitro.immagine->node.mln_Pred=NULL;
        p->palla.immagine->node.mln_Succ=p->palla.immagine->node.mln_Pred=NULL;
        p->extras->node.mln_Succ=p->extras->node.mln_Pred=NULL;

        p->possesso= p->squadra[(LONG)p->possesso];

		if(p->player_injuried)	{
			ULONG l=(ULONG)p->player_injuried;
			l--;

			p->player_injuried=( l>=11 ? &(p->squadra[1]->giocatore[l-11]) : &(p->squadra[0]->giocatore[l]) );
		}

		if(pl->sq_palla) {
			pl->sq_palla= p->squadra[ ((ULONG)pl->sq_palla)-1];
		}

		if(pl->gioc_palla) 	{
			ULONG l=(ULONG)pl->gioc_palla;

			l--;

			pl->gioc_palla = ( l>=11 ? &(p->squadra[1]->giocatore[l-11]) : &(p->squadra[0]->giocatore[l]) );
		}
	}

// Era questa la causa del bug!!!

	p->arbitro.OnScreen=FALSE;

	if(!pl->Hide) {
		AddAnimObj(pl->immagine,10,10,0);
	}

	if(p->penalty_onscreen)
		AddAnimObj(p->extras,0,0,(p->marker_x>640 ? 1 : 0 ));

	real_counter=counter;
	counter = match[Set].ReplayCounter;
	field_x = match[Set].field_x;
	field_y = match[Set].field_y;
   

    for (i = 0; i < MAX_PLAYERS; i++)
    	need_release[i] = match[Set].need_release[i];

	D(bug("Replay: start %ld - end %ld (startset %ld) (TC: %ld->%ld)\n",counter,real_counter,Set,old_tc,p->TabCounter));

	if(arcade) {
		Set*=MAX_ARCADE_ON_FIELD;

		for(i=0;i<MAX_ARCADE_ON_FIELD;i++)
		{
			bonus[i]->world_x=arcade_buf[Set+i].x;
			bonus[i]->world_y=arcade_buf[Set+i].y;
		}
	}

// free the pointers backup
	if(highlight)
		free(a);
}

void StartReplay(void)
{
	if(highlight) {
		LoadReplay(0);
	}
	else if(full_replay) {
		OriginalReplaySet=ReplaySet+1;
		StoreReplay(OriginalReplaySet);
		LoadReplay(0);
		StartReplaySet=0;
		full_replay=FALSE;
	}
	else if(SetLimit==1) {
		OriginalReplaySet=ReplaySet^1;
		StoreReplay(OriginalReplaySet);
		StartReplaySet=ReplaySet;
		LoadReplay(ReplaySet);
	}
	else {
		OriginalReplaySet=ReplaySet+1;

		if(OriginalReplaySet>SetLimit)
			OriginalReplaySet=0;

		StoreReplay(OriginalReplaySet);

// Ho allungato il replay di un set

		if(ReplaySet>2) {
			StartReplaySet=ReplaySet-3;
			LoadReplay(StartReplaySet);
		}
		else if(replay_looped) {
			StartReplaySet=SetLimit-2+ReplaySet;
			LoadReplay(StartReplaySet);
		}
		else {
			StartReplaySet=0;
			LoadReplay(0);
		}
	}

	if(!full_replay) {
		if(detail_level&USA_RADAR) {
			detail_level&=~USA_RADAR;
			was_using_radar=TRUE;
		}
		else was_using_radar=FALSE;

		if(detail_level&USA_RISULTATO) {
			detail_level&=~USA_RISULTATO;
			was_using_result=TRUE;
		}
		else was_using_result=FALSE;

		if(!no_sound) {
			SetCrowd(REPLAY);
			was_using_nosound=FALSE;
			no_sound=TRUE;
		}
		else was_using_nosound=TRUE;
	}

	start_replay=FALSE;
	replay_mode=TRUE;
	draw_r=TRUE;

	if(highlight) {
		counter=0;
		real_counter=highsize;
	}
}

void HandleReplay(void)
{
	if (!allow_replay)
		return;

	if (start_replay) {
		StartReplay();
	}
	else if (replay_mode) {
		smallcounter++;

		if (smallcounter > 15) {
			smallcounter=0;

			draw_r= ( (draw_r==TRUE) ? FALSE : TRUE );
		}

		if (MyReadPort0(0)&JPF_BUTTON_RED ||
			MyReadPort1(1)&JPF_BUTTON_RED ) {
			counter=real_counter;

            if (!highlight) {
	    		EndReplay();
    			LoadReplay(OriginalReplaySet);
            }
		}

		if (counter>=real_counter) {
            if (highlight) {
                D(bug("Highlight finished, quitting."));

                if(slow_motion)	{
                    slow_motion=FALSE;
                    MY_CLOCKS_PER_SEC_50>>=2;
                }

                if(!was_using_nosound)
                    no_sound=FALSE;

                quit_game=TRUE;
                replay_mode=FALSE;
            }
			else
				EndReplay();
		}

	}
    else if((counter&0xff)==0) {
        StoreReplay(ReplaySet);

        ReplaySet++;

        if (ReplaySet>SetLimit) {
            D(bug("Replay looped!\n"));
            replay_looped=TRUE;
            ReplaySet=0;
        }
    }

	counter++;

	if (counter > CounterLimit) {
		D(bug("Replay looped!\n"));
		replay_looped=TRUE;
		counter=0;
	}
}

void EndReplay(void)
{
	mytimer EndReplayTime;

	EndReplayTime=Timer();


	D(bug("Replay end (%ld) - TabCounter: %ld\n",counter,p->TabCounter));


	if(old_tc!=p->TabCounter) {
		D(bug("*** Error or interrupted replay sequence, forcing coherency...\n"));
		LoadReplay(OriginalReplaySet);
	}

	EndTime+=(EndReplayTime-StartReplayTime);
	StartGameTime+=(EndReplayTime-StartReplayTime);

	if(InAnimList(replay))	{
		RemAnimObj(replay);
	}

	if(was_using_radar)
		detail_level|=USA_RADAR;

	if(was_using_result)
		detail_level|=USA_RISULTATO;
		
	if(!was_using_nosound) {
		no_sound=FALSE;

		SetCrowd(FONDO);
	}

	if(slow_motion)	{
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
	char buffer[16]="t/replay.001";
    extern struct Squadra_Disk leftteam_dk,rightteam_dk;

	if(real_counter<=match[StartReplaySet].ReplayCounter)
		return;

	while ((f=fopen(buffer,"r"))) {
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

	if ((f=fopen(buffer,"wb"))) {
		int i, j;
		WORD highsize;
		struct MatchStatus *m;

        WriteGameConfig(f);        

		WriteTeam(f, &leftteam_dk);
		WriteTeam(f, &rightteam_dk);

		highsize = real_counter-match[StartReplaySet].ReplayCounter+1;
		WRITE_WORD(highsize, f);

		D(bug("Saving from %ld to %ld (Set %ld), %ld frames\n",
			match[StartReplaySet].ReplayCounter, real_counter,
            StartReplaySet, highsize));

// Da qui viene letto da "game"

		if ((m = malloc(sizeof(struct MatchStatus)))) {
			WRITE_WORD(swaps, f);

			*m=match[StartReplaySet];

			if(m->partita.player_injuried)
				m->partita.player_injuried=(Giocatore *)(1+m->partita.player_injuried->SNum*11+m->partita.player_injuried->GNum);

			m->partita.possesso=(Squadra *)(m->partita.possesso==p->squadra[0] ? 0 : 1);
			
			if(m->partita.palla.sq_palla)
				m->partita.palla.sq_palla=(Squadra *)(m->partita.palla.sq_palla==p->squadra[0] ? 1 : 2);

			for(i=0;i<2;i++) {
				char *c=m->squadra[i].tattica->Name;

				m->squadra[i].attivo=(Giocatore *)((int)m->squadra[i].attivo->GNum);

				m->squadra[i].tattica=(struct Tactic *)( (c[0]<<24)|(c[1]<<16)|(c[2]<<8)|c[4]);
			}

			if(m->partita.palla.gioc_palla)
				m->partita.palla.gioc_palla=(Giocatore *)(m->partita.palla.gioc_palla->SNum*11+m->partita.palla.gioc_palla->GNum+1);

			WriteMatch(f, m);

			for (i=0;i<MAX_PLAYERS;i++) 
                for (j = 0; j < highsize; j++) 
                    WRITE_LONG(r_controls[i][m->ReplayCounter + j], f);
                
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

	if(highlight) {
		D(bug("Setting replay buffers...\n"));
		size=(highsize/256)+3; //per sicurezza
	}

	CounterLimit=(UWORD)(size*256-1);
	SetLimit=size-1;

	for(i=0;i<MAX_PLAYERS;i++)
	{
		if(!(r_controls[i]=malloc(size*256*sizeof(LONG))))	{
			D(bug("Non ho memoria per allocare i r_controls buffers!\n"));
			return FALSE;
		}
	}

	if(!(match=malloc(size*sizeof(struct MatchStatus)))) {
		D(bug("Non ho memoria per allocare i match buffer!\n"));
		return FALSE;
	}

	if(arcade) {
		if(!(arcade_buf=malloc(size*sizeof(struct Pos)*MAX_ARCADE_ON_FIELD))) {
			D(bug("Non ho memoria per allocare gli arcade buffer!\n"));
			return FALSE;
		}
	}

	D(bug("Replay configured: SetLimit:%ld CounterLimit:%ld\n",(LONG)SetLimit,(LONG)CounterLimit));
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

	if ((fh=fopen(HIGH_FILE,"rb")))	{
		WORD swaps;
		int i, j;

		READ_WORD(swaps, fh);

        D(bug("Loading an highlight...(%d frames, %d swaps)\n", highsize, swaps));

        ReadMatch(fh, &match[0]);

		for(i=0;i<MAX_PLAYERS;i++) 
            for (j = 0; j < highsize; j++) 
                READ_LONG(r_controls[i][j], fh);
            
		fclose(fh);

        j = swaps; // swapteams() change the swaps value!

		for(i = 0; i < j; i++)
			SwapTeams();

        swaps = j;

		start_replay=TRUE;
	}
	else {
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
