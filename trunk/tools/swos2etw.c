/*
 * SWOS2ETW 1.0, written by Gabriele Greco of Hurricane Studios
 *
 * This program converts SWOS team files to ETW team files.
 *
 * This program is Public Domain.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "swos2etw.h"

#include <arpa/inet.h>

#define LNIBBLE(x) (x&0x0f)
#define HNIBBLE(x) ((x>>4)&0x0f)

// SWOS stats are from 0-15, ETW ones from 0-9 (but 0-1 are reserved!)

#define ConvertHi(x) (HNIBBLE(x)/2+2)
#define ConvertLo(x) (LNIBBLE(x)/2+2)

char VERSION[]="$VER: swos2etw 1.3 (29.12.98) by Gabriele Greco\n";

struct Nation
{
	char Name[4];
	char number;
};

char nation_array[50];

struct Nation nations[]=
{
	"ALB",29, //no!
	"AUT",11,
	"BEL",5,
	"BUL",29,
	"CRO",18,
	"CYP",19, //no!
	"TCH",13,
	"DEN",24,
	"ENG",3,
	"EST",15, //no!
	"FAR",3, //no!
	"FIN",22,
	"FRA",6,
	"GER",10,
	"GRE",19,
	"HUN",29, //no!
	"ISL",22, //no!
	"ISR",19, //no!
	"ITA",0,
	"LAT",0, //no!
	"LIT",15, //no!
	"LUX",10, //no!
	"MLT",0, //no!
	"HOL",4,
	"NIR",22, //no!
	"NOR",22,
	"POL",25,
	"POR",8,
	"ROM",20,
	"RUS",15,
	"SMR",15, //no!
	"SCO",31,
	"SLO",14,
	"SWE",21,
	"TUR",19, //no!
	"UKR",16,
	"WAL",3, //no!
	"YUG",17,
	"BLS",15, //no!
	"SVK",16, //no!
	"ESP",7,
	"ARM",15, //no!
	"BOS",17, //no!
	"AZB",16, //no!
	"GEO",15, //no!
	"SWI",12,
	"IRL",10,
	"MAC",10, //no!
	"TRK",19, //no!
	"LIE",15, //no!
	"MOL",16, //no!
	"CRC",15, //no!
	"SAL",16, //no!
	"GUA",26, //no!
	"HON",39, //no!
	"BHM",26, //no!
	"MEX",30,
	"PAN",44, //no!
	"USA",27,
	"BAH",27, //no!
	"NIC",26, //no!
	"BER",26, //no!
	"JAM",44,
	"TRI",42, //no!
	"CAN",28,
	"BAR",42, //no!
	"ELS",39, //no!
	"SVC",39, //no!
	"ARG",2,
	"BOL",26, //no!
	"BRA",1,
	"CHL",33,
	"COL",42,
	"ECU",33, //no!
	"PAR",39,
	"SUR",33, //no!
	"URU",26,
	"VEN",33, //no!
	"GUY",26, //no!
	"PER",42, //no!
	"ALG",41, //no!
	"SAF",42, //no!
	"BOT",35, //no!
	"BFS",41, //no!
	"BUR",35, //no!
	"LES",32, //no!
	"ZAI",37, //no!
	"ZAM",37, //no!
	"GHA",37, //no!
	"SEN",35, //no!
	"CIV",35, //no!
	"TUN",41,
	"MLI",37, //no!
	"MDG",35, //no!
	"CMR",35, //no!
	"CHD",35, //no!
	"UGA",37, //no!
	"LIB",41, //no!
	"MOZ",32, //no!
	"KEN",35, //no!
	"SUD",35,
	"SWA",32, //no!
	"ANG",35, //no!
	"TOG",37, //no!
	"ZIM",37, //no!
	"EGY",32, //no!
	"TAN",37, //no!
	"NIG",37,
	"ETH",35, //no!
	"GAB",37, //no!
	"SIE",37, //no!
	"BEN",37, //no!
	"CON",41, //no!
	"GUI",32, //no!
	"SRL",32, //no!
	"MAR",32,
	"GAM",37, //no!
	"MLW",43, //no!
	"JAP",43,
	"TAI",43, //no!
	"IND",43, //no!
	"BAN",36, //No
	"BRU",36, //No
	"IRA",40, //no!
	"JOR",40, //no!
	"SRI",40, //no!
	"SYR",40, //no!
	"KOR",43, //no!
	"IRN",40,
	"VIE",43, //no!
	"MLY",36, //No
	"SAU",36,
	"YEM",43, //no!
	"KUW",40, //no!
	"LAO",43, //no!
	"NKR",40, //no!
	"OMA",36, //no!
	"PAK",36, //no!
	"PHI",36, //no!
	"CHN",40, //no!
	"SGP",36, //no!
	"MAU",36, //no!
	"MYA",36, //no!
	"PAP",36, //no!
	"TAD",36, //no!
	"UZB",36, //no!
	"QAT",36, //no!
	"UAE",37, // no!
	"AUS",36, //no!
	"NZL",36, //no!
	"FIJ",36, //no!
	"SOL",36, //no!
	"CUS",36, //no!
};

long costs[]=
{
	15,
	25,
	30,
	40,
	50,
	65,
	75,
	85,
	100,
	110,
	130,
	160,
	180,
	200,
	225,
	250,
	300,
	350,
	450,
	500,
	550,
	600,
	650,
	700,
	750,
	800,
	850,
	950,
	1000,
	1100,
	1300,
	1500,
	1600,
	1800,
	1900,
	2000,
	2250,
	2750,
	3000,
	3500,
	4500,
	5000,
	6000,
	7000,
	8000,
	9000,
	10000,
	12000,
	15000,
	20000,
};

#define P_VERDE0 0
#define P_BIANCO 1
#define P_VERDE1 2
#define P_VERDE2 3
#define P_NERO  4
#define P_ARANCIO1 5
#define P_BLU2   6
#define P_ROSSO1 7
#define P_BLU1 8
#define P_ROSSO0 9
#define P_ROSSO2 10
#define P_ARANCIO0 11
#define P_GIALLO 12
#define P_GRIGIO1 13
#define P_GRIGIO0 14
#define P_BLU0 15

unsigned char color_table[]=
{
	P_GRIGIO0,
	P_BIANCO,
	P_NERO,
	P_ARANCIO0,
	P_ROSSO0,
	P_BLU1,
	P_ROSSO1,
	P_BLU0,
	P_VERDE0,
	P_GIALLO
};

char *tactics[]=
{
	"4-4-2",
	"5-4-1",
	"5-4-1", // 4-5-1 missing
	"5-3-2",
	"3-5-2",
	"4-3-3",
	"4-3-3", // 4-2-4
	"3-4-3",
	"5-4-1", // sweep
	"5-3-2", // 5-2-3
	"3-4-3", // at
	"5-4-1", // def
	"4-4-2",
	"4-4-2",
	"4-4-2",
	"4-4-2",
	"4-4-2",
	"4-4-2",

};

struct Campionato_Disk camp=
{
	"Imported from SWOS",
	CAMP_LEAGUE,2,3,1,0,
	0
};

unsigned ConvertRole(unsigned char role)
{
	if((role&R_A)==R_A)
		return P_ATTACCO;
	else if((role&R_M)==R_M)
		return P_CENTRO;
	else if((role&R_D)==R_D)
		return P_DIFESA;
	else if((role&R_LW)==R_LW)
		return P_CENTRO|P_MANCINO;
	else if((role&R_RW)==R_RW)
		return P_CENTRO|P_DESTRO;
	else if((role&R_LB)==R_LB)
		return P_DIFESA|P_MANCINO;
	else if((role&R_RB)==R_RB)
		return P_DIFESA|P_DESTRO;
	else
		return 0;
}

void AdjustFormation(struct Squadra_Disk *s)
{
	int attack,center,defence,la,lc,ld,i;
	struct Giocatore_Disk temp;

	attack=s->Tattiche[0][4]-'0';
	defence=s->Tattiche[0][0]-'0';
	center=s->Tattiche[0][2]-'0';

	if((defence+attack+center)!=10)
	{
		attack=2;
		defence=4;
		center=4;
	}

	la=9;
	lc=la-attack;
	ld=lc-center;

	for(i=0;i<s->NumeroGiocatori&&attack>0;i++)
	{
		if(s->giocatore[i].Posizioni&P_ATTACCO)
		{
			temp=s->giocatore[i];
			s->giocatore[i]=s->giocatore[la];
			s->giocatore[la]=temp;

			attack--;
			la--;
		}
	}

	for(i=0;i<s->NumeroGiocatori&&center>0;i++)
	{
		if(s->giocatore[i].Posizioni&P_CENTRO)
		{
			temp=s->giocatore[i];
			s->giocatore[i]=s->giocatore[lc];
			s->giocatore[lc]=temp;

			center--;
			lc--;
		}
	}

	for(i=0;i<s->NumeroGiocatori&&defence>0;i++)
	{
		if(s->giocatore[i].Posizioni&P_DIFESA)
		{
			temp=s->giocatore[i];
			s->giocatore[i]=s->giocatore[ld];
			s->giocatore[ld]=temp;

			defence--;
			ld--;
		}
	}
}

void ConvertTeam(struct SWOS_Team *src,struct Squadra_Disk *dest)
{
	unsigned char role;
	char *tmp_c;
	int i,j;

	memset(dest,0,sizeof(struct Squadra_Disk));
	memset(nation_array,0,sizeof(nation_array));

	strcpy(dest->nome,src->TeamName);
	strcpy(dest->allenatore,src->CoachName);
	
	if(src->Division)
		dest->Flags=1<<(src->Division-1);
	else
		dest->Flags=0;

	for(i=0;i<3;i++)
		strcpy(dest->Tattiche[i],tactics[src->Formation]);

	for(i=0;i<2;i++)
	{
		dest->maglie[i].Colore1=color_table[src->Shirts[i].Pantaloni];

		switch(src->Shirts[i].Type)
		{
			case 1:
				dest->maglie[i].Tipo=2;
				dest->maglie[i].Colore2=color_table[src->Shirts[i].Shirt1];
				dest->maglie[i].Colore0=color_table[src->Shirts[i].Shirt2];
				break;
			case 2:
				dest->maglie[i].Tipo=1;
				dest->maglie[i].Colore0=color_table[src->Shirts[i].Shirt1];
				dest->maglie[i].Colore2=color_table[src->Shirts[i].Shirt2];
				break;
			default:
				dest->maglie[i].Tipo=0;
				dest->maglie[i].Colore0=color_table[src->Shirts[i].Shirt1];
		}
	}

	for(i=0;i<16;i++)
	{
		char *tmp_n=src->Players[i].Name;
		j=0;

		while(tmp_n[j]>' ')
			j++;

		if(tmp_n[j]==0)
		{
// j==0 -> empty slot
			if(j==0)
				continue;

			tmp_c=tmp_n;
			tmp_n=NULL;
		}
		else
		{
			tmp_c=(&tmp_n[j])+1;
			tmp_n[j]=0;
		}

		nation_array[nations[src->Players[i].Nation].number]++;

		if(!(role=ConvertRole(src->Players[i].Role)))
		{
// Player i is a gk...
			if(dest->NumeroPortieri>2)
			{
				printf("Too many keepers in %s, ETW supports up to 3 GK!\n",src->TeamName);
				exit(20);
			}

			if(tmp_n)
				strcpy(dest->portiere[dest->NumeroPortieri].Nome,tmp_n);
			else
				*dest->portiere[dest->NumeroPortieri].Nome=0;

			strcpy(dest->portiere[dest->NumeroPortieri].Cognome,tmp_c);

			dest->portiere[dest->NumeroPortieri].Nazionalita=nations[src->Players[i].Nation].number;
			dest->portiere[dest->NumeroPortieri].Numero=src->Players[i].Number;
			dest->portiere[dest->NumeroPortieri].valore=htonl(costs[src->Players[i].value]);

			if(src->Players[i].value>46)
			{
				dest->portiere[dest->NumeroPortieri].Velocita=9;
				dest->portiere[dest->NumeroPortieri].Parata=9;
				dest->portiere[dest->NumeroPortieri].Attenzione=9;
			}
			else if(src->Players[i].value>40)
			{
				dest->portiere[dest->NumeroPortieri].Velocita=8;
				dest->portiere[dest->NumeroPortieri].Parata=8;
				dest->portiere[dest->NumeroPortieri].Attenzione=9;
			}
			else if(src->Players[i].value>35)
			{
				dest->portiere[dest->NumeroPortieri].Velocita=7;
				dest->portiere[dest->NumeroPortieri].Parata=8;
				dest->portiere[dest->NumeroPortieri].Attenzione=8;
			}
			else if(src->Players[i].value>30)
			{
				dest->portiere[dest->NumeroPortieri].Velocita=7;
				dest->portiere[dest->NumeroPortieri].Parata=7;
				dest->portiere[dest->NumeroPortieri].Attenzione=8;

			}
			else if(src->Players[i].value>25)
			{
				dest->portiere[dest->NumeroPortieri].Velocita=6;
				dest->portiere[dest->NumeroPortieri].Parata=7;
				dest->portiere[dest->NumeroPortieri].Attenzione=8;
			}
			else if(src->Players[i].value>20)
			{
				dest->portiere[dest->NumeroPortieri].Velocita=6;
				dest->portiere[dest->NumeroPortieri].Parata=7;
				dest->portiere[dest->NumeroPortieri].Attenzione=7;
			}
			else if(src->Players[i].value>18)
			{
				dest->portiere[dest->NumeroPortieri].Velocita=6;
				dest->portiere[dest->NumeroPortieri].Parata=6;
				dest->portiere[dest->NumeroPortieri].Attenzione=6;
			}
			else if(src->Players[i].value>14)
			{
				dest->portiere[dest->NumeroPortieri].Velocita=5;
				dest->portiere[dest->NumeroPortieri].Parata=5;
				dest->portiere[dest->NumeroPortieri].Attenzione=5;
			}
			else if(src->Players[i].value>10)
			{
				dest->portiere[dest->NumeroPortieri].Velocita=4;
				dest->portiere[dest->NumeroPortieri].Parata=5;
				dest->portiere[dest->NumeroPortieri].Attenzione=4;
			}
			else if(src->Players[i].value>8)
			{
				dest->portiere[dest->NumeroPortieri].Velocita=4;
				dest->portiere[dest->NumeroPortieri].Parata=4;
				dest->portiere[dest->NumeroPortieri].Attenzione=4;

			}
			else if(src->Players[i].value>4)
			{
				dest->portiere[dest->NumeroPortieri].Velocita=3;
				dest->portiere[dest->NumeroPortieri].Parata=4;
				dest->portiere[dest->NumeroPortieri].Attenzione=3;
			}
			else
			{
				dest->portiere[dest->NumeroPortieri].Velocita=2;
				dest->portiere[dest->NumeroPortieri].Parata=2;
				dest->portiere[dest->NumeroPortieri].Attenzione=2;
			}

			dest->NumeroPortieri++;
		}
		else
		{
			if(tmp_n)
				strcpy(dest->giocatore[dest->NumeroGiocatori].Nome,tmp_n);
			else
				*dest->giocatore[dest->NumeroGiocatori].Nome=0;

			strcpy(dest->giocatore[dest->NumeroGiocatori].Cognome,tmp_c);

			dest->giocatore[dest->NumeroGiocatori].Nazionalita=nations[src->Players[i].Nation].number;
			dest->giocatore[dest->NumeroGiocatori].Numero=src->Players[i].Number;
			dest->giocatore[dest->NumeroGiocatori].valore=htonl(costs[src->Players[i].value]);

			dest->giocatore[dest->NumeroGiocatori].Posizioni=role | ( (src->Players[i].Role&GIOCATORE_PELLE_NERA) ? P_PELLE_NERA : 0L) ;

			dest->giocatore[dest->NumeroGiocatori].Velocita=ConvertHi(src->Players[i].SpeedFitness);
			dest->giocatore[dest->NumeroGiocatori].Contrasto=ConvertHi(src->Players[i].TackleControl);
			dest->giocatore[dest->NumeroGiocatori].Tiro=ConvertHi(src->Players[i].ShotHeading);
			dest->giocatore[dest->NumeroGiocatori].Durata=ConvertLo(src->Players[i].SpeedFitness);
			dest->giocatore[dest->NumeroGiocatori].Resistenza=ConvertLo(src->Players[i].ShotHeading); // Resistenza is Heading!
			dest->giocatore[dest->NumeroGiocatori].Creativita=ConvertLo(src->Players[i].EmptyPassing);
			dest->giocatore[dest->NumeroGiocatori].Tecnica=ConvertLo(src->Players[i].TackleControl);
			dest->giocatore[dest->NumeroGiocatori].Prontezza=ConvertHi(src->Players[i].SpeedFitness);

// SWOS doesn't support player age...

			dest->giocatore[dest->NumeroGiocatori].Eta=19+(rand()%13);

			dest->NumeroGiocatori++;
		}
	}

// Trovo la nazionalita'...

        role=99;
	j=0;

	for(i=0;i<sizeof(nation_array);i++)
	{
		if(nation_array[i]>j)
		{
			j=nation_array[i];
			role=i;
                }
	}

	if(role==99)
		role=0;

	dest->Nazione=role;

	AdjustFormation(dest);
}

int main(int argc, char *argv[])
{
	FILE *f,*out;
	struct SWOS_Team swos_team;
	struct Squadra_Disk etw_team;

	if(argc<3||argc>4)
	{
		printf("Usage: %s SWOS-TeamFile ETW-TeamFile [League Name]\n",argv[0]);
		exit(0);
	}

	if(argc==4)
		strcpy(camp.Nome,argv[3]);

	if(f=fopen(argv[1],"rb"))
	{
		short team_number;
		int i,k,allow_skip=0;

		fread(&team_number,sizeof(short),1,f);

        team_number = ntohs(team_number);

        if(team_number==0x524e) {
			printf("This file is packed with RNC, you have to depack it!\n");
			exit(0);
		}
		printf("%d teams found in <%s>...\n",team_number,argv[1]);

		if(!(out=fopen(argv[2],"wb")))
		{
			printf("Unable to create the output file <%s>!\n",argv[2]);
			exit(20);
		}

		if(team_number<1)
		{
			printf("Incorrect team number!\n");
			exit(20);
		}

		if(team_number>64)
		{
			printf("WARNING, more than 64 teams (%d), skipping the ones with div = 0\n",team_number);
			allow_skip=1;
		}

		camp.NumeroSquadre=team_number-1;

		fwrite(&camp,sizeof(struct Campionato_Disk),1,out);

		for(i=0,k=0;i<team_number && k<64;i++)
		{
			fread(&swos_team,sizeof(struct SWOS_Team),1,f);

			if(allow_skip && swos_team.Division>1 )
				continue;

			k++;
			ConvertTeam(&swos_team,&etw_team);
			fwrite(&etw_team,sizeof(struct Squadra_Disk),1,out);
		}

		if(allow_skip)
		{
			if(k==64)
				printf("WARNING, could not reach the end of the file (too many teams!)\n");

			printf("%d teams skipped!\n",team_number-k);

			camp.NumeroSquadre=k-1;
			fseek(out,0,SEEK_SET);
			fwrite(&camp,sizeof(struct Campionato_Disk),1,out);
		}

		printf("%d teams written to <%s>\n",k,argv[2]);

		fclose(out);
		fclose(f);
	}
	else
	{
		printf("Unable to open source file!\n");
		exit(20);
	}
}
