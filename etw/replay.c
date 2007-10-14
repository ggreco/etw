#include "eat.h"
#include "files.h"

ULONG *r_controls[MAX_PLAYERS];
// UWORD actual_control[MAX_PLAYERS];

void EndReplay(void);

extern int highsize;

UBYTE ReplaySet = 0, smallcounter = 0, old_tc;
UBYTE OriginalReplaySet, SetLimit, StartReplaySet;
BOOL replay_mode = FALSE, start_replay = FALSE, was_using_radar = FALSE;
BOOL was_using_nosound = FALSE, was_using_result = FALSE, slow_motion = FALSE;
BOOL mantieni_distanza = FALSE, no_record = FALSE, replay_looped = FALSE;
BOOL full_replay = FALSE, time_stopped = FALSE;
UWORD real_counter, counter = 0, CounterLimit;

extern BOOL draw_r;
extern void WriteGameConfig(FILE *);
struct Pos *arcade_buf;

struct MatchStatus
{
    struct Partita partita;
    struct Team team[2];
    UWORD ReplayCounter;
    WORD field_x,field_y;
    uint8_t need_release[MAX_PLAYERS];
};

struct MatchStatus *match, InitialStatus;
mytimer StartReplayTime, StopTimeVal;
int matchstatus_size = sizeof(struct MatchStatus);

static void ReadATeam(FILE *f, Team *s)
{
    int i;
    
    s->TPossesso = fread_u8(f);
    s->Reti = fread_u8(f);
    s->Falli = fread_u8(f);
    s->Ammonizioni = fread_u8(f);
    s->Espulsioni = fread_u8(f);
    s->Tiri = fread_u8(f);
    s->Rigori = fread_u8(f);
    s->Corner = fread_u8(f);
    s->Sostituzioni = fread_u8(f);
    s->Possesso = fread_u8(f);
    s->Schema = fread_u8(f);
    s->Joystick = fread_u8(f);

    s->tattica = (Tactic *)fread_u32(f); // must be converted in a pointer!
    s->TempoPossesso = fread_u32(f);
    s->attivo = (Player *)fread_u32(f); // must be converted in a pointer!

// goalkeeper
    s->keepers.world_x = fread_u16(f); 
    s->keepers.world_y = fread_u16(f); 
    s->keepers.OnScreen = fread_u8(f); 
    s->keepers.AnimType = fread_u16(f); 
    s->keepers.AnimFrame = fread_u16(f); 
    s->keepers.ObjectType = fread_u8(f); 
    s->keepers.Direzione = fread_u8(f); 
    s->keepers.ActualSpeed = fread_u8(f); 
    s->keepers.FrameLen = fread_u8(f); 
    s->keepers.Tick = fread_u16(f); 
    s->keepers.team = (Team *) fread_u32(f);  // must be converted in a pointeR!
    s->keepers.NameLen = fread_u8(f); 
    s->keepers.SNum = fread_u8(f); 
    s->keepers.Ammonito = fread_u8(f); 
    s->keepers.Special = fread_u8(f); 
    s->keepers.FirePressed = fread_u8(f); 
    s->keepers.SpecialData = fread_u16(f); 
    s->keepers.TimePress = fread_u16(f); 
    s->keepers.number = fread_u8(f); 
    s->keepers.velocita = fread_u8(f); 
    s->keepers.Parata = fread_u8(f); 
    s->keepers.Attenzione = fread_u8(f); 

// field players
    for (i = 0; i < 10; i++) {
        Giocatore *g = &s->players[i];

        g->world_x = fread_u16(f);
        g->world_y = fread_u16(f);
        g->OnScreen = fread_u8(f);
        g->AnimType = fread_u16(f);
        g->AnimFrame = fread_u16(f);
        g->ObjectType = fread_u8(f);
        g->Direzione = fread_u8(f);
        g->ActualSpeed = fread_u8(f);
        g->FrameLen = fread_u8(f);
        g->Tick = fread_u16(f);
        g->team = (Team *)fread_u32(f);  // must be converted in a pointeR!
        g->NameLen = fread_u8(f);
        g->GNum = fread_u8(f);
        g->Ammonito = fread_u8(f);
        g->Special = fread_u8(f);
        g->FirePressed = fread_u8(f);
        g->SpecialData = fread_u16(f);
        g->TimePress = fread_u16(f);
        g->number = fread_u8(f);
        g->Velocita = fread_u8(f);
        g->Contrasto = fread_u8(f);
        g->Tiro = fread_u8(f);
        g->Durata = fread_u8(f);
        g->Resistenza = fread_u8(f);
        g->Prontezza = fread_u8(f);
        g->settore = fread_u8(f);
        g->Creativita = fread_u8(f);
        g->Tecnica = fread_u8(f);
        g->Posizioni = fread_u8(f);
        g->SNum = fread_u8(f);
        g->Controlled = fread_u8(f);
        g->Marker = fread_u8(f);
        g->WaitForControl = fread_u16(f);
        g->Comando = fread_u8(f);
        g->Argomento = fread_u8(f);
        fread_data(&g->CA, sizeof(g->CA), f);
        g->OldStat = fread_u8(f);
        g->ArcadeEffect = fread_u8(f);
        g->ArcadeCounter = fread_u16(f);
    }
    
// other generic team datas
    s->Marker_X = fread_u16(f);
    s->Marker_Y = fread_u16(f);
    s->MarkerFrame = fread_u16(f);

    s->MarkerOnScreen = fread_u8(f);
    s->gioco_ruolo = fread_u8(f);
    fread_data(&s->name, sizeof(s->name), f);
    s->MarkerRed = fread_u8(f);
    s->ArcadeEffect = fread_u8(f);
    s->NumeroRiserve = fread_u8(f);
    s->ArcadeCounter = fread_u16(f);
}

void ReadMatch(FILE *f, struct MatchStatus *m)
{
    int i;
// write ball related data 
    m->partita.palla.gioc_palla = (Player *)fread_u32(f);
    m->partita.palla.sq_palla = (Team *)fread_u32(f);
    m->partita.palla.world_x = fread_u16(f);
    m->partita.palla.world_y = fread_u16(f);
    m->partita.palla.delta_x = fread_u16(f);
    m->partita.palla.delta_y = fread_u16(f);
    m->partita.palla.InGioco = fread_u8(f);
    m->partita.palla.Hide = fread_u8(f);
    m->partita.palla.ToTheTop = fread_u8(f);
    m->partita.palla.ThisQuota = fread_u8(f);
    m->partita.palla.Stage = fread_u8(f);
    m->partita.palla.TipoTiro = fread_u8(f);
    m->partita.palla.ActualFrame = fread_u8(f);
    m->partita.palla.MaxQuota = fread_u8(f);
    m->partita.palla.SpeedUp = fread_u8(f);
    m->partita.palla.Rimbalzi = fread_u8(f);
    m->partita.palla.velocita = fread_u8(f);
    m->partita.palla.Direzione = fread_u8(f);
    m->partita.palla.quota = fread_u8(f);
    m->partita.palla.settore = fread_u8(f);

// write refree related datas
    m->partita.arbitro.world_x = fread_u16(f);    
    m->partita.arbitro.world_y = fread_u16(f);    
    m->partita.arbitro.AnimType = fread_u16(f);    
    m->partita.arbitro.AnimFrame = fread_u16(f);    
    m->partita.arbitro.Tick = fread_u16(f);    
    m->partita.arbitro.Argomento = fread_u16(f);    
    m->partita.arbitro.OnScreen = fread_u8(f);
    m->partita.arbitro.Special = fread_u8(f);
    m->partita.arbitro.ObjectType = fread_u8(f);
    m->partita.arbitro.Direzione = fread_u8(f);
    m->partita.arbitro.ActualSpeed = fread_u8(f);
    m->partita.arbitro.FrameLen = fread_u8(f);
    m->partita.arbitro.NameLen = fread_u8(f);
    m->partita.arbitro.Comando = fread_u8(f);
    m->partita.arbitro.velocita = fread_u8(f);
    m->partita.arbitro.abilita = fread_u8(f);
    m->partita.arbitro.recupero = fread_u8(f);
    m->partita.arbitro.cattiveria = fread_u8(f);
    
// write generic datas
    m->partita.TempoPassato = fread_u32(f);
    m->partita.show_panel = fread_u32(f);
    m->partita.show_time = fread_u32(f);
    m->partita.possesso = (Team *) fread_u32(f);
    m->partita.player_injuried = (Player *)fread_u32(f);
    m->partita.check_sector = fread_u16(f);

    for (i = 0; i < SHOT_LENGTH; i++)
        m->partita.shotheight[i] = fread_u16(f);
    
    m->partita.flash_pos = fread_u16(f);
    m->partita.goal = fread_u8(f);
    m->partita.sopra_rete = fread_u8(f);
    m->partita.flash_mode = fread_u8(f);
    m->partita.doing_shot = fread_u8(f);
    m->partita.mantieni_distanza = fread_u8(f);
    m->partita.penalty_onscreen = fread_u8(f);

    m->partita.arcade_counter = fread_u16(f);
    m->partita.marker_x = fread_u16(f);
    m->partita.marker_y = fread_u16(f);
    m->partita.penalty_counter = fread_u16(f);
    m->partita.adder = fread_u16(f);

    m->partita.TabCounter = fread_u8(f);
    m->partita.result_len = fread_u8(f);
    m->partita.last_touch = fread_u8(f);
    m->partita.arcade_on_field = fread_u8(f);
    m->partita.RiservaAttuale = fread_u8(f);
    
    ReadATeam(f, &m->team[0]);
    ReadATeam(f, &m->team[1]);
    
    m->ReplayCounter = fread_u16(f);
    
    m->field_x = fread_u16(f);
    m->field_y = fread_u16(f);

    fread_data(&m->need_release, sizeof(m->need_release), f);
}

static void WriteATeam(FILE *f, Team *s)
{
    int i;
    
    fwrite_u8(s->TPossesso, f);
    fwrite_u8(s->Reti, f);
    fwrite_u8(s->Falli, f);
    fwrite_u8(s->Ammonizioni, f);
    fwrite_u8(s->Espulsioni, f);
    fwrite_u8(s->Tiri, f);
    fwrite_u8(s->Rigori, f);
    fwrite_u8(s->Corner, f);
    fwrite_u8(s->Sostituzioni, f);
    fwrite_u8(s->Possesso, f);
    fwrite_u8(s->Schema, f);
    fwrite_u8(s->Joystick, f);
    
    fwrite_u32((int32_t)s->tattica, f);
    fwrite_u32(s->TempoPossesso, f);
    fwrite_u32((int32_t)s->attivo, f);

// goalkeeper
    fwrite_u16(s->keepers.world_x, f); 
    fwrite_u16(s->keepers.world_y, f); 
    fwrite_u8(s->keepers.OnScreen, f); 
    fwrite_u16(s->keepers.AnimType, f); 
    fwrite_u16(s->keepers.AnimFrame, f); 
    fwrite_u8(s->keepers.ObjectType, f); 
    fwrite_u8(s->keepers.Direzione, f); 
    fwrite_u8(s->keepers.ActualSpeed, f); 
    fwrite_u8(s->keepers.FrameLen, f); 
    fwrite_u16(s->keepers.Tick, f); 
    fwrite_u32((int32_t)s->keepers.team, f); 
    fwrite_u8(s->keepers.NameLen, f); 
    fwrite_u8(s->keepers.SNum, f); 
    fwrite_u8(s->keepers.Ammonito, f); 
    fwrite_u8(s->keepers.Special, f); 
    fwrite_u8(s->keepers.FirePressed, f); 
    fwrite_u16(s->keepers.SpecialData, f); 
    fwrite_u16(s->keepers.TimePress, f); 
    fwrite_u8(s->keepers.number, f); 
    fwrite_u8(s->keepers.velocita, f); 
    fwrite_u8(s->keepers.Parata, f); 
    fwrite_u8(s->keepers.Attenzione, f); 

// field players
    for (i = 0; i < 10; i++) {
        Giocatore *g = &s->players[i];

        fwrite_u16(g->world_x, f);
        fwrite_u16(g->world_y, f);
        fwrite_u8(g->OnScreen, f);
        fwrite_u16(g->AnimType, f);
        fwrite_u16(g->AnimFrame, f);
        fwrite_u8(g->ObjectType , f);
        fwrite_u8(g->Direzione , f);
        fwrite_u8(g->ActualSpeed , f);
        fwrite_u8(g->FrameLen , f);
        fwrite_u16(g->Tick, f);
        fwrite_u32((int32_t)g->team, f);
        fwrite_u8(g->NameLen , f);
        fwrite_u8(g->GNum , f);
        fwrite_u8(g->Ammonito, f);
        fwrite_u8(g->Special, f);
        fwrite_u8(g->FirePressed, f);
        fwrite_u16(g->SpecialData, f);
        fwrite_u16(g->TimePress, f);
        fwrite_u8(g->number , f);
        fwrite_u8(g->Velocita , f);
        fwrite_u8(g->Contrasto , f);
        fwrite_u8(g->Tiro , f);
        fwrite_u8(g->Durata , f);
        fwrite_u8(g->Resistenza , f);
        fwrite_u8(g->Prontezza , f);
        fwrite_u8(g->settore , f);
        fwrite_u8(g->Creativita , f);
        fwrite_u8(g->Tecnica , f);
        fwrite_u8(g->Posizioni , f);
        fwrite_u8(g->SNum , f);
        fwrite_u8(g->Controlled, f);
        fwrite_u8(g->Marker, f);
        fwrite_u16(g->WaitForControl, f);
        fwrite_u8(g->Comando , f);
        fwrite_u8(g->Argomento , f);
        fwrite_data(&g->CA, sizeof(g->CA), f);
        fwrite_u8(g->OldStat , f);
        fwrite_u8(g->ArcadeEffect, f);
        fwrite_u16(g->ArcadeCounter, f);
    }
    
// other generic team datas
    fwrite_u16(s->Marker_X, f);
    fwrite_u16(s->Marker_Y, f);
    fwrite_u16(s->MarkerFrame, f);

    fwrite_u8(s->MarkerOnScreen, f);
    fwrite_u8(s->gioco_ruolo, f);
    fwrite_data(&s->name, sizeof(s->name), f);
    fwrite_u8(s->MarkerRed, f);
    fwrite_u8(s->ArcadeEffect, f);
    fwrite_u8(s->NumeroRiserve, f);
    fwrite_u16(s->ArcadeCounter, f);
}

// WARNING: gioc_palla, sq_palla, possesso, player_injured are pointers
void WriteMatch(FILE *f, struct MatchStatus *m)
{
    int i;
// write ball related data 
    fwrite_u32((int32_t)m->partita.palla.gioc_palla, f);
    fwrite_u32((int32_t)m->partita.palla.sq_palla, f);
    fwrite_u16(m->partita.palla.world_x, f);
    fwrite_u16(m->partita.palla.world_y, f);
    fwrite_u16(m->partita.palla.delta_x, f);
    fwrite_u16(m->partita.palla.delta_y, f);
    fwrite_u8(m->partita.palla.InGioco, f);
    fwrite_u8(m->partita.palla.Hide, f);
    fwrite_u8(m->partita.palla.ToTheTop, f);
    fwrite_u8(m->partita.palla.ThisQuota, f);
    fwrite_u8(m->partita.palla.Stage, f);
    fwrite_u8(m->partita.palla.TipoTiro, f);
    fwrite_u8(m->partita.palla.ActualFrame, f);
    fwrite_u8(m->partita.palla.MaxQuota, f);
    fwrite_u8(m->partita.palla.SpeedUp, f);
    fwrite_u8(m->partita.palla.Rimbalzi, f);
    fwrite_u8(m->partita.palla.velocita, f);
    fwrite_u8(m->partita.palla.Direzione, f);
    fwrite_u8(m->partita.palla.quota, f);
    fwrite_u8(m->partita.palla.settore, f);

// write refree related datas
    fwrite_u16(m->partita.arbitro.world_x, f);    
    fwrite_u16(m->partita.arbitro.world_y, f);    
    fwrite_u16(m->partita.arbitro.AnimType, f);    
    fwrite_u16(m->partita.arbitro.AnimFrame, f);    
    fwrite_u16(m->partita.arbitro.Tick, f);    
    fwrite_u16(m->partita.arbitro.Argomento, f);    
    fwrite_u8(m->partita.arbitro.OnScreen, f);
    fwrite_u8(m->partita.arbitro.Special, f);
    fwrite_u8(m->partita.arbitro.ObjectType, f);
    fwrite_u8(m->partita.arbitro.Direzione, f);
    fwrite_u8(m->partita.arbitro.ActualSpeed, f);
    fwrite_u8(m->partita.arbitro.FrameLen, f);
    fwrite_u8(m->partita.arbitro.NameLen, f);
    fwrite_u8(m->partita.arbitro.Comando, f);
    fwrite_u8(m->partita.arbitro.velocita, f);
    fwrite_u8(m->partita.arbitro.abilita, f);
    fwrite_u8(m->partita.arbitro.recupero, f);
    fwrite_u8(m->partita.arbitro.cattiveria, f);
    
// write generic datas
    fwrite_u32(m->partita.TempoPassato, f);
    fwrite_u32(m->partita.show_panel, f);
    fwrite_u32(m->partita.show_time, f);
    fwrite_u32((int32_t)m->partita.possesso, f);
    fwrite_u32((int32_t)m->partita.player_injuried, f);
    fwrite_u16(m->partita.check_sector, f);

    for (i = 0; i < SHOT_LENGTH; i++)
        fwrite_u16(m->partita.shotheight[i], f);
    
    fwrite_u16(m->partita.flash_pos, f);
    fwrite_u8(m->partita.goal, f);
    fwrite_u8(m->partita.sopra_rete, f);
    fwrite_u8(m->partita.flash_mode, f);
    fwrite_u8(m->partita.doing_shot, f);
    fwrite_u8(m->partita.mantieni_distanza, f);
    fwrite_u8(m->partita.penalty_onscreen, f);

    fwrite_u16(m->partita.arcade_counter, f);
    fwrite_u16(m->partita.marker_x, f);
    fwrite_u16(m->partita.marker_y, f);
    fwrite_u16(m->partita.penalty_counter, f);
    fwrite_u16(m->partita.adder, f);

    fwrite_u8(m->partita.TabCounter, f);
    fwrite_u8(m->partita.result_len, f);
    fwrite_u8(m->partita.last_touch, f);
    fwrite_u8(m->partita.arcade_on_field, f);
    fwrite_u8(m->partita.RiservaAttuale, f);
    
    WriteATeam(f, &m->team[0]);
    WriteATeam(f, &m->team[1]);

    fwrite_u16(m->ReplayCounter, f);
    fwrite_u16(m->field_x, f);
    fwrite_u16(m->field_y, f);

    fwrite_data(&m->need_release, sizeof(m->need_release), f);
}

void StopTime(void)
{
    if(!time_stopped&&!replay_mode)
    {
        StopTimeVal = Timer();
        time_stopped = TRUE;
    }
}

void RestartTime(void)
{
    if(time_stopped)
    {
        mytimer temp;

        temp=Timer();

        EndTime += (temp - StopTimeVal);
        StartGameTime += (temp - StopTimeVal);
        time_stopped = FALSE;
    }
}

void StoreReplay(UBYTE Set)
{
    register LONG i;

// Refree and ball

    match[Set].partita=*p;

    match[Set].team[0] = *p->team[0];
    match[Set].team[1] = *p->team[1];


    match[Set].ReplayCounter = counter;
    match[Set].field_x = field_x;
    match[Set].field_y = field_y;

    for (i = 0; i < MAX_PLAYERS; i++)
        match[Set].need_release[i] = need_release[i];

    if(arcade) {

// Moltiplico Set per il numero di bonus su schermo...

        Set *= MAX_ARCADE_ON_FIELD;

        for(i = 0; i < MAX_ARCADE_ON_FIELD; i++) {
            arcade_buf[Set + i].x = bonus[i]->world_x;
            arcade_buf[Set + i].y = bonus[i]->world_y;
        }
    }

    if(Set == 0 && first_kickoff)
        InitialStatus = match[0];
}

#define SQ_PTR (14+22)

// Mi servono 36 puntatori per ogni squadra, 11 per le immagini, 11 per nomi, 11 per cognomi e tre per cose varie.

void LoadReplay(UBYTE Set)
{
    register LONG i, j;
    APTR *a = NULL;

    i = 0;

    RestartTime();

    StartReplayTime = Timer();

    if(!pl->Hide)
        RemAnimObj(pl->immagine);

    if(p->penalty_onscreen)
        RemAnimObj(p->extras);
 
    p->penalty_onscreen = FALSE;

    if (highlight) {
// Salvo tutti i puntatori...

        a = (APTR *)malloc(sizeof(APTR) * (SQ_PTR * 2 + 6));

        if(!a) {
            quit_game = TRUE;
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

    for(i = 0; i < 4; i++)
        pezzi_porte[i]->world_x=-1000;

    for(i = 0; i < 2; i++) {

        if (p->team[i]->MarkerOnScreen)
            RemAnimObj(p->team[i]->Marker);

        if (a) {
            a[i * SQ_PTR]      = p->team[i]->keepers.immagine;
            a[i * SQ_PTR + 11] = p->team[i]->tattica;
            a[i * SQ_PTR + 12] = p->team[i]->Marker;
            a[i * SQ_PTR + 13] = p->team[i]->NomeAttivo;
            a[i * SQ_PTR + 34] = p->team[i]->keepers.name;
            a[i * SQ_PTR + 35] = p->team[i]->keepers.surname;
        }
        
        for (j = 0; j < 10; j++) {
            if (p->team[i]->players[j].OnScreen)
                RemAnimObj(p->team[i]->players[j].immagine);

            if (a) {
                a[j+i*SQ_PTR+1]  = p->team[i]->players[j].immagine;
                a[j+i*SQ_PTR+14] = p->team[i]->players[j].name;
                a[j+i*SQ_PTR+24] = p->team[i]->players[j].surname;
            }
        }

        // copy over the playing team the saved team
        *p->team[i] = match[Set].team[i];
        p->team[i]->keepers.OnScreen=FALSE;
        p->team[i]->MarkerOnScreen=FALSE;

        if (a) {
            char *c;
            ULONG d = (ULONG)p->team[i]->tattica, e;
// first fix the pointers then the reference INSIDE them!
            p->team[i]->keepers.immagine = a[i * SQ_PTR];
            p->team[i]->Marker = a[i * SQ_PTR + 12];

            p->team[i]->keepers.immagine->node.mln_Succ = 
                p->team[i]->keepers.immagine->node.mln_Pred = NULL;

            p->team[i]->keepers.team  = p->team[i];
            p->team[i]->keepers.name     = a[i * SQ_PTR + 34];
            p->team[i]->keepers.surname  = a[i * SQ_PTR + 35];

            p->team[i]->tattica = a[i * SQ_PTR + 11];
            p->team[i]->Marker->node.mln_Succ = 
                p->team[i]->Marker->node.mln_Pred = NULL;


            p->team[i]->NomeAttivo = a[i * SQ_PTR + 13];

            p->team[i]->attivo = &p->team[i]->players[(ULONG)p->team[i]->attivo];

            c = p->team[i]->tattica->Name;

            e = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[4];

            if (e != d) {
                char buffer[40];

                sprintf(buffer, "tct/%c%c%c-%c",
                        (char)((d & 0xff000000) >> 24),
                        (char)((d & 0x00ff0000) >> 16),
                        (char)((d & 0xff00) >> 8),
                        (char)(d & 0xff) );

                if(!(p->team[i]->tattica=LoadTactic(buffer))) {
                    quit_game = TRUE;
                    p->team[i]->tattica = a[i * SQ_PTR + 11];
                }
                else
                    FreeTactic(p->team[i]->tattica);
            }
        }

        for(j = 0; j < 10; j++) {
            p->team[i]->players[j].OnScreen = FALSE;

            if (a) {
                p->team[i]->players[j].team  = p->team[i];
                p->team[i]->players[j].immagine = a[j + i * SQ_PTR + 1];
                p->team[i]->players[j].name     = a[j + 14 + i * SQ_PTR];
                p->team[i]->players[j].surname  = a[j + 24 + i * SQ_PTR];

                p->team[i]->players[j].immagine->node.mln_Succ = 
                   p->team[i]->players[j].immagine->node.mln_Pred = NULL;
            }
        }
        
        PrintSmall(p->team[i]->NomeAttivo,
                   p->team[i]->attivo->surname,
                   p->team[i]->attivo->NameLen);
    }

//per debug
    old_tc = p->TabCounter;

    if (a) {
        a[SQ_PTR * 2]     = p->arbitro.immagine;
        a[SQ_PTR * 2 + 1] = pl->immagine;
        a[SQ_PTR * 2 + 2] = p->result;
        a[SQ_PTR * 2 + 3] = p->extras;
        a[SQ_PTR * 2 + 4] = p->team[0];
        a[SQ_PTR * 2 + 5] = p->team[1];
    }
    
    *p = match[Set].partita;

    if (a) {

        p->arbitro.immagine = a[SQ_PTR * 2];
        p->palla.immagine   = a[SQ_PTR * 2 + 1];
        p->result           = a[SQ_PTR * 2 + 2];
        p->extras           = a[SQ_PTR * 2 + 3];
        p->team[0]       = a[SQ_PTR * 2 + 4];
        p->team[1]       = a[SQ_PTR * 2 + 5];
    
        p->arbitro.immagine->node.mln_Succ =
            p->arbitro.immagine->node.mln_Pred = NULL;
        p->palla.immagine->node.mln_Succ =
            p->palla.immagine->node.mln_Pred = NULL;
        p->extras->node.mln_Succ =
            p->extras->node.mln_Pred = NULL;

        // setting serialized "match" pointers: possesso, player_injuried, sq_palla, gioc_palla
        p->possesso = p->team[(LONG)p->possesso];

        if(p->player_injuried) {
            ULONG l = (ULONG)p->player_injuried;
            l--;

            p->player_injuried=(l >= 11 ? &(p->team[1]->players[l-11]) : &(p->team[0]->players[l]) );
        }

        if(pl->sq_palla) {
            pl->sq_palla = p->team[((ULONG)pl->sq_palla) - 1];
        }

        if(pl->gioc_palla) {
            ULONG l = (ULONG)pl->gioc_palla;

            l--;

            pl->gioc_palla = ( l>=11 ? &(p->team[1]->players[l-11]) : &(p->team[0]->players[l]) );
        }
    }

// Era questa la causa del bug!!!

    p->arbitro.OnScreen = FALSE;

    if(!pl->Hide) {
        AddAnimObj(pl->immagine, 10, 10, 0);
    }

    if(p->penalty_onscreen)
        AddAnimObj(p->extras, 0, 0, (p->marker_x>640 ? 1 : 0 ));

    real_counter = counter;
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
            bonus[i]->world_x = arcade_buf[Set + i].x;
            bonus[i]->world_y = arcade_buf[Set + i].y;
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
        OriginalReplaySet = ReplaySet+1;
        StoreReplay(OriginalReplaySet);
        LoadReplay(0);
        StartReplaySet = 0;
        full_replay = FALSE;
    }
    else if(SetLimit==1) {
        OriginalReplaySet = ReplaySet ^ 1;
        StoreReplay(OriginalReplaySet);
        StartReplaySet = ReplaySet;
        LoadReplay(ReplaySet);
    }
    else {
        OriginalReplaySet = ReplaySet + 1;

        if(OriginalReplaySet > SetLimit)
            OriginalReplaySet = 0;

        StoreReplay(OriginalReplaySet);

// Ho allungato il replay di un set

        if(ReplaySet > 2) {
            StartReplaySet = ReplaySet - 3;
            LoadReplay(StartReplaySet);
        }
        else if(replay_looped) {
            StartReplaySet=SetLimit - 2 + ReplaySet;
            LoadReplay(StartReplaySet);
        }
        else {
            StartReplaySet = 0;
            LoadReplay(0);
        }
    }

    if(!full_replay) {
        if(detail_level & USA_RADAR) {
            detail_level &= ~USA_RADAR;
            was_using_radar = TRUE;
        }
        else was_using_radar = FALSE;

        if(detail_level & USA_RISULTATO) {
            detail_level &= ~USA_RISULTATO;
            was_using_result = TRUE;
        }
        else was_using_result = FALSE;

        if(!no_sound) {
            SetCrowd(REPLAY);
            was_using_nosound = FALSE;
            no_sound = TRUE;
        }
        else was_using_nosound = TRUE;
    }

    start_replay = FALSE;
    replay_mode = TRUE;
    draw_r = TRUE;

    if(highlight) {
        counter = 0;
        real_counter = highsize;
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
            smallcounter = 0;

            draw_r= ( (draw_r == TRUE) ? FALSE : TRUE );
        }

        if (MyReadPort0(0) & JPF_BUTTON_RED ||
            MyReadPort1(1) & JPF_BUTTON_RED ) {
            counter = real_counter;

            if (!highlight) {
                EndReplay();
                LoadReplay(OriginalReplaySet);
            }
        }

        if (counter >= real_counter) {
            if (highlight) {
                D(bug("Highlight finished, quitting."));

                if(slow_motion)    {
                    slow_motion = FALSE;
                    MY_CLOCKS_PER_SEC_50 >>= 2;
                }

                if(!was_using_nosound)
                    no_sound = FALSE;

                quit_game = TRUE;
                replay_mode = FALSE;
            }
            else
                EndReplay();
        }

    }
    else if((counter & 0xff) == 0) {
        StoreReplay(ReplaySet);

        ReplaySet++;

        if (ReplaySet>SetLimit) {
            D(bug("Replay looped!\n"));
            replay_looped = TRUE;
            ReplaySet = 0;
        }
    }

    counter++;

    if (counter > CounterLimit) {
        D(bug("Replay looped!\n"));
        replay_looped = TRUE;
        counter = 0;
    }
}

void EndReplay(void)
{
    mytimer EndReplayTime;

    EndReplayTime = Timer();


    D(bug("Replay end (%ld) - TabCounter: %ld\n",counter,p->TabCounter));


    if(old_tc != p->TabCounter) {
        D(bug("*** Error or interrupted replay sequence, forcing coherency...\n"));
        LoadReplay(OriginalReplaySet);
    }

    EndTime += (EndReplayTime - StartReplayTime);
    StartGameTime += (EndReplayTime - StartReplayTime);

    if(InAnimList(replay)) {
        RemAnimObj(replay);
    }

    if(was_using_radar)
        detail_level |= USA_RADAR;

    if(was_using_result)
        detail_level |= USA_RISULTATO;
        
    if(!was_using_nosound) {
        no_sound = FALSE;

        SetCrowd(FONDO);
    }

    if(slow_motion) {
        slow_motion = FALSE;
        MY_CLOCKS_PER_SEC_50 >>= 2;
    }

    replay_mode = FALSE;

    p->flash_mode = TRUE;
    p->flash_pos = 0;
}


void SaveReplay(void)
{
    char buffer[16] = "t/replay.001";
    extern struct team_disk leftteam_dk, rightteam_dk;
    FILE *f;
    int i, j;
    WORD highsize;
    struct MatchStatus *m;


    if(real_counter <= match[StartReplaySet].ReplayCounter)
        return;

    while ((f=fopen(buffer,"r"))) {
        fclose(f);
        buffer[11]++;

        if(buffer[11] > '9')
        {
            buffer[11] = '0';
            buffer[10]++;

            if(buffer[10] > '9')
            {
                buffer[10] = '0';
                buffer[9]++;
            }
        }
    }

    f = fopen(buffer,"wb");
    if (!f)
        return;

    WriteGameConfig(f);        

    WriteTeam(f, &leftteam_dk);
    WriteTeam(f, &rightteam_dk);

    highsize = real_counter-match[StartReplaySet].ReplayCounter + 1;
    fwrite_u16(highsize, f);

    D(bug("Saving from %ld to %ld (Set %ld), %ld frames\n",
          match[StartReplaySet].ReplayCounter, real_counter,
          StartReplaySet, highsize));

// Da qui viene letto da "game"

    m = malloc(sizeof(struct MatchStatus));
    if (!m) {
        fclose(f);
        return;
    }

    fwrite_u16(swaps, f);

    *m = match[StartReplaySet];

    if(m->partita.player_injuried)
        m->partita.player_injuried=(Giocatore *)(1+m->partita.player_injuried->SNum*11+m->partita.player_injuried->GNum);

    m->partita.possesso=(Team *)(m->partita.possesso==p->team[0] ? 0 : 1);
    
    if(m->partita.palla.sq_palla)
        m->partita.palla.sq_palla=(Team *)(m->partita.palla.sq_palla==p->team[0] ? 1 : 2);

    for(i = 0; i < 2; i++) {
        char *c = m->team[i].tattica->Name;

        m->team[i].attivo=(Giocatore *)((int)m->team[i].attivo->GNum);

        m->team[i].tattica=(struct Tactic *)( (c[0]<<24)|(c[1]<<16)|(c[2]<<8)|c[4]);
    }

    if(m->partita.palla.gioc_palla)
        m->partita.palla.gioc_palla = (Giocatore *)(m->partita.palla.gioc_palla->SNum * 11 + m->partita.palla.gioc_palla->GNum + 1);

    WriteMatch(f, m);

    for (i = 0; i < MAX_PLAYERS; i++) 
        for (j = 0; j < highsize; j++) 
            fwrite_u32(r_controls[i][m->ReplayCounter + j], f);
        
    PlayBackSound(sound[DOG]);

    free(m);
    fclose(f);
}

BOOL AllocReplayBuffers(void)
{
    int i, size = 128, fast = os_avail_mem();

    if(fast < 100000)
        size = 2;
    else if(fast<1000000)
        size = 8;
    else if(fast < 2000000)
        size = 64;
    else if(fast > 4000000)
        size = 256;

    if(highlight) {
        D(bug("Setting replay buffers...\n"));
        size = (highsize / 256) + 3; //per sicurezza
    }

    CounterLimit = (UWORD)(size * 256 - 1);
    SetLimit = size - 1;

    for(i = 0; i < MAX_PLAYERS; i++)
    {
        if(!(r_controls[i] = malloc(size * 256 * sizeof(LONG)))) {
            D(bug("Non ho memoria per allocare i r_controls buffers!\n"));
            return FALSE;
        }
    }

    if(!(match = malloc(size * sizeof(struct MatchStatus)))) {
        D(bug("Non ho memoria per allocare i match buffer!\n"));
        return FALSE;
    }

    if(arcade) {
        if(!(arcade_buf = malloc(size * sizeof(struct Pos) * MAX_ARCADE_ON_FIELD))) {
            D(bug("Non ho memoria per allocare gli arcade buffer!\n"));
            return FALSE;
        }
    }

    D(bug("Replay configured: SetLimit:%ld CounterLimit:%ld\n", (LONG)SetLimit, (LONG)CounterLimit));
    return TRUE;
}

void FreeReplayBuffers(void)
{
    int i;

    for(i = 0; i < MAX_PLAYERS; i++)
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
    WORD swaps;
    int i, j;

    fh = fopen(HIGH_FILE,"rb");
    if (!fh) {
        D(bug("Unable to open the highlight file!\n"));
        quit_game = TRUE;
        return;
    }

    swaps = fread_u16(fh);

    D(bug("Loading an highlight...(%d frames, %d swaps)\n", highsize, swaps));

    ReadMatch(fh, &match[0]);

    for(i = 0; i < MAX_PLAYERS; i++) 
        for (j = 0; j < highsize; j++) 
            r_controls[i][j] = fread_u32(fh);

    fclose(fh);

    j = swaps; // swapteams() change the swaps value!

    for(i = 0; i < j; i++)
        SwapTeams();

    swaps = j;

    start_replay = TRUE;
}


void RestartGame(void)
{
    StartGameTime = Timer();
    EndTime = StartGameTime + time_length * MY_CLOCKS_PER_SEC;

    match[0] = InitialStatus;

    LoadReplay(0);

    first_kickoff = TRUE;
    first_half = TRUE;
    starting_team = MyRangeRand(2);
    p->team[starting_team]->Possesso = 1;
    p->team[starting_team ^ 1]->Possesso = 0;
}

