#include "etw_locale.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>

int arcade_score=0,score_number=0;

/* REAL France98 groups 
   
   int8_t f98groups[8][4]={    {3,25,19,21},{15,6,5,1},{11,26,24,9},{28,20,22,4},
            {13,2,27,18},{12,30,31,14},{23,7,10,29},{0,17,16,8}
              };
*/

// brazil 2014
int8_t f98groups[8][4]={  {5, 6, 11, 23}, // A
                          {2, 7, 18, 28}, // B
                          {8, 17, 10, 22}, // C
                          {9, 13, 21, 31}, // D
                          {12, 14, 19, 29}, // E
                          {1, 4, 20, 24}, // F
                          {15, 16, 25, 30}, // G
                          {0, 3, 26, 27} // H
                       };
        
int8_t groups[8][4];
int8_t start_groups[8][4];
int8_t league_pos[64];

BOOL GroupsClear(void)
{
    int i,j,k;

    if(random_draw)
    {
        for(i=0;i<8;i++)
            for(j=0;j<4;j++)
                groups[i][j]=-1;

        for(j=0;j<4;j++)
            for(i=0;i<8;i++)
            {
                do
                {
                    k=rand()%8;
                }
                while(groups[k][j]!=-1);

                groups[k][j]=f98groups[i][j];
            }
    }
    else
    {
        for(j=0;j<4;j++)
            for(i=0;i<8;i++)
                groups[i][j]=f98groups[i][j];

        for(i=0;i<32;i++)
        {
            if(teamarray[i]>31)
            {
                request("You HAVE to select the first 32 teams\nwithout random draw!"); 
                return FALSE;
            }
        }
    }

    for(i=0;i<32;i++)
        if(teamarray[i]!=i)
            groups[i/4][i%4]=teamarray[groups[i/4][i%4]];

    k=FixedScaledY(8);

    for(i=0;i<64;i++)
        memset(&DatiCampionato[i],0,sizeof(struct teamstats_disk));

    for(i=0;i<8;i++)
        for(j=0;j<4;j++)
        {
            int t,q,s;

            start_groups[i][j]=groups[i][j];

            s=(j+i*4)*5;

            q=wcp[32*5+i*5].Y2+2+j*(k+1);

            for(t=0;t<5;t++)
            {
                wcp[s+t].Y1=q;
                wcp[s+t].Y2=q+k;
            }
        }

    return TRUE;
}

void OrderGroup(int n)
{
    int8_t temp[4],best;
    int16_t maxpunti,maxdiff,maxgol;
    int i,k=0;

    for(i=0;i<4;i++)
        temp[i]=groups[n][i];

    while(k<4)
    {
        maxpunti=0;
        maxdiff=-100;
        maxgol=-1;

        best=-1;

        for(i=0;i<4;i++)
            if(temp[i]>=0)
            {
                if(    DatiCampionato[temp[i]].Punti>maxpunti ||
                    (    DatiCampionato[temp[i]].Punti==maxpunti &&
                         maxdiff<(DatiCampionato[temp[i]].GolFatti-DatiCampionato[temp[i]].GolSubiti)
                    ) ||
                    (
                        DatiCampionato[temp[i]].Punti==maxpunti &&
                         maxdiff==(DatiCampionato[temp[i]].GolFatti-DatiCampionato[temp[i]].GolSubiti) &&
                        maxgol>DatiCampionato[temp[i]].GolFatti
                    )
                )
                {
                    best=i;
                    maxpunti=DatiCampionato[temp[i]].Punti;
                    maxgol=DatiCampionato[temp[i]].GolFatti;
                    maxdiff=DatiCampionato[temp[i]].GolFatti-DatiCampionato[temp[i]].GolSubiti;
                }
            }

        if(best<0)
        {
            i=0;

            while(temp[i]<0)
                i++;

            best=i;
        }

        groups[n][k]=temp[best];
        temp[best]=-1;
        k++;
    }
}

void GroupsUpdate(void)
{
    int i,j,k;

    
    for(i=0;i<8;i++)
    {
        OrderGroup(i);

        for(j=0;j<4;j++)
        {
            k=i*4+j;

            if(!wcp[k*5+1].Text)
            {
                char *t;

                t=(char *)malloc(16);
                wcp[k*5+1].Text=t;
                wcp[k*5+2].Text=t+4;
                wcp[k*5+3].Text=t+8;
                wcp[k*5+4].Text=t+12;
            }

            wcp[k*5].Text=teamlist[groups[i][j]].name;
            wcp[k*5].Color=colore_team[controllo[groups[i][j]]+1];
            wcp[k*5].Highlight=highlight_team[controllo[groups[i][j]]+1];

            sprintf(wcp[k*5+1].Text,"%d",(int) DatiCampionato[groups[i][j]].Giocate);
            sprintf(wcp[k*5+2].Text,"%d",(int) DatiCampionato[groups[i][j]].Punti);
            sprintf(wcp[k*5+3].Text,"%d",(int) DatiCampionato[groups[i][j]].GolFatti);
            sprintf(wcp[k*5+4].Text,"%d",(int) DatiCampionato[groups[i][j]].GolSubiti);
        }
    }
}

void UpdateLeagueTable(void)
{
    int8_t temp[32];
    int16_t maxpunti,maxdiff,maxgol;
    int i,j=0,best,ns=nteams;

    if(teamarray[ns-1]==FAKE_TEAM)
        ns--;

    for(i=0;i<ns;i++)
        temp[i]=teamarray[i];

    while(j<ns)
    {
        maxpunti=-1;
        maxdiff=-100;
        maxgol=-1;

        best=-1;

        for(i=0;i<ns;i++)
        {
            if(temp[i]>=0)
            {
                if(    DatiCampionato[temp[i]].Punti>maxpunti ||
                    (    DatiCampionato[temp[i]].Punti==maxpunti &&
                         maxdiff<(DatiCampionato[temp[i]].GolFatti-DatiCampionato[temp[i]].GolSubiti)
                    ) ||
                    (
                        DatiCampionato[temp[i]].Punti==maxpunti &&
                         maxdiff==(DatiCampionato[temp[i]].GolFatti-DatiCampionato[temp[i]].GolSubiti) &&
                        maxgol>DatiCampionato[temp[i]].GolFatti
                    )
                )
                {
                    best=i;
                    maxpunti=DatiCampionato[temp[i]].Punti;
                    maxgol=DatiCampionato[temp[i]].GolFatti;
                    maxdiff=DatiCampionato[temp[i]].GolFatti-DatiCampionato[temp[i]].GolSubiti;
                }
            }
        }

        j++;

        if(best<0)
        {
            D(bug("Errore nell'ordinamento della classifica!!!\n"));
        }
        else
        {
            league_pos[j-1]=temp[best];

            league[j*8].Text=teamlist[temp[best]].name;
            league[j*8].Color=colore_team[controllo[temp[best]]+1];
            league[j*8].Highlight=highlight_team[controllo[temp[best]]+1];

            sprintf(league[j*8+1].Text,"%d",(int) DatiCampionato[temp[best]].Giocate);
            sprintf(league[j*8+2].Text,"%d",(int) DatiCampionato[temp[best]].Punti);
            sprintf(league[j*8+3].Text,"%d",(int) DatiCampionato[temp[best]].Vittorie);
            sprintf(league[j*8+4].Text,"%d",(int) DatiCampionato[temp[best]].Pareggi);
            sprintf(league[j*8+5].Text,"%d",(int) DatiCampionato[temp[best]].Sconfitte);
            sprintf(league[j*8+6].Text,"%d",(int) DatiCampionato[temp[best]].GolFatti);
            sprintf(league[j*8+7].Text,"%d",(int) DatiCampionato[temp[best]].GolSubiti);

            temp[best]=-1;
        }
    }
}

void InitTable(void)
{
    int i,j,ns=nteams;
    char *c;

    if(turno==0)
        for(i=0;i<64;i++)
            memset(&DatiCampionato[i],0,sizeof(struct teamstats_disk));

    lb[0].ID=MENU_MATCHES;

    if(teamarray[ns-1]==FAKE_TEAM)
        ns--;

    for(j=8;j<(8*(ns+1));j+=8)
    {
        if(!league[j+1].Text)
        {
            c=(char *)malloc(54); // 8 caratteri per ogni stringa;
        
            for(i=0;i<7;i++)
            {
                league[j+i+1].Text=&c[i*8];
            }
        }
    }

    for(j=(8*(ns+1));j<(8*21);j+=8)
    {
        league[j].Text=NULL;

        if(league[j+1].Text)
        {
            free(league[j+1].Text);

            for(i=1;i<8;i++)
                league[j+i].Text=NULL;
        }
    }
}


void ClearScores(void)
{
    int i;

    remove("ETWScores"/*-*/);

    score_number=0;

    for(i=0;i<score_number;i++)
    {
        if(scores[i*2+1].Text)
        {
            free(scores[i*2+1].Text);
            scores[i*2+1].Text=NULL;
        }
    }
    
    reset_achievements();
}

void PositionScores(void)
{
    int i;
    int16_t size=FixedScaledY(7),start=FixedScaledY(25),diff=FixedScaledY(9);

    for(i=0;i<score_number;i++)
    {
        scores[i*2+1].Y1=scores[i*2].Y1=start+diff*i;
        scores[i*2+1].Y2=scores[i*2].Y2=start+diff*i+size;
    }
}

void InsertScoreName(int i,char *buffer)
{
    int j;
    char *c;

    scores[i*2+1].Text=strdup(buffer);

    c=scores[i*2].Text=&scores[i*2+1].Text[6];

    for(j=0;j<6;j++)
        if(scores[i*2+1].Text[j]==' ')
            scores[i*2+1].Text[j]=0;

    while(*c)
    {
        if(*c<' ')
            *c=0;

        c++;
    }
}

void LoadScores(void)
{
    char buffer[100];
    FILE *f;


    if ((f=fopen(SCORE_FILE,"r"))) {
        if(fgets(buffer,99,f))
        {
            if(sscanf(buffer,"%d",&score_number)==1)
            {
                int i;

                for(i=0;i<score_number;i++)
                {
                    if(fgets(buffer,99,f))
                    {
                        InsertScoreName(i,buffer);
                    }
                    else 
                    {
                        score_number=i;
                        break;
                    }
                }
            }
        }
        PositionScores();
        fclose(f);
    }
    else {
        D(bug("Scores file not found"));
    }
}

void WriteScores(void)
{
    FILE *f;

    if ((f=fopen(SCORE_FILE, "w")))     {
        int i,j;

        fprintf(f, "%d\n", score_number);

        for(i=0;i<score_number;i++)        {
            for(j=0;j<6;j++)
                if(scores[i*2+1].Text[j]==0)
                    scores[i*2+1].Text[j]=' ';

            scores[i*2+1].Text[5]=' ';
            fprintf(f,"%s\n",scores[i*2+1].Text);

            for(j=0;j<6;j++)
                if(scores[i*2+1].Text[j]==' ')
                    scores[i*2+1].Text[j]=0;
        }

        fclose(f);
    }
    else {
        D(bug("Unable to write scores to %s\n", SCORE_FILE));
    }
}

void AddScore(uint8_t team)
{
    char buffer[100];
    int i=0;
    // let's add the score to the "world" rankings
    add_score(arcade_score);
    
    if(arcade_score<0)
        return;

    while(i<score_number && arcade_score<atol(scores[i*2+1].Text))
        i++;

    if(i>=20)
    {
        arcade_score=0;
        return;
    }
    else if(i<score_number)
    {
        int j;

        for(j=score_number;j>i;j--)
        {
            if(j<20)
            {
                scores[j*2].Text=scores[(j-1)*2].Text;
                scores[j*2+1].Text=scores[(j-1)*2+1].Text;
            }
        }
    }

    if(arcade_score<0)
        arcade_score=0;

    sprintf(buffer, "%-5d %s", arcade_score, teamlist[team].name);

    arcade_score=0;

    InsertScoreName(i,buffer);

    if(score_number<20)
    {
        score_number++;
        PositionScores();
    }

    WriteScores();
}
