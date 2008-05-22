#include "eat.h"

#include "highdirent.h"

void ReadTeam(FILE *fh, struct team_disk *s)
{
    int i;

    fread(&s->disponibilita, sizeof(uint32_t), 1, fh);
    SWAP32(s->disponibilita);
    fread(&s->nplayers, sizeof(uint8_t), 1, fh);
    fread(&s->nkeepers, sizeof(uint8_t), 1, fh);
    fread(&s->nation, sizeof(uint8_t), 1, fh);
    fread(&s->Flags, sizeof(uint8_t), 1, fh);

    for(i = 0; i < 2; i++)
    {
        fread(&s->jerseys[i].type, sizeof(uint8_t), 1, fh);
        fread(&s->jerseys[i].color0, sizeof(uint8_t), 1, fh);
        fread(&s->jerseys[i].color1, sizeof(uint8_t), 1, fh);
        fread(&s->jerseys[i].color2, sizeof(uint8_t), 1, fh);
    }

    for(i = 0; i < 3; i++)
        fread(&s->tactics[i], sizeof(char), 16, fh);

    fread(s->name, sizeof(char), 52, fh);
    fread(s->allenatore, sizeof(char), 52, fh);

    // Teams always have 3 keepers and 21 players because their size is fixed!

    for(i = 0; i < 3; i++)
    {
        fread(s->keepers[i].name, sizeof(char), 20, fh);
        fread(s->keepers[i].surname, sizeof(char), 20, fh);
        fread(&s->keepers[i].value, sizeof(uint32_t), 1, fh);
        SWAP32(s->keepers[i].value);
        fread(&s->keepers[i].number, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].speed, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].Parata, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].Attenzione, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].nation, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].Eta, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].injury, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].Flags, sizeof(uint8_t), 1, fh);
    }

    for(i = 0; i < 21; i++)
    {
        fread(s->players[i].name, sizeof(char), 20, fh);
        fread(s->players[i].surname, sizeof(char), 20, fh);
        fread(&s->players[i].value, sizeof(uint32_t), 1, fh);
        SWAP32(s->players[i].value);
        fread(&s->players[i].number, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].speed, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].tackle, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].Tiro, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].Durata, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].stamina, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].quickness, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].nation, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].creativity, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].technique, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].Eta, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].injury, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].Ammonizioni, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].Posizioni, sizeof(uint8_t), 1, fh);
    }
}


void WriteTeam(FILE *fh, struct team_disk *s)
{
    int i;

    SWAP32(s->disponibilita);
    fwrite(&s->disponibilita, sizeof(uint32_t), 1, fh);
    SWAP32(s->disponibilita);
    fwrite(&s->nplayers, sizeof(uint8_t), 1, fh);
    fwrite(&s->nkeepers, sizeof(uint8_t), 1, fh);
    fwrite(&s->nation, sizeof(uint8_t), 1, fh);
    fwrite(&s->Flags, sizeof(uint8_t), 1, fh);

    for(i = 0; i < 2; i++)
    {
        fwrite(&s->jerseys[i].type, sizeof(uint8_t), 1, fh);
        fwrite(&s->jerseys[i].color0, sizeof(uint8_t), 1, fh);
        fwrite(&s->jerseys[i].color1, sizeof(uint8_t), 1, fh);
        fwrite(&s->jerseys[i].color2, sizeof(uint8_t), 1, fh);
    }

    for(i = 0; i < 3; i++)
        fwrite(&s->tactics[i], sizeof(char), 16, fh);

    fwrite(s->name, sizeof(char), 52, fh);
    fwrite(s->allenatore, sizeof(char), 52, fh);

    // The number of keepers and players written is always the same

    for(i = 0; i < 3; i++)
    {
        fwrite(s->keepers[i].name, sizeof(char), 20, fh);
        fwrite(s->keepers[i].surname, sizeof(char), 20, fh);
        SWAP32(s->keepers[i].value);
        fwrite(&s->keepers[i].value, sizeof(uint32_t), 1, fh);
        SWAP32(s->keepers[i].value);
        fwrite(&s->keepers[i].number, sizeof(uint8_t), 1, fh);
        fwrite(&s->keepers[i].speed, sizeof(uint8_t), 1, fh);
        fwrite(&s->keepers[i].Parata, sizeof(uint8_t), 1, fh);
        fwrite(&s->keepers[i].Attenzione, sizeof(uint8_t), 1, fh);
        fwrite(&s->keepers[i].nation, sizeof(uint8_t), 1, fh);
        fwrite(&s->keepers[i].Eta, sizeof(uint8_t), 1, fh);
        fwrite(&s->keepers[i].injury, sizeof(uint8_t), 1, fh);
        fwrite(&s->keepers[i].Flags, sizeof(uint8_t), 1, fh);
    }

    for(i = 0; i < 21; i++)
    {
        fwrite(s->players[i].name, sizeof(char), 20, fh);
        fwrite(s->players[i].surname, sizeof(char), 20, fh);
        SWAP32(s->players[i].value);
        fwrite(&s->players[i].value, sizeof(uint32_t), 1, fh);
        SWAP32(s->players[i].value);
        fwrite(&s->players[i].number, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].speed, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].tackle, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].Tiro, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].Durata, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].stamina, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].quickness, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].nation, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].creativity, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].technique, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].Eta, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].injury, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].Ammonizioni, sizeof(uint8_t), 1, fh);
        fwrite(&s->players[i].Posizioni, sizeof(uint8_t), 1, fh);
    }
}

char team_name[2][16] = { { '\0' }, { '\0' } };
int result_width,scivolate_modificate=0;
linesman_t *linesman;

UBYTE cols[2][4],NumeroTattiche=0,TotaleRiserve[2];
BOOL teams_swapped=FALSE,has_black[2]={FALSE,FALSE};
WORD swaps=0;
char *tactics[32];

struct player_disk Riserve[2][12];

anim_t *players[2]={NULL,NULL},*keepers=NULL,*ports,*arcade_anim;
anim_t *g_neri[2]={NULL,NULL};
object_t *pezzi_porte[4];
object_t *bonus[MAX_ARCADE_ON_FIELD];

char snum,divisa;

extern struct pos portieri[2][2][SECTORS+SPECIALS];

BOOL NumeroDiverso(team_t *s,char n)
{
    register int i;

    for(i=0;i<10;i++)
            if(s->players[i].number==n)
                    return FALSE;

    return TRUE;
}

void RimuoviGuardalinee(void)
{
    if((detail_level&USA_GUARDALINEE)
        && linesman)    {
        D(bug("Removing linesmen...\n"));

        RimuoviLista((object_t *)&linesman[1]);
        RimuoviLista((object_t *)&linesman[0]);
        FreeAnimObj(linesman[1].anim);
        FreeAnimObj(linesman[0].anim);
        free(linesman);
        linesman = NULL;
    }
}

void ChangeImmagine(player_t *g, anim_t *o)
{
    WORD x,y,f,b;
    BOOL g_nero=IsBlack(g);

    x=g->anim->x_pos;
    y=g->anim->y_pos;
    f=g->anim->current_frame;
    b=g->anim->bottom;

// Devo usare forbid/permit per evitare rendering indesiderati!

    if(g->anim->Flags&AOBJ_CLONED)
    {
        D(bug("Changing skin colors, simple case!\n"));

        FreeAnimObj(g->anim);
        g->anim=CloneAnimObj(o);
    }
    else
    {
        team_t *s=g->team;
        player_t *g2=NULL;
        int i;

        for(i=0;i<10;i++)
        {
            if(i==g->GNum)
                continue;

            if(g_nero && IsBlack(&s->players[i]))
            {
                g2=&s->players[i];
                break;
            }
            else if(!g_nero && !IsBlack(&s->players[i]))
            {
                g2=&s->players[i];
                break;
            }
        }

        if(g2)
        {
            D(bug("Changing skin colors, trick case A!\n"));
            g->anim->x_pos=g2->anim->x_pos;
            g->anim->y_pos=g2->anim->y_pos;
            g->anim->current_frame=g2->anim->current_frame;
            g->anim->bottom=g2->anim->bottom;

            FreeAnimObj(g2->anim);

            g2->anim=g->anim;
            g->anim=CloneAnimObj(o);
        }
        else
        {
// G e' l'unico nero (o bianco), quindi lo rimuovo!
            int snum=g->SNum;

            D(bug("Changing skin colors, trick case B!\n"));

            if(teams_swapped)
                snum^=1;

            FreeAnimObj(g->anim);
            has_black[snum]=FALSE;

            g->anim=CloneAnimObj(o);
        }
    }

    g->anim->x_pos=x;
    g->anim->y_pos=y;
    g->anim->current_frame=f;
    g->anim->bottom=b;
}

void SwapTeams(void)
{
    extern UBYTE ReplaySet;
    extern char golrig[2];
    char c;
    team_t *s;
    int i,j;
    UBYTE tmp;
    void (*Temp)(int);

    D(bug("Inverting teams in pitch...\n"));

    swaps++;

    s=p->team[0];
    p->team[0]=p->team[1];
    p->team[1]=s;

    for(j=0;j<2;j++) {
        p->team[j]->keepers.SNum=j;

        for(i=0;i<10;i++)
            p->team[j]->players[i].SNum=j;
    }

    
    Temp=HandleTeam0;
    HandleTeam0=HandleTeam1;
    HandleTeam1=Temp;
    InvertTactic(p->team[0]->tactic);
    InvertTactic(p->team[1]->tactic);

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
        struct player_disk temp;

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
            RemapAnimObjColor(p->team[0]->players[9].anim,Pens[P_ARANCIO1],Pens[P_GIALLO]);
        else if(team_b==3)
            RemapAnimObjColor(p->team[1]->players[9].anim,Pens[P_ARANCIO1],Pens[P_GIALLO]);
    }


    /* AC: I put Progress() out of the for, so the number of progres block are 20 and not 22,
     * in order to have a plain result from the division between window width and number of
     * blocks
     */ 
    Progress();
    
    for(i=0;i<2;i++)
    {
        team_t *s=p->team[i];

        D(bug("Check skin color team %ld\n",i));
        neri=0;

        for(j=0;j<10;j++)
        {
            if(IsBlack(&s->players[j]))
                neri++;
        }

// if all the players are black remap every player
        if( neri>9 ) {
            has_black[i]=FALSE;

            RemapAnimObjColor(s->players[9].anim,Pens[P_ROSSO2],Pens[P_NERO]);
            RemapAnimObjColor(s->players[9].anim,Pens[P_ARANCIO1],Pens[P_ROSSO2]);
        }
        else if(neri>0)    {
            anim_t *o;

            if ((o=CopyAnimObj(s->players[9].anim)))    {
                BOOL fatti=FALSE;
                has_black[i]=TRUE;

                RemapAnimObjColor(o,Pens[P_ROSSO2],Pens[P_NERO]);
                RemapAnimObjColor(o,Pens[P_ARANCIO1],Pens[P_ROSSO2]);

                for(j=0;j<9;j++)
                {
                    if(IsBlack(&s->players[j]))
                    {
                        FreeAnimObj(s->players[j].anim);

                        if(!fatti)
                        {
                            s->players[j].anim=o;
                            fatti=TRUE;
                        }
                        else
                        {
                            s->players[j].anim=CloneAnimObj(o);
                        }
                    }
                }

                if(IsBlack(&s->players[9]))
                {
                    j=8;

                    while(IsBlack(&s->players[j]))
                        j--;

                    FreeAnimObj(s->players[j].anim);
                    s->players[j].anim=s->players[9].anim;
                    s->players[9].anim=CloneAnimObj(o);
                }                
                g_neri[i]=o;
            }
        }

        D(bug("%ld black skinned players.\n",neri));
    }
}

void MakeName(player_t *g,struct player_disk *gd)
{
    int j,l=strlen(gd->name)+strlen(gd->surname)+1;

    g->name=malloc(l+1);

    strcpy(g->name,gd->name);
    strcat(g->name," ");
        
    strcat(g->name,gd->surname);
    g->NameLen=strlen(gd->surname);

    g->surname=g->name+1+strlen(gd->name);

    for(j=0;j<l;j++)
    {
        g->name[j]=toupper(g->name[j]);
    }
}

void ChangePlayer(struct player_disk *src,player_t *dest)
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
            team_t *s=dest->team;
            int i;

            for(i=0;i<10;i++)
            {
                if(!IsBlack(&s->players[i]))
                {
                    ChangeImmagine(dest,s->players[i].anim);
                    break;
                }
            }
        }
        else if( IsBlack(src) && !IsBlack(dest))
        {
// E' entrato un giocatore nero al posto di un bianco...
            team_t *s=dest->team;
            int i;

            for(i=0;i<10;i++)
            {
                if(IsBlack(&s->players[i]))
                {
                    ChangeImmagine(dest,s->players[i].anim);
                    break;
                }
            }
        }
    }

    dest->number=src->number;
    dest->speed=src->speed;
    dest->tackle=src->tackle;
    dest->Tiro=src->Tiro;
    dest->Durata=src->Durata;
    dest->stamina=src->stamina;
    dest->quickness=src->quickness;
    dest->creativity=src->creativity;
    dest->technique=src->technique;
    dest->Posizioni=src->Posizioni;
    
    free(dest->name);
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

            tactics[NumeroTattiche]=strdup(ent->d_name);
            NumeroTattiche++;
        }
        closedir(dir);
    }
    D(bug("%ld available tactics.\n",NumeroTattiche));
}

BOOL InizializzaOggetti(game_t *p)
{
    int i;
    object_t *o;
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
        struct pos temp;

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

    if (arcade)    {
        for (i=0;i<MAX_ARCADE_ON_FIELD;i++)    {
            if ((o=malloc(sizeof(object_t)))) {
                o->world_x=-1000;

                o->otype=TIPO_BONUS;
                o->AnimFrame=0;
                o->AnimType=20+i*3;
                o->OnScreen=FALSE;
            
                o->world_y=0;

                if (!(o->anim=CloneAnimObj(p->team[0]->Marker)))    {
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
        if ((o=malloc(sizeof(object_t)))) {
            o->world_x=-1000;

            o->otype=TIPO_PORTA;
            o->AnimFrame=0;
            o->AnimType=i;
            o->OnScreen=FALSE;

            if(!arcade)    {
                if(i!=3) {
                    if(!(o->anim=CloneAnimObj(ports))) {
                        D(bug("Non c'e' piu' memoria per gli animobj!\n"));
                        ok=FALSE;
                        break;
                    }
                }
                else
                    o->anim=ports;
            
                if(i==0||i==2) {
                    o->anim->Flags|=AOBJ_OVER;
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
                if(o->otype>TIPO_ARBITRO)
                {
                    FreeAnimObj(o->anim);
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

void DestroyTeam(team_t *s)
{
    int i;

    D(bug("Freeing team...\n"));

    if(s->Marker)
        FreeAnimObj(s->Marker);

    FreeAnimObj(s->keepers.anim);
    free(s->keepers.name);

    free(s->NomeAttivo);
    FreeTactic(s->tactic);

    for(i=0;i<10;i++)
    {
        free(s->players[i].name);
        FreeAnimObj(s->players[i].anim);
    }

    players[(int)s->players[0].SNum]=NULL;
    keepers=NULL;

    free(s);

    D(bug("Ok.\n"));
}

void DisponiPortiere(team_t *s,int settore,BOOL possesso)
{

    s->keepers.world_x=portieri[(int)s->keepers.SNum][possesso][settore].x;
    s->keepers.world_y=portieri[(int)s->keepers.SNum][possesso][settore].y;

    if(settore==GOALKICK)
    {
        if(possesso)
        {
            if(pl->world_y>CENTROCAMPO_Y)
            {
                s->keepers.world_y=((346-14)*8);

                if(s==p->team[1])
                {
                    s->keepers.world_x=(70*8);
                }
                else    s->keepers.world_x=(1196*8);
            }
            else
            {
                s->keepers.world_y=((198-14)*8);

                if(s==p->team[1])
                {
                    s->keepers.world_x=(80*8);
                }
                else    s->keepers.world_x=(1186*8);
            }
        }
    }
}

void DisponiSquadra(team_t *s,int settore,BOOL possesso)
{
    int i;

    possesso = (possesso ? 1 : 0);

    if(s->ArcadeEffect)
        RemoveArcadeEffect(&s->players[0],s->ArcadeEffect);

    for(i=0;i<10;i++)
    {

// Per debug, causo un enforcer hit se ho problemi

        if(s->players[i].AnimType!=GIOCATORE_ESPULSO&&s->players[i].Comando!=STAI_FERMO&&s->players[i].Comando!=ESCI_CAMPO)
        {
        if(s->players[i].world_x<0 ||
            s->players[i].world_x>10400 ||
            s->players[i].speed<3 ||
            s->players[i].dir>7 ||
            s->players[i].dir<0 ||
            s->players[i].GNum!=i ||
            s->players[i].ActualSpeed<0 ||
            s->players[i].ActualSpeed>3 )
        {
//            char *a=NULL;

            D(bug(" *** Player %ld of team %ld with internal problems!\n",i+2,s->players[i].SNum));
//            a[0]=1;
        }

        s->players[i].world_x=s->tactic->Position[possesso][i][settore].x;
        s->players[i].world_y=s->tactic->Position[possesso][i][settore].y;

// Metto questa roba per vedere se risolvo il problema dell'omino ballerino!

        s->players[i].ArcadeEffect=NESSUN_COMANDO;
        s->players[i].Comando=NESSUN_COMANDO;
        s->players[i].Special=FALSE;
        s->players[i].FrameLen=10;
        s->players[i].AnimType=GIOCATORE_RESPIRA;
        s->players[i].AnimFrame=0;
        s->players[i].ActualSpeed=0;
        s->players[i].dir=FindDirection(s->players[i].world_x,s->players[i].world_y,G2P_X(pl->world_x),G2P_Y(pl->world_y));
        }
        else
        {
        D(bug("Skipping player %ld of team %ld(A:%ld,S:%ld,C:%ld)\n",i+2,s->players[0].SNum,
            s->players[i].AnimType,s->players[i].Special,s->players[i].Comando));
        }
    }

    DisponiPortiere(s,settore,possesso);
}

FILE *OpenTeam(char *name)
{
    char path[100];
    FILE *fh;

    strcpy(path,TEMP_DIR);
    strcat(path,name);

    if(!(fh=fopen(path,"rb")))
    {
        strcpy(path,TEAMS_DIR);
        strcat(path,name);

        if(!(fh=fopen(path,"rb")))
        {
            return NULL;
        }
    }

    return fh;
}

void ReadSquadra(FILE *fh, struct team_disk *s)
{
    int i;

    fread(&s->disponibilita, sizeof(uint32_t), 1, fh);
    SWAP32(s->disponibilita);
    fread(&s->nplayers, sizeof(uint8_t), 1, fh);
    fread(&s->nkeepers, sizeof(uint8_t), 1, fh);
    fread(&s->nation, sizeof(uint8_t), 1, fh);
    fread(&s->Flags, sizeof(uint8_t), 1, fh);

    for(i=0;i<2;i++)
    {
        fread(&s->jerseys[i].type, sizeof(uint8_t), 1, fh);
        fread(&s->jerseys[i].color0, sizeof(uint8_t), 1, fh);
        fread(&s->jerseys[i].color1, sizeof(uint8_t), 1, fh);
        fread(&s->jerseys[i].color2, sizeof(uint8_t), 1, fh);
    }

    for(i=0;i<3;i++)
        fread(&s->tactics[i], sizeof(char), 16, fh);

    fread(s->name, sizeof(char), 52, fh);
    fread(s->allenatore, sizeof(char), 52, fh);

    for(i=0;i<s->nkeepers;i++)
    {
        fread(s->keepers[i].name, sizeof(char), 20, fh);
        fread(s->keepers[i].surname, sizeof(char), 20, fh);
        fread(&s->keepers[i].value, sizeof(uint32_t), 1, fh);
        SWAP32(s->keepers[i].value);
        fread(&s->keepers[i].number, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].speed, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].Parata, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].Attenzione, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].nation, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].Eta, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].injury, sizeof(uint8_t), 1, fh);
        fread(&s->keepers[i].Flags, sizeof(uint8_t), 1, fh);
    }

    for(i=0;i<s->nplayers;i++)
    {
        fread(s->players[i].name, sizeof(char), 20, fh);
        fread(s->players[i].surname, sizeof(char), 20, fh);
        fread(&s->players[i].value, sizeof(uint32_t), 1, fh);
        SWAP32(s->players[i].value);
        fread(&s->players[i].number, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].speed, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].tackle, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].Tiro, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].Durata, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].stamina, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].quickness, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].nation, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].creativity, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].technique, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].Eta, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].injury, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].Ammonizioni, sizeof(uint8_t), 1, fh);
        fread(&s->players[i].Posizioni, sizeof(uint8_t), 1, fh);
    }
}

team_t *CreateTeam(int num)
{
    char path[100];
    team_t *s;
    extern struct team_disk leftteam_dk,rightteam_dk;
    struct team_disk sd;
#if 0
    FILE *fh;
#endif
    int i;

    if(!(s=calloc(1,sizeof(team_t))))
        return NULL;

#if 0
    if(!(fh=OpenTeam(name)))
    {
        D(bug("Non trovo %s!\n",name));
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

//    fread(&sd,sizeof(struct team_disk),1,fh); prima la leggevo cosi'...


    if(keepers)
    {
        if(!(keepers=CloneAnimObj(keepers)))
            return NULL;
    }
    else
    {
        if(!(keepers=LoadAnimObject((current_field==7 ? "gfx/portsnow.obj" : "gfx/portiere.obj"),Pens)))
            return NULL;
    }


    for(i=0;i<strlen(sd.name);i++)
    {
        s->name[i]=toupper(sd.name[i]);
    }

    s->name[i]=0;

    s->players[9].anim=players[snum];

    for(i=0;i<9;i++)
    {
        if(!(s->players[i].anim=CloneAnimObj(players[snum])))
        {
            free(s);
            return NULL;
        }
    }

    strcpy(path,TCT_DIR);
    strcat(path,sd.tactics[0]);

    if(!(s->tactic=LoadTactic(path)))
    {
        D(bug("Non trovo la tattica: %s!\n",path));
        free(s);
        return NULL;
    }

    Progress();

    if(!(s->NomeAttivo=malloc(CHAR_X*MAX_LEN*CHAR_Y)))
    {
        FreeTactic(s->tactic);
        free(s);
        return NULL;
    }

    cols[snum][0]=sd.jerseys[0].color0;
    cols[snum][1]=sd.jerseys[0].color1;
    cols[snum][2]=sd.jerseys[0].color2;

    if(sd.jerseys[0].color0!=COLORE_STANDARD_MAGLIE)
        RemapAnimObjColor(players[snum],Pens[COLORE_STANDARD_MAGLIE],Pens[sd.jerseys[0].color0]);
    if(sd.jerseys[0].color1!=COLORE_STANDARD_CALZONI)
        RemapAnimObjColor(players[snum],Pens[COLORE_STANDARD_CALZONI],Pens[sd.jerseys[0].color1]);
    if(sd.jerseys[0].color2!=COLORE_STANDARD_AUSILIARIO&&sd.jerseys[0].type>0)
        RemapAnimObjColor(players[snum],Pens[COLORE_STANDARD_AUSILIARIO],Pens[sd.jerseys[0].color2]);

// Copio i dati dei giocatori.

    if(player_type[snum]<0)
    {
        for(i=0;i<sd.nplayers;i++)
        {
            if(sd.players[i].speed<5)
                sd.players[i].speed++;
            if(sd.players[i].speed<7)
                sd.players[i].speed++;

            if(sd.players[i].tackle<5)
                sd.players[i].tackle++;
            if(sd.players[i].tackle<7)
                sd.players[i].tackle++;
            
            if(sd.players[i].quickness<5)
                sd.players[i].quickness++;
            if(sd.players[i].quickness<7)
                sd.players[i].quickness++;
        }
    }

    for(i=0;i<10;i++)
    {
// Copio le 10 stats del giocatore

        if(sd.players[i].Ammonizioni>1)
        {
            int k=10;

            while(k<sd.nplayers&&(sd.players[k].Ammonizioni>1||sd.players[k].injury>2))
                k++;

            if(k==sd.nplayers)
            {
                DoSpecialAnim( (&s->players[i]),GIOCATORE_ESPULSO);
                s->players[i].Comando=STAI_FERMO;
                s->players[i].world_x=CENTROCAMPO_X;
                s->players[i].world_y=(FIELD_HEIGHT-30)*8;
            }
            else
            {
                struct player_disk temp=sd.players[i];

                sd.players[i]=sd.players[k-1];
                sd.players[k-1]=temp;
            }
        }

        if(sd.players[i].injury>0)
        {
            uint8_t t = sd.players[i].injury;

            if(t > 3)
                t = 3;
    
            t *= 2;

            sd.players[i].speed=max(1,sd.players[i].speed-t);
            sd.players[i].Durata=max(1,sd.players[i].Durata-t);
            sd.players[i].tackle=max(1,sd.players[i].tackle-t);
            sd.players[i].Tiro=max(1,sd.players[i].Tiro-t);
        }

#ifdef OLD_VERSION
        memcpy(&s->players[i].number,&sd.players[i].number,sizeof(char)*10);
        s->players[i].Posizioni=sd.players[i].Posizioni;
#else
        s->players[i].number=sd.players[i].number;
        s->players[i].speed=sd.players[i].speed;
        s->players[i].tackle=sd.players[i].tackle;
        s->players[i].Tiro=sd.players[i].Tiro;
        s->players[i].Durata=sd.players[i].Durata;
        s->players[i].stamina=sd.players[i].stamina;
        s->players[i].quickness=sd.players[i].quickness;
        s->players[i].creativity=sd.players[i].creativity;
        s->players[i].technique=sd.players[i].technique;
        s->players[i].Posizioni=sd.players[i].Posizioni;
#endif
        MakeName(&s->players[i],&sd.players[i]);


        s->players[i].GNum=i;
        s->players[i].SNum=snum;
        s->players[i].otype=TIPO_GIOCATORE;
        s->players[i].team=s;
    }

    TotaleRiserve[snum]=s->NumeroRiserve=sd.nplayers-10;

    for(i=10;i<sd.nplayers;i++)
    {
        int k=0;

        Riserve[snum][i-10]=sd.players[i];

        while(Riserve[snum][i-10].surname[k])
        {
                Riserve[snum][i-10].surname[k]=toupper(Riserve[snum][i-10].surname[k]);
                k++;
        }
    }

    s->keepers.anim=keepers;
    s->keepers.SNum=snum;
    s->keepers.otype=TIPO_PORTIERE;
    s->keepers.team=s;
    s->keepers.number=sd.keepers[0].number;
    s->keepers.Parata=sd.keepers[0].Parata;
    s->keepers.Attenzione=sd.keepers[0].Attenzione;
    s->keepers.velocita=sd.keepers[0].speed;
    
// Rinforzo i portieri computerizzati, non definitivo

    if(player_type[snum]<0)
    {
        if(s->keepers.Parata<5)
            s->keepers.Parata+=3;
        else if(s->keepers.Parata<6)
            s->keepers.Parata+=2;
        else if(s->keepers.Parata<7)
            s->keepers.Parata+=1;

        if(s->keepers.Attenzione<5)
            s->keepers.Attenzione+=3;
        else if(s->keepers.Attenzione<6)
            s->keepers.Attenzione+=2;
        else if(s->keepers.Attenzione<7)
            s->keepers.Attenzione+=1;

        D(bug("Enhanced goalkeeper %ld, P: %ld (%ld) - A: %ld (%ld)\n",snum,
            s->keepers.Parata,sd.keepers[0].Parata,
            s->keepers.Attenzione,sd.keepers[0].Attenzione));
    }

    MakeName((player_t *)&s->keepers,(struct player_disk *)&sd.keepers[0]);
    AggiungiLista((object_t *) &s->keepers);

    if(role[snum])
    {
        s->attivo=&s->players[role[snum]-1];
        s->attivo->Controlled=TRUE;
        s->gioco_ruolo=TRUE;
    }
    else
    {
        s->attivo=&s->players[8];
        s->attivo->Controlled=TRUE;
    }

    PrintSmall(s->NomeAttivo,s->attivo->surname,s->attivo->NameLen);

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

game_t *SetupSquadre(void)
{
    int i;

// Questo non servira' piu' quando correggero' i dati...

    patch_anim_datas();

    for(i=0;i<DIMENSIONI_LISTA_OGGETTI;i++)
        object_list[i]=NULL;

    if(!(p=calloc(1,sizeof(game_t))))
        return NULL;

    Progress();

    if(!(p->ball.anim=LoadAnimObject( current_field==7 ? "gfx/ballsnow.obj" : "gfx/ball.obj",Pens)))
    {
        free(p);
        return NULL;
    }

    ScanTactics();

    Progress();

    snum=0;

    if(!(players[0]=LoadAnimObject(shirt[0],Pens)))
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

        if(!(players[1]=CopyAnimObj(players[0])))
        {
            free(p);
            return NULL;
        }
    }
    else
    {
        anim_t *obj;

        D(bug("Loading %s for team 1 shirt...\n",shirt[1]));

        cols[1][3]=shirt[1][strlen(shirt[1])-5];

        if(!(obj=players[1]=LoadAnimObject(shirt[1],Pens)))
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

    if(!(p->team[0]=CreateTeam(0)))
    {
        free(p);
        return NULL;
    }

    snum=1;

#ifdef DEMOVERSION
    LoadPLogo("gfx/control.gfx");
    os_delay(100);
#endif

    if(!(p->team[1]=CreateTeam(1)))
    {
        DestroyTeam(p->team[0]);
        free(p);
        return NULL;
    }


    if(!(p->team[0]->Marker=LoadAnimObject( (arcade ? "gfx/arcade.obj" : "gfx/marker.obj"),Pens)))
    {
        DestroyTeam(p->team[1]);
        DestroyTeam(p->team[0]);
        free(p);
        return NULL;
    }

    Progress();

    if(!(p->team[1]->Marker=CloneAnimObj(p->team[0]->Marker)))
    {
        DestroyTeam(p->team[0]);
        DestroyTeam(p->team[1]);
        free(p);
        return NULL;
    }
    
// La squadra 1 attacca da destra a sinistra.

    InvertTactic(p->team[0]->tactic);

/*  Setto a tutte e due le squadre lo stesso joystick per evitare problemi
    nelle rimesse e nei corner (non ancora implementati per il computer,
    con il quale andrebbe usato -1 come joystick)
 */


// Ricordarsi che i portieri sono inversi rispetto alle squadre come piazzamento!!!

    p->team[0]->Joystick=1;
    p->team[1]->Joystick=1;

    if(!(p->extras=LoadAnimObject("gfx/extras.obj",Pens)))
    {
        DestroyTeam(p->team[0]);
        DestroyTeam(p->team[1]);
        free(p);
        return NULL;
    }

    Progress();

    if(detail_level&USA_ARBITRO)
    {
        if(!(p->referee.anim=LoadAnimObject( (current_field==7 ? "gfx/arbisnow.obj" : "gfx/arbitro.obj"),Pens))) {
// Non riesco a caricare l'arbitro... Lo disabilito.

            detail_level&=~USA_ARBITRO;
        }
        else {
            if(detail_level&USA_ARBITRO)
                AggiungiLista((object_t *)&p->referee);

            p->referee.otype=TIPO_ARBITRO;
        }

        Progress();
    }

    if(detail_level&USA_GUARDALINEE)
    {
        if (!(linesman = calloc(2, sizeof(linesman_t)))) {
            detail_level&=~USA_GUARDALINEE;
        }
        else if(!(linesman[0].anim=LoadAnimObject( 
                        (current_field==7 ? NEWGFX_DIR "gls.obj" : 
                         NEWGFX_DIR "gl.obj") ,Pens)))    {
// Non riesco a caricare i guardalinee... Li disabilito.
            free(linesman);
            detail_level&=~USA_GUARDALINEE;
        }
        else {
            if ((linesman[1].anim=CloneAnimObj(linesman[0].anim))) {
                int k;

                for(k=0;k<2;k++) {
                    AggiungiLista((object_t *)&linesman[k]);
                    linesman[k].otype=TIPO_GUARDALINEE;
                    linesman[k].OnScreen=FALSE;
                    linesman[k].world_x=CENTROCAMPO_X;

                    if(k) {
                        linesman[k].world_y=(RIMESSA_Y_S-20);
                        linesman[k].dir=6;
                    }
                    else {
                        linesman[k].dir=2;
                        linesman[k].world_y=(RIMESSA_Y_N-140);
                    }
                }
            }
            else {
                FreeAnimObj(linesman[0].anim);
                free(linesman);
                detail_level&=~USA_GUARDALINEE;
            }
        }        
        Progress();
    }

    if(detail_level&USA_RISULTATO) {
        result_width=(strlen(p->team[0]->name)+strlen(p->team[1]->name)+9)*VS_CHAR_X;

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
            RimuoviLista((object_t *)&p->referee);

        RimuoviGuardalinee();
        DestroyTeam(p->team[0]);
        DestroyTeam(p->team[1]);
        free(p);
        return NULL;
    }

    CheckPelle();


    p->referee.OnScreen=FALSE;
    p->referee.cattiveria=strictness;
    p->referee.Tick=0;
    pl=&p->ball;

    if(penalties||free_kicks)
    {
        detail_level&=(~USA_RADAR);
        p->referee.Comando=FISCHIA_PREPUNIZIONE;
    }
    else
    {
        p->referee.world_x=CENTROCAMPO_X;
        p->referee.world_y=(340*8);
        p->referee.Comando=FISCHIA_RIPRESA;

        pl->settore=KICKOFF;

        starting_team=MyRangeRand(2);

        pl->sq_palla=p->team[starting_team];
        p->team[starting_team]->Possesso=1;
        p->team[starting_team^1]->Possesso=0;
    
        pl->world_x=CENTROCAMPO_X;
        pl->world_y=CENTROCAMPO_Y;
        pl->InGioco=FALSE;
    }
/*
    DisponiSquadra(p->team[0],KICKOFF,TRUE);
    DisponiSquadra(p->team[1],KICKOFF,FALSE);
*/
    D(bug("GK 0: P:%ld A:%ld\nGK 1: P:%ld A:%ld\n",
        p->team[0]->keepers.Parata,    p->team[0]->keepers.Attenzione,
        p->team[1]->keepers.Parata,    p->team[1]->keepers.Attenzione));

    return p;
}

void LiberaPartita(game_t *p)
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

    for(i=0;i<totale_lista;i++)    {
        if(object_list[i])    {
// Arbitro e portieri vengono liberati qui

            if(object_list[i]->otype > TIPO_ARBITRO)    {
                if (object_list[i]->otype == TIPO_GUARDALINEE) {
                    D(bug("Warning, linesmen still in list, skipping!"));
                    continue;
                }
                FreeAnimObj(object_list[i]->anim);
                free(object_list[i]);
            }

            object_list[i]=NULL;
        }
    }

    totale_lista=0;

    DestroyTeam(p->team[1]);
    DestroyTeam(p->team[0]);

    if(detail_level&USA_ARBITRO)
        FreeAnimObj(p->referee.anim);

    FreeAnimObj(p->ball.anim);

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

    p->result_len=sprintf(buffer," %s %d-%d %s",
            p->team[1]->name, (int) p->team[1]->Reti,
            (int) p->team[0]->Reti, p->team[0]->name);

    rectfill(p->result,0,0,result_width-1, VS_CHAR_Y,Pens[P_VERDE2],result_width);

    PrintVerySmall(p->result,buffer,p->result_len,result_width);

    p->result_len++;
}
