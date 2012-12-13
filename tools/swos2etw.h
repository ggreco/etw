// SWOS data structs...

#define GIOCATORE_PELLE_NERA 0x10
#define GIOCATORE_BIONDO 0x08
#define R_RB	0x20
#define R_LB	0x40
#define R_D	0x60
#define R_RW	0x80
#define R_LW	0xa0
#define R_M	0xc0
#define R_A	0xe0

struct SWOS_Player
{
	unsigned char Nation,pad1,Number;
	char Name[23];
	unsigned char Role,pad2;
	unsigned char EmptyPassing,ShotHeading,TackleControl,SpeedFitness;
	char value; // 0-31
	char pads[5];
};

#define MAGLIA_UNITA 0
#define MAGLIA_MANICHE 1
#define MAGLIA_RIGHE_VERTICALI 2
#define MAGLIA_RIGHE_ORIZ 3

struct SWOS_Shirt
{
	char Type,Shirt1,Shirt2,Pantaloni,Calze;
	
};

struct SWOS_Team
{
	char pad[5];
	char TeamName[19];
	char Formation;
	char Division;
	struct SWOS_Shirt Shirts[2];
	char CoachName[25];
	char pad2[15];
	struct SWOS_Player Players[16];
};

// ETW data structs

#define P_DIFESA	1
#define P_CENTRO	2
#define P_ATTACCO	4
#define P_DESTRO	8
#define P_MANCINO	16
#define P_PELLE_NERA		32

#define CAMP_CUP 0
#define CAMP_LEAGUE 1
#define CAMP_WORLDCUP 2
#define CAMP_CHAMPIONSLEAGUE 3

#pragma pack(push, 2)

struct Maglia
{
	char Tipo;
	char Colore0;
	char Colore1;
	char Colore2;
};

struct Giocatore_Disk
{
	char Nome[20];
	char Cognome[20];
	long valore;
	char Numero,Velocita,Contrasto,Tiro;
	char Durata,Resistenza,Prontezza,Nazionalita;
	char Creativita,Tecnica;
	char Eta,Infortuni,Ammonizioni,Posizioni;
};

struct Portiere_Disk
{
	char Nome[20];
	char Cognome[20];
	long valore;
	char Numero,Velocita,Parata,Attenzione;
	char Nazionalita,Eta,Infortuni,Flags;
};

struct Squadra_Disk
{
	long disponibilita;
	char NumeroGiocatori,NumeroPortieri,Nazione,Flags;
	struct Maglia maglie[2];
	char Tattiche[3][16];
	char nome[52];
	char allenatore[52];
	struct Portiere_Disk portiere[3];
	struct Giocatore_Disk giocatore[21];	
};

struct Campionato_Disk
{
	char Nome[64];
	char Tipo, Scontri;
	char Vittoria,Pareggio,Sconfitta; // Punti assegnati nei vari casi
	char NumeroSquadre;
};

#pragma pack(pop)
