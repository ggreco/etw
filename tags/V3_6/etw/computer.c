#include "etw_locale.h"
#include "menu.h"

#define NUMERO_CHEAT 8

// LONG framerate=25;

char *cheats[NUMERO_CHEAT]=
{
    "hurricane"/*-*/,
    "final"/*-*/,
    "kwick"/*-*/,
    "slow"/*-*/,
    "extra"/*-*/,
    "injury"/*-*/,
    "warp"/*-*/,
    "outro"/*-*/
};

void CheckCheat(UBYTE c )
{
    static int cheat_counter=0,current_cheat=-1;
    int i;

    if(current_cheat>=0)
    {
        if(cheats[current_cheat][cheat_counter]==c)
        {
            cheat_counter++;

            if(cheat_counter==strlen(cheats[current_cheat]))
            {
                if(!menusound[0])
                    menusound[0]=LoadSound(menu_soundname[0]);

                if(menusound[0])
                    PlayBackSound(menusound[0]);

                D(bug("Eseguo il cheat: %ld\n"/*-*/,current_cheat));

                switch(current_cheat)
                {
                    case 0:
// Gioco l'arcade con l'hurricane force!
                        if(current_menu==MENU_ARCADE_SELECTION)
                        {
                            *teamarray=ARCADE_TEAMS;
                            controllo[ARCADE_TEAMS]=1;
                            turno=0;
                            competition=MENU_CHALLENGE;
                            cb[0].ID=MENU_CHALLENGE;
                            SetupMatches();
                            ChangeMenu(MENU_CHALLENGE);
                        }
                        break;
// Mostra sempre la sequenza finale
                    case 1:
                        final=TRUE;
                        break;
                    case 2:
                        framerate=35;
                        break;
                    case 3:
                        framerate=15;
                        break;
                    case 4:
                        nopari=TRUE;
                        break;
                    case 5:
                        killer=TRUE;
                        break;
                    case 6:
                        warp=TRUE;
                        break;
#ifdef CD_VERSION
                    case 7:
                        Outro();
                        break;
#endif
                }
            }

            return;
        }
        else
        {
            D(bug("Cheat errato: %ld...(%lc invece di %lc al char %ld)\n"/*-*/,current_cheat,c,cheats[current_cheat][cheat_counter],cheat_counter));
            current_cheat=-1;
            cheat_counter=0;
        }
    }

    for(i=0;i<NUMERO_CHEAT;i++)
    {
        if(c==*cheats[i])
        {
            current_cheat=i;
            cheat_counter=1;
            D(bug("Probabile cheat: %ld\n"/*-*/,i));
            return;
        }
    }
}

WORD CalcolaPotenza(struct team_disk *s,char POS)
{
    int i;
    WORD potenza=0;

    for(i=0;i<10;i++)
    {
        if(s->players[i].Posizioni&POS)
        {
            potenza+=s->players[i].speed;

            switch(POS)
            {
                case P_ATTACCO:
                    potenza+=s->players[i].Tiro;
                    potenza+=s->players[i].technique;
                    break;
                case P_DIFESA:
                    potenza+=s->players[i].tackle;
                    potenza+=s->players[i].stamina;
                    break;
                case P_CENTRO:
                    potenza+=s->players[i].creativity;
                    potenza+=s->players[i].Durata;
                    break;
            }
        }
    }

    return potenza;
}

WORD ComputerMatch(BYTE a,BYTE b)
{
    int i,t;
    WORD gol_a=0,gol_b=0,attacco_a,attacco_b,centro_a,centro_b,difesa_a,difesa_b;

    difesa_a=CalcolaPotenza(&teamlist[a],P_DIFESA);
    centro_a=CalcolaPotenza(&teamlist[a],P_CENTRO);
    attacco_a=CalcolaPotenza(&teamlist[a],P_ATTACCO);
    difesa_b=CalcolaPotenza(&teamlist[b],P_DIFESA);
    centro_b=CalcolaPotenza(&teamlist[b],P_CENTRO);
    attacco_b=CalcolaPotenza(&teamlist[b],P_ATTACCO);

/* In campionati e coppe con andata e ritorno chi e' in casa ha un bonus di 1/5 della
   sua potenza reale
 */
    if( (competition==MENU_LEAGUE || competition==MENU_MATCHES) &&i_scontri>1)
    {
        difesa_a*=6;
        difesa_a/=5;

        centro_a*=6;
        centro_a/=5;

        attacco_a*=6;
        attacco_a/=5;
    }

    for(i=0;i<90;i++)
    {
        t=centro_a-centro_b+RangeRand(201)-100;

        if(gol_a>4||gol_b>4)
            t=t*2/3;
        else if(gol_a>2||gol_b>2)
            t=t*3/4;

        if(t>50)
        {
            t=attacco_a-difesa_b+RangeRand(81);

            if(t>40)
            {
                gol_a++;
                attacco_a-=5;
                centro_a-=5;
                difesa_a-=5;
            }
        }
        else if(t<-50)
        {
            t=attacco_b-difesa_a+RangeRand(81);

            if(t>40)
            {
                gol_b++;
                attacco_b-=5;
                centro_b-=5;
                difesa_b-=5;
            }
        }
    }

    i=max(gol_a,gol_b);

    while(i>7)
    {
        i=i*2/3;
        gol_a=gol_a*2/3;
        gol_b=gol_b*2/3;
    }

    while(gol_a>2&&gol_b>2 && (RangeRand(10)>1) )
    {
        gol_a/=2;
        gol_b/=2;
    }

    if(RangeRand(10)>3 && (gol_a>4 || gol_b>4) )
    {
        gol_a=gol_a/2;
        gol_b=gol_b/2;        
    }

    if(abs(gol_a-gol_b)>3 && RangeRand(10)>3)
    {
        gol_a/=2;
        gol_b/=2;
    }

    if(nopari&&gol_a==gol_b)
    {
        if(RangeRand(2))
            gol_b+=1;
        else
            gol_a+=1;
    }

    return (WORD) (gol_a | (gol_b<<8) );
}

char *ElaboraRisultato(int8_t a, int8_t b, uint16_t risultato)
{
    static char res[10];
    uint16_t gol_a, gol_b;

    gol_b=((risultato&0xff00)>>8);
    gol_a=risultato&0xff;

    DatiCampionato[a].Giocate++;
    DatiCampionato[b].Giocate++;

    DatiCampionato[a].GolFatti+=gol_a;
    DatiCampionato[a].GolSubiti+=gol_b;
    DatiCampionato[b].GolFatti+=gol_b;
    DatiCampionato[b].GolSubiti+=gol_a;

    if(gol_a>gol_b)
    {
        DatiCampionato[a].Vittorie+=1;
        DatiCampionato[b].Sconfitte+=1;
        DatiCampionato[a].Punti+=ppv;
        DatiCampionato[b].Punti+=pps;
    }
    else if(gol_b>gol_a)
    {
        DatiCampionato[b].Vittorie+=1;
        DatiCampionato[a].Sconfitte+=1;
        DatiCampionato[b].Punti+=ppv;
        DatiCampionato[a].Punti+=pps;
    }
    else
    {
        DatiCampionato[b].Pareggi+=1;
        DatiCampionato[a].Pareggi+=1;
        DatiCampionato[a].Punti+=ppp;
        DatiCampionato[b].Punti+=ppp;
    }

    sprintf(res,"%d-%d", gol_a, gol_b);

    return res;
}
