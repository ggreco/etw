#define AVAILABLE_CHANNELS 8
#define AUDIO_CROWD AVAILABLE_CHANNELS
#define AUDIO_SPEAK AVAILABLE_CHANNELS+1

#define SHOT_LENGTH 65

#define NUMERO_CORI 17

#define MYBUTTONMASK       (JPF_BUTTON_BLUE|JPF_BUTTON_RED|JPF_BUTTON_YELLOW|JPF_BUTTON_GREEN)

// Tipi di controlli

#define CTRL_JOY    0
#define CTRL_JOYPAD    1
#define CTRL_JOY2B    2
#define CTRL_KEY_1    3
#define CTRL_KEY_2    4

#define CONTROLS    (CTRL_KEY_2+2)

// Stili di gioco

#define SFONDAMENTO_CENTRALE    0
#define FASCE_LATERALE_N    1
#define FASCE_LATERALE_S    2
#define PALLA_LUNGA        3

// Comandi arcade

#define ARCADE_SPEED    1
#define ARCADE_GLUE    2
#define ARCADE_CANNON    3
#define ARCADE_FREEZE    4 // Questo e' relativo a tutta la squadra

#define MAX_ARCADE_ON_FIELD 4 // Massimo numero di bonus sul campo contemporaneamente
#define ARCADE_REPOP_TIME 500 // Ripoppa un simbolo ogni 10 secondi;

// Comandi per i giocatori

#define NESSUN_COMANDO    0
#define STAI_BARRIERA    1
#define STAI_FERMO    2
#define ESEGUI_ROTAZIONE 3
#define ESEGUI_TIRO    4
#define ESEGUI_PASSAGGIO 5
#define VAI_PALLA    6
#define ESCI_AREA    7
#define ESEGUI_RIMESSA  8
#define ESEGUI_BATTUTA  9
#define MANTIENI_DISTANZA 10
#define ESCI_CAMPO     11
#define ASPETTA_PALLA    12

// Comandi per l'arbitro 

#define FISCHIA_INIZIO    1
#define FISCHIA_FINE    2
#define FISCHIA_GOL    3
#define FISCHIA_RIGORE    4
#define FISCHIA_FUORI   5
#define ESPULSIONE    6
#define AMMONIZIONE    7
#define FISCHIA_RIPRESA 8
#define FISCHIA_FALLO    9
#define BATTI_FALLO    10
#define METTI_IN_GIOCO  11
#define FISCHIA_REPLAY  12 /* Usato per attivare il replay dopo il gol */
#define FISCHIA_SCRITTA 13
#define FISCHIA_KICKOFF 14
#define FISCHIA_PREPUNIZIONE 15
#define FISCHIA_PRIMO_TEMPO 16
#define FISCHIA_OFFSIDE 17

// Pallone

#define TIRO_RASOTERRA    0
#define TIRO_ALTO    1
#define TIRO_PALLONETTO 2
#define TIRO_CORNER    3
#define TIRO_RIMESSA    4
#define TIRO_PASSAGGIO_SMARCANTE 5

// Palette pens of the GAME (not menus!)

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

#define COLORE_STANDARD_MAGLIE P_ROSSO1
#define COLORE_STANDARD_CALZONI P_BLU2
#define COLORE_STANDARD_AUSILIARIO P_GIALLO

#define RADAR_TEAM_A P_BLU2
#define RADAR_TEAM_B P_ROSSO0
#define RADAR_PALLA P_BIANCO

#define MARKER_TEAM_B 10

// Direzioni per FindDirection ed altro...

#define D_NORD 0
#define D_NORD_EST 1
#define D_EST  2
#define D_SUD_EST 3
#define D_SUD 4
#define D_SUD_OVEST  5
#define D_OVEST 6
#define D_NORD_OVEST 7

// Numero di oggetti statici, definiti in data.c

#define NUMERO_OGGETTI 46

// Tipi per object type (estendibili)

#define TIPO_GIOCATORE 1
#define TIPO_PORTIERE 2
#define TIPO_ARBITRO 3
#define TIPO_BANDIERINA 4
#define TIPO_MASSAGGIATORE 5
#define TIPO_ALLENATORE 6
#define TIPO_PORTA 7
#define TIPO_FOTOGRAFO 8
#define TIPO_POLIZIOTTO 9
#define TIPO_POLIZIOTTO_CANE 10
#define TIPO_TUTA 11
#define TIPO_LATTINA 12
#define TIPO_SCARPA 13
#define TIPO_CAMERAMEN 14
#define TIPO_PERSONA 15
#define TIPO_PALLA_RISERVA 16
#define TIPO_BORSA 17
#define TIPO_BONUS 18
#define TIPO_GUARDALINEE 19
#define TIPO_INDEFINITO 99

#define DIMENSIONI_LISTA_OGGETTI 10

// Per detail_level (bits)

#define USA_RADAR    1
#define USA_ARBITRO    2
#define USA_POLIZIOTTI    4
#define USA_FOTOGRAFI    8
#define USA_NOMI    16
#define USA_RISULTATO    32
#define USA_GUARDALINEE    64

// Sponsors e spalti...

#define SPONSOR_Y_POS   27
#define SPONSOR_X_2    751
#define SPONSOR_X    106 
// era 106
#define SPONSOR_Y    9
#define SPONSORS    30

#define SPALTI        6
#define SPALTI_Y    27
#define SPALTI_X    290
#define SPALTI_X_2    666

// Campo...

#define FIELD_WIDTH    1280
#define FIELD_HEIGHT    544

#define TIPI_PORTE    2
#define PORTE_Y        219

#define SOFT_PASS    16     //di nuovo 16 per star dietro ai tiri

// Relativi ai nomi

#define CHAR_X 6
#define CHAR_Y 6
#define MAX_LEN 20

// Carattere verysmall.

#define VS_CHAR_X 4
#define VS_CHAR_Y 5

// Quota

#define MAX_QUOTA 23

// posizioni palla per Corners, Goalkicks, rigori...

#define CENTROCAMPO_Y (270*8)
#define CENTROCAMPO_X (637*8)

#define CORNER_X_NO (69*8)
#define CORNER_X_NE (1201*8)
#define CORNER_X_SO (31*8)
#define CORNER_X_SE (1243*8)
#define CORNER_Y_N (49*8)
#define CORNER_Y_S (497*8)

#define FONDO_O_X (40*8)
#define FONDO_E_X (1230*8)

#define GOALKICK_Y_S (346*8)
#define GOALKICK_X_SO (100*8)
#define GOALKICK_X_SE (1176*8)
#define GOALKICK_Y_N (198*8)
#define GOALKICK_X_NO (110*8)
#define GOALKICK_X_NE (1166*8)

#define RIMESSA_Y_N (45*8)
#define RIMESSA_Y_S (501*8)

#define GOAL_Y_N (238*8)

// Era 239

#define GOAL_Y_S (301*8)

// era 300

#define PORTA_Y CENTROCAMPO_Y
#define PORTA_O_Y PORTA_Y
#define PORTA_O_X (40*8)
#define PORTA_E_Y PORTA_Y
#define PORTA_E_X (1235*8)

#define AREA_RIGORE_Y_N (134*8)
#define AREA_RIGORE_Y_S (414*8)
#define AREA_RIGORE_X_O (191*8)
#define AREA_RIGORE_X_E (1083*8)

#define RIGORE_X_E (1113*8)
#define RIGORE_X_O (159*8)

#define RIGORE_Y CENTROCAMPO_Y

// Tipi di pannelli

#define PANEL_TIME    1
#define PANEL_RESULT    2
#define PANEL_GOAL    4
#define PANEL_PAUSE    8
#define PANEL_KICKOFF    16
#define PANEL_STATS    32
#define PANEL_REPLAY    64
#define PANEL_CARD    128
#define PANEL_SUBSTITUTION 256
#define PANEL_CHANGE_TACTIC 512
#define PANEL_SUBSTITUTION_2 1024
#define PANEL_INJURIED  2048
#define PANEL_SUBSTITUTION_3 4096
#define PANEL_PLAY_INJURIED (1<<16)
#define PANEL_OFFSIDE (1<<17)
#define PANEL_CHAT (1<<18)
#define PANEL_DISPLAYCHAT (1<<19)

// Il numero di frames che dura il pannello del gol!

#define GOAL_TIME 50

#define MAX_PLAYERS 2

#define NUMERO_STADI 8

// Valori di CanScore

#define CS_NO    0
#define CS_SI    1
#define CS_CROSS 2

// Stati della partita

#define S_TEAM_A        -3
#define S_TEAM_B        -4
#define S_NOMESQUADRA        -2
#define S_RESET            -1
#define S_NON_INIZIATO        0
#define S_ENTRATA_IN_CAMPO    1
#define S_CALCIO_D_INIZIO    2
#define S_CALCIO_D_ANGOLO    3
#define S_CALCIO_DI_PUNIZIONE    4
#define S_CALCIO_DI_RIGORE    5
#define S_GOAL_TEAM_A    6
#define S_GOAL_TEAM_B    7
#define S_PALLA_TIRATA        8
#define S_TRAVERSA_COLPITA    9
#define S_PALO_COLPITO        10
#define S_PARATA_NON_BLOCCA    11
#define S_PARATA        12
#define S_RIMESSA_DAL_FONDO    13
#define S_RIMESSA_LATERALE    14
#define S_INIZIO_REPLAY        15
#define S_FINE_PARTITA        16
#define S_TIRO            17
#define S_PALLA_PERSA        18
#define S_FINE_PRIMO_TEMPO    19
#define S_AMMONIZIONE        20
#define S_ESPULSIONE        21
#define S_USCITA        22
#define S_FINE_PRIMO        23
#define S_FINE_SECONDO        24
#define S_ULTIMA_AZIONE        25

// Defines per player_type

#define TYPE_COMPUTER        -1
#define TYPE_JOYSTICK0        0
#define TYPE_JOYSTICK1        1

#define FINAL_WIDTH    256
#define FINAL_REAL_WIDTH 248
#define FINAL_HEIGHT    173


// Goal array size, numero massimo di marcature memorizzate

#define GA_SIZE    48
