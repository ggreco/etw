#include "eat.h"
#include "files.h"

tactic_t *LoadTactic(char *name)
{
    tactic_t *t;
    char *c;
    int i,j,k;
    FILE *fh;

    if(!(t=calloc(1,sizeof(tactic_t)))) {
        D(bug("Not enough memory!\n"));
        return NULL;
    }

    if(!(fh=fopen(name,"rb"))) {
        D(bug("Unable to find wanted tactic (%s)...\n", name));
        free(t);
        return NULL;
    }

    t->NameLen = fread_u8(fh);

    if(!(t->Name=malloc(t->NameLen+1))) {
        free(t);
        fclose(fh);
        return NULL;
    }

    fread(t->Name,t->NameLen,1,fh);

    t->Name[t->NameLen]=0;

    // note: tactics have WRONG names inside the files, so we change the name to the filename
    if ((c = strrchr(name, '/'))) {
        free(t->Name);
        t->Name = strdup(c + 1);
    }

    for(i=0;i<2;i++)
        for(j=0;j<PLAYERS;j++)
            for(k=0;k<(SECTORS+SPECIALS);k++) {
                t->Position[i][j][k].x = fread_u16(fh);
                t->Position[i][j][k].y = fread_u16(fh);

                t->Position[i][j][k].x = (t->Position[i][j][k].x>>5)*31+280;
                t->Position[i][j][k].y= ( t->Position[i][j][k].y>>2)*3-96;
                t->Position[i][j][k].sector = t->Position[i][j][k].x/2560
                                         + (t->Position[i][j][k].y/1450 << 2);
            }

    fclose(fh);

    return t;
}

void FreeTactic(tactic_t *t)
{
    free(t->Name);
    free(t);
}

void InvertTactic(tactic_t *t)
{
    int i,j,k;

    for(i=0;i<2;i++)
    {
        for(j=0;j<PLAYERS;j++)
        {
            struct pos temp;

            for(k=0;k<(SECTORS+SPECIALS);k++)
            {
                t->Position[i][j][k].x= (((1280/4)*31)+200)-t->Position[i][j][k].x;
                t->Position[i][j][k].y= ((544*7)+440)-t->Position[i][j][k].y;
                t->Position[i][j][k].sector=t->Position[i][j][k].x/2560 + (t->Position[i][j][k].y/1450 << 2);
            }
            
            for(k=0;k<3;k++)
            {
                temp=t->Position[i][j][k*4+3];
                t->Position[i][j][k*4+3]=t->Position[i][j][k*4];
                t->Position[i][j][k*4]=temp;
                temp=t->Position[i][j][k*4+2];
                t->Position[i][j][k*4+2]=t->Position[i][j][k*4+1];
                t->Position[i][j][k*4+1]=temp;
            }

            for(k=0;k<4;k++)
            {
                temp=t->Position[i][j][k+8];
                t->Position[i][j][k+8]=t->Position[i][j][k];
                t->Position[i][j][k]=temp;
            }

        }
    }
}

BOOL change_tactic(team_t *cnt, const char *newname)
{
    char path[128];
    tactic_t *oldtct=cnt->tactic;

    sprintf(path, "tct/%s", newname);
    if(!(cnt->tactic=LoadTactic(path))) {
        D(bug("Unable to find new tactic! (%s)\n",path));
        cnt->tactic=oldtct;
    }
    else {
        D(bug("Changing tactic for %s from %s to %s\n", cnt->name, oldtct->Name, newname));

        FreeTactic(oldtct);
        if (cnt == p->team[0])
            InvertTactic(cnt->tactic);

        return TRUE;
    }
    return FALSE;
}
