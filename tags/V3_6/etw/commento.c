#include "eat.h"
#include "preinclude.h"
#include "commento.h"

#define NON_DECISA -1

int game_status = S_NON_INIZIATO;

char *comment_file=NULL, *fondobase;

uint32_t *Offsets, *Lengths = NULL;
uint32_t NumeroCommenti;
int frase = NON_DECISA, previous_status = NON_DECISA,
     special_status, FondoOffset = 0, fondolen;
int urgent_status = S_RESET;

BOOL c_played=FALSE;

char spk_basename[64]="talk/english";

FILE *commento = NULL;

void CheckStatus(void)
{
    switch(game_status)
    {
        case S_PALLA_PERSA:
            if(MyRangeRand(2))
                frase=PALLA_PERSA;
            else
            {
                special_status=S_TEAM_A-(pl->gioc_palla->SNum^1);
                frase=PERDE_LA_PALLA+MyRangeRand(2);
            }
            break;
        case S_FINE_PRIMO:
            frase=FINE_PRIMO_TEMPO+MyRangeRand(2);
            break;
        case S_FINE_SECONDO:
            frase=FINE_SECONDO_TEMPO;
            break;
        case S_TIRO:
            frase=TIRA+MyRangeRand(4);
            break;
        case S_TRAVERSA_COLPITA:
            frase=TRAVERSA+MyRangeRand(2);
            break;
        case S_PALO_COLPITO:
            frase=PALOOO+MyRangeRand(2);
            break;
        case S_CALCIO_D_INIZIO:
            if(first_kickoff)
            {
                if(first_half)
                {
                    if(!extratime)
                        frase=INIZIA_LA_PARTITA;
                    else
                        frase=SONO_INIZIATI_I_TEMPI_SUPPLEMENTARI;
                }
                else
                {
                    if(extratime)
                        frase=SECONDO_TEMPO_SUPPLEMENTARE;
                    else
                        frase=SECONDO_TEMPO;
                }
            }
            else
                frase=CALCIO_D_INIZIO;
            break;
        case S_CALCIO_D_ANGOLO:
            frase=CALCIO_D_ANGOLO;
            break;
        case S_CALCIO_DI_RIGORE:
            frase=CALCIO_DI_RIGORE;
            break;
        case S_RIMESSA_DAL_FONDO:
            frase=RINVIO_DA_FONDO_CAMPO+MyRangeRand(2);
            break;
        case S_CALCIO_DI_PUNIZIONE:
            frase=CALCIO_DI_PUNIZIONE;
            break;
        case S_RIMESSA_LATERALE:
            frase=RIMESSA_LATERALE;
            break;
        case S_PARATA:
            frase=IL_PORTIERE_BLOCCA_IL_PALLONE+MyRangeRand(7);
            break;
        case S_PARATA_NON_BLOCCA:
            frase=GRAN_PARATA_DEL_PORTIERE+MyRangeRand(6);
            break;
        case S_USCITA:
            frase=USCITA_DEL_PORTIERE+MyRangeRand(4);
            break;
        case S_AMMONIZIONE:
            frase=L_ARBITRO_ESTRAE_IL_CARTELLINO_GIALLO;
            break;
        case S_ESPULSIONE:
            frase=L_ARBITRO_ESTRAE_IL_CARTELLINO_ROSSO;
            break;
        case S_GOAL_TEAM_A:
        case S_GOAL_TEAM_B:
            frase=ED_E_GOL+MyRangeRand(5);
            break;
        case S_INIZIO_REPLAY:
            frase=VEDIAMO_L_AZIONE_AL_REPLAY+MyRangeRand(2);
            break;
        case S_NON_INIZIATO:
            frase=LE_SQUADRE_STANNO_ENTRANDO_IN_CAMPO;
            break;
        case S_ULTIMA_AZIONE:
            frase=IL_TEMPO_STA_PER_FINIRE+MyRangeRand(2);
            break;
        default:
            if(previous_status==S_NON_INIZIATO)
            {
                switch(MyRangeRand(10))
                {
                    case 0:
                        switch(current_field)
                        {
                            case 4:
                            case 5:
                            case 6:
                                frase=IL_TERRENO_E_IN_PESSIME_CONDIZIONI;
                                break;
                            case 7:
                                frase=IL_CAMPO_E_COPERTO_DALLA_NEVE;
                                break;
                            default:
                                frase=IL_TERRENO_E_IN_OTTIME_CONDIZIONI;
                        }

                        break;
                    case 1:
                        frase=GLI_SPALTI_SONO_ESAURITI_IN_OGNI_ORDINE_DI_POSTI;
                        break;
                    case 2:
                        frase=SARA_SICURAMENTE_UNA_PARTITA_MOLTO_INTERESSANTE;
                        break;
                    case 3:
                        frase=LE_SQUADRE_SONO_SCHIERATE;
                        break;
                }
            }
            else if(pl->InGioco&&!replay_mode&&!pause_mode&&!quit_game)
            {
                player_t *g;

                switch(MyRangeRand(45))
                {
                    case 0:
                    case 10:
                        if(pl->world_x<(CENTROCAMPO_X+1000) &&
                                pl->world_x>(CENTROCAMPO_X-1000) )
                        {
                            if(GetTable()>3||
                                    (pl->world_y>(RIMESSA_Y_N+400) &&
                                     pl->world_y<(RIMESSA_Y_S-400)) )
                            {
                                frase=PALLA_A_CENTROCAMPO+MyRangeRand(3);
                            }
                            else
                            {
                                special_status=S_TEAM_A-pl->gioc_palla->SNum;
                                frase=CI_PROVA_SULLA_FASCIA+MyRangeRand(2);
                            }
                        }
                        else if((g=pl->gioc_palla))
                        {
                            if(CanScore(g)==CS_SI)
                            {
                                if(p->team[g->SNum]->Reti > 
                                        p->team[g->SNum^1]->Reti+1
                                        && MyRangeRand(3)!=0)
                                    frase=E_UN_ASSEDIO+MyRangeRand(2);
                                else
                                    frase=HA_LA_POSSIBILITA_DI_TIRARE;
                            }
                            else if(MyRangeRand(4))
                                frase=PORTA_AVANTI_IL_PALLONE+MyRangeRand(3);
                            else 
                                frase=MANTIENE_IL_POSSESSO_DEL_PALLONE;

                            if(frase!=E_UN_ASSEDIO
                                    && frase!=SI_GIOCA_AD_UNA_PORTA)
                                special_status=S_TEAM_A-g->SNum;
                        }
                        break;
                    case 1:
                        if(p->team[0]->Reti>p->team[1]->Reti+1)
                        {

                            switch(MyRangeRand(7))
                            {
                                case 0:
                                case 5:
                                    special_status=S_TEAM_A;
                                    frase=STA_DOMINANDO_LA_PARTITA;
                                    break;
                                case 2:
                                case 1:
                                    special_status=S_TEAM_B;
                                    frase=E_IN_BALIA_DELL_AVVERSARIO;
                                    break;    
                            }
                        }
                        else if(p->team[1]->Reti>p->team[0]->Reti+1)
                        {
                            switch(MyRangeRand(7))
                            {
                                case 0:
                                case 5:
                                    special_status=S_TEAM_B;
                                    frase=STA_DOMINANDO_LA_PARTITA;
                                    break;
                                case 2:
                                case 1:
                                    special_status=S_TEAM_A;
                                    frase=E_IN_BALIA_DELL_AVVERSARIO;
                                    break;    
                            }
                        }
                        else if(MyRangeRand(4)==2)
                        {
                            frase=LA_PARTITA_E_MOLTO_EQUILIBRATA;
                        }
                    default:
                        break;
                }
            }
    }
}

BOOL speaker2memory(void)
{
    char buffer[100];
    FILE *f;
    BOOL ok = FALSE;
    
    strcpy(buffer, spk_basename);
    strcat(buffer, ".spk");

    if ((f = fopen(buffer, "rb"))) {
        long l;

        fseek(f, 0, SEEK_END);
        l = ftell(f);
        fseek(f, 0, SEEK_SET);

        if ((comment_file = malloc((int) l))) {
            fread(comment_file, 1, (int) l, f);
            ok = TRUE;
        }

        fclose(f);
    }

    return ok;
}

void init_speaker(void)
{
    FILE *f;
    int i;
    char buffer[120];

    strcpy(buffer,spk_basename);
    strcat(buffer,".spk");

    if(!audio2fast)
        if(!(commento=fopen(buffer,"rb")))
            use_speaker=FALSE;

    strcpy(buffer,spk_basename);
    strcat(buffer,".sdf");

    if(!(f=fopen(buffer,"rb")))
    {
        if(!audio2fast)
            fclose(commento);
        use_speaker=FALSE;
    }

    fread(&NumeroCommenti,sizeof(uint32_t),1,f);
    SWAP32(NumeroCommenti);

    D(bug("Alloco indice per %d frasi.\n", NumeroCommenti));

    if(NumeroCommenti<1 || NumeroCommenti>300)
    {
        D(bug("Errore nel file .sdf, disattivo lo speaker!\n"));
        use_speaker=FALSE;

        fclose(f);
        if(!audio2fast)
            fclose(commento);

        commento = NULL;
        return;
    }

// Faccio solo un'allocazione, per semplicita'.

    if(!(Lengths=malloc(NumeroCommenti*sizeof(uint32_t)*2))) {
        fclose(f);
        if(!audio2fast)
            fclose(commento);

        use_speaker=FALSE;
        commento = NULL;

        return;
    }


    Offsets=&Lengths[NumeroCommenti];

    fread(Lengths,sizeof(uint32_t),NumeroCommenti,f);
    fread(Offsets,sizeof(uint32_t),NumeroCommenti,f);

    for(i=0; i<NumeroCommenti; i++) {
        SWAP32(Lengths[i]);
        SWAP32(Offsets[i]);
//        D(bug("Commento %d: L:%ld O:%ld\n", i, Lengths[i], Offsets[i]));
    }

    fclose(f);
}

void free_speaker(void)
{
   if(Lengths)
       free(Lengths);

   
   if(!audio2fast && commento)
       fclose(commento);


   frase = NON_DECISA;
   game_status = S_NON_INIZIATO;

   fondolen = sound[FONDO]->Length;
   fondobase = sound[FONDO]->SoundData;

   if (audio2fast && comment_file) 
       free(comment_file);
   
   Lengths = NULL;
   commento = NULL;
   comment_file = NULL;
}




#define FRASERATE 11025;
#define FONDO_CHUNK 4096

struct SoundInfo *handle_speaker(void)
{
    if (!commento)
        return NULL;

    if(frase == NON_DECISA)
    {
        CheckStatus();
        previous_status = game_status;
    }

    if(frase == NON_DECISA)
    {
        int Length, Size = fondolen - FondoOffset;

        if(Size < 0)
            FondoOffset=0;

        Length=min(FONDO_CHUNK,Size);

        // Qui suono il fondo
        sound[FONDO]->SoundData = fondobase + FondoOffset;
        sound[FONDO]->Length = Length;
        
        FondoOffset += Length;

        c_played=TRUE;

        return sound[FONDO];
    }
    else
    {
//        D(bug("Frase %ld (%ld len, %ld off)...\n",frase,Lengths[frase],Offsets[frase]));

        switch(special_status)
        {
            case S_TEAM_A:
                special_status=S_NOMESQUADRA;
                previous_status=frase;
                frase=team_a;
                break;
            case S_TEAM_B:
                special_status=S_NOMESQUADRA;
                previous_status=frase;
                frase=team_b;
                break;
            case S_NOMESQUADRA:
                special_status=S_RESET;
            default:
                break;
        }

        game_status=S_RESET;

        if(!audio2fast)
            fseek(commento,Offsets[frase],SEEK_SET);

        // Qui la frase...

        sound[COMMENTO]->Length = Lengths[frase];

        if(audio2fast) {
            sound[COMMENTO]->SoundData = comment_file + Offsets[frase];
            sound[COMMENTO]->Flags = SOUND_DISK;
        }
        else {
            sound[COMMENTO]->SoundData = commento;
            sound[COMMENTO]->Flags = 0L;                        
        }
        if(special_status != S_NOMESQUADRA)
            frase=NON_DECISA;
        else
            frase=previous_status;

        urgent_status=S_RESET;

        return sound[COMMENTO];
    }

    return NULL;
}

#ifdef CD_VERSION

void UrgentSpeaker(int x)
{
    if(use_speaker&&!no_sound&&urgent_status!=(x)) 
    {
// qui c'era un'AbortIO
        urgent_status=x;
        special_status=S_RESET;
        game_status=urgent_status;
        CheckStatus();
    }
}
#endif
