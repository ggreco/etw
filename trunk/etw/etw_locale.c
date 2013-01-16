#include <stdlib.h>
#include <stdio.h>
#ifdef WIN
#include <windows.h>
#endif

#include "eat.h"
#include "files.h"

struct __LString
{
   struct __LString *Next;
   int CatalogID;
   char Str[1];
};

struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__0 = {0,0,"CONTINUE"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__1 = {&__LTool__0,1,"CHALLENGE"
};
struct {void *Prev;int Cat_ID;char Str[(12)+1];} __LTool__2 = {&__LTool__1,2,"RANDOM"
};
struct {void *Prev;int Cat_ID;char Str[(1)+1];} __LTool__3 = {&__LTool__2,3,"D"
};
struct {void *Prev;int Cat_ID;char Str[(1)+1];} __LTool__4 = {&__LTool__3,4,"M"
};
struct {void *Prev;int Cat_ID;char Str[(1)+1];} __LTool__5 = {&__LTool__4,5,"A"
};
struct {void *Prev;int Cat_ID;char Str[(22)+1];} __LTool__6 = {&__LTool__5,6,"PARENT MENU"
};
struct {void *Prev;int Cat_ID;char Str[(8)+1];} __LTool__7 = {&__LTool__6,7,"ON"
};
struct {void *Prev;int Cat_ID;char Str[(8)+1];} __LTool__8 = {&__LTool__7,8,"OFF"
};
struct {void *Prev;int Cat_ID;char Str[(12)+1];} __LTool__9 = {&__LTool__8,9,"DAY"
};
struct {void *Prev;int Cat_ID;char Str[(12)+1];} __LTool__10 = {&__LTool__9,10,"NIGHT"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__11 = {&__LTool__10,11,"BIG TOPLEFT"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__12 = {&__LTool__11,12,"BIG TOP"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__13 = {&__LTool__12,13,"BIG TOPRIGHT"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__14 = {&__LTool__13,14,"BIG BOTTOMLEFT"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__15 = {&__LTool__14,15,"BIG BOTTOM"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__16 = {&__LTool__15,16,"BIG BOTTOMRIGHT"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__17 = {&__LTool__16,17,"SMALL TOPLEFT"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__18 = {&__LTool__17,18,"SMALL TOP"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__19 = {&__LTool__18,19,"SMALL TOPRRIGHT"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__20 = {&__LTool__19,20,"SMALL BOTTOMLEFT"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__21 = {&__LTool__20,21,"SMALL BOTTOM"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__22 = {&__LTool__21,22,"SMALL BOTTOMRIGHT"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__23 = {&__LTool__22,23,"NORMAL"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__24 = {&__LTool__23,24,"DRY"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__25 = {&__LTool__24,25,"HARD"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__26 = {&__LTool__25,26,"SOFT"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__27 = {&__LTool__26,27,"FROZEN"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__28 = {&__LTool__27,28,"MUDDY"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__29 = {&__LTool__28,29,"WET"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__30 = {&__LTool__29,30,"SNOW"
};
struct {void *Prev;int Cat_ID;char Str[(26)+1];} __LTool__31 = {&__LTool__30,31,"VIEW SELECTED"
};
struct {void *Prev;int Cat_ID;char Str[(14)+1];} __LTool__32 = {&__LTool__31,32,"ETW CUP"
};
struct {void *Prev;int Cat_ID;char Str[(32)+1];} __LTool__33 = {&__LTool__32,33,"THE WHISTLE TOUR"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__34 = {&__LTool__33,34,"Load tactic..."
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__35 = {&__LTool__34,35,"PITCH TYPE"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__36 = {&__LTool__35,36,"TARGETED"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__37 = {&__LTool__36,37,"FREE"
};
struct {void *Prev;int Cat_ID;char Str[(98)+1];} __LTool__38 = {&__LTool__37,38,"Spoken comment available\nonly in the CD version!"
};
struct {void *Prev;int Cat_ID;char Str[(90)+1];} __LTool__39 = {&__LTool__38,39,"Menu music available\nonly in the CD version!"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__40 = {&__LTool__39,40,"ASK"
};
struct {void *Prev;int Cat_ID;char Str[(32)+1];} __LTool__41 = {&__LTool__40,41,"ARCADE CHALLENGE"
};
struct {void *Prev;int Cat_ID;char Str[(30)+1];} __LTool__42 = {&__LTool__41,42,"THE FINAL MATCH"
};
struct {void *Prev;int Cat_ID;char Str[(24)+1];} __LTool__43 = {&__LTool__42,43,"CUP FIXTURES"
};
struct {void *Prev;int Cat_ID;char Str[(30)+1];} __LTool__44 = {&__LTool__43,44,"LEAGUE FIXTURES"
};
struct {void *Prev;int Cat_ID;char Str[(44)+1];} __LTool__45 = {&__LTool__44,45,"WORLD CUP: FIRST MATCH"
};
struct {void *Prev;int Cat_ID;char Str[(46)+1];} __LTool__46 = {&__LTool__45,46,"WORLD CUP: SECOND MATCH"
};
struct {void *Prev;int Cat_ID;char Str[(44)+1];} __LTool__47 = {&__LTool__46,47,"WORLD CUP: THIRD MATCH"
};
struct {void *Prev;int Cat_ID;char Str[(36)+1];} __LTool__48 = {&__LTool__47,48,"WORLD CUP: PHASE 2"
};
struct {void *Prev;int Cat_ID;char Str[(32)+1];} __LTool__49 = {&__LTool__48,49,"WORLD CUP FINALS"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__50 = {&__LTool__49,50,"FINAL"
};
struct {void *Prev;int Cat_ID;char Str[(22)+1];} __LTool__51 = {&__LTool__50,51,"THIRD PLACE"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__52 = {&__LTool__51,52,"GAME OVER"
};
struct {void *Prev;int Cat_ID;char Str[(48)+1];} __LTool__53 = {&__LTool__52,53,"THE WHISTLE TOUR RESULTS"
};
struct {void *Prev;int Cat_ID;char Str[(22)+1];} __LTool__54 = {&__LTool__53,54,"CUP RESULTS"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__55 = {&__LTool__54,55,"LEAGUE RESULTS"
};
struct {void *Prev;int Cat_ID;char Str[(14)+1];} __LTool__56 = {&__LTool__55,56,"RESULTS"
};
struct {void *Prev;int Cat_ID;char Str[(46)+1];} __LTool__57 = {&__LTool__56,57,"Eat The Whistle Request"
};
struct {void *Prev;int Cat_ID;char Str[(8)+1];} __LTool__58 = {&__LTool__57,58,"Ok"
};
struct {void *Prev;int Cat_ID;char Str[(176)+1];} __LTool__59 = {&__LTool__58,59,"A competition is already in progress.\nStarting a new one wi"
"ll delete\nthe previous one!"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__60 = {&__LTool__59,60,"Proceed|Cancel"
};
struct {void *Prev;int Cat_ID;char Str[(146)+1];} __LTool__61 = {&__LTool__60,61,"Warning a career/tournament\nis already in progress,\ndatas "
"will be lost!"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__62 = {&__LTool__61,62,"Ok|Cancel"
};
struct {void *Prev;int Cat_ID;char Str[(30)+1];} __LTool__63 = {&__LTool__62,63,"VIEW HIGHLIGHTS"
};
struct {void *Prev;int Cat_ID;char Str[(30)+1];} __LTool__64 = {&__LTool__63,64,"SAVE HIGHLIGHTS"
};
struct {void *Prev;int Cat_ID;char Str[(26)+1];} __LTool__65 = {&__LTool__64,65,"Are you sure?"
};
struct {void *Prev;int Cat_ID;char Str[(30)+1];} __LTool__66 = {&__LTool__65,66,"Continue|Cancel"
};
struct {void *Prev;int Cat_ID;char Str[(70)+1];} __LTool__67 = {&__LTool__66,67,"Arcade competitions can't be saved!"
};
struct {void *Prev;int Cat_ID;char Str[(46)+1];} __LTool__68 = {&__LTool__67,68,"Save the competition..."
};
struct {void *Prev;int Cat_ID;char Str[(138)+1];} __LTool__69 = {&__LTool__68,69,"Unable to save ETW settings!\nNeither in PROGDIR: neither in"
" ENVARC:!"
};
struct {void *Prev;int Cat_ID;char Str[(30)+1];} __LTool__70 = {&__LTool__69,70,"Settings saved."
};
struct {void *Prev;int Cat_ID;char Str[(26)+1];} __LTool__71 = {&__LTool__70,71,"Load teams..."
};
struct {void *Prev;int Cat_ID;char Str[(42)+1];} __LTool__72 = {&__LTool__71,72,"Save current teams..."
};
struct {void *Prev;int Cat_ID;char Str[(124)+1];} __LTool__73 = {&__LTool__72,73,"Actually you have to use\nThe MUI editor available\nin Amine"
"t!"
};
struct {void *Prev;int Cat_ID;char Str[(34)+1];} __LTool__74 = {&__LTool__73,74,"Load highlight..."
};
struct {void *Prev;int Cat_ID;char Str[(152)+1];} __LTool__75 = {&__LTool__74,75,"Sorry, this feature is\nnot yet implemented.\nwill be availa"
"ble in ETW 98-99"
};
struct {void *Prev;int Cat_ID;char Str[(32)+1];} __LTool__76 = {&__LTool__75,76,"Load a career..."
};
struct {void *Prev;int Cat_ID;char Str[(34)+1];} __LTool__77 = {&__LTool__76,77,"Load a league/cup"
};
struct {void *Prev;int Cat_ID;char Str[(32)+1];} __LTool__78 = {&__LTool__77,78,"Save a career..."
};
struct {void *Prev;int Cat_ID;char Str[(34)+1];} __LTool__79 = {&__LTool__78,79,"Save a league/cup"
};
struct {void *Prev;int Cat_ID;char Str[(12)+1];} __LTool__80 = {&__LTool__79,80,"COACH "
};
struct {void *Prev;int Cat_ID;char Str[(40)+1];} __LTool__81 = {&__LTool__80,81,"(ALPHABETICAL ORDER)"
};
struct {void *Prev;int Cat_ID;char Str[(16)+1];} __LTool__82 = {&__LTool__81,82,"Continue"
};
struct {void *Prev;int Cat_ID;char Str[(10)+1];} __LTool__83 = {&__LTool__82,83,"Abort"
};
struct {void *Prev;int Cat_ID;char Str[(244)+1];} __LTool__84 = {&__LTool__83,84,"Warning!\nAn error has occurred,\nmaybe there is not enough "
"free\nmemory or you may not have\ncorrecly installed the gam"
"e."
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__85 = {&__LTool__84,85,"GAME_START"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__86 = {&__LTool__85,86,"PREFERENCES"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__87 = {&__LTool__86,87,"TEAMS"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__88 = {&__LTool__87,88,"HIGHLIGHTS"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__89 = {&__LTool__88,89,"SCORES"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__90 = {&__LTool__89,90,"CREDITS"
};
struct {void *Prev;int Cat_ID;char Str[(22)+1];} __LTool__91 = {&__LTool__90,91,"EXIT TO DOS"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__92 = {&__LTool__91,92,"GAME"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__93 = {&__LTool__92,93,"VIDEO"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__94 = {&__LTool__93,94,"AUDIO"
};
struct {void *Prev;int Cat_ID;char Str[(34)+1];} __LTool__95 = {&__LTool__94,95,"BACK TO MAIN MENU"
};
struct {void *Prev;int Cat_ID;char Str[(26)+1];} __LTool__96 = {&__LTool__95,96,"SAVE SETTINGS"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__97 = {&__LTool__96,97,"VIEW"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__98 = {&__LTool__97,98,"SAVE"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__99 = {&__LTool__98,99,"LOAD"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__100 = {&__LTool__99,100,"EDIT"
};
struct {void *Prev;int Cat_ID;char Str[(14)+1];} __LTool__101 = {&__LTool__100,101,"DEFAULT"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__102 = {&__LTool__101,102,"ARCADE"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__103 = {&__LTool__102,103,"SIMULATION"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__104 = {&__LTool__103,104,"CAREER"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__105 = {&__LTool__104,105,"PRACTICE"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__106 = {&__LTool__105,106,"SINGLE MATCH"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__107 = {&__LTool__106,107,"WHISTLE TOUR"
};
struct {void *Prev;int Cat_ID;char Str[(42)+1];} __LTool__108 = {&__LTool__107,108,"BACK TO PREVIOUS MENU"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__109 = {&__LTool__108,109,"FRIENDLY"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__110 = {&__LTool__109,110,"LEAGUE"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__111 = {&__LTool__110,111,"CUP"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__112 = {&__LTool__111,112,"MENAGE ONLY"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__113 = {&__LTool__112,113,"MENAGE AND PLAY"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__114 = {&__LTool__113,114,"ROLE MODE"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__115 = {&__LTool__114,115,"PLAY SESSION"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__116 = {&__LTool__115,116,"FREE KICKS"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__117 = {&__LTool__116,117,"PENALTIES"
};
struct {void *Prev;int Cat_ID;char Str[(10)+1];} __LTool__118 = {&__LTool__117,118,"RESET"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__119 = {&__LTool__118,119,"DURATION"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__120 = {&__LTool__119,120,"PITCH CONDITION"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__121 = {&__LTool__120,121,"REFREE STRICTNESS"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__122 = {&__LTool__121,122,"INJURIES"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__123 = {&__LTool__122,123,"SUBSTITUTIONS"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__124 = {&__LTool__123,124,"BOOKINGS"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__125 = {&__LTool__124,125,"AUTO REPLAY"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__126 = {&__LTool__125,126,"LONG PASS"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__127 = {&__LTool__126,127,"JOY PORT (BLUE)"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__128 = {&__LTool__127,128,"MOUSE PORT (RED)"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__129 = {&__LTool__128,129,"DAYTIME"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__130 = {&__LTool__129,130,"SCREEN MODE"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__131 = {&__LTool__130,131,"RADAR"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__132 = {&__LTool__131,132,"STATUS BAR"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__133 = {&__LTool__132,133,"REFREE"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__134 = {&__LTool__133,134,"PEOPLE"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__135 = {&__LTool__134,135,"PLAYER NAMES"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__136 = {&__LTool__135,136,"CROWD"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__137 = {&__LTool__136,137,"SPEAKER"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__138 = {&__LTool__137,138,"MENU MUSIC"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__139 = {&__LTool__138,139,"ARCADE TEAMS"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__140 = {&__LTool__139,140,"NORMAL TEAMS"
};
struct {void *Prev;int Cat_ID;char Str[(1)+1];} __LTool__141 = {&__LTool__140,141,"G"
};
struct {void *Prev;int Cat_ID;char Str[(14)+1];} __LTool__142 = {&__LTool__141,142,"GROUP A"
};
struct {void *Prev;int Cat_ID;char Str[(2)+1];} __LTool__143 = {&__LTool__142,143,"PL"
};
struct {void *Prev;int Cat_ID;char Str[(2)+1];} __LTool__144 = {&__LTool__143,144,"PT"
};
struct {void *Prev;int Cat_ID;char Str[(2)+1];} __LTool__145 = {&__LTool__144,145,"G+"
};
struct {void *Prev;int Cat_ID;char Str[(2)+1];} __LTool__146 = {&__LTool__145,146,"G-"
};
struct {void *Prev;int Cat_ID;char Str[(14)+1];} __LTool__147 = {&__LTool__146,147,"GROUP B"
};
struct {void *Prev;int Cat_ID;char Str[(14)+1];} __LTool__148 = {&__LTool__147,148,"GROUP C"
};
struct {void *Prev;int Cat_ID;char Str[(14)+1];} __LTool__149 = {&__LTool__148,149,"GROUP D"
};
struct {void *Prev;int Cat_ID;char Str[(14)+1];} __LTool__150 = {&__LTool__149,150,"GROUP E"
};
struct {void *Prev;int Cat_ID;char Str[(14)+1];} __LTool__151 = {&__LTool__150,151,"GROUP F"
};
struct {void *Prev;int Cat_ID;char Str[(14)+1];} __LTool__152 = {&__LTool__151,152,"GROUP G"
};
struct {void *Prev;int Cat_ID;char Str[(14)+1];} __LTool__153 = {&__LTool__152,153,"GROUP H"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__154 = {&__LTool__153,154,"TEAM NAME"
};
struct {void *Prev;int Cat_ID;char Str[(3)+1];} __LTool__155 = {&__LTool__154,155,"PLD"
};
struct {void *Prev;int Cat_ID;char Str[(3)+1];} __LTool__156 = {&__LTool__155,156,"PTS"
};
struct {void *Prev;int Cat_ID;char Str[(3)+1];} __LTool__157 = {&__LTool__156,157,"WIN"
};
struct {void *Prev;int Cat_ID;char Str[(3)+1];} __LTool__158 = {&__LTool__157,158,"DRW"
};
struct {void *Prev;int Cat_ID;char Str[(3)+1];} __LTool__159 = {&__LTool__158,159,"LST"
};
struct {void *Prev;int Cat_ID;char Str[(12)+1];} __LTool__160 = {&__LTool__159,160,"WINNER"
};
struct {void *Prev;int Cat_ID;char Str[(18)+1];} __LTool__161 = {&__LTool__160,161,"RUNNER-UP"
};
struct {void *Prev;int Cat_ID;char Str[(10)+1];} __LTool__162 = {&__LTool__161,162,"THIRD"
};
struct {void *Prev;int Cat_ID;char Str[(10)+1];} __LTool__163 = {&__LTool__162,163,"FORTH"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__164 = {&__LTool__163,164,"MAIN MENU"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__165 = {&__LTool__164,165,"GAME START"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__166 = {&__LTool__165,166,"TRAINING"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__167 = {&__LTool__166,167,"TEAM SELECTION"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__168 = {&__LTool__167,168,"GAME OPTIONS"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__169 = {&__LTool__168,169,"VIDEO OPTIONS"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__170 = {&__LTool__169,170,"AUDIO OPTIONS"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__171 = {&__LTool__170,171,"WORLD CUP"
};
struct {void *Prev;int Cat_ID;char Str[(28)+1];} __LTool__172 = {&__LTool__171,172,"LEAGUE TABLE"
};
struct {void *Prev;int Cat_ID;char Str[(40)+1];} __LTool__173 = {&__LTool__172,173,"TEAM TYPE SELECTION"
};
struct {void *Prev;int Cat_ID;char Str[(40)+1];} __LTool__174 = {&__LTool__173,174,"ARCADE TEAM SELECTION"
};
struct {void *Prev;int Cat_ID;char Str[(40)+1];} __LTool__175 = {&__LTool__174,175,"WORLD CUP RESULT"
};
struct {void *Prev;int Cat_ID;char Str[(30)+1];} __LTool__176 = {&__LTool__175,176,"MATCH RESULT"
};
struct {void *Prev;int Cat_ID;char Str[(30)+1];} __LTool__177 = {&__LTool__176,177,"HIGHSCORE"
};
struct {void *Prev;int Cat_ID;char Str[(198)+1];} __LTool__178 = {&__LTool__177,178,"Warning:\nActually ETW support leagues with up\nto 20 teams,"
" so addictional teams\nwill be ignored."
};
struct {void *Prev;int Cat_ID;char Str[(100)+1];} __LTool__179 = {&__LTool__178,179,"Warning, the competition\nin progress will be lost."
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__180 = {&__LTool__179,180,"RESET CURRENT"
};
struct {void *Prev;int Cat_ID;char Str[(20)+1];} __LTool__181 = {&__LTool__180,181,"INTRO"
};
struct {void *Prev;int Cat_ID;char Str[(41)+1];} __LTool__182 = {&__LTool__181,182,"ABANDON MATCH"};
struct {void *Prev;int Cat_ID;char Str[(39)+1];} __LTool__183 = {&__LTool__182,183,"ACHIEVEMENTS"};
struct {void *Prev;int Cat_ID;char Str[(37)+1];} __LTool__184 = {&__LTool__183,184,"AUDIO VIDEO"};
struct {void *Prev;int Cat_ID;char Str[(31)+1];} __LTool__185 = {&__LTool__184,185,"BUTTON 1"};
struct {void *Prev;int Cat_ID;char Str[(33)+1];} __LTool__186 = {&__LTool__185,186,"BUTTON 10"};
struct {void *Prev;int Cat_ID;char Str[(33)+1];} __LTool__187 = {&__LTool__186,187,"BUTTON 11"};
struct {void *Prev;int Cat_ID;char Str[(33)+1];} __LTool__188 = {&__LTool__187,188,"BUTTON 12"};
struct {void *Prev;int Cat_ID;char Str[(33)+1];} __LTool__189 = {&__LTool__188,189,"BUTTON 13"};
struct {void *Prev;int Cat_ID;char Str[(31)+1];} __LTool__190 = {&__LTool__189,190,"BUTTON 2"};
struct {void *Prev;int Cat_ID;char Str[(31)+1];} __LTool__191 = {&__LTool__190,191,"BUTTON 3"};
struct {void *Prev;int Cat_ID;char Str[(31)+1];} __LTool__192 = {&__LTool__191,192,"BUTTON 4"};
struct {void *Prev;int Cat_ID;char Str[(31)+1];} __LTool__193 = {&__LTool__192,193,"BUTTON 5"};
struct {void *Prev;int Cat_ID;char Str[(31)+1];} __LTool__194 = {&__LTool__193,194,"BUTTON 6"};
struct {void *Prev;int Cat_ID;char Str[(31)+1];} __LTool__195 = {&__LTool__194,195,"BUTTON 7"};
struct {void *Prev;int Cat_ID;char Str[(31)+1];} __LTool__196 = {&__LTool__195,196,"BUTTON 8"};
struct {void *Prev;int Cat_ID;char Str[(31)+1];} __LTool__197 = {&__LTool__196,197,"BUTTON 9"};
struct {void *Prev;int Cat_ID;char Str[(27)+1];} __LTool__198 = {&__LTool__197,198,"CANCEL"};
struct {void *Prev;int Cat_ID;char Str[(41)+1];} __LTool__199 = {&__LTool__198,199,"CHANGE ACTIVE"};
struct {void *Prev;int Cat_ID;char Str[(29)+1];} __LTool__200 = {&__LTool__199,200,"CLASSIC"};
struct {void *Prev;int Cat_ID;char Str[(51)+1];} __LTool__201 = {&__LTool__200,201,"Career files|*.car"};
struct {void *Prev;int Cat_ID;char Str[(27)+1];} __LTool__202 = {&__LTool__201,202,"DELETE"};
struct {void *Prev;int Cat_ID;char Str[(31)+1];} __LTool__203 = {&__LTool__202,203,"FAST RUN"};
struct {void *Prev;int Cat_ID;char Str[(43)+1];} __LTool__204 = {&__LTool__203,204,"FILTERING PASS"};
struct {void *Prev;int Cat_ID;char Str[(59)+1];} __LTool__205 = {&__LTool__204,205,"JOYSTICK CONFIGURATION"};
struct {void *Prev;int Cat_ID;char Str[(45)+1];} __LTool__206 = {&__LTool__205,206,"KEYBOARD CONFIG"};
struct {void *Prev;int Cat_ID;char Str[(59)+1];} __LTool__207 = {&__LTool__206,207,"KEYBOARD CONFIGURATION"};
struct {void *Prev;int Cat_ID;char Str[(33)+1];} __LTool__208 = {&__LTool__207,208,"MOVE DOWN"};
struct {void *Prev;int Cat_ID;char Str[(33)+1];} __LTool__209 = {&__LTool__208,209,"MOVE LEFT"};
struct {void *Prev;int Cat_ID;char Str[(35)+1];} __LTool__210 = {&__LTool__209,210,"MOVE RIGHT"};
struct {void *Prev;int Cat_ID;char Str[(29)+1];} __LTool__211 = {&__LTool__210,211,"MOVE UP"};
struct {void *Prev;int Cat_ID;char Str[(39)+1];} __LTool__212 = {&__LTool__211,212,"NETWORK PLAY"};
struct {void *Prev;int Cat_ID;char Str[(49)+1];} __LTool__213 = {&__LTool__212,213,"NO JOYSTICK FOUND"};
struct {void *Prev;int Cat_ID;char Str[(25)+1];} __LTool__214 = {&__LTool__213,214,"PAUSE"};
struct {void *Prev;int Cat_ID;char Str[(27)+1];} __LTool__215 = {&__LTool__214,215,"REPLAY"};
struct {void *Prev;int Cat_ID;char Str[(35)+1];} __LTool__216 = {&__LTool__215,216,"RESOLUTION"};
struct {void *Prev;int Cat_ID;char Str[(29)+1];} __LTool__217 = {&__LTool__216,217,"SCALING"};
struct {void *Prev;int Cat_ID;char Str[(37)+1];} __LTool__218 = {&__LTool__217,218,"SCALING RES"};
struct {void *Prev;int Cat_ID;char Str[(75)+1];} __LTool__219 = {&__LTool__218,219,"SELECT 2, 4, 8, 16 OR 32 TEAMS"};
struct {void *Prev;int Cat_ID;char Str[(51)+1];} __LTool__220 = {&__LTool__219,220,"SELECT EIGHT TEAMS"};
struct {void *Prev;int Cat_ID;char Str[(97)+1];} __LTool__221 = {&__LTool__220,221,"SELECT THREE OR MORE TEAMS FOR THE LEAGUE"};
struct {void *Prev;int Cat_ID;char Str[(47)+1];} __LTool__222 = {&__LTool__221,222,"SELECT TWO TEAMS"};
struct {void *Prev;int Cat_ID;char Str[(47)+1];} __LTool__223 = {&__LTool__222,223,"SELECT YOUR TEAM"};
struct {void *Prev;int Cat_ID;char Str[(35)+1];} __LTool__224 = {&__LTool__223,224,"SHORT PASS"};
struct {void *Prev;int Cat_ID;char Str[(23)+1];} __LTool__225 = {&__LTool__224,225,"SHOT"};
struct {void *Prev;int Cat_ID;char Str[(77)+1];} __LTool__226 = {&__LTool__225,226,"SORRY, SELECTED KEY IS RESERVED"};
struct {void *Prev;int Cat_ID;char Str[(27)+1];} __LTool__227 = {&__LTool__226,227,"SYSTEM"};
struct {void *Prev;int Cat_ID;char Str[(49)+1];} __LTool__228 = {&__LTool__227,228,"Save highlight..."};
struct {void *Prev;int Cat_ID;char Str[(29)+1];} __LTool__229 = {&__LTool__228,229,"TACTICS"};
struct {void *Prev;int Cat_ID;char Str[(31)+1];} __LTool__230 = {&__LTool__229,230,"TUTORIAL"};
struct {void *Prev;int Cat_ID;char Str[(29)+1];} __LTool__231 = {&__LTool__230,231,"VIEW %s"};
struct {void *Prev;int Cat_ID;char Str[(49)+1];} __LTool__232 = {&__LTool__231,232,"WAITING BUTTON..."};
struct {void *Prev;int Cat_ID;char Str[(25)+1];} __LTool__233 = {&__LTool__232,233,"WORLD"};
struct {void *Prev;int Cat_ID;char Str[(121)+1];} __LTool__234 = {&__LTool__233,234,"You can enable scaling only\nif you play in a window!"};
struct {void *Prev;int Cat_ID;char Str[(379)+1];} __LTool__235 = {&__LTool__234,235,"Arcade mode is an original football variation played in a speedball-like arena, with pickable bonuses, and crazy teams. Try to win the arcade challenge or the whistle cup if you can!"};
struct {void *Prev;int Cat_ID;char Str[(305)+1];} __LTool__236 = {&__LTool__235,236,"During a match you can record an highlight using the red circle icon during the replay, through this menu you can view an highlight or remove it."};
struct {void *Prev;int Cat_ID;char Str[(357)+1];} __LTool__237 = {&__LTool__236,237,"Eat the Whistle has two different game modes: Simulation and Arcade.\n\nBefore trying one of them you'll better practice a bit with the controls using the practice option."};
struct {void *Prev;int Cat_ID;char Str[(391)+1];} __LTool__238 = {&__LTool__237,238,"Eat the whistle supports Game Center for achievements and scores. Please remember that you need to log in to Game Center in order to record your achievements and compete with your friends."};
struct {void *Prev;int Cat_ID;char Str[(211)+1];} __LTool__239 = {&__LTool__238,239,"Now play a training session.\n\nThis will let you learn the basics and practice the game controls."};
struct {void *Prev;int Cat_ID;char Str[(569)+1];} __LTool__240 = {&__LTool__239,240,"Pressing the RED button when the ball is in no possession will perform a special action that will change depending from the ball height and your position, an head shot, a bicycle kick, a tackle... anything can happens if you tap it with the right timing and in a good position!"};
struct {void *Prev;int Cat_ID;char Str[(249)+1];} __LTool__241 = {&__LTool__240,241,"Simulation mode will offer you an old-style soccer game experience, you can play a league, a cup or a friendly match!"};
struct {void *Prev;int Cat_ID;char Str[(307)+1];} __LTool__242 = {&__LTool__241,242,"Tap a button when the arrow points to the goal position you want to shoot to, the longer you keep the button pressed the higher the shoot will be."};
struct {void *Prev;int Cat_ID;char Str[(395)+1];} __LTool__243 = {&__LTool__242,243,"Tapping the RED button when the other team has the ball possession will make the active player try to get the ball with a sliding tackle toward the ball, but be careful to not commit a foul!"};
struct {void *Prev;int Cat_ID;char Str[(381)+1];} __LTool__244 = {&__LTool__243,244,"The Arcade Challenge is the ultimate Eat The Whistle... challenge, you will have to win against all the arcade teams, except the one you have chosen, and a secret team to complete it."};
struct {void *Prev;int Cat_ID;char Str[(463)+1];} __LTool__245 = {&__LTool__244,245,"The virtual joystick has two different speed for each direction, if you move the red ball more distant from the center the player will run faster but you'll have a less responsive control if you want to change his direction."};
struct {void *Prev;int Cat_ID;char Str[(203)+1];} __LTool__246 = {&__LTool__245,246,"This menu let you change audio-video settings, you can also disable this tutorial if you want."};
struct {void *Prev;int Cat_ID;char Str[(561)+1];} __LTool__247 = {&__LTool__246,247,"This menu let you choose the arcade team you want to use, an human team is marked with a blue p, a computer controller one with the light gray letter c.\n\nThe number of teams you have to select may vary: 1 for the challenge, 2 for a friendly match, 8 for the whistle tour."};
struct {void *Prev;int Cat_ID;char Str[(621)+1];} __LTool__248 = {&__LTool__247,248,"This menu let you choose the team you want to use, an human team is identified with a blue background and the letter p, a computer controller one with a light gray background and the letter c.\n\nthe number of teams you have to select may vary: 1 for training, 2 for a friendly match, 3+ for a league..."};
struct {void *Prev;int Cat_ID;char Str[(365)+1];} __LTool__249 = {&__LTool__248,249,"This screen let you change your team tactic or the 11 initial players before a match.\nThe stars near the player name are a global valutation of the abilities of your players."};
struct {void *Prev;int Cat_ID;char Str[(345)+1];} __LTool__250 = {&__LTool__249,250,"To shoot:\nSwipe from the ball in the direction of your shot, the longer the swipe the stronger the shot.\n\nTo pass:\nTouch the player you want to pass the ball to."};
struct {void *Prev;int Cat_ID;char Str[(437)+1];} __LTool__251 = {&__LTool__250,251,"Welcome to the Eat The Whistle Tutorial!\n\nWe'll guide you to discover the game features and mechanics.\n\nYou can deactivate this tutorial in any time through the audio video settings, in the preferences menu."};
struct {void *Prev;int Cat_ID;char Str[(241)+1];} __LTool__252 = {&__LTool__251,252,"Welcome to the Eat The Whistle training session, we will guide you through the basics of the game control system."};
struct {void *Prev;int Cat_ID;char Str[(435)+1];} __LTool__253 = {&__LTool__252,253,"When the ball is in possession of the other team you can change the player you control touching another player, alternatively by tapping the blue button you can activate the nearest player to the ball position."};
struct {void *Prev;int Cat_ID;char Str[(481)+1];} __LTool__254 = {&__LTool__253,254,"When the ball is in possession of your team you can use the BLUE button to pass the ball, tap it for a short pass, keep it pressed for a long pass. You can also pass the ball simply touching with a finger the man you want to pass to!"};
struct {void *Prev;int Cat_ID;char Str[(369)+1];} __LTool__255 = {&__LTool__254,255,"You are playing a match without having played a training session, it's strongly suggested to start with a training session to learn the basics of Eat The Whistle control system."};
struct {void *Prev;int Cat_ID;char Str[(339)+1];} __LTool__256 = {&__LTool__255,256,"You can pause the game, change your tactic or view replay or leave the game in any moment through the pause menu (icon on the top right corner of the touchscreen)"};
struct {void *Prev;int Cat_ID;char Str[(441)+1];} __LTool__257 = {&__LTool__256,257,"You can shoot with the RED button, the joystick position when you RELEASE the button will decide the direction and the type of shot, experiment various joystick positions in a training session to find all of them!"};
struct {void *Prev;int Cat_ID;char Str[(403)+1];} __LTool__258 = {&__LTool__257,258,"You have the control of the player with a blue triangle on his head, you can move it with the virtual joystick that appears as soon as you put a finger on the lower left part of the touchscreen."};


#define STR_BEGIN_ENTRY &__LTool__258
#include <stdio.h>

#ifndef CROSSCOMPILER
#include "mytypes.h"
#endif
#include <string.h>

#include "os_defs.h"
#include "mydebug.h"

#define L10N_ITALIAN    "italiano"
#define L10N_GERMAN    "deutsch"
#define L10N_ENGLISH    "english"
#define L10N_FRENCH    "francais"
#define L10N_SPANISH    "espanol"

struct MyString
{
    long offset,id,len;
    char *string;
};

struct MyCatalog
{
    FILE *handle;
    int strings;
    int offsetfirst;
    struct MyString *strings_ptr;
};

#define CAT_DIR GAME_DIR "catalogs/"

#if defined(WIN)

// Questa semplice routine trova la locale di default di win.

char *GetLanguage(void)
{

    WORD langid;

    if ((langid=GetSystemDefaultLangID()))    {
        switch(PRIMARYLANGID(langid)) {
            case LANG_ITALIAN:
                return L10N_ITALIAN;
            case LANG_GERMAN:
                return L10N_GERMAN;
            default:
                return L10N_ENGLISH;
        }
    }
    else
        return L10N_ENGLISH;
}
#elif defined(MACOSX)
char *GetLanguage(void)
{
    extern int MacGetLanguage(void);
    int langid;
    if((langid = MacGetLanguage()))
    {
        switch(langid)
        {
            case 1:
                return L10N_ITALIAN;
            case 2:
                return L10N_GERMAN;
            default:
                return L10N_ENGLISH;
        }
    }
    else
        return L10N_ENGLISH;
}
#elif defined(IPHONE)
char *GetLanguage(void)
{
    extern const char *get_lang_id();
    const char *lang;
    if((lang = get_lang_id()) != NULL)
    {
        if(strstr(lang,"it"))
            return L10N_ITALIAN;
        else if(strstr(lang,"de"))
            return L10N_GERMAN;
    }
        
    return L10N_ENGLISH;
}
#else
char *GetLanguage(void)
{
    char *lang;
    if((lang = getenv("LANG")) != NULL)
    {
        if(strstr(lang,"it"))
            return L10N_ITALIAN;
        else if(strstr(lang,"de"))
            return L10N_GERMAN;
    }
        
    return L10N_ENGLISH;
}

#endif

void AddCtgString(struct MyCatalog *cat,long id,long offset,long len)
{
    static long poolsize=0;

    if(poolsize==0)
    {
        cat->strings_ptr=(struct MyString *)malloc(sizeof(struct MyString)*5);
        poolsize=5;
    }
    else if(poolsize<=(cat->strings+1))
    {
        poolsize*=3;
        poolsize/=2;

        cat->strings_ptr=(struct MyString *)realloc((char *)cat->strings_ptr,sizeof(struct MyString)*poolsize);
    }

    if(cat->strings_ptr)
    {
        cat->strings_ptr[cat->strings].id=id;
        cat->strings_ptr[cat->strings].offset=offset;
        cat->strings_ptr[cat->strings].len=len;
        cat->strings_ptr[cat->strings].string=NULL;
        cat->strings++;
    }
}

static void * OpenCatalog(char *catalog)
{
    char buffer[100],*lang=GetLanguage(); // debug, dovrebbe essere la lingua di sistema
    FILE *f;
    struct MyCatalog *cat;

    strcpy(buffer,CAT_DIR);
    strcat(buffer,lang); 
    strcat(buffer,"/");
    strcat(buffer,catalog);

    if((f=fopen(buffer,"rb")))     {
        D(bug("Found language file %s...\n",buffer));

        if ((cat=(struct MyCatalog *)malloc(sizeof(struct MyCatalog))))    {
            int l,offset=0,t;
            long clen,id,slen;

            cat->handle=f;
            cat->strings=0;
            cat->strings_ptr=NULL;

            fread(buffer,4,1,f);

            if(strncmp(buffer,"FORM",4))
                goto fallback;

            fseek(f,4,SEEK_CUR);
            fread(buffer,4,1,f);

            if(strncmp(buffer,"CTLG",4))
                goto fallback;


            while (strncmp(buffer,"LANG",4)&&!feof(f)) {
                fseek(f,-3,SEEK_CUR);
                fread(buffer,4,1,f);
            }

            if (feof(f))    {
                D(bug("Chunk not found!\n"));
                goto fallback;
            }

            fseek(f,3,SEEK_CUR);
            l=fgetc(f);
            fread(buffer,l,1,f);

            if (stricmp(lang,buffer)) {
                D(bug("Error, language and catalog don't match!\n"));
                goto fallback;
            }

            while(strncmp(buffer,"STRS",4)&&!feof(f))    {
                fseek(f,-3,SEEK_CUR);
                fread(buffer,4,1,f);
            }

            if(feof(f))    {
                D(bug("Chunk non trovato!\n"));
                goto fallback;
            }

            clen = fread_u32(f);

            cat->offsetfirst=ftell(f);

            D(bug("Catalog length: %ld bytes\n",clen));

            while (clen > offset && !feof(f))
            {
                id = fread_u32(f);
                slen = fread_u32(f);

                offset += 8;

                AddCtgString(cat,id,offset,slen);

                fseek(f,slen,SEEK_CUR);
                offset+=slen;

                t=(offset>>2)<<2;

                if(t!=offset) {
                    t+=4;
                    t>>=2;
                    t<<=2;
                    fseek(f,t-offset,SEEK_CUR);
                    offset=t;
                }
            }

            D(bug("Found %ld language strings in catalog...\n",cat->strings));

            return cat;
        }
        else {
fallback:
            D(bug("Not an IFF catalog!\n"));
            fclose(f);
            return NULL;
        }
    }
    else {
        D(bug("*** %s not found, using builtin language.\n",buffer));
        return NULL;
    }
}

static void CloseCatalog(void * ctg)
{
    if(ctg)    {
        struct MyCatalog *c=ctg;
        int i;

        fclose(c->handle);

        if(c->strings_ptr) {
            for(i=0;i<c->strings;i++) {
                if(c->strings_ptr[i].string)
                    free(c->strings_ptr[i].string);
            }

            free(c->strings_ptr);
        }

        free(c);
    }
}

static char *GetCatalogStr(void *Ctg, long num, char *def)
{
    struct MyCatalog *cat=(struct MyCatalog *)Ctg;

    if(cat!=NULL && cat->strings_ptr) {
        int i;

        for(i=0;i<cat->strings;i++)
            if(cat->strings_ptr[i].id==num) {
                if(cat->strings_ptr[i].string)
                    return cat->strings_ptr[i].string;
                else {
                    cat->strings_ptr[i].string=(char *)malloc(cat->strings_ptr[i].len+1);

                    if(cat->strings_ptr[i].string) {
                        fseek(cat->handle,cat->offsetfirst+cat->strings_ptr[i].offset,SEEK_SET);
                        fread(cat->strings_ptr[i].string,cat->strings_ptr[i].len,1,cat->handle);
                        cat->strings_ptr[i].string[cat->strings_ptr[i].len]=0;
                            
                        return cat->strings_ptr[i].string;
                    }
                }
            }

    }
    return def;
}


// this function can be used to recover old .ct files
//#define CREATE_CATALOG

void InitStrings(void)
{
#ifdef CREATE_CATALOG
   extern void ct_insert(int, const char*, const char*);
   extern void ct_flush();
#endif
   void * Catalog;
   struct __LString *lstr=(struct __LString *)STR_BEGIN_ENTRY;

   Catalog  = OpenCatalog("etw.catalog");

   while(lstr) {
         const char *str = GetCatalogStr(Catalog,lstr->CatalogID,NULL);
         /* We get NULL, if there is no translation ^^*/
#ifdef CREATE_CATALOG
         ct_insert(lstr->CatalogID, str, lstr->Str);
#endif
         if (str) strcpy(lstr->Str,str); /* override builtin string */
         lstr=lstr->Next;
   }
#ifdef CREATE_CATALOG
   ct_flush();
#endif
   CloseCatalog(Catalog);
}


