#include "eat.h"

object_t *object_list[DIMENSIONI_LISTA_OGGETTI] = {0};
struct DOggetto *c_list[NUMERO_OGGETTI+1] = {0};
int totale_lista=0,totale_lista_c=0; // Numero di oggetti nelle liste

// Suoni

struct SoundInfo *sound[NUMERO_SUONI+2];

char *soundname[]=
{
    "snd/shoot.wav",
    "-snd/crash.wav",
    "-snd/dog.wav",
    "snd/fischiocorto.wav",
    "snd/fischiolungo.wav",
    "+snd/crowd.wav",
    "-snd/rigore.wav",
    "+.snd/replay.wav", // Tengo in ram un po' piu' roba per default...
    "+.snd/fondo.wav",
    "+snd/boou.wav",
    "-snd/hallelujah.wav",
    "snd/pass.wav",
    "snd/shot.wav",
    "snd/palo.wav",
    "snd/ouch.wav",
    "snd/ouch2.wav",
    "-snd/bandierina.wav",
    "snd/shoot2.wav",
    "snd/shoot3.wav",
    "-snd/speed.wav",
    "-snd/glue.wav",
    "-snd/cannon.wav",
    "-snd/freeze.wav",
    "-snd/repop.wav",
    "-snd/letsgo.wav",
    NULL
};

// Statistiche

int8_t prontezza[10]={40,35,30,26,22,18,14,12,10,8};

// Vettore delle velocita'. Sono 3 come le velocita' possibili, 10 come gli
// attributi dei giocatori, 8 come le direzioni.

// Animazioni dei giocatori

int16_t Fermo[]={0};
int16_t Respira[]={8,16};
int16_t RespiraPalla[]={24,32};
int16_t CorsaParziale[]={40,48};
int16_t CorsaVeloce[]={40,48,56,64,72,80};
int16_t CorsaVelocePalla[]={88,96,104,112,120,128};
int16_t Scivolata[]={152,160,168};
int16_t Alzati[]={176,192};
int16_t TestaAvanti[]={224,248};
int16_t TestaSinistra[]={232,248};
int16_t TestaDestra[]={240,248};
int16_t PassaggioF=152,Tacco=304,CambioAnti=136,CambioOrario=144,
    PreparaRimessa=272,StopPiede=464,NonPossibile=472,RialzaCarponi=192;
int16_t RecuperaPalla[]={256,264};
int16_t Rimessa[]={272,280,288,272};
int16_t Infortunio[]={504,176,184,176,184,176,184};
int16_t DopoRimessa[]={296,296,480,472};
int16_t Tiro[]={312,320,328};
int16_t StopPetto[]={448,464};
int16_t Caduta[]={400,400,424,432,432};
int16_t Caduta2[]={416,416,424,432,432};
int16_t Caduta3[]={408,416,416};
int16_t PreTuffo[]={408};
int16_t TuffoTesta[]={416,424,432,440,440,440};
int16_t InversioneMarcia[]={136,304};
int16_t Contrasto[]={456,152,8}; // Era 152 8 8
int16_t RialzaAvanti[]={432,432,440,440,416,192};
int16_t Stacco[]={200,208,216};
int16_t Pregirata[]={336,344};
int16_t Rovesciata[]={352,360};
int16_t GirataDestra[]={368,376};
int16_t GirataSinistra[]={384,392};
int16_t Atterra[]={248,480,472};
int16_t Esultanza1[]={448,530-D_SUD_EST,531-D_SUD_EST,531-D_SUD_EST,531-D_SUD_EST,531-D_SUD_EST,531-D_SUD_EST};
int16_t Esultanza2[]={448,534-D_SUD_OVEST,535-D_SUD_OVEST,535-D_SUD_OVEST,535-D_SUD_OVEST,534-D_SUD_OVEST,535-D_SUD_OVEST};
int16_t Esultanza3[]={458,334,496,496,496,496,496,496,496,496,496,496,496};
int16_t Esultanza4[]={520,296,520,296,520,296,520,296,520,296};
int16_t Esultanza5[]={200,208,200,520,520,520,520,520,520,520,520,520,520};
int16_t Esultanza6[]={408,416,432,432,432,432,440,440,440,440,440,440,440};
int16_t Esultanza7[]={520,532-D_SUD_OVEST,533-D_SUD_OVEST,520,532-D_SUD_OVEST,533-D_SUD_OVEST,520,532-D_SUD_OVEST,533-D_SUD_OVEST,520,532-D_SUD_OVEST,533-D_SUD_OVEST,533-D_SUD_OVEST};
int16_t DisperazioneEst[]={528-D_SUD_EST,529-D_SUD_EST,529-D_SUD_EST,529-D_SUD_EST};
int16_t DisperazioneOvest[]={532-D_SUD_OVEST,533-D_SUD_OVEST,533-D_SUD_OVEST,533-D_SUD_OVEST};

struct Animazione Animation[]=
{
        {15,1,Fermo},
        {15,2,Respira},
        {15,2,RespiraPalla},
        {4,6,CorsaVeloce},
        {3,6,CorsaVeloce},
        {2,6,CorsaVelocePalla},
        {4,6,CorsaVeloce},
        {3,6,CorsaVeloce},
        {2,6,CorsaVelocePalla},
        {2,1,&CambioOrario},
        {2,1,&CambioAnti},
        {2,3,Scivolata},
        {8,2,Alzati},
        {3,2,TestaAvanti},
        {3,2,TestaSinistra},
        {3,2,TestaDestra},
        {3,4,Rimessa},
        {8,1,&Tacco},
        {4,3,Tiro},
        {6,1,&StopPiede},
        {4,2,StopPetto},
        {3,5,Caduta},
        {3,6,TuffoTesta},
        {4,2,InversioneMarcia},
        {6,3,Contrasto},
        {15,4,DopoRimessa},
        {0,1,&PreparaRimessa}, // this should last 1 frame otherwise we loose events
        {12,2,RecuperaPalla},
        {2,2,CorsaParziale},
    {6,6,RialzaAvanti},
    {0,1,Fermo}, // BATTUTA
        {2,2,InversioneMarcia},
        {3,5,Caduta2},
    {5,1,&PassaggioF},
    {2,3,Stacco},
    {3,3,Atterra},
    {50,1,Fermo},
    {2,2,Rovesciata},
    {6,1,Fermo},
    {4,1,&NonPossibile},
    {2,2,GirataSinistra},
    {2,2,GirataDestra},
    {3,2,Pregirata},
    {6,1,PreTuffo},
        {4,3,Caduta3},
    {15,1,&RialzaCarponi},
    {14,7,Infortunio},
    {10,7,Esultanza1},
    {10,7,Esultanza2},
    {5,13,Esultanza3},
    {7,10,Esultanza4},
    {5,13,Esultanza5},
    {5,13,Esultanza6},
    {5,13,Esultanza7},
    {10,7,DisperazioneEst},
    {10,7,DisperazioneOvest},
};

// Animazioni del portiere

int16_t PFermoPalla=8,PAttento=80;
int16_t PRinvioManoPrima_E[]={114-D_EST,115-D_EST,138-D_EST},
     PRinvioManoDopo_E[]={139-D_EST,0,0,0},
     PRinvioManoPrima_O[]={118-D_OVEST,119-D_OVEST,142-D_OVEST},
     PRinvioManoDopo_O[]={143-D_OVEST,0,0,0},
     PRinvioLiberoPrima_E[]={112-D_EST,113-D_EST},
     PRinvioLiberoPrima_O[]={116-D_OVEST,117-D_OVEST};
int16_t PCorsa[]={16,24,32,40,48,56};
int16_t PCamminaLaterale[]={72,80};
int16_t PRinvioPiede_E[]={136-D_EST,137-D_EST,72,80};
int16_t PRinvioPiede_O[]={140-D_OVEST,141-D_OVEST,72,80};
int16_t PRaccogliPalla[]={88,96,8};
int16_t PTuffoUscitaPrendi[]={144,152,64,64,64,64,64,216,216,8};
int16_t PTuffoUscitaManca[]={144,152,176,176,176,248,248,0};
int16_t PSaltoPrendi[]={160,168,184,192,200,208};
int16_t PSaltoManca[]={160,168,184,224,232,240};
int16_t PPrendiMezzaria[]={0,96,8}; // era 200
int16_t PTuffoDestra[]={272,280};
int16_t PTuffoSinistra[]={256,264};
int16_t PTuffoSinistraPrendi[]={296,304};
int16_t PTuffoDestraPrendi[]={312,320};
int16_t PTuffoSinistraManca[]={264,256};
int16_t PTuffoDestraManca[]={280,272};
int16_t PTerra[]={288,288,288,176,176,248,248,0};
int16_t PTerraPalla[]={64,64,64,128,128,216,216,8};

struct Animazione PortAnim[]=
{
    {2,1,Fermo},
    {2,1,&PFermoPalla},
    {3,6,PCorsa},
    {16,1,&PAttento},
    {4,4,PRinvioPiede_E},
    {4,4,PRinvioPiede_O},
    {3,3,PRinvioManoPrima_E},
    {4,4,PRinvioManoDopo_E},
    {3,3,PRinvioManoPrima_O},
    {4,4,PRinvioManoDopo_O},
    {3,2,PRinvioLiberoPrima_E},
    {3,2,PRinvioLiberoPrima_O},
    {4,3,PRaccogliPalla},
    {2,10,PTuffoUscitaPrendi},
    {2,8,PTuffoUscitaManca},
    {1,6,PSaltoPrendi},
    {2,6,PSaltoManca},
    {4,3,PPrendiMezzaria},
    {6,1,PTuffoDestra},
    {6,1,PTuffoSinistra},
    {2,2,PTuffoDestraPrendi},
    {2,2,PTuffoSinistraPrendi},
    {2,2,PTuffoDestraManca},
    {2,2,PTuffoSinistraManca},
    {2,1,PTuffoDestraManca+1},
    {2,1,PTuffoSinistraManca+1},
    {3,1,PTuffoDestraPrendi+1},
    {3,1,PTuffoSinistraPrendi+1},
    {3,2,PTuffoDestra},
    {3,2,PTuffoSinistra},
    {6,8,PTerraPalla},
    {6,8,PTerra},
    {4,2,PCamminaLaterale},
    {4,1,&PAttento},
};

// Animazioni dell'arbitro

int16_t referee_list[]={/* fischia */56,/* Fallo */ 64,65,/* Cartellino*/ 72,80,80,80,80,80,72,
                /* Corsa */ 8,16,24,32,40,48 };

struct Animazione ArbAnim[]=
{
        {15,1,Fermo},
        {3,6,&referee_list[10]},
        {20,1,&referee_list[0]},     
        {15,2,&referee_list[1]}, // 4 dir
        {8,7,&referee_list[3]} 
};

int16_t linesman_list[]={/*Fermo*/0,/* Fuori 1*/ 2, /* Fallo */ 4, /*Fuori 0 */ 6, /* Inversione */ 8,
    /* Corsa */ 10,12,14,16,18,20};

struct Animazione GLAnim[]=
{
    {15,1,linesman_list}, // GL_FERMO
    {4,6,&linesman_list[5]}, // GL_CORSA
    {10,1,&linesman_list[4]}, // GL_INVERSIONE
    {50,1,&linesman_list[2]}, // GL_FALLO
    {50,1,&linesman_list[1]}, // GL_FUORI_1
    {50,1,&linesman_list[3]}, // GL_FUORI_0
};

int32_t Pens[256];

/* Le velocita' sono in pixel * 8, in questo modo e' possibile differenziarle
   in modo migliore.
 */

int8_t opposto[]=
{
        D_SUD,
        D_SUD_OVEST,
        D_OVEST,
        D_NORD_OVEST,
        D_NORD,
        D_NORD_EST,
        D_EST,
        D_SUD_EST
};

int8_t joy_opposto[]=
{
        JPF_JOY_DOWN,
        JPF_JOY_DOWN|JPF_JOY_LEFT,
        JPF_JOY_LEFT,
        JPF_JOY_LEFT|JPF_JOY_UP,
        JPF_JOY_UP,
        JPF_JOY_UP|JPF_JOY_RIGHT,
        JPF_JOY_RIGHT,
        JPF_JOY_DOWN|JPF_JOY_RIGHT
};

int8_t dir_pred[]=
{
    7,
    0,
    1,
    2,
    3,
    4,
    5,
    6,
};

int8_t dir_next[]=
{
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    0
};

int16_t inversione_x[8][8]=
{
    {1,1,2,2,2,2,1,1},
    {-1,-2,-3,-5,-6,-7,-9,-11},
    {-2,-4,-6,-8,-10,-12,-14,-16},
    {-1,-2,-3,-5,-6,-7,-9,-11},
    {-1,-1,-2,-2,-2,-2,-1,-1},
    {1,2,3,5,6,7,9,11},
    {2,4,6,8,10,12,14,16},
    {1,2,3,5,6,7,9,11},
};

int16_t inversione_y[8][8]=
{
    {2,4,6,8,9,10,11,12},
    {1,2,3,4,5,6,7,9},
    {1,1,2,2,2,2,1,1},
    {-1,-2,-3,-4,-5,-6,-7,-9},
    {-2,-4,-6,-8,-9,-10,-11,-12},
    {-1,-2,-3,-4,-5,-6,-7,-9},
    {-1,-1,-2,-2,-2,-2,-1,-1},
    {1,2,3,4,5,6,7,9},
};

int16_t cambio_x[8]=
{
    -3,-3,-2,+2,+3,+3,+2,-2,
};

int16_t cambio_y[8]=
{
    2,-2,-3,-3,-2,+2,+3,+3,
};


int16_t avanzamento_x[8]=
{
    32, 72, 96, 72, 40, 0, -24, 0,
};

int16_t avanzamento_y[8]=
{
        88, 96, 120, 152, 160, 152, 120, 96
};

struct Rect ingombri[]=
{
    {0,0,0,0},  // Tipo 0 non definito 
    {-48,+150,+100,+180}, // GIOCATORE
    {-48,+150,+100,+180}, // PORTIERE
    {-48,+150,+100,+190}, // ARBITRO
    {-48,70,16,90}, // BANDIERINA
    {-48,+150,+100,+180}, // MASSAGGIATORE
    {-48,+150,+100,+180}, // ALLENATORE
    {-48,+150,+100,+180}, // PORTA ...
    {-48,+150,+100,+190}, // FOTOGRAFO
    {-48,+150,+100,+190}, // POLIZIOTTO
    {-48,+150,+250,+190}, // POLIZIOTTO_CANE
    {-48,+150,+100,+180}, // TUTA
    {0,0,0,0},  // LATTINA
    {0,0,0,0},  // SCARPA
    {-48,+150,+100,+180}, // CAMERAMEN
    {-48,+150,+100,+180}, // PERSONA
    {0,0,0,0},  // PALLA_RISERVA
    {0,0,0,0},  // BORSA
    {0,0,0,0},  // BONUS
    {0,0,0,0},  // GUARDALINEE
    {0,0,0,0},  // Tipo finale non definito 
};

int16_t porte_x[]={1,1236,8,1236};

int16_t quota_mod_x[MAX_QUOTA]={0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2};
int16_t quota_mod_y[MAX_QUOTA]={0,2,4,6,8,11,13,15,17,20,22,24,26,29,31,33,35,37,40,42,44,46,48};

int16_t av_palla_y[8][12]=
{
        {0,-4,-6,-8,-10,-11,-10,-8,-6,-4,-2,0},
        {0,-2,-4,-6, -7,-7,-6,-4,-3,-2,-1, 0},
        {0,0,0,0,0,0,0,0,0,0,0,0},
        {0,2,4,6,7, 7,6,4,3,2,1,0},
        {0,4,6,8,10,11,10,8,6,4,2,0},
        {0,2,4,6,7, 7,6,4,3,2,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0},
        {0,-2,-4,-6, -7,-7,-6,-4,-3,-2,-1, 0},
};

int16_t av_palla_x[8][12]=
{
        {0,0,0,0,0,0,0,0,0,0,0,0},
        {0,2,4,6,7, 7,6,4,3,2,1,0},
        {0,3,7,10,11,11,10,9,7,5,3,0},
        {0,2,4,6,7, 7,6,4,3,2,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0},
        {0,-2,-4,-6, -7,-7,-6,-4,-3,-2,-1, 0},
        {0,-3,-7,-10,-11,-11,-10,-9,-7,-5,-3,0},
        {0,-2,-4,-6, -7,-7,-6,-4,-3,-2,-1, 0},
};

// Vecchi, meno fluidi ma meno flickeranti!

char *controls[CONTROLS]=
{
    "JOYSTICK",
    "JOYPAD",
    "2B JOYSTICK",
    "KEYBOARD 1",
    "KEYBOARD 2",
    "MOUSE",
    NULL,
/*    "JOY+KEY",
    "KEYBOARD",
*/
};

int8_t slowdown[]=
{
    1, // normal
    2, // dry
    1, // hard
    2, // soft
    1, // frozen
    3, // muddy
    1, // Wet
    1, // snow
};

int8_t dist1[]={0,1,2,3,-1,-1,-1};
int8_t dist2[]={4,5,6,7,-1,-1,-1};
int8_t dist3[]={0,1,2,3};
int8_t dist4[]={4,5,6,7};
int8_t dist5[]={0,1,2,3,-1,-1,-1};
int8_t dist6[]={4,5,6,7,-1,-1,-1};
int8_t dist7[]={0,1,2,3,-1,-1,-1};
int8_t dist8[]={4,5,6,7,-1,-1,-1};
int8_t dist9[]={0,1,2,3,-1,-1};
int8_t dist10[]={4,5,6,7,-1,-1};
int8_t dist11[]={24,25,26,28,30,-1,-1,-1};
int8_t dist12[]={17,25,27,29,31,-1,-1,-1};
int8_t dist13[]={16,20,22};
int8_t dist14[]={17,21,23};
int8_t dist15[]={16,0,1,2,3,24,26,28,30,-1,-1,-1,-1,-1,-1};
int8_t dist16[]={17,4,5,6,7,25,27,29,31,-1,-1,-1,-1,-1,-1};
int8_t dist17[]={0,1,2,03,40,42,44,46,48,50,52,54,56,58,-1,-1,-1,-1,-1};
int8_t dist18[]={4,05,06,07,41,43,45,47,49,51,53,55,57,59,-1,-1,-1,-1,-1};
int8_t dist19[]={40,42,44,46,48,50,52,54,56,58,-1,-1,-1,-1,-1};
int8_t dist20[]={41,43,45,47,49,51,53,55,57,59,-1,-1,-1,-1,-1};
int8_t dist21[]={40,42,44,46,48,50,52,54,56,58};
int8_t dist22[]={41,43,45,47,49,51,53,55,57,59};
int8_t dist23[]={40,42,44,46,48,50,52,54,56,58};
int8_t dist24[]={41,43,45,47,49,51,53,55,57,59};
int8_t dist25[]={60,62,64,66,68};
int8_t dist26[]={61,63,65,67,69};
int8_t dist27[]={60,62,64,66,68,-1,-1,-1,-1};
int8_t dist28[]={61,63,64,66,68,-1,-1,-1,-1};
int8_t dist29[]={32,34,36,38,19,-1,-1,-1,-1};
int8_t dist30[]={33,35,37,39,18,-1,-1,-1,-1};
int8_t dist31[]={8,9,10,11};
int8_t dist32[]={12,13,14,15};
int8_t dist33[]={32,34,36,38,8,9,10,11,-1,-1,-1,-1,-1,-1};
int8_t dist34[]={33,35,37,39,12,13,14,15,-1,-1,-1,-1,-1,-1};
int8_t dist35[]={60,62,64,66,68,8,9,10,11,-1,-1,-1,-1,-1};
int8_t dist36[]={61,63,65,67,69,12,13,14,15,-1,-1,-1,-1,-1};
int8_t dist37[]={70,72,74,76,8,9,10,11,-1,-1,-1,-1,-1};
int8_t dist38[]={71,73,75,77,12,13,14,15,-1,-1,-1,-1,-1};
int8_t dist39[]={8,9,10,11,92,93,94,95,96,97,98,99,100,101,-1,-1,-1,-1,-1};
int8_t dist40[]={12,13,14,15,92,93,94,95,96,97,98,99,100,101,-1,-1,-1,-1,-1};
int8_t dist41[]={86,87,88,89,90,91,98,99,100,101,-1,-1,-1,-1,-1,-1};
int8_t dist42[]={86,87,88,89,90,91,98,99,100,101,-1,-1,-1,-1,-1,-1};
int8_t dist43[]={78,80,82,84,-1,-1,-1,-1,-1};
int8_t dist44[]={78,80,82,84,-1,-1,-1,-1,-1};
int8_t dist45[]={79,81,83,85,-1,-1,-1,-1,-1};
int8_t dist46[]={79,81,83,85,-1,-1,-1,-1,-1};

struct DOggetto peoples[]=
{
{200,23,7, FALSE,dist1},
{1065,24,7,FALSE,dist2},
{213,24,4,FALSE,dist3},
{1050,23,4,FALSE,dist4},
{220,22,7,FALSE,dist5},
{1055,23,7,FALSE,dist6},
{560,23,7,FALSE,dist7},
{749,23,7,FALSE,dist8},
{569,22,6,FALSE,dist9},
{732,25,6,FALSE,dist10},
{44,22,8,FALSE,dist11},
{1250,20,8,FALSE,dist12},
{22,40,3,FALSE,dist13},
{1255,39,3,FALSE,dist14},
{30,49,15,TRUE,dist15},
{1261,50,15,TRUE,dist16},
{32,100,19,TRUE,dist17},
{1249,110,19,TRUE,dist18},
{31,120,15,TRUE,dist19},
{1252,127,15,TRUE,dist20},
{30,142,10,TRUE,dist21},
{1253,143,10,TRUE,dist22},
{25,156,10,TRUE,dist23},
{1259,154,10,TRUE,dist24},
{1,364,5,TRUE,dist25},
{1270,365,5,TRUE,dist26},
{2,378,9,TRUE,dist27},
{1271,378,9,TRUE,dist28},
{0,310,9,TRUE,dist29},
{1270,307,9,TRUE,dist30},
{244,508,4,FALSE,dist31},
{1030,506,4,FALSE,dist32},
{259,507,14,FALSE,dist33},
{1009,507,14,FALSE,dist34},
{277,509,14,FALSE,dist35},
{998,507,14,FALSE,dist36},
{450,507,13,FALSE,dist37},
{850,508,13,FALSE,dist38},
{439,508,19,FALSE,dist39},
{864,509,19,FALSE,dist40},
{430,510,16,FALSE,dist41},
{855,507,16,FALSE,dist42},
{550,515,9,FALSE,dist43},
{522,515,9,FALSE,dist44},
{730,515,9,FALSE,dist45},
{755,515,9,FALSE,dist46},
};

uint8_t people_type[]=
{
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_TUTA,
    TIPO_POLIZIOTTO, /* 16 */
    TIPO_POLIZIOTTO,
    TIPO_POLIZIOTTO,
    TIPO_POLIZIOTTO,
    TIPO_POLIZIOTTO_CANE, /* 20 */
    TIPO_POLIZIOTTO_CANE,
    TIPO_POLIZIOTTO_CANE,
    TIPO_POLIZIOTTO_CANE,
    TIPO_CAMERAMEN, /* 24 */
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_CAMERAMEN,
    TIPO_FOTOGRAFO, /* 40 */
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_FOTOGRAFO,
    TIPO_MASSAGGIATORE, /* 70 */
    TIPO_MASSAGGIATORE,
    TIPO_MASSAGGIATORE,
    TIPO_MASSAGGIATORE,
    TIPO_PERSONA, /* 74 */
    TIPO_PERSONA,
    TIPO_PERSONA,
    TIPO_PERSONA,
    TIPO_ALLENATORE, /* 78 */
    TIPO_ALLENATORE,
    TIPO_ALLENATORE,
    TIPO_ALLENATORE,
    TIPO_ALLENATORE,
    TIPO_ALLENATORE,
    TIPO_ALLENATORE,
    TIPO_ALLENATORE,
    TIPO_LATTINA, /* 86 */
    TIPO_LATTINA,
    TIPO_LATTINA,
    TIPO_LATTINA,
    TIPO_LATTINA,
    TIPO_LATTINA,
    TIPO_PALLA_RISERVA, /* 92 */
    TIPO_PALLA_RISERVA,
    TIPO_SCARPA,    /* 94 */
    TIPO_SCARPA,
    TIPO_SCARPA,
    TIPO_SCARPA,
    TIPO_BORSA,    /* 98 */
    TIPO_BORSA,
    TIPO_BORSA,    
    TIPO_BORSA,
};

