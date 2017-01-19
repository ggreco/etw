#include "etw_locale.h"
#include <ctype.h>
#include "mytypes.h"
#include "menu.h"
#include "tactics.h"
#include "files.h"

uint8_t colore_team[3]={P_GRIGIO0, P_ROSSO0, P_BLU2};
uint8_t highlight_team[3]={P_BIANCO, P_ROSSO1, P_BLU1}, totale_giornate;

int8_t teamarray[64], competition=MENU_TEAMS, turno=0, actual_team=-1;
int8_t controllo[64], nteams, ruolo[64], ppv=3, pps=0, ppp=1;
char teamfile[256];

BOOL saved=TRUE, random_draw=FALSE, special=FALSE;
char division=0, scontri=0, i_scontri=0;

char career_file[256]={0};

struct championship_disk campionato;
struct team_disk *teamlist=NULL;
struct manager_disk manager;

struct teamstats_disk DatiCampionato[64];
struct control_disk *giocatori[64];

struct Match turni[64][32];

const char *empty=" "/*-*/;

void ViewEliminazioneDiretta(int n)
{
    int i;

    for(i=0; i<n; i++)
    {
        mp[i*4].Color=colore_team[controllo[teamarray[i*2]]+1];
        mp[i*4].Highlight=highlight_team[controllo[teamarray[i*2]]+1];
        mp[i*4+2].Color=colore_team[controllo[teamarray[i*2+1]]+1];
        mp[i*4+2].Highlight=highlight_team[controllo[teamarray[i*2+1]]+1];
        mp[i*4].Text=teamlist[teamarray[i*2]].name;
        mp[i*4+1].Text="-"/*-*/;
        mp[i*4+2].Text=teamlist[teamarray[i*2+1]].name;

        if(mp[i*4+3].Text)
        {
            free(mp[i*4+3].Text);
            mp[i*4+3].Text=NULL;
        }
    }
    ClearMatches(n);
}

void SetupSpecialEvent(struct Button *b)
{
    int i;

    special=TRUE;

    if(competition!=MENU_TEAMS)
    {
        b->ID=competition;
        return;
    }

    switch(campionato.type)
    {
        case CAMP_CUP:
            competition=MENU_MATCHES;
            b->ID=MENU_TEAM_SELECTION;
            wanted_number=campionato.nteams;
            menu[MENU_TEAM_SELECTION].Button[TS_RIGHE * TS_COLONNE].Text=msg_0;
            menu[MENU_TEAM_SELECTION].Button[TS_RIGHE * TS_COLONNE + 2].Text=NULL;
            i_scontri=scontri=1; // campionato.Scontri; Non ancora supportato
            break;
        case CAMP_LEAGUE:
            competition=MENU_LEAGUE;
            b->ID=MENU_TEAM_SELECTION;

            if(!division)
                wanted_number=campionato.nteams;
            else
            {
                wanted_number=0;

// Attualmente permetto solo di giocare partite di serie A.

                for(i=0; i<campionato.nteams; i++)
                {
                    if(teamlist[i].Flags==0)
                        wanted_number++;
                }
                D(bug("League with %ld teams\n", wanted_number));
            }
            ppv=campionato.win;
            ppp=campionato.draw;
            pps=campionato.loss;

            i_scontri=scontri=campionato.Scontri;
            menu[MENU_TEAM_SELECTION].Button[TS_RIGHE * TS_COLONNE].Text=msg_0;
            menu[MENU_TEAM_SELECTION].Button[TS_RIGHE * TS_COLONNE + 2].Text=NULL;
            break;
        case CAMP_WORLDCUP:
            menu[MENU_TEAM_SELECTION].Button[TS_RIGHE * TS_COLONNE].Text=msg_1;
            if (menu[MENU_TEAM_SELECTION].Button[TS_RIGHE * TS_COLONNE + 2].Text)
                free(menu[MENU_TEAM_SELECTION].Button[TS_RIGHE * TS_COLONNE + 2].Text);
            menu[MENU_TEAM_SELECTION].Button[TS_RIGHE * TS_COLONNE + 2].Text=strdup(msg_2);

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

    while(selected_number<wanted_number&&i<(TS_RIGHE *TS_COLONNE))
    {
        while(!teamselection[i].Text&&i< (TS_RIGHE *TS_COLONNE))
            i++;

        if(competition==MENU_LEAGUE&&division)
            while(teamlist[-teamselection[i].ID-1].Flags)
                i++;

        selected_number++;

        teamselection[i].Color=COLOR_COMPUTER;
        i++;
    }

    for(i=0; i < 64; i++)
        controllo[i]=-1;

    if(selected_number<wanted_number)
    {
        D(bug("Error in teams, I cannot select %ld of them!\n"/*-*/, wanted_number));
    }
}

BOOL LoadMenuTactic(char *name, tactic_t *t)
{
    int i, j, k;
    FILE *fh;

    if(!(fh=fopen(name, "rb"/*-*/)))
        return FALSE;

    fread(&t->NameLen, 1, sizeof(char), fh);
    fseek(fh, t->NameLen+1, SEEK_SET);

    for(i=0; i<2; i++)
        for(j=0; j<PLAYERS; j++)
            for(k=0; k<(SECTORS+SPECIALS); k++)
            {
                t->Position[i][j][k].x = fread_u16(fh);
                t->Position[i][j][k].y = fread_u16(fh);
            }

    fclose(fh);

    return TRUE;
}

void DisplayTactic(const char* tct, int xs, int ys, const char *pos[], int team_type)
{
// Pitch is 108*156, tactics 10240, 4352
    char buffer[120];
    tactic_t t;

    team_type += 17; // i use this value for the symbol color

    sprintf(buffer, "tct/%s"/*-*/, tct);

    setfont(smallfont);

    if (LoadMenuTactic(buffer, &t)) {
         int i;

         for(i=0; i<PLAYERS; i++) {
             if (pos[i]) {
                 int y = FixedScaledY(110-t.Position[0][i][GOALKICK].x*156/10240),
                     x = FixedScaledX(t.Position[0][i][GOALKICK].y*100/4352-15);
                 int l = (int)strlen(pos[i]);

                 BltAnimObj(symbols, main_bitmap, team_type, x+xs, y+ys, bitmap_width);

                 TextShadow(x+xs+(10-l*smallfont->width)/2+2-l,
                         y+ys+smallfont->height+1, pos[i], l);
             }
         }
    }
}

void MakeLeague(int ns)
{
    if(ns>20||ns<3)
        return;

    nteams=ns;

// Se le squadre sono dispari ne aggiungo sempre una che e' in effetti la
// squadra a riposo.

    if( ((nteams>>1)<<1) != nteams )
    {
        teamarray[nteams]=FAKE_TEAM;
        nteams++;
    }

    totale_giornate=nteams-1;
    turno=0;


    
    switch((nteams-4)/2)
    {
        case 0:
            {
                extern struct Match camp4[3][2];
                int i, j;

                for(i=0; i<3; i++)
                    for(j=0; j<2; j++)
                        turni[i][j]=camp4[i][j];
            }
            break;
        case 1:
            {
                extern struct Match camp6[5][3];
                int i, j;

                for(i=0; i<5; i++)
                    for(j=0; j<3; j++)
                        turni[i][j]=camp6[i][j];
            }
            break;
        case 2:
            {
                extern struct Match camp8[7][4];
                int i, j;

                for(i=0; i<7; i++)
                    for(j=0; j<4; j++)
                        turni[i][j]=camp8[i][j];
            }
            break;
        case 3:
            {
                extern struct Match camp10[9][5];
                int i, j;

                for(i=0; i<9; i++)
                    for(j=0; j<5; j++)
                        turni[i][j]=camp10[i][j];
            }
            break;
        case 4:
            {
                extern struct Match camp12[11][6];
                int i, j;

                for(i=0; i<11; i++)
                    for(j=0; j<6; j++)
                        turni[i][j]=camp12[i][j];
            }
            break;
        case 5:
            {
                extern struct Match camp14[13][7];
                int i, j;

                for(i=0; i<13; i++)
                    for(j=0; j<7; j++)
                        turni[i][j]=camp14[i][j];
            }
            break;
        case 6:
            {
                extern struct Match camp16[15][8];
                int i, j;

                for(i=0; i<15; i++)
                    for(j=0; j<8; j++)
                        turni[i][j]=camp16[i][j];
            }
            break;
        case 7:
            {
                extern struct Match camp18[17][9];
                int i, j;

                for(i=0; i<17; i++)
                    for(j=0; j<9; j++)
                        turni[i][j]=camp18[i][j];
            }
            break;
        case 8:
            {
                extern struct Match camp20[19][10];
                int i, j;

                for(i=0; i<19; i++)
                    for(j=0; j<10; j++)
                        turni[i][j]=camp20[i][j];
            }
            break;

    }

// debug

#ifndef DEBUG_DISABLED
    {
        int i, j;

        for(i=0; i<totale_giornate; i++)
        {
            D(bug("Round %ld\n", i+1));

            for(j=0; j<(nteams/2); j++)
                D(bug("%2ld-%2ld\n", turni[i][j].t1, turni[i][j].t2));

            D(bug(".\n"));
        }
    }
#endif

}


static void AddName(struct player_disk *g, int posizione)
{
    char namebuffer[60];
    char *c=namebuffer, *d=g->surname;

    *c++='-';


    if(*g->name)
    {
        if(*g->surname)
        {
            *c++=*g->name;
            *c++='.';
        }
        else
            strcpy(c, g->name);
    }


    while(*d)
    {
        *c++=toupper(*d);
        d++;
    }

    *c=0;

    if(teamsettings[posizione*2+1].Text)
        free(teamsettings[posizione*2+1].Text);

    teamsettings[posizione*2+1].Text=strdup(namebuffer);


}

static void SetPlayerStatus(int posizione, char infortuni, char ammonizioni, long v)
{
    int i;

    if(pannelli[posizione*3].Text!=NULL &&
        pannelli[posizione*3].Text != empty)
        free(pannelli[posizione*3].Text);

    if(infortuni>0)
    {
        char t=infortuni;

        if(t>3)
            t=3;

        D(bug("Mark %ld injuried for %ld\n", t, posizione));

        pannelli[posizione*3].Text=malloc(2);

        pannelli[posizione*3].Text[0]=12+t;
        
    }
    else if(ammonizioni>0)
    {
        char t=ammonizioni;

        if(t>3)
            t=3;

        D(bug("Mark %ld yellow card for %ld\n", t, posizione));

        pannelli[posizione*3].Text=malloc(2);

        pannelli[posizione*3].Text[0]=4+t;
    }
    else
    {
        pannelli[posizione*3].Text=(char *)empty;
    }

    if(pannelli[posizione*3+2].Text==NULL ||
        pannelli[posizione*3+2].Text == empty)
        pannelli[posizione*3+2].Text=malloc(6);

    pannelli[posizione*3+2].Text[5]=13;

// Deve dipendere dal valore!

    if(v<1)
    {
        pannelli[posizione*3+2].Text[0]=' ';
        pannelli[posizione*3+2].Text[1]=0;
        return;
    }
    
    v--;

    if(v>9)
        v=9;

//    D(bug("value: %ld -> %ld\n", value, v));

    for(i=0; i<10; i+=2)
    {
        if(i<=v)
            pannelli[posizione*3+2].Text[i/2]= ( (i<v) ? i/2 : (i/2 + 8) );
        else
            pannelli[posizione*3+2].Text[i/2]=13;
    }
}

static void AddPlayer(struct player_disk *g, int posizione)
{
    char buffer[8];

    AddName(g, posizione);

    *buffer=0;
    
    if(g->Posizioni&P_DIFESA)
    {
        strcat(buffer, msg_3);
    }
    if(g->Posizioni&P_CENTRO)
    {
        strcat(buffer, msg_4);
    }
    if(g->Posizioni&P_ATTACCO)
    {
        strcat(buffer, msg_5);
    }

    if(pannelli[posizione*3+1].Text&&pannelli[posizione*3+1].Text!=empty)
        free(pannelli[posizione*3+1].Text);

    pannelli[posizione*3+1].Text = strdup(buffer);
    teamsettings[posizione*2].Text   = (char*) numbers[g->number];

    SetPlayerStatus(posizione, g->injury, g->Ammonizioni,
        (((g->Tiro+g->tackle+g->speed*2+g->technique+g->creativity-2*6+3)*10)/7)/6 );
}

extern struct GfxMenu *actual_menu;

void update_menu_tactic()
{
    const char *nums[PLAYERS];
    int i;

    bltchunkybitmap(back, actual_menu->X, actual_menu->Y, main_bitmap,
            actual_menu->X, actual_menu->Y, 
            FixedScaledX(108), FixedScaledY(108), bitmap_width, bitmap_width);

    blit_scaled_logo();
    for (i = 0; i < PLAYERS; ++i) {
        if (teamlist[actual_team].players[i].Ammonizioni < 3)
            nums[i] = numbers[teamlist[actual_team].players[i].number];
        else
            nums[i] = NULL;
    }

    DisplayTactic(teamlist[actual_team].tactics[0], actual_menu->X,  actual_menu->Y, nums, controllo[actual_team]);
}

BOOL TeamSettings(int16_t button)
{
    struct Button *b;
    static int sel1 = -1;

    if (button < 0)
        return TRUE;

    b = &actual_menu->Button[button];

    if (button == 42)
    {
        sel1 = -1;

        if (teamsettings[42].Text == msg_6)
            ChangeMenu(b->ID);
        else
            return FALSE;
    }
    else if (button == 43)
    {
        // "Default" To develop!
        D(bug("We shouldn't pass here!"));
    }
    else if (controllo[actual_team] >= 0 && can_modify)
    {
        if (button < 34)
        {
            if (button == 32)
            {
                if (teamlist[actual_team].nplayers > 15)
                {
                    int i;

                    if (teamlist[actual_team].nplayers > (b->ID - 1))
                    {
                        AddPlayer(&teamlist[actual_team].players[b->ID - 1], 16);
                        b->ID++;
                    }
                    else
                    {
                        b->ID = 16;
                        AddPlayer(&teamlist[actual_team].players[14], 16);
                    }

                    teamsettings[button].Text = ".."/*-*/;

                    if (sel1 == 16)
                        sel1 = -1;

                    RedrawButton(&actual_menu->Button[33],
                                 actual_menu->Button[33].Color);

                    for (i = 0; i < 3; i++)
                        RedrawButton(&pannelli[48 + i],
                                     pannelli[48 + i].Color);
                }
            }
            else
            {
                struct Button *b2;
                int pos, selected = button / 2;

                b2 = &actual_menu->Button[selected * 2 + 1];

                if (sel1 >= 0)
                {

                    if (sel1 == selected)
                    {
                        if (!ruolo[actual_team]
                             || ruolo[actual_team] != selected)
                            RedrawButton(b2, b2->Color);
                        else
                            RedrawButton(b2, P_GIALLO);
                    }
                    else
                    {
                        struct Button *b3 = &actual_menu->Button[sel1 * 2 + 1];
                        int i;

                        pos = selected;

                        if ( (pos == 0 && sel1 == 11) || (sel1 == 0 && pos == 11))
                        {
                            struct keeper_disk p;

                            p = teamlist[actual_team].keepers[1];
                            teamlist[actual_team].keepers[1] = teamlist[actual_team].keepers[0];
                            teamlist[actual_team].keepers[0] = p;

                            AddName((struct player_disk *)&teamlist[actual_team].keepers[0], 0);
                            SetPlayerStatus(0, teamlist[actual_team].keepers[0].injury, 0,
                                (((teamlist[actual_team].keepers[0].Parata * 2 + teamlist[actual_team].keepers[0].Attenzione - 2 * 3 + 2) * 10) / 7) / 3);
                            AddName((struct player_disk *)&teamlist[actual_team].keepers[1], 11);

                            SetPlayerStatus(11, teamlist[actual_team].keepers[1].injury, 0,
                                (((teamlist[actual_team].keepers[0].Parata * 2 + teamlist[actual_team].keepers[1].Attenzione - 2 * 3 + 2) * 10) / 7) / 3);
                            RedrawButton(b2, b2->Color);


                            teamsettings[0].Text = (char*)numbers[teamlist[actual_team].keepers[0].number];
                            teamsettings[22].Text = (char*)numbers[teamlist[actual_team].keepers[1].number];
                            RedrawButton(b2 - 1, b2->Color);
                            RedrawButton(b - 1, b->Color);

                            for (i = 0; i < 3; i++)
                            {
                                RedrawButton(&pannelli[33 + i],
                                             pannelli[33 + i].Color);
                                RedrawButton(&pannelli[i], pannelli[i].Color);
                            }
                        }
                        else if (pos != 0 && pos != 11 &&sel1 != 0 &&sel1 != 11)
                        {
                            struct player_disk g;

                            int pos2;

                            if (selected == 16)
                            {
                                pos = actual_menu->Button[32].ID - 2;
                            }
                            else
                            {
                                pos--;

                                if (pos > 10)
                                    pos--;
                            }

                            g = teamlist[actual_team].players[pos];

                            if (sel1 == 16)
                                pos2 = actual_menu->Button[32].ID - 2;
                            else
                            {
                                pos2 = sel1 - 1;

                                if (pos2 > 10)
                                    pos2--;
                            }

                            teamlist[actual_team].players[pos] = teamlist[actual_team].players[pos2];
                            teamlist[actual_team].players[pos2] = g;
                            AddPlayer(&teamlist[actual_team].players[pos], selected);
                            AddPlayer(&teamlist[actual_team].players[pos2], sel1);

                            if (!ruolo[actual_team] || ruolo[actual_team] != selected)
                                RedrawButton(b2, b2->Color);
                            else
                                RedrawButton(b2, P_GIALLO);

                            RedrawButton(b2 - 1, b2->Color);
                            RedrawButton(b3 - 1, b3->Color);

                            for (i = 0; i < 3; i++)
                            {
                                RedrawButton(&pannelli[selected * 3 + i], pannelli[selected * 3 + i].Color);
                                RedrawButton(&pannelli[sel1 * 3 + i], pannelli[sel1 * 3 + i].Color);
                            }
                            update_menu_tactic();
                        }

                        if (!ruolo[actual_team] || ruolo[actual_team] != sel1)
                            RedrawButton(b3, b3->Color);
                        else
                            RedrawButton(b3, P_GIALLO);

                    }
                    sel1 = -1;
                }
                else
                {
                    sel1 = selected;
                    RedrawButton(b2, b2->Highlight);
                }
                ScreenSwap();
            }
        }
        else if (button < 40 || button == 41)
        {
            if (b->Color != COLOR_TATTICA_SELEZIONATA)
changetactic:
            {
                extern void DrawBox(int16_t);
                int i;
                strcpy(teamlist[actual_team].tactics[0], b->Text);

                update_menu_tactic();

                for (i = 0; i < 9; i++) {
                    struct Button *t = &teamsettings[34 + i];
                    if (t == b)
                        continue;
                    t->Color = COLOR_TATTICA_NON_SELEZIONATA;
                    RedrawButton(t, COLOR_TATTICA_NON_SELEZIONATA);
                }

                b->Color = COLOR_TATTICA_SELEZIONATA;

                RedrawButton(b, COLOR_TATTICA_SELEZIONATA);
                DrawBox(button);
                ScreenSwap();
            }
        }
        else if (button == 40)
        {
            freq.Title = msg_34;
            freq.Dir = "tct/"/*-*/;

            if (FileRequest(&freq))
            {
                char *c;

                b = &actual_menu->Button[41];

                if (b->Text)
                    free(b->Text);

                c = freq.File + strlen(freq.File)-1;

                while(*c != '/' && *c != '\\' && c > freq.File)
                    c--;

                b->Text = strdup(c);

                goto changetactic;
            }
        }
    }

    return TRUE;
}

void SetTeamSettings(int16_t team, BOOL starting)
{
    int i, k;

    actual_team=team;

    for(i=0; i<11; i++)
    {
        teamsettings[i*2+1].Color=
            teamsettings[i*2].Color=pannelli[i*3].Color=colore_team[controllo[team]+1];

        teamsettings[i*2+1].Highlight=
            teamsettings[i*2].Highlight=pannelli[i*3].Highlight=highlight_team[controllo[team]+1];
    }

    for(i=11; i<16; i++)
    {
        teamsettings[i*2+1].Color=
            teamsettings[i*2].Color=pannelli[i*3].Color=highlight_team[controllo[team]+1];

        teamsettings[i*2+1].Highlight=
            teamsettings[i*2].Highlight=pannelli[i*3].Highlight=colore_team[controllo[team]+1];
    }

    for(i=0, k=1; i<min(15, teamlist[team].nplayers); i++) {
        if(k==11)
            k++;

        AddPlayer(&teamlist[team].players[i], k);

        k++;
    }

    for(i=0; i<min(17, teamlist[team].nplayers+teamlist[team].nkeepers); i++)
    {
        teamsettings[i*2].ID=i;
//        pannelli[i*3].Text=empty;
    }

    if(teamlist[team].nplayers<15)
    {
        if(k==11)
            k++;

        for(i=k; i<17; i++)
        {
            if(teamsettings[i*2+1].Text)
                free(teamsettings[i*2+1].Text);

            teamsettings[i*2+1].Text=NULL;
            teamsettings[i*2].Text=NULL;

            if(pannelli[i*3].Text&&pannelli[i*3].Text!=empty)
                free(pannelli[i*3].Text);

            if(pannelli[i*3+1].Text&&pannelli[i*3+1].Text!=empty)
                free(pannelli[i*3+1].Text);

            if(pannelli[i*3+2].Text&&pannelli[i*3+2].Text!=empty)
                free(pannelli[i*3+2].Text);

            pannelli[i*3+1].Text=pannelli[i*3].Text=pannelli[i*3+2].Text=NULL;
        }
    }
    
    for(i=0; i<6; i++) {
        if(!stricmp(teamsettings[34+i].Text, teamlist[team].tactics[0]))
        {
            teamsettings[34+i].Color=COLOR_TATTICA_SELEZIONATA;
        }
        else teamsettings[34+i].Color=COLOR_TATTICA_NON_SELEZIONATA;
    }

    AddName((struct player_disk *)&teamlist[team].keepers[0], 0);
    SetPlayerStatus(0, teamlist[team].keepers[0].injury, 0, (((teamlist[team].keepers[0].Parata*2+teamlist[team].keepers[0].Attenzione-2*3+2)*10)/7)/3);
    teamsettings[0].Text = (char*)numbers[teamlist[actual_team].keepers[0].number];
   
    if(teamlist[team].nkeepers<2)
    {
        D(bug("if(teamlist[team].nkeepers<2)"));
        
        if(teamsettings[11*2+1].Text)
            free(teamsettings[11*2+1].Text);

        if(pannelli[11*3].Text&&pannelli[11*3].Text!=empty)
            free(pannelli[11*3].Text);

/*
 * This code could crash since it used to free a const char *,
 * this is an error, but it cannot be removed because often, when teams
 * have less than 2 goalkeepers we remove the second "G" panel
 * to replace it with another position name, so I've made an extra
 * check about msg_141, the "G" letter, so that it's freed only
 * it we really need to free it.
 */
        if(     pannelli[11*3+1].Text &&
                pannelli[11*3+1].Text!=empty &&
                pannelli[11*3+1].Text != msg_141) 
            free(pannelli[11*3+1].Text);

        if(pannelli[11*3+2].Text&&pannelli[11*3+2].Text!=empty)
            free(pannelli[11*3+2].Text);

        pannelli[11*3+2].Text=pannelli[11*3+1].Text=pannelli[11*3].Text=NULL;
        teamsettings[11*2].Text=teamsettings[11*2+1].Text=NULL;
    }
    else {
        if(     pannelli[11*3+1].Text &&
                pannelli[11*3+1].Text!=empty &&
                pannelli[11*3+1].Text != msg_141) 
            free(pannelli[11*3+1].Text);

        AddName((struct player_disk *)&teamlist[team].keepers[1], 11);
        pannelli[11*3+1].Text= msg_141;
        SetPlayerStatus(11, teamlist[team].keepers[1].injury, 0,
                        (teamlist[team].keepers[1].Parata*2+teamlist[team].keepers[1].Attenzione+2)/3);
       teamsettings[22].Text = (char*) numbers[teamlist[actual_team].keepers[1].number];
    }

    if (starting) 
        teamsettings[42].Text = msg_0;
    else
        teamsettings[42].Text = msg_6;

    if (teamsettings[16 * 2 + 1].Text)
        teamsettings[16 * 2].Text = ".."/*-*/;
    
    D(bug("SetTeamSettings for %d (%d) ok\n", team, starting));
}

void SetTeamSelection(void)
{
    int righe, start, i;

    righe=campionato.nteams/TS_COLONNE;

    if((righe*TS_COLONNE)<campionato.nteams)
        righe++;

    start=TS_RIGHE/2-righe/2;

    for(i=0; i<TS_RIGHE*TS_COLONNE; i++)
    {
        teamselection[i].Text=NULL;
        teamselection[i].Color=COLOR_UNSELECTED;
        teamselection[i].ID=-65;
    }

    for(i=0; i<campionato.nteams; i++)
    {
        teamselection[i+start*TS_COLONNE].ID=-i-1;
        teamselection[i+start*TS_COLONNE].Text=teamlist[i].name;
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
    if(menu[current_menu].Button[4].Text==NULL)
    {
        menu[current_menu].Button[4].Text="SAVE";
        ChangeMenu(current_menu);
    }
*/
}

void SaveLeague(void)
{
    SaveTeams(career_file);

    saved=TRUE;
    request(msg_279);
}

void SaveTeams(char *name)
{
    FILE *fh;
    int i;

    if ((fh=fopen(name, "wb"/*-*/))) {
        // AC: Old files had one team missing (probably due to a 0-based variable
        // new files like Brazil 2014 does not have this limitation; but we need
        // not to increase the number of available teams.
        // At the moment the implemented method is the only one came to my mind...
        if (strcmp(campionato.name,"BRAZIL 2014") != 0) {
            campionato.nteams--;
            
            fwrite(&campionato, sizeof(struct championship_disk), 1, fh);
            
            campionato.nteams++;
        }
        else
            fwrite(&campionato, sizeof(struct championship_disk), 1, fh);

        for(i=0; i<campionato.nteams; i++)
            WriteTeam(fh, &teamlist[i]);

        if(competition!=MENU_TEAMS) {
            fwrite(DatiCampionato, sizeof(struct teamstats_disk)*campionato.nteams, 1, fh);

            for(i=0; i<campionato.nteams; i++)    {
                if(DatiCampionato[i].Controllata) {
                    fwrite(giocatori[i], sizeof(struct control_disk), 1, fh);

                    if(giocatori[i]->ManagerType)
                        fwrite(&manager, sizeof(struct manager_disk), 1, fh);
                }
            }

            
            fwrite(&competition, sizeof(competition), 1, fh);
            fwrite(&nteams, sizeof(nteams), 1, fh);

// Per ricavarmi correttamente i_scontri uso un patch su turno

            if(i_scontri>scontri) {
                turno+=(nteams-1)*(i_scontri-scontri);
            }

            fwrite(&turno, sizeof(turno), 1, fh);

            if(i_scontri>scontri) {
                turno-=(nteams-1)*(i_scontri-scontri);
            }

// Per non distruggere il comportamento attuale uso un patch su totale_giornate...

            if(scontri>1) {
                totale_giornate+=(nteams-1)*(scontri-1);
            }

            fwrite(&totale_giornate, sizeof(totale_giornate), 1, fh);

            if(scontri>1) {
                totale_giornate-=(nteams-1)*(scontri-1);
            }

            fwrite(teamarray, sizeof(teamarray), 1, fh);
            fwrite(controllo, sizeof(controllo), 1, fh);

            if(competition==MENU_WORLD_CUP)    {
                extern int8_t start_groups[8][4]; //, groups[8][4]; i don't need this
                
                for(i=0; i<8; i++) {
                    fwrite(start_groups[i], 4, sizeof(int8_t), fh);
                }
            }
            else if(competition==MENU_LEAGUE) {
                fwrite(league_pos, sizeof(league_pos), 1, fh);
            }
            
        }
        fclose(fh);
    }
}

void LoadTeams(char *name)
{
    FILE *fh;
    struct championship_disk temp;

    if(teamfile!=name)
    strcpy(teamfile, name);

    temp=campionato;

    if ((fh=fopen(name, "rb"/*-*/))) {
        BOOL ok=FALSE;

        D(bug("Loading teams from %s...\n", name));

        if(fread(&campionato, 1, sizeof(struct championship_disk), fh)==sizeof(struct championship_disk)) {
            struct team_disk *teamold;
            D(bug("League: %s V:%ld-P:%ld-L:%ld-S:%ld\n", campionato.name, campionato.win,
                    campionato.draw, campionato.loss, campionato.nteams+1));

            // AC: Old files had one team missing (probably due to a 0-based variable
            // new files like Brazil 2014 does not have this limitation; but we need
            // not to increase the number of available teams.
            // At the moment the implemented method is the only one came to my mind...
            if (strcmp(campionato.name,"BRAZIL 2014") != 0)
                campionato.nteams++;

            menu[MENU_SIMULATION].Button[0].Text=(*campionato.name!=0) ? campionato.name : NULL;

            {
                int i=0;

                while(campionato.name[i]) {
                    campionato.name[i]=toupper(campionato.name[i]);
                    i++;
                }
            }

            teamold=teamlist;

            if ((teamlist=malloc(campionato.nteams*sizeof(struct team_disk)) ))    {
                int i;
                char *s;

                for(i=0; i<campionato.nteams; i++)    {
                    ReadTeam(fh, &teamlist[i]);
                    
                    s=teamlist[i].name;

                    while(*s) {
                        *s=toupper(*s);
                        s++;
                    }

                    if(division<teamlist[i].Flags)
                        division=teamlist[i].Flags;

                    s=teamlist[i].allenatore;

                    while(*s) {
                        *s=toupper(*s);
                        s++;
                    }
                }

                if(i==campionato.nteams)    {
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

                if(competition!=MENU_TEAMS)    {
                    extern struct Button mrb[];
                    training = FALSE;
                    if(fread(DatiCampionato, sizeof(struct teamstats_disk)*campionato.nteams, 1, fh)==sizeof(struct teamstats_disk)*campionato.nteams) {
                        for(i=0; i<campionato.nteams; i++)
                        {
                            if(DatiCampionato[i].Controllata) {
                                if(!giocatori[i])
                                    giocatori[i]=malloc( sizeof(struct control_disk) ); 

                                if(giocatori[i]) {
                                    fread(giocatori[i], sizeof(struct control_disk), 1, fh);

                                    if(giocatori[i]->ManagerType)
                                        fread(&manager, sizeof(struct manager_disk), 1, fh);
                                }
                            }
                        }
                    }

                    /* AC: C'e' un problema qui. i valori di nteams e turno
                     * o sono salvati male o sono caricati male, sono entrambi a zero!
                     * Indaghero'.
                     */
                    fread(&competition, sizeof(competition), 1, fh);
                    fread(&nteams, sizeof(nteams), 1, fh);

                    if(competition==MENU_LEAGUE)
                        MakeLeague(nteams);

// Mi serve sapere inizialmente quanti scontri avevo...

                    i_scontri=1;

                    fread(&turno, sizeof(turno), 1, fh);

                    while(turno>=nteams) {
                        /* AC: Al primo ciclo del while, nteams vale 0 
                         * e turno 1.
                         * Dopo questa istruzione turno vale 2 perch - * - = +
                         * Se nteams  vero che deve valere zero, allora
                         * si pu˜ usare turno += (nteams-1)
                         * altrimenti non va.
                        turno-=(nteams-1); */
                        if(nteams != 0)
                            turno-=(nteams-1);
                        else
                            turno+=(nteams-1);
                        i_scontri++;
                    }

                    scontri=1;

                    fread(&totale_giornate, sizeof(totale_giornate), 1, fh);

// Patch per gestire correttamente il caricamento di un campionato con + di uno scontro!

                    if (totale_giornate>=nteams) {
                        /* AC: Stesso discorso del while precedente */
                        if(nteams != 0)
                            totale_giornate-=(nteams-1);
                        else
                            totale_giornate+=(nteams-1);
                        scontri++;
                    }

                    if(i_scontri>1&&i_scontri!=scontri)    {
// Significa che devo fare uno swap delle squadre...
                        SwapAllTeams();
                    }

                    fread(teamarray, sizeof(teamarray), 1, fh);
                    fread(controllo, sizeof(controllo), 1, fh);

                    if(competition==MENU_WORLD_CUP)    {
                        extern int8_t start_groups[8][4], groups[8][4];
                        
                        for(i=0; i<8; i++) {
                            fread(start_groups[i], sizeof(int8_t), 4, fh);

                            memcpy(groups[i], start_groups[i], 4*sizeof(int8_t));
                        }

                        GroupsUpdate();
                    }
                    else if(competition==MENU_LEAGUE) {
                        fread(league_pos, sizeof(league_pos), 1, fh);

                        InitTable();
                        mb[0].ID = MENU_LEAGUE;
                        mrb[0].ID = MENU_MATCHES;
                        UpdateLeagueTable();
                        ChangeMenu(MENU_LEAGUE);
                    }
                    else if (competition == MENU_MATCHES) {
                        ViewEliminazioneDiretta(nteams / 2);
                        mb[0].ID = MENU_MATCHES;
                        mrb[0].ID = MENU_MATCHES;
                        ChangeMenu(MENU_MATCHES);
                    }
                }
            }
        }

        if(!ok)
            campionato=temp;

        fclose(fh);
    }
    else {
        D(bug("Unable to load %s!\n", name));
    }
}
