#include "etw_locale.h"
#include "menu.h"

struct SoundInfo *menusound[NUMERO_SUONI_MENU+1];

char *menu_soundname[]=
{
	"+intro/bombdrop.wav"/*-*/,
	"intro/engine.wav"/*-*/,
	"+intro/brake.wav"/*-*/,
	"-.arcadesnd/t0.wav"/*-*/,
	"-.arcadesnd/t1.wav"/*-*/,
	"-.arcadesnd/t2.wav"/*-*/,
	"-.arcadesnd/t3.wav"/*-*/,
	"-.arcadesnd/t4.wav"/*-*/,
	"-.arcadesnd/t5.wav"/*-*/,
	"-.arcadesnd/t6.wav"/*-*/,
	"-.arcadesnd/t7.wav"/*-*/,
	"-.arcadesnd/t8.wav"/*-*/,
	"-.arcadesnd/t9.wav"/*-*/,
	"-.arcadesnd/t10.wav"/*-*/,
	"+intro/buuan.wav"/*-*/,
	"+intro/swam.wav"/*-*/,
	"+intro/swam2.wav"/*-*/,
	"+intro/ground.wav"/*-*/,
	NULL,
};

/* Starting from version 1.2 every menu has its own list of buttons, this method permit a
 * VERY simpler way to add voices to any menu!
 */

struct Bottone main_bottoni[]=
{
	{27,76,150,91,4,7,9,msg_85}, // Main menu
	{27,111,150,126,MENU_PREFS,3,0,msg_86},
	{27,148,150,163,3,6,8,msg_87},
	{172,76,295,91,2,11,12,msg_88},
	{172,111,295,126,10,8,15,msg_89},
	{172,148,295,163,-1,9,11,msg_90},
	{70,224,250,236,ID_QUIT_GAME,14,13,msg_91},
};

struct Bottone prefs_bottoni[]=
{
	{20,67,150,80,MENU_GAME_PREFS,3,0,msg_92},  // Preferences
	{20,93,150,106,MENU_VIDEO_PREFS,0,13,msg_93},
	{20,119,150,132,MENU_AUDIO_PREFS,0,13,msg_94},
	{20,145,150,158,MENU_SYSTEM_PREFS,0,13,"SYSTEM"},
	{72,210,246,222,0,14,13,msg_95},
	{72,229,246,241,-1,9,11,msg_96},
};

struct Bottone high_bottoni[]=
{
	{20,67,150,80,MENU_HIGH_SELECTION,11,12,msg_97},  // Highlights
	{20,93,150,106,MENU_HIGH_SELECTION,11,12,msg_98},
	{20,119,150,132,-1,11,12,msg_99},
	{72,210,246,222,0,14,13,msg_95},
};

struct Bottone teams_bottoni[]=
{
	{20,67,150,80,-1,6,8,msg_100},  // Teams
	{20,93,150,106,-1,6,8,msg_99},
	{20,119,150,132,-1,6,8,msg_98},
	{20,145,150,158,-1,8,15,msg_101},
	{72,210,246,222,0,14,13,msg_95},
	{72,229,246,241,-1,9,11,msg_96},
};

struct Bottone start_bottoni[]=
{
	{20,57,150,70,MENU_ARCADE,7,9,msg_102},  // Game Start
	{20,83,150,96,MENU_SIMULATION,7,9,msg_103},
	{20,109,150,122,-1,7,9,msg_104},
	{20,135,150,148,MENU_TEAM_SELECTION,9,11,"NETWORK PLAY"},
	{20,161,150,175,MENU_TRAINING,9,11,msg_105},
	{72,210,246,222,MENU_MAIN_MENU,14,13,msg_95},
};

struct Bottone arcade_bottoni[]=
{
	{20,67,150,80,MENU_SELECTION_TYPE,7,9,msg_106},  // Arcade
	{20,93,150,106,MENU_SELECTION_TYPE,7,9,msg_107},
	{20,119,150,132,MENU_ARCADE_SELECTION,7,9,msg_1},
	{72,210,246,222,4,14,13,msg_108},
};

struct Bottone sim_bottoni[]=
{
	{20,47,150,60,0,7,9,NULL},  // Simulation
	{20,73,150,86,MENU_TEAM_SELECTION,7,9,msg_109},
	{20,99,150,112,MENU_TEAM_SELECTION,7,9,msg_110},
	{20,125,150,138,MENU_TEAM_SELECTION,7,9,msg_111},
	{20,151,150,164,-1,9,11,msg_99},
	{20,177,150,190,-1,9,11,msg_180},
	{72,210,246,222,4,14,13,msg_108},
};

struct Bottone career_bottoni[]=
{
	{20,67,150,80,MENU_TEAM_SELECTION,7,9,msg_112},  // Career
	{20,93,150,106,MENU_TEAM_SELECTION,7,9,msg_113},
	{20,119,150,132,MENU_TEAM_SELECTION,7,9,msg_114},
	{20,145,150,158,-1,9,11,msg_99},
	{20,171,150,184,-1,9,11,NULL},
	{72,210,246,222,4,14,13,msg_108},
};

struct Bottone train_bottoni[]=
{
	{20,67,150,80,MENU_TEAM_SELECTION,7,9,msg_115},  // Training
	{20,93,150,106,MENU_TEAM_SELECTION,7,9,msg_116},
	{20,119,150,132,MENU_TEAM_SELECTION,7,9,msg_117},
	{72,210,246,222,4,14,13,msg_108},
};

struct Bottone score_bottoni[]=
{
	{20,67,150,80,MENU_VIEW_SCORES,8,15,msg_97},  // Scores
	{20,93,150,106,-1,8,15,msg_118},
	{72,210,246,222,MENU_MAIN_MENU,14,13,msg_95},
};

struct Bottone gprefs_bottoni[]=
{
	{10,64,100,71,-1,0,3,msg_119},{101,64,184,71,-1,3,0,NULL},
	{10,75,100,82,-1,0,3,msg_120},{101,75,184,82,-1,3,0,NULL},
	{10,86,100,93,-1,0,3,msg_121},{101,86,184,93,-1,3,0,NULL},
	{10,97,100,104,-1,0,3,msg_122},{101,97,184,104,-1,3,0,NULL},
	{10,108,100,115,-1,0,3,msg_123},{101,108,184,115,-1,3,0,NULL},
	{10,119,100,126,-1,0,3,msg_124},{101,119,184,126,-1,3,0,NULL},
	{10,130,100,137,-1,0,3,msg_125},{101,130,184,137,-1,3,0,NULL},
	{10,141,100,148,-1,0,3,msg_126},{101,141,184,148,-1,3,0,NULL},
	{10,152,100,159,-1,0,3,msg_129},{101,152,184,159,-1,3,0,msg_129},
	{10,163,100,170,-1,0,3,msg_181},{101,163,184,170,-1,3,0,msg_181},
	{10,174,100,181,-1,0,3,NULL},{101,174,184,181,-1,3,0,NULL},
	{10,185,100,192,-1,0,3,"CHANGE ACTIVE"},{101,185,184,192,-1,3,0,NULL},
	{10,196,100,203,-1,0,3,"OFFSIDE"},{101,196,184,203,-1,3,0,NULL},	
	{72,220,246,232,MENU_PREFS,14,13,msg_108},
};

struct Bottone sprefs_bottoni[]=
{
	{10,75,100,82,-1,0,3,"POWERPC"},{101,75,184,82,-1,3,0,NULL},
	{10,86,100,93,-1,0,3,"CHUNKY"},{101,86,184,93,-1,3,0,NULL},
	{10,97,100,104,-1,0,3,msg_127},{101,97,184,104,-1,3,0,NULL},
	{10,108,100,115,-1,0,3,msg_128},{101,108,184,115,-1,3,0,NULL},
	{10,119,100,126,-1,0,3,msg_186},{101,119,184,126,-1,3,0,NULL},
	{72,180,246,192,MENU_JOYCFG,7,9,NULL},
	{72,195,246,207,MENU_KEYCFG,7,9,"KEYBOARD CONFIG"},
	{72,220,246,232,MENU_PREFS,14,13,msg_108},
};

struct Bottone vprefs_bottoni[]=
{
	{10,64,100,71,-1,0,3,msg_130},{101,64,184,71,-1,3,0,NULL},
	{10,75,100,82,-1,0,3,msg_131},{101,75,184,82,-1,3,0,NULL},
	{10,86,100,93,-1,0,3,msg_132},{101,86,184,93,-1,3,0,NULL},
	{10,97,100,104,-1,0,3,msg_133},{101,97,184,104,-1,3,0,NULL},
	{10,108,100,115,-1,0,3,msg_134},{101,108,184,115,-1,3,0,NULL},
	{10,119,100,126,-1,0,3,msg_135},{101,119,184,126,-1,3,0,NULL},
	{10,130,100,137,-1,0,3,msg_182},{101,130,184,137,-1,3,0,NULL},
	{10,141,100,148,-1,0,3,msg_184},{101,141,184,148,-1,3,0,NULL},
	{10,152,100,159,-1,0,3,"SCALING"},{101,152,184,159,-1,3,0,NULL},
	{10,163,100,170,-1,0,3,NULL},{101,163,184,170,-1,3,0,NULL},
	{10,174,100,181,-1,0,3,"BUFFERING"},{101,174,184,181,-1,3,0,NULL},
	{10,185,100,192,-1,0,3,"NEW PITCHES"},{101,185,184,192,-1,3,0,NULL},
	{10,196,100,203,-1,0,3,"RESOLUTION"},{101,196,184,203,-1,3,0,NULL},
//	{10,207,100,214,-1,0,3,NULL},{101,207,184,214,-1,3,0,NULL},
	{72,220,246,232,MENU_PREFS,14,13,msg_108},
};

struct Bottone aprefs_bottoni[]=
{
	{10,64,100,71,-1,0,3,msg_94},{101,64,184,71,-1,3,0,NULL},
	{10,75,100,82,-1,0,3,msg_136},{101,75,184,82,-1,3,0,NULL},
	{10,86,100,93,-1,0,3,msg_137},{101,86,184,93,-1,3,0,NULL},
	{10,97,100,104,-1,0,3,msg_138},{101,97,184,104,-1,3,0,NULL},
	{10,108,100,115,-1,0,3,msg_185},{101,108,184,115,-1,3,0,NULL},
	{72,210,246,222,MENU_PREFS,14,13,msg_108},
};

struct Bottone seltype_bottoni[]=
{
	{20,67,150,80,MENU_ARCADE_SELECTION,7,9,msg_139},  // Selection Type
	{20,93,150,106,MENU_TEAM_SELECTION,7,9,msg_140},
	{72,210,246,222,MENU_ARCADE,14,13,msg_108},
};

struct Bottone scorelist_bottoni[]=
{
	{60,229,259,241,MENU_SCORES,7,9,msg_0}, // Score list
};



struct Bottone teamselection[]=
{
	{5,31,79,38	,-1,14,13,NULL},{83,31,157,38	,-17,14,13,NULL},{161,31,235,38	,-33,14,13,NULL},{239,31,313,38,-49,14,13,NULL},
	{5,42,79,49	,-2,14,13,NULL},{83,42,157,49	,-18,14,13,NULL},{161,42,235,49	,-34,14,13,NULL},{239,42,313,49,-50,14,13,NULL},
	{5,53,79,60	,-3,14,13,NULL},{83,53,157,60	,-19,14,13,NULL},{161,53,235,60	,-35,14,13,NULL},{239,53,313,60,-51,14,13,NULL},
	{5,64,79,71	,-4,14,13,NULL},{83,64,157,71	,-20,14,13,NULL},{161,64,235,71	,-36,14,13,NULL},{239,64,313,71,-52,14,13,NULL},
	{5,75,79,82	,-5,14,13,NULL},{83,75,157,82	,-21,14,13,NULL},{161,75,235,82	,-37,14,13,NULL},{239,75,313,82,-53,14,13,NULL},
	{5,86,79,93	,-6,14,13,NULL},{83,86,157,93	,-22,14,13,NULL},{161,86,235,93	,-38,14,13,NULL},{239,86,313,93,-54,14,13,NULL},
	{5,97,79,104	,-7,14,13,NULL},{83,97,157,104	,-23,14,13,NULL},{161,97,235,104,-39,14,13,NULL},{239,97,313,104,-55,14,13,NULL},
	{5,108,79,115,-8,14,13,NULL},{83,108,157,115,-24,14,13,NULL},{161,108,235,115,-40,14,13,NULL},{239,108,313,115,-56,14,13,NULL},
	{5,119,79,126,-9,14,13,NULL},{83,119,157,126,-25,14,13,NULL},{161,119,235,126,-41,14,13,NULL},{239,119,313,126,-57,14,13,NULL},
	{5,130,79,137,-10,14,13,NULL},{83,130,157,137,-26,14,13,NULL},{161,130,235,137,-42,14,13,NULL},{239,130,313,137,-58,14,13,NULL},
	{5,141,79,148,-11,14,13,NULL},{83,141,157,148,-27,14,13,NULL},{161,141,235,148,-43,14,13,NULL},{239,141,313,148,-59,14,13,NULL},
	{5,152,79,159,-12,14,13,NULL},{83,152,157,159,-28,14,13,NULL},{161,152,235,159,-44,14,13,NULL},{239,152,313,159,-60,14,13,NULL},
	{5,163,79,170,-13,14,13,NULL},{83,163,157,170,-29,14,13,NULL},{161,163,235,170,-45,14,13,NULL},{239,163,313,170,-61,14,13,NULL},
	{5,174,79,181,-14,14,13,NULL},{83,174,157,181,-30,14,13,NULL},{161,174,235,181,-46,14,13,NULL},{239,174,313,181,-62,14,13,NULL},
	{5,185,79,192,-15,14,13,NULL},{83,185,157,192,-31,14,13,NULL},{161,185,235,192,-47,14,13,NULL},{239,185,313,192,-63,14,13,NULL},
	{5,196,79,203,-16,14,13,NULL},{83,196,157,203,-32,14,13,NULL}, 	{161,196,235,203,-48,14,13,NULL},{239,196,313,203,-64,14,13,NULL},
	{10,229,106,241,-1,7,9,NULL},
	{111,229,207,241,-1,14,13,msg_6},
	{212,229,309,241,-1,11,12,NULL},
};


char numero[17][3]={
	"1"/*-*/,
	"2"/*-*/,
	"3"/*-*/,
	"4"/*-*/,
	"5"/*-*/,
	"6"/*-*/,
	"7"/*-*/,
	"8"/*-*/,
	"9"/*-*/,
	"10"/*-*/,
	"11"/*-*/,
	"12"/*-*/,
	"13"/*-*/,
	"14"/*-*/,
	"15"/*-*/,
	"16"/*-*/,
	".."/*-*/,
};

#if 0


struct Bottone teamsettings[]=
{
	{14,31,22,38,-1,8,6,numero[0]},{24,31,136,38,-1,8,6,NULL},     
	{14,42,22,49,-1,8,6,numero[1]},{24,42,136,49,-1,8,6,NULL},       
	{14,53,22,60,-1,8,6,numero[2]},{24,53,136,60,-1,8,6,NULL},       
	{14,64,22,71,-1,8,6,numero[3]},{24,64,136,71,-1,8,6,NULL},       
	{14,75,22,82,-1,8,6,numero[4]},{24,75,136,82,-1,8,6,NULL},       
	{14,86,22,93,-1,8,6,numero[5]},{24,86,136,93,-1,8,6,NULL},     
	{14,97,22,104,-1,8,6,numero[6]},{24,97,136,104,-1,8,6,NULL},   
	{14,108,22,115,-1,8,6,numero[7]},{24,108,136,115,-1,8,6,NULL}, 
	{14,119,22,126,-1,8,6,numero[8]},{24,119,136,126,-1,8,6,NULL},   
	{14,130,22,137,-1,8,6,numero[9]},{24,130,136,137,-1,8,6,NULL},  
	{14,141,22,148,-1,8,6,numero[10]},{24,141,136,148,-1,8,6,NULL},

	{14,157,22,164,-1,6,8,numero[11]},{24,157,136,164,-1,6,8,NULL},
	{14,168,22,175,-1,6,8,numero[12]},{24,168,136,175,-1,6,8,NULL},  
	{14,179,22,186,-1,6,8,numero[13]},{24,179,136,186,-1,6,8,NULL},  
	{14,190,22,197,-1,6,8,numero[14]},{24,190,136,197,-1,6,8,NULL},
	{14,201,22,208,-1,6,8,numero[15]},{24,201,136,208,-1,6,8,NULL},
	{14,218,22,225,-1,14,13,numero[16]},{24,218,136,225,-1,14,13,NULL},

	{210,192,237,199,-100,14,13,"4-4-2"/*-*/},{241,192,268,199,-101,14,13,"4-3-3"/*-*/},{272,192,299,199,-102,14,13,"4-2-4"/*-*/},
	{210,203,237,210,-103,14,13,"5-3-2"/*-*/},{241,203,268,210,-104,14,13,"5-4-1"/*-*/},{272,203,299,210,-105,14,13,"5-4-1"/*-*/},
	{210,214,237,221,-106,14,13,"3-2-5"/*-*/},{241,214,268,221,-107,14,13,"3-5-2"/*-*/},{272,214,299,221,-108,14,13,"3-4-3"/*-*/},
	{210,225,250,232,-109,14,13,"DEFAULT"/*-*/},{254,225,299,232,-110,14,13,"LOAD"/*-*/},
	{10,239,154,251,-1,14,13,NULL},
	{165,239,309,251,-1,7,9,msg_101}
};

struct Bottone pannelli[]=
{
	{4,31,12,38,-1,8,6,NULL},    {138,31,150,38,-1,14,13,msg_141},{152,31,192,38,-1,14,13,NULL}, 
	{4,42,12,49,-1,8,6,NULL},   {138,42,150,49,-1,14,13,NULL},{152,42,192,49,-1,14,13,NULL},
	{4,53,12,60,-1,8,6,NULL},   {138,53,150,60,-1,14,13,NULL},{152,53,192,60,-1,14,13,NULL},
	{4,64,12,71,-1,8,6,NULL},   {138,64,150,71,-1,14,13,NULL},{152,64,192,71,-1,14,13,NULL},
	{4,75,12,82,-1,8,6,NULL},   {138,75,150,82,-1,14,13,NULL},{152,75,192,82,-1,14,13,NULL},
	{4,86,12,93,-1,8,6,NULL},   {138,86,150,93,-1,14,13,NULL},{152,86,192,93,-1,14,13,NULL},
	{4,97,12,104,-1,8,6,NULL},  {138,97,150,104,-1,14,13,NULL},{152,97,192,104,-1,14,13,NULL},
	{4,108,12,115,-1,8,6,NULL}, {138,108,150,115,-1,14,13,NULL},{152,108,192,115,-1,14,13,NULL},
	{4,119,12,126,-1,8,6,NULL}, {138,119,150,126,-1,14,13,NULL},{152,119,192,126,-1,14,13,NULL},
	{4,130,12,137,-1,8,6,NULL}, {138,130,150,137,-1,14,13,NULL},{152,130,192,137,-1,14,13,NULL},
	{4,141,12,148,-1,8,6,NULL}, {138,141,150,148,-1,14,13,NULL},{152,141,192,148,-1,14,13,NULL},
	{4,157,12,164,-1,6,8,NULL}, {138,157,150,164,-1,14,13,msg_141},{152,157,192,164,-1,14,13,NULL}, 
	{4,168,12,175,-1,6,8,NULL}, {138,168,150,175,-1,14,13,NULL},{152,168,192,175,-1,14,13,NULL}, 
	{4,179,12,186,-1,6,8,NULL}, {138,179,150,186,-1,14,13,NULL},{152,179,192,186,-1,14,13,NULL}, 
	{4,190,12,197,-1,6,8,NULL}, {138,190,150,197,-1,14,13,NULL},{152,190,192,197,-1,14,13,NULL}, 
	{4,201,12,208,-1,6,8,NULL}, {138,201,150,208,-1,14,13,NULL},{152,201,192,208,-1,14,13,NULL}, 
	{4,218,12,225,-1,14,13,NULL},{138,218,150,225,-1,14,13,NULL},{152,218,192,225,-1,14,13,NULL},
};

#else

struct Bottone pannelli[]=
{
	{4,31,12,38,-1,8,6,NULL},    {138,31,146,38,-1,14,13,msg_141},{148,31,192,38,-1,14,13,NULL}, 
	{4,42,12,49,-1,8,6,NULL},   {138,42,146,49,-1,14,13,NULL},{148,42,192,49,-1,14,13,NULL},
	{4,53,12,60,-1,8,6,NULL},   {138,53,146,60,-1,14,13,NULL},{148,53,192,60,-1,14,13,NULL},
	{4,64,12,71,-1,8,6,NULL},   {138,64,146,71,-1,14,13,NULL},{148,64,192,71,-1,14,13,NULL},
	{4,75,12,82,-1,8,6,NULL},   {138,75,146,82,-1,14,13,NULL},{148,75,192,82,-1,14,13,NULL},
	{4,86,12,93,-1,8,6,NULL},   {138,86,146,93,-1,14,13,NULL},{148,86,192,93,-1,14,13,NULL},
	{4,97,12,104,-1,8,6,NULL},  {138,97,146,104,-1,14,13,NULL},{148,97,192,104,-1,14,13,NULL},
	{4,108,12,115,-1,8,6,NULL}, {138,108,146,115,-1,14,13,NULL},{148,108,192,115,-1,14,13,NULL},
	{4,119,12,126,-1,8,6,NULL}, {138,119,146,126,-1,14,13,NULL},{148,119,192,126,-1,14,13,NULL},
	{4,130,12,137,-1,8,6,NULL}, {138,130,146,137,-1,14,13,NULL},{148,130,192,137,-1,14,13,NULL},
	{4,141,12,148,-1,8,6,NULL}, {138,141,146,148,-1,14,13,NULL},{148,141,192,148,-1,14,13,NULL},
	{4,157,12,164,-1,6,8,NULL}, {138,157,146,164,-1,14,13,msg_141},{148,157,192,164,-1,14,13,NULL}, 
	{4,168,12,175,-1,6,8,NULL}, {138,168,146,175,-1,14,13,NULL},{148,168,192,175,-1,14,13,NULL}, 
	{4,179,12,186,-1,6,8,NULL}, {138,179,146,186,-1,14,13,NULL},{148,179,192,186,-1,14,13,NULL}, 
	{4,190,12,197,-1,6,8,NULL}, {138,190,146,197,-1,14,13,NULL},{148,190,192,197,-1,14,13,NULL}, 
	{4,201,12,208,-1,6,8,NULL}, {138,201,146,208,-1,14,13,NULL},{148,201,192,208,-1,14,13,NULL}, 
	{4,218,12,225,-1,14,13,NULL},{138,218,146,225,-1,14,13,NULL},{148,218,192,225,-1,14,13,NULL},
};

struct Bottone teamsettings[]=
{
	{14,31,22,38,-1,8,6,numero[0]},{24,31,136,38,-1,8,6,NULL},     
	{14,42,22,49,-1,8,6,numero[1]},{24,42,136,49,-1,8,6,NULL},       
	{14,53,22,60,-1,8,6,numero[2]},{24,53,136,60,-1,8,6,NULL},       
	{14,64,22,71,-1,8,6,numero[3]},{24,64,136,71,-1,8,6,NULL},       
	{14,75,22,82,-1,8,6,numero[4]},{24,75,136,82,-1,8,6,NULL},       
	{14,86,22,93,-1,8,6,numero[5]},{24,86,136,93,-1,8,6,NULL},     
	{14,97,22,104,-1,8,6,numero[6]},{24,97,136,104,-1,8,6,NULL},   
	{14,108,22,115,-1,8,6,numero[7]},{24,108,136,115,-1,8,6,NULL}, 
	{14,119,22,126,-1,8,6,numero[8]},{24,119,136,126,-1,8,6,NULL},   
	{14,130,22,137,-1,8,6,numero[9]},{24,130,136,137,-1,8,6,NULL},  
	{14,141,22,148,-1,8,6,numero[10]},{24,141,136,148,-1,8,6,NULL},

	{14,157,22,164,-1,6,8,numero[11]},{24,157,136,164,-1,6,8,NULL},
	{14,168,22,175,-1,6,8,numero[12]},{24,168,136,175,-1,6,8,NULL},  
	{14,179,22,186,-1,6,8,numero[13]},{24,179,136,186,-1,6,8,NULL},  
	{14,190,22,197,-1,6,8,numero[14]},{24,190,136,197,-1,6,8,NULL},
	{14,201,22,208,-1,6,8,numero[15]},{24,201,136,208,-1,6,8,NULL},
	{14,218,22,225,-1,14,13,numero[16]},{24,218,136,225,-1,14,13,NULL},

	{211,187,238,194,-100,14,13,"4-4-2"/*-*/},{242,187,269,194,-101,14,13,"4-3-3"/*-*/},{273,187,300,194,-102,14,13,"3-4-3"/*-*/},
	{211,198,238,205,-103,14,13,"5-3-2"/*-*/},{242,198,269,205,-104,14,13,"5-4-1"/*-*/},{273,198,300,205,-105,14,13,"3-5-2"/*-*/},
	{211,209,254,216,-110,14,13,msg_99},{256,209,300,216,-110,14,13,NULL},
	{60,239,259,251,-1,14,13,NULL},

/*
	{10,239,154,251,-1,14,13,NULL},
	{165,239,309,251,-1,7,9,"DEFAULT"}
*/
};

#endif

/* AC: I don't know because these data differs from some others (stored in some
 * place) used in the first page of the World Championship.
 * The old ones I've replaced creates an overlap between the header and the other
 * rows.
 */
struct Bottone wcp[]=
{
// A
	{5,32,95,39,-1,8,6,NULL},{97,32,109,39,-1,8,6,NULL},{111,32,123,39,-1,8,6,NULL},{125,32,137,39,-1,8,6,NULL},{139,32,151,39,-1,8,6,NULL},
	{5,40,95,47,-1,8,6,NULL},{97,40,109,47,-1,8,6,NULL},{111,40,123,47,-1,8,6,NULL},{125,40,137,47,-1,8,6,NULL},{139,40,151,47,-1,8,6,NULL},
	{5,48,95,55,-1,8,6,NULL},{97,48,109,55,-1,8,6,NULL},{111,48,123,55,-1,8,6,NULL},{125,48,137,55,-1,8,6,NULL},{139,48,151,55,-1,8,6,NULL},
	{5,56,95,63,-1,8,6,NULL},{97,56,109,63,-1,8,6,NULL},{111,56,123,63,-1,8,6,NULL},{125,56,137,63,-1,8,6,NULL},{139,56,151,63,-1,8,6,NULL},
// B
	{5,76,95,83,-1,8,6,NULL},{97,76,109,83,-1,8,6,NULL},{111,76,123,83,-1,8,6,NULL},{125,76,137,83,-1,8,6,NULL},{139,76,151,83,-1,8,6,NULL},
	{5,84,95,91,-1,8,6,NULL},{97,84,109,91,-1,8,6,NULL},{111,84,123,91,-1,8,6,NULL},{125,84,137,91,-1,8,6,NULL},{139,84,151,91,-1,8,6,NULL},
	{5,92,95,99,-1,8,6,NULL},{97,92,109,99,-1,8,6,NULL},{111,92,123,99,-1,8,6,NULL},{125,92,137,99,-1,8,6,NULL},{139,92,151,99,-1,8,6,NULL},
	{5,100,95,107,-1,8,6,NULL},{97,100,109,107,-1,8,6,NULL},{111,100,123,107,-1,8,6,NULL},{125,100,137,107,-1,8,6,NULL},{139,100,151,107,-1,8,6,NULL},
// C
	{5,121,95,128,-1,8,6,NULL},{97,121,109,128,-1,8,6,NULL},{111,121,123,128,-1,8,6,NULL},{125,121,137,128,-1,8,6,NULL},{139,121,151,128,-1,8,6,NULL},
	{5,129,95,136,-1,8,6,NULL},{97,129,109,136,-1,8,6,NULL},{111,129,123,136,-1,8,6,NULL},{125,129,137,136,-1,8,6,NULL},{139,129,151,136,-1,8,6,NULL},
	{5,137,95,144,-1,8,6,NULL},{97,137,109,144,-1,8,6,NULL},{111,137,123,144,-1,8,6,NULL},{125,137,137,144,-1,8,6,NULL},{139,137,151,144,-1,8,6,NULL},
	{5,145,95,152,-1,8,6,NULL},{97,145,109,152,-1,8,6,NULL},{111,145,123,152,-1,8,6,NULL},{125,145,137,152,-1,8,6,NULL},{139,145,151,152,-1,8,6,NULL},
// D
	{5,166,95,173,-1,8,6,NULL},{97,166,109,173,-1,8,6,NULL},{111,166,123,173,-1,8,6,NULL},{125,166,137,173,-1,8,6,NULL},{139,166,151,173,-1,8,6,NULL},
	{5,174,95,181,-1,8,6,NULL},{97,174,109,181,-1,8,6,NULL},{111,174,123,181,-1,8,6,NULL},{125,174,137,181,-1,8,6,NULL},{139,174,151,181,-1,8,6,NULL},
	{5,182,95,189,-1,8,6,NULL},{97,182,109,189,-1,8,6,NULL},{111,182,123,189,-1,8,6,NULL},{125,182,137,189,-1,8,6,NULL},{139,182,151,189,-1,8,6,NULL},
	{5,190,95,197,-1,8,6,NULL},{97,190,109,197,-1,8,6,NULL},{111,190,123,197,-1,8,6,NULL},{125,190,137,197,-1,8,6,NULL},{139,190,151,197,-1,8,6,NULL},
// E
	{165,32,255,39,-1,8,6,NULL},{257,32,269,39,-1,8,6,NULL},{271,32,283,39,-1,8,6,NULL},{285,32,297,39,-1,8,6,NULL},{299,32,311,39,-1,8,6,NULL},
	{165,40,255,47,-1,8,6,NULL},{257,40,269,47,-1,8,6,NULL},{271,40,283,47,-1,8,6,NULL},{285,40,297,47,-1,8,6,NULL},{299,40,311,47,-1,8,6,NULL},
	{165,48,255,55,-1,8,6,NULL},{257,48,269,55,-1,8,6,NULL},{271,48,283,55,-1,8,6,NULL},{285,48,297,55,-1,8,6,NULL},{299,48,311,55,-1,8,6,NULL},
	{165,56,255,63,-1,8,6,NULL},{257,56,269,63,-1,8,6,NULL},{271,56,283,63,-1,8,6,NULL},{285,56,297,63,-1,8,6,NULL},{299,56,311,63,-1,8,6,NULL},
// F
	{165,76,255,83,-1,8,6,NULL},{257,76,269,83,-1,8,6,NULL},{271,76,283,83,-1,8,6,NULL},{285,76,297,83,-1,8,6,NULL},{299,76,311,83,-1,8,6,NULL},
	{165,84,255,91,-1,8,6,NULL},{257,84,269,91,-1,8,6,NULL},{271,84,283,91,-1,8,6,NULL},{285,84,297,91,-1,8,6,NULL},{299,84,311,91,-1,8,6,NULL},
	{165,92,255,99,-1,8,6,NULL},{257,92,269,99,-1,8,6,NULL},{271,92,283,99,-1,8,6,NULL},{285,92,297,99,-1,8,6,NULL},{299,92,311,99,-1,8,6,NULL},
	{165,100,255,107,-1,8,6,NULL},{257,100,269,107,-1,8,6,NULL},{271,100,283,107,-1,8,6,NULL},{285,100,297,107,-1,8,6,NULL},{299,100,311,107,-1,8,6,NULL},
// G
	{165,121,255,128,-1,8,6,NULL},{257,121,269,128,-1,8,6,NULL},{271,121,283,128,-1,8,6,NULL},{285,121,297,128,-1,8,6,NULL},{299,121,311,128,-1,8,6,NULL},
	{165,129,255,136,-1,8,6,NULL},{257,129,269,136,-1,8,6,NULL},{271,129,283,136,-1,8,6,NULL},{285,129,297,136,-1,8,6,NULL},{299,129,311,136,-1,8,6,NULL},
	{165,137,255,144,-1,8,6,NULL},{257,137,269,144,-1,8,6,NULL},{271,137,283,144,-1,8,6,NULL},{285,137,297,144,-1,8,6,NULL},{299,137,311,144,-1,8,6,NULL},
	{165,145,255,152,-1,8,6,NULL},{257,145,269,152,-1,8,6,NULL},{271,145,283,152,-1,8,6,NULL},{285,145,297,152,-1,8,6,NULL},{299,145,311,152,-1,8,6,NULL},

// H
	{165,166,255,173,-1,8,6,NULL},{257,166,269,173,-1,8,6,NULL},{271,166,283,173,-1,8,6,NULL},{285,166,297,173,-1,8,6,NULL},{299,166,311,173,-1,8,6,NULL},
	{165,174,255,181,-1,8,6,NULL},{257,174,269,181,-1,8,6,NULL},{271,174,283,181,-1,8,6,NULL},{285,174,297,181,-1,8,6,NULL},{299,174,311,181,-1,8,6,NULL},
	{165,182,255,189,-1,8,6,NULL},{257,182,269,189,-1,8,6,NULL},{271,182,283,189,-1,8,6,NULL},{285,182,297,189,-1,8,6,NULL},{299,182,311,189,-1,8,6,NULL},
	{165,190,255,197,-1,8,6,NULL},{257,190,269,197,-1,8,6,NULL},{271,190,283,197,-1,8,6,NULL},{285,190,297,197,-1,8,6,NULL},{299,190,311,197,-1,8,6,NULL},

// Nomi
	{5,24,95,31,-1,14,13,msg_142},{97,24,109,31,-1,14,13,msg_143},{111,24,123,31,-1,14,13,msg_144},{125,24,137,31,-1,14,13,msg_145},{139,24,151,31,-1,14,13,msg_146},
	{5,68,95,75,-1,14,13,msg_147},{97,68,109,75,-1,14,13,msg_143},{111,68,123,75,-1,14,13,msg_144},{125,68,137,75,-1,14,13,msg_145},{139,68,151,75,-1,14,13,msg_146},
	{5,113,95,120,-1,14,13,msg_148},{97,113,109,120,-1,14,13,msg_143},{111,113,123,120,-1,14,13,msg_144},{125,113,137,120,-1,14,13,msg_145},{139,113,151,120,-1,14,13,msg_146},
	{5,158,95,165,-1,14,13,msg_149},{97,158,109,165,-1,14,13,msg_143},{111,158,123,165,-1,14,13,msg_144},{125,158,137,165,-1,14,13,msg_145},{139,158,151,165,-1,14,13,msg_146},
	{165,24,255,31,-1,14,13,msg_150},{257,24,269,31,-1,14,13,msg_143},{271,24,283,31,-1,14,13,msg_144},{285,24,297,31,-1,14,13,msg_145},{299,24,311,31,-1,14,13,msg_146},
	{165,68,255,75,-1,14,13,msg_151},{257,68,269,75,-1,14,13,msg_143},{271,68,283,75,-1,14,13,msg_144},{285,68,297,75,-1,14,13,msg_145},{299,68,311,75,-1,14,13,msg_146},
	{165,113,255,120,-1,14,13,msg_152},{257,113,269,120,-1,14,13,msg_143},{271,113,283,120,-1,14,13,msg_144},{285,113,297,120,-1,14,13,msg_145},{299,113,311,120,-1,14,13,msg_146},
	{165,158,255,165,-1,14,13,msg_153},{257,158,269,165,-1,14,13,msg_143},{271,158,283,165,-1,14,13,msg_144},{285,158,297,165,-1,14,13,msg_145},{299,158,311,165,-1,14,13,msg_146},
};

struct Bottone wcb[]=
{
	{10,229,106,241,MENU_MATCHES,7,9,msg_0},
	{111,229,207,241,-1,14,13,msg_98},
	{212,229,309,241,MENU_SIMULATION,11,12,msg_6},
};

struct Bottone mb[]=
{
	{10,229,106,241,MENU_MATCHES,7,9,msg_0},
	{111,229,207,241,-1,14,13,msg_98},
	{212,229,309,241,MENU_SIMULATION,11,12,msg_6},
};


struct Bottone lb[]=
{
	{10,229,106,241,MENU_MATCHES,7,9,msg_0},
	{111,229,207,241,-1,14,13,msg_98},
	{212,229,309,241,MENU_SIMULATION,11,12,msg_6},
};

/*
struct Bottone cb[]=
{
	{10,229,106,241,MENU_MATCHES,7,9,"CONTINUE"},
	{111,229,207,241,-1,14,13,"SAVE"},
	{212,229,309,241,MENU_SIMULATION,11,12,"PARENT MENU"},
};
*/

/* In reality control 0 would be the BLUE keyboard, but for praticity
 * since the things are inverted, at the moment we hold first red and
 * then the blue.
 */
struct Bottone keycfg_bottoni[]=
{
	/* "RED" keyboard configuration */
	{10,24,80,31,-1,7,9,"MOVE UP"},{81,24,154,31,-1,9,11,NULL},
	{10,35,80,42,-1,7,9,"MOVE RIGHT"},{81,35,154,42,-1,9,11,NULL},
	{10,46,80,53,-1,7,9,"MOVE DOWN"},{81,46,154,53,-1,9,11,NULL},
	{10,57,80,64,-1,7,9,"MOVE LEFT"},{81,57,154,64,-1,9,11,NULL},
	{10,68,80,75,-1,7,9,"SHOT"},{81,68,154,75,-1,9,11,NULL},
	{10,79,80,86,-1,7,9,"SHORT PASS"},{81,79,154,86,-1,9,11,NULL},
	{170,24,240,31,-1,7,9,"LONG PASS"},{241,24,311,31,-1,9,11,NULL},
	{170,35,240,42,-1,7,9,"FILTERING PASS"},{241,35,311,42,-1,9,11,NULL},
	{170,46,240,53,-1,7,9,"CHANGE ACTIVE"},{241,46,311,53,-1,9,11,NULL},
	{170,57,240,64,-1,7,9,"FAST RUN"},{241,57,311,64,-1,9,11,NULL},
	//{10,90,154,102,0,7,9,msg_96},

	/* "BLUE" keyboard configuration */
	{10,135,80,142,-1,8,15,"MOVE UP"},{81,135,154,142,-1,6,8,NULL},
	{10,146,80,153,-1,8,15,"MOVE RIGHT"},{81,146,154,153,-1,6,8,NULL},
	{10,157,80,164,-1,8,15,"MOVE DOWN"},{81,157,154,164,-1,6,8,NULL},
	{10,168,80,175,-1,8,15,"MOVE LEFT"},{81,168,154,175,-1,6,8,NULL},
	{10,179,80,186,-1,8,15,"SHOT"},{81,179,154,186,-1,6,8,NULL},
	{10,190,80,197,-1,8,15,"SHORT PASS"},{81,190,154,197,-1,6,8,NULL},
	{170,157,240,164,-1,8,15,"LONG PASS"},{241,157,311,164,-1,6,8,NULL},
	{170,168,240,175,-1,8,15,"FILTERING PASS"},{241,168,311,175,-1,6,8,NULL},
	{170,179,240,186,-1,8,15,"CHANGE ACTIVE"},{241,179,311,186,-1,6,8,NULL},
	{170,190,240,197,-1,8,15,"FAST RUN"},{241,190,311,197,-1,6,8,NULL},
	//{10,210,154,222,1,8,15,msg_96},

	/* Save configurations */
	{10,210,154,222,0,7,9,msg_96},
	{170,210,311,222,1,8,15,msg_96},
	
	//{90,229,230,241,MENU_SYSTEM_PREFS,14,13,msg_6},
	{72,229,246,241,MENU_SYSTEM_PREFS,14,13,msg_108},
};

struct Bottone joycfg_bottoni[]=
{
	{10,64,100,71,-1,0,3,"SHOT"},{101,64,184,71,-1,3,0,NULL},
	{10,75,100,82,-1,0,3,"SHORT PASS"},{101,75,184,82,-1,3,0,NULL},
	{10,86,100,93,-1,0,3,"LONG PASS"},{101,86,184,93,-1,3,0,NULL},
	{10,97,100,104,-1,0,3,"FILTERING PASS"},{101,97,184,104,-1,3,0,NULL},
	{10,108,100,115,-1,0,3,"CHANGE ACTIVE"},{101,108,184,115,-1,3,0,NULL},
	{10,119,100,126,-1,0,3,"FAST RUN"},{101,119,184,126,-1,3,0,NULL},
	{10,130,100,137,-1,0,3,"PAUSE"},{101,130,184,137,-1,3,0,NULL},
/*	{10,141,100,148,-1,0,3,msg_126},{101,141,184,148,-1,3,0,NULL},
	{10,152,100,159,-1,0,3,msg_129},{101,152,184,159,-1,3,0,msg_129},
	{10,163,100,170,-1,0,3,msg_181},{101,163,184,170,-1,3,0,msg_181},
*/
	{90,229,230,241,MENU_SYSTEM_PREFS,11,12,msg_6},
};

struct Bottone league[]=
{
	{25,24,147,31,-1,14,13,msg_154},{149,24,168,31,-1,14,13,msg_155},{170,24,189,31,-1,14,13,msg_156},{191,24,210,31,-1,14,13,msg_157},{212,24,231,31,-1,14,13,msg_158},{233,24,252,31,-1,14,13,msg_159},{254,24,273,31,-1,14,13,msg_145},{275,24,293,31,-1,14,13,msg_146},
	{25,33,147,40,-1,14,13,NULL},{149,33,168,40,-1,14,13,NULL},{170,33,189,40,-1,14,13,NULL},{191,33,210,40,-1,14,13,NULL},{212,33,231,40,-1,14,13,NULL},{233,33,252,40,-1,14,13,NULL},{254,33,273,40,-1,14,13,NULL},{275,33,293,40,-1,14,13,NULL},
	{25,42,147,49,-1,14,13,NULL},{149,42,168,49,-1,14,13,NULL},{170,42,189,49,-1,14,13,NULL},{191,42,210,49,-1,14,13,NULL},{212,42,231,49,-1,14,13,NULL},{233,42,252,49,-1,14,13,NULL},{254,42,273,49,-1,14,13,NULL},{275,42,293,49,-1,14,13,NULL},
	{25,51,147,58,-1,14,13,NULL},{149,51,168,58,-1,14,13,NULL},{170,51,189,58,-1,14,13,NULL},{191,51,210,58,-1,14,13,NULL},{212,51,231,58,-1,14,13,NULL},{233,51,252,58,-1,14,13,NULL},{254,51,273,58,-1,14,13,NULL},{275,51,293,58,-1,14,13,NULL},
	{25,60,147,67,-1,14,13,NULL},{149,60,168,67,-1,14,13,NULL},{170,60,189,67,-1,14,13,NULL},{191,60,210,67,-1,14,13,NULL},{212,60,231,67,-1,14,13,NULL},{233,60,252,67,-1,14,13,NULL},{254,60,273,67,-1,14,13,NULL},{275,60,293,67,-1,14,13,NULL},
	{25,69,147,76,-1,14,13,NULL},{149,69,168,76,-1,14,13,NULL},{170,69,189,76,-1,14,13,NULL},{191,69,210,76,-1,14,13,NULL},{212,69,231,76,-1,14,13,NULL},{233,69,252,76,-1,14,13,NULL},{254,69,273,76,-1,14,13,NULL},{275,69,293,76,-1,14,13,NULL},
	{25,78,147,85,-1,14,13,NULL},{149,78,168,85,-1,14,13,NULL},{170,78,189,85,-1,14,13,NULL},{191,78,210,85,-1,14,13,NULL},{212,78,231,85,-1,14,13,NULL},{233,78,252,85,-1,14,13,NULL},{254,78,273,85,-1,14,13,NULL},{275,78,293,85,-1,14,13,NULL},
	{25,87,147,94,-1,14,13,NULL},{149,87,168,94,-1,14,13,NULL},{170,87,189,94,-1,14,13,NULL},{191,87,210,94,-1,14,13,NULL},{212,87,231,94,-1,14,13,NULL},{233,87,252,94,-1,14,13,NULL},{254,87,273,94,-1,14,13,NULL},{275,87,293,94,-1,14,13,NULL},
	{25,96,147,103,-1,14,13,NULL},{149,96,168,103,-1,14,13,NULL},{170,96,189,103,-1,14,13,NULL},{191,96,210,103,-1,14,13,NULL},{212,96,231,103,-1,14,13,NULL},{233,96,252,103,-1,14,13,NULL},{254,96,273,103,-1,14,13,NULL},{275,96,293,103,-1,14,13,NULL},
	{25,105,147,112,-1,14,13,NULL},{149,105,168,112,-1,14,13,NULL},{170,105,189,112,-1,14,13,NULL},{191,105,210,112,-1,14,13,NULL},{212,105,231,112,-1,14,13,NULL},{233,105,252,112,-1,14,13,NULL},{254,105,273,112,-1,14,13,NULL},{275,105,293,112,-1,14,13,NULL},
	{25,114,147,121,-1,14,13,NULL},{149,114,168,121,-1,14,13,NULL},{170,114,189,121,-1,14,13,NULL},{191,114,210,121,-1,14,13,NULL},{212,114,231,121,-1,14,13,NULL},{233,114,252,121,-1,14,13,NULL},{254,114,273,121,-1,14,13,NULL},{275,114,293,121,-1,14,13,NULL},
	{25,123,147,130,-1,14,13,NULL},{149,123,168,130,-1,14,13,NULL},{170,123,189,130,-1,14,13,NULL},{191,123,210,130,-1,14,13,NULL},{212,123,231,130,-1,14,13,NULL},{233,123,252,130,-1,14,13,NULL},{254,123,273,130,-1,14,13,NULL},{275,123,293,130,-1,14,13,NULL},
	{25,132,147,139,-1,14,13,NULL},{149,132,168,139,-1,14,13,NULL},{170,132,189,139,-1,14,13,NULL},{191,132,210,139,-1,14,13,NULL},{212,132,231,139,-1,14,13,NULL},{233,132,252,139,-1,14,13,NULL},{254,132,273,139,-1,14,13,NULL},{275,132,293,139,-1,14,13,NULL},
	{25,141,147,148,-1,14,13,NULL},{149,141,168,148,-1,14,13,NULL},{170,141,189,148,-1,14,13,NULL},{191,141,210,148,-1,14,13,NULL},{212,141,231,148,-1,14,13,NULL},{233,141,252,148,-1,14,13,NULL},{254,141,273,148,-1,14,13,NULL},{275,141,293,148,-1,14,13,NULL},
	{25,150,147,157,-1,14,13,NULL},{149,150,168,157,-1,14,13,NULL},{170,150,189,157,-1,14,13,NULL},{191,150,210,157,-1,14,13,NULL},{212,150,231,157,-1,14,13,NULL},{233,150,252,157,-1,14,13,NULL},{254,150,273,157,-1,14,13,NULL},{275,150,293,157,-1,14,13,NULL},
	{25,159,147,166,-1,14,13,NULL},{149,159,168,166,-1,14,13,NULL},{170,159,189,166,-1,14,13,NULL},{191,159,210,166,-1,14,13,NULL},{212,159,231,166,-1,14,13,NULL},{233,159,252,166,-1,14,13,NULL},{254,159,273,166,-1,14,13,NULL},{275,159,293,166,-1,14,13,NULL},
	{25,168,147,175,-1,14,13,NULL},{149,168,168,175,-1,14,13,NULL},{170,168,189,175,-1,14,13,NULL},{191,168,210,175,-1,14,13,NULL},{212,168,231,175,-1,14,13,NULL},{233,168,252,175,-1,14,13,NULL},{254,168,273,175,-1,14,13,NULL},{275,168,293,175,-1,14,13,NULL},
	{25,177,147,184,-1,14,13,NULL},{149,177,168,184,-1,14,13,NULL},{170,177,189,184,-1,14,13,NULL},{191,177,210,184,-1,14,13,NULL},{212,177,231,184,-1,14,13,NULL},{233,177,252,184,-1,14,13,NULL},{254,177,273,184,-1,14,13,NULL},{275,177,293,184,-1,14,13,NULL},
	{25,186,147,193,-1,14,13,NULL},{149,186,168,193,-1,14,13,NULL},{170,186,189,193,-1,14,13,NULL},{191,186,210,193,-1,14,13,NULL},{212,186,231,193,-1,14,13,NULL},{233,186,252,193,-1,14,13,NULL},{254,186,273,193,-1,14,13,NULL},{275,186,293,193,-1,14,13,NULL},
	{25,195,147,202,-1,14,13,NULL},{149,195,168,202,-1,14,13,NULL},{170,195,189,202,-1,14,13,NULL},{191,195,210,202,-1,14,13,NULL},{212,195,231,202,-1,14,13,NULL},{233,195,252,202,-1,14,13,NULL},{254,195,273,202,-1,14,13,NULL},{275,195,293,202,-1,14,13,NULL},
	{25,204,147,211,-1,14,13,NULL},{149,204,168,211,-1,14,13,NULL},{170,204,189,211,-1,14,13,NULL},{191,204,210,211,-1,14,13,NULL},{212,204,231,211,-1,14,13,NULL},{233,204,252,211,-1,14,13,NULL},{254,204,273,211,-1,14,13,NULL},{275,204,293,211,-1,14,13,NULL},
};

struct Bottone mp[]=
{
	{30,24,138,31,-1,13,1,NULL},{140,24,148,31,-1,13,1,NULL},{150,24,258,31,-1,13,1,NULL},{260,24,299,31,-1,13,1,NULL},
	{30,33,138,40,-1,13,1,NULL},{140,33,148,40,-1,13,1,NULL},{150,33,258,40,-1,13,1,NULL},{260,33,299,40,-1,13,1,NULL},
	{30,42,138,49,-1,13,1,NULL},{140,42,148,49,-1,13,1,NULL},{150,42,258,49,-1,13,1,NULL},{260,42,299,49,-1,13,1,NULL},
	{30,51,138,58,-1,13,1,NULL},{140,51,148,58,-1,13,1,NULL},{150,51,258,58,-1,13,1,NULL},{260,51,299,58,-1,13,1,NULL},
	{30,60,138,67,-1,13,1,NULL},{140,60,148,67,-1,13,1,NULL},{150,60,258,67,-1,13,1,NULL},{260,60,299,67,-1,13,1,NULL},
	{30,69,138,76,-1,13,1,NULL},{140,69,148,76,-1,13,1,NULL},{150,69,258,76,-1,13,1,NULL},{260,69,299,76,-1,13,1,NULL},
	{30,78,138,85,-1,13,1,NULL},{140,78,148,85,-1,13,1,NULL},{150,78,258,85,-1,13,1,NULL},{260,78,299,85,-1,13,1,NULL},
	{30,87,138,94,-1,13,1,NULL},{140,87,148,94,-1,13,1,NULL},{150,87,258,94,-1,13,1,NULL},{260,87,299,94,-1,13,1,NULL},
	{30,96,138,103,-1,13,1,NULL},{140,96,148,103,-1,13,1,NULL},{150,96,258,103,-1,13,1,NULL},{260,96,299,103,-1,13,1,NULL},
	{30,105,138,112,-1,13,1,NULL},{140,105,148,112,-1,13,1,NULL},{150,105,258,112,-1,13,1,NULL},{260,105,299,112,-1,13,1,NULL},
	{30,114,138,121,-1,13,1,NULL},{140,114,148,121,-1,13,1,NULL},{150,114,258,121,-1,13,1,NULL},{260,114,299,121,-1,13,1,NULL},
	{30,123,138,130,-1,13,1,NULL},{140,123,148,130,-1,13,1,NULL},{150,123,258,130,-1,13,1,NULL},{260,123,299,130,-1,13,1,NULL},
	{30,132,138,139,-1,13,1,NULL},{140,132,148,139,-1,13,1,NULL},{150,132,258,139,-1,13,1,NULL},{260,132,299,139,-1,13,1,NULL},
	{30,141,138,148,-1,13,1,NULL},{140,141,148,148,-1,13,1,NULL},{150,141,258,148,-1,13,1,NULL},{260,141,299,148,-1,13,1,NULL},
	{30,150,138,157,-1,13,1,NULL},{140,150,148,157,-1,13,1,NULL},{150,150,258,157,-1,13,1,NULL},{260,150,299,157,-1,13,1,NULL},
	{30,159,138,166,-1,13,1,NULL},{140,159,148,166,-1,13,1,NULL},{150,159,258,166,-1,13,1,NULL},{260,159,299,166,-1,13,1,NULL},
};

// WARNING: They are octal numbers!

struct Bottone asb[]=
{
	{16,20,85,89,0,COLORE_UNSELECTED,9,"\20\0"/*-*/},{88,20,157,89,1,COLORE_UNSELECTED,9,"\20\1"/*-*/},{160,20,229,89,2,COLORE_UNSELECTED,9,"\20\2"/*-*/},{232,20,301,89,3,COLORE_UNSELECTED,9,"\20\3"/*-*/},
	{16,92,85,161,4,COLORE_UNSELECTED,9,"\20\4"/*-*/},{232,92,301,161,7,COLORE_UNSELECTED,9,"\20\7"/*-*/},
	{16,164,85,233,5,COLORE_UNSELECTED,9,"\20\5"/*-*/},{88,164,157,233,8,COLORE_UNSELECTED,9,"\20\10"/*-*/},{160,164,229,233,9,COLORE_UNSELECTED,9,"\20\11"/*-*/},{232,164,301,233,6,COLORE_UNSELECTED,9,"\20\6"/*-*/},
	{10,238,106,250,-1,7,9,NULL},
	{111,238,207,250,-1,14,13,msg_6},
	{212,238,309,250,-1,11,12,NULL},
};

struct Bottone cp[]=
{
	{52,40,121,109,0,COLORE_UNSELECTED,9,"\20a"/*-*/},
	{198,40,267,109,0,COLORE_UNSELECTED,9,"\20b"/*-*/},
	{32,121,141,140,-1,0,0,NULL},{178,121,287,140,-1,0,0,NULL},
	{77,151,96,170,-1,0,0,NULL},{223,151,242,170,-1,0,0,NULL},
	{100,190,219,209,-1,0,0,NULL}, // For the game over
};

struct Bottone cb[]=
{
	{20,229,154,241,MENU_CHALLENGE,7,9,msg_0},
	{165,229,299,241,MENU_ARCADE,11,12,msg_6},
};


struct Bottone mr[]=
{
/* I leave them here for eventual flags...

	{52,40,121,109,0,COLORE_UNSELECTED,9,"\20a"},
	{198,40,267,109,0,COLORE_UNSELECTED,9,"\20b"},
*/
	{32,51,141,70,-1,0,0,NULL},{178,51,287,70,-1,0,0,NULL},
	{77,91,96,110,-1,0,0,NULL},{223,91,242,110,-1,0,0,NULL},
};

struct Bottone mrb[]=
{
	{60,229,259,241,MENU_SIMULATION,7,9,msg_0},
};

struct Bottone wcfb[]=
{
	{60,229,259,241,MENU_SIMULATION,7,9,msg_0},
};

struct Bottone scores[]=
{
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
	{32,57,237,76,-1,8,15,NULL},{240,57,288,76,-1,8,15,NULL},
};

struct Bottone wcfp[]=
{
// The first four teams...
	{120,57,230,76,-1,0,0,NULL},
	{120,97,230,116,-1,0,0,NULL},
	{120,137,230,156,-1,0,0,NULL},
	{120,177,230,196,-1,0,0,NULL},
// The positions...
	{60,40,180,55,-1,P_GIALLO,P_BIANCO,msg_160},
	{60,80,180,95,-1,P_GIALLO,P_BIANCO,msg_161},
	{60,120,180,135,-1,P_GIALLO,P_BIANCO,msg_162},
	{60,160,180,175,-1,P_GIALLO,P_BIANCO,msg_163},
};

struct Bottone hl[]=
{
	{5,31,79,38	,-1,14,13,NULL},{83,31,157,38	,-17,14,13,NULL},{161,31,235,38	,-33,14,13,NULL},{239,31,313,38,-49,14,13,NULL},
	{5,42,79,49	,-2,14,13,NULL},{83,42,157,49	,-18,14,13,NULL},{161,42,235,49	,-34,14,13,NULL},{239,42,313,49,-50,14,13,NULL},
	{5,53,79,60	,-3,14,13,NULL},{83,53,157,60	,-19,14,13,NULL},{161,53,235,60	,-35,14,13,NULL},{239,53,313,60,-51,14,13,NULL},
	{5,64,79,71	,-4,14,13,NULL},{83,64,157,71	,-20,14,13,NULL},{161,64,235,71	,-36,14,13,NULL},{239,64,313,71,-52,14,13,NULL},
	{5,75,79,82	,-5,14,13,NULL},{83,75,157,82	,-21,14,13,NULL},{161,75,235,82	,-37,14,13,NULL},{239,75,313,82,-53,14,13,NULL},
	{5,86,79,93	,-6,14,13,NULL},{83,86,157,93	,-22,14,13,NULL},{161,86,235,93	,-38,14,13,NULL},{239,86,313,93,-54,14,13,NULL},
	{5,97,79,104	,-7,14,13,NULL},{83,97,157,104	,-23,14,13,NULL},{161,97,235,104,-39,14,13,NULL},{239,97,313,104,-55,14,13,NULL},
	{5,108,79,115,-8,14,13,NULL},{83,108,157,115,-24,14,13,NULL},{161,108,235,115,-40,14,13,NULL},{239,108,313,115,-56,14,13,NULL},
	{5,119,79,126,-9,14,13,NULL},{83,119,157,126,-25,14,13,NULL},{161,119,235,126,-41,14,13,NULL},{239,119,313,126,-57,14,13,NULL},
	{5,130,79,137,-10,14,13,NULL},{83,130,157,137,-26,14,13,NULL},{161,130,235,137,-42,14,13,NULL},{239,130,313,137,-58,14,13,NULL},
	{5,141,79,148,-11,14,13,NULL},{83,141,157,148,-27,14,13,NULL},{161,141,235,148,-43,14,13,NULL},{239,141,313,148,-59,14,13,NULL},
	{5,152,79,159,-12,14,13,NULL},{83,152,157,159,-28,14,13,NULL},{161,152,235,159,-44,14,13,NULL},{239,152,313,159,-60,14,13,NULL},
	{5,163,79,170,-13,14,13,NULL},{83,163,157,170,-29,14,13,NULL},{161,163,235,170,-45,14,13,NULL},{239,163,313,170,-61,14,13,NULL},
	{5,174,79,181,-14,14,13,NULL},{83,174,157,181,-30,14,13,NULL},{161,174,235,181,-46,14,13,NULL},{239,174,313,181,-62,14,13,NULL},
	{5,185,79,192,-15,14,13,NULL},{83,185,157,192,-31,14,13,NULL},{161,185,235,192,-47,14,13,NULL},{239,185,313,192,-63,14,13,NULL},
	{5,196,79,203,-16,14,13,NULL},{83,196,157,203,-32,14,13,NULL}, 	{161,196,235,203,-48,14,13,NULL},{239,196,313,203,-64,14,13,NULL},
	{60,225,259,240,MENU_HIGHLIGHT,14,13,msg_6},

};

struct GfxMenu menu[]=
{
	{msg_164,main_bottoni,NULL,7,0,-1,0,0,-1,14,13,NULL},
	{msg_86,prefs_bottoni,NULL,6,0,-1,160,38,0,3,0,NULL},
	{msg_88,high_bottoni,NULL,4,0,-1,187,54,1,11,12,NULL},
	{msg_87,teams_bottoni,NULL,5,0,-1,181,56,2,6,8,NULL},
	{msg_165,start_bottoni,NULL,6,0,-1,187,57,3,7,9,NULL},
	{msg_102,arcade_bottoni,NULL,4,0,-1,187,57,3,7,9,NULL},
	{msg_103,sim_bottoni,NULL,7,0,-1,187,57,3,7,9,NULL},
	{msg_104,career_bottoni,NULL,6,0,-1,187,57,3,7,9,NULL},
	{msg_166,train_bottoni,NULL,4,0,-1,187,57,3,7,9,NULL},
	{msg_167,teamselection,NULL,67,0,-1,0,0,-1,7,9,(APTR)TeamSelection},
	{msg_89,score_bottoni,NULL,3,0,-1,187,57,4,8,15,NULL},
	{msg_154,teamsettings,pannelli,43 /* Era 44 */,51,-1,202,31,5,6,8,(APTR)TeamSettings},
	{msg_168,gprefs_bottoni,NULL,27,0,-1,190,38,0,3,0,(APTR)GamePrefs},
	{msg_169,vprefs_bottoni,NULL,27,0,-1,190,38,0,3,0,(APTR)VideoPrefs},
	{msg_170,aprefs_bottoni,NULL,11,0,-1,190,38,0,3,0,(APTR)AudioPrefs},
	{"SYSTEM",sprefs_bottoni,NULL,13,0,-1,190,38,0,3,0,(APTR)SystemPrefs},
	{msg_171,wcb,wcp,3,5*40,-1,0,0,-1,7,9,NULL},
	{NULL,mb,mp,3,16*4,-1,0,0,-1,7,9,NULL},
	{msg_172,lb,league,3,8*21,-1,0,0,-1,7,9,NULL},
	{msg_173,seltype_bottoni,NULL,3,0,-1,187,57,3,7,9,NULL},
	{msg_174,asb,NULL,ARCADE_TEAMS+3,0,-1,0,0,-1,7,9,(APTR)ArcadeTeamSelection},
	{msg_41,cb,cp,2,7,-1,0,0,-1,7,9,NULL},
	{msg_175,wcfb,wcfp,1,8,-1,0,0,-1,7,9,NULL},
	{msg_176,mrb,mr,1,4,-1,0,0,-1,7,9,(APTR)ReturnFalse},
	{msg_177,scorelist_bottoni,scores,1,40,-1,0,0,-1,8,15,NULL},
	{NULL,hl,NULL,64+1,0,-1,0,0,-1,11,12,(APTR)HighSelection},
	{"KEYBOARD CONFIGURATION",keycfg_bottoni,NULL,43,0,-1,200 /*190*/,77 /*38*/,0,3,0,(APTR)KeyCfg},
	{"JOYSTICK CONFIGURATION",joycfg_bottoni,NULL,15,0,-1,190,38,0,3,0,(APTR)JoyCfg},
};

