#include "eat.h"

tactic_t *LoadTactic(char *name)
{
    tactic_t *t;
    int i,j,k;
    FILE *fh;

    if(!(t=calloc(1,sizeof(tactic_t))))
    {
        printf("Non c'e' memoria!\n");
        return NULL;
    }

    if(!(fh=fopen(name,"rb")))
    {
        printf("Unable to find wanted tactic...\n");
        free(t);
        return NULL;
    }

    fread(&t->NameLen, sizeof(char), 1, fh);
    
    if(!(t->Name=malloc(t->NameLen+1)))
    {    
        free(t);
        fclose(fh);
        return NULL;
    }

    fread(t->Name,t->NameLen,1,fh);

    t->Name[t->NameLen]=0;

    for(i=0;i<2;i++)
        for(j=0;j<PLAYERS;j++)
            for(k=0;k<(SECTORS+SPECIALS);k++)
            {
                fread(&t->Position[i][j][k].x,sizeof(uint16_t),1,fh);
                fread(&t->Position[i][j][k].y,sizeof(uint16_t),1,fh);
                SWAP16(t->Position[i][j][k].x);
                SWAP16(t->Position[i][j][k].y);

                t->Position[i][j][k].x=(t->Position[i][j][k].x>>5)*31+280;
                t->Position[i][j][k].y=(t->Position[i][j][k].y>>2)*3-96;
                t->Position[i][j][k].settore=t->Position[i][j][k].x/2560 + (t->Position[i][j][k].y/1450 << 2);
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
                t->Position[i][j][k].settore=t->Position[i][j][k].x/2560 + (t->Position[i][j][k].y/1450 << 2);
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
