// extern void GriddedWipe(UBYTE Type, struct BitMap *source);
#include "menu.h"

extern void LoadMenuLogo(char *name);
extern BOOL InitMenuFonts(void);
extern void Intro(void);
extern void Outro(void);
extern void RemoveReqs(void);
extern void EnableReqs(void);
extern void FreeMenuFonts(void);
/* AC: _WINUSER_ is for Microsoft Visual C++ _WINUSER_H for MinGW */
#if !defined(_WINUSER_) && !defined(_WINUSER_H)
extern void ChangeMenu(WORD );
#endif
extern void OpenMenuScreen(void);
extern void PlayMenuMusic(void);
extern void StopMenuMusic(void);
extern void read_menu_config(void);
extern void write_config(char *);
extern BOOL LoadMenuStuff(void);
extern BOOL LoadBack(void);
extern BOOL LoadArcadeBack(void);
extern void FreeMenuStuff(void);
extern void InvertiSquadre(void);
extern void MakeLeague(int);
extern void LoadLeague(void);
extern void LoadArcadeGfx(void);
extern void LoadPLogo(char *);
extern void SaveLeague(void);
extern void InitTable(void);
extern void UpdateLeagueTable(void);
extern void SaveTeams(char *);
extern void LoadTeams(char *);
extern void RedrawBottone(struct Bottone *, UBYTE);
extern void CancellaBottone(struct Bottone *);
extern void ClearScores(void);
extern void PrintShadow(int, int, char *, int, struct myfont *);
extern void MyRestoreBack(void);
extern void ClearScores(void);
extern void LoadScores(void);
extern void ShowCredits(void);
extern void AddScore(UBYTE);
extern BOOL FileRequest(struct MyFileRequest *);
/*
extern void FreeFrames(struct AnimInstData *);
extern struct AnimInstData *LoadFrames( FILE * );
extern void DisplayAnim(struct AnimInstData *);
extern void AllocAnimScreenBuffers(void);
extern void FreeAnimScreenBuffers(void);
extern struct FrameNode *LoadFrame(struct AnimInstData *, ULONG );
extern LONG LoadFrameNode(struct AnimInstData *, struct FrameNode *);
extern void UnloadFrame(struct AnimInstData *, struct FrameNode *);
extern LONG MergeAnim(struct AnimInstData *, FILE *);
extern void DisplayFrame(struct FrameNode *);
extern struct FrameNode *GetFrameNode(struct AnimInstData *, int );
*/
extern void CheckCheat(UBYTE);
extern BOOL CheckNewPitches(void);

// Gestione AHI

void CloseAHI(void);
BOOL OpenAHI(void);
BOOL GetAHIMode(void);

// Procedure speciali dei menu...

extern struct Task *SoundTask[4];

extern BOOL TeamSelection(WORD);
extern BOOL ArcadeTeamSelection(WORD);
extern BOOL TeamSettings(WORD);
extern BOOL VideoPrefs(WORD);
extern BOOL SystemPrefs(WORD);
extern BOOL GamePrefs(WORD);
extern BOOL ReturnFalse(WORD);
extern BOOL AudioPrefs(WORD);
extern BOOL HighSelection(WORD);
extern BOOL JoyCfg(WORD);
extern BOOL KeyCfg(WORD);

extern void SetTeamSettings(WORD, BOOL);
extern void DisplayTactic(int, int);
extern void AddPlayer(struct player_disk *, int);
extern void AddName(struct player_disk *, int);
extern void SetPlayerStatus(int, char, char, long);
extern WORD StartMatch(BYTE, BYTE);
extern void ScaleGfxObj(gfx_t *, uint8_t *);
extern void UpdateButtonList(void);

extern void StoreButtonList(void);

extern void UpdatePrefs(BYTE);
extern void SetupSpecialEvent(struct Bottone *);
extern void ClearSelection(void);
extern void ClearMatches(int);
extern void GroupsUpdate(void);
extern void SetupMatches(void);
extern void PlayMatches(void);
extern WORD ComputerMatch(BYTE, BYTE);
extern WORD PlayMatch(BYTE, BYTE);
char *ElaboraRisultato(int8_t, int8_t, uint16_t);
extern BOOL GroupsClear(void);
extern void request(char *);
extern void EliminazioneDiretta(int);
extern void ViewEliminazioneDiretta(int);
extern BOOL HandleMenuIDCMP(void);
extern void LoadHigh(char *);
extern void SetHighSelection(void);

extern player *network_player;
extern struct Bottone teamselection[], teamsettings[], pannelli[], wcp[], league[], mp[], mb[], lb[],
    asb[], cb[], cp[], wcfp[], mr[], scores[], hl[], sprefs_bottoni[];
extern BOOL no_sound, nosync, allow_replay, nocpu, saved, wb_game, big, injuries, substitutions,
    arcade, situation, training, use_crowd, use_speaker, use_replay, bookings, free_longpass, offside,
    menu_music, use_speaker, no_sound, use_crowd, special, random_draw, nopari, make_setup, id_change,
    window_opened, game_start, penalties, free_kicks, friendly, arcade_back, arcade_teams, newpitches,
    cgxmode, final, nointro, killer, savehigh, can_modify, warp, music_playing, golden_gol, newchange, network_game,
    audio_to_fast, use_gfx_scaling, wpa8, triple, force_single, chunky_version;
extern int32_t Pens[256];
extern int framerate, display_id, framerate, overscan;
extern long int t_l, situation_time;
extern uint32_t detail_level, ahimode;
extern struct Match turni[64][32];
extern int arcade_score;

extern int8_t competition, selected_number, wanted_number, actual_team, controllo[], p_control[],
    situation_result[2], strictness, field, duration, field_type, ruolo[];
extern UBYTE totale_giornate;
extern int8_t teamarray[], turno, nteams, daytime, league_pos[64], ppp, ppv, pps;

extern int WINDOW_WIDTH, WINDOW_HEIGHT, oldwidth, oldheight, wanted_width, wanted_height;
extern int radar_position, players, current_menu;
extern char control[], career_file[], *menu_soundname[], localename[], scontri, i_scontri;
extern struct myfont *bigfont, *smallfont, *titlefont;
extern struct GfxMenu menu[];
extern anim_t *logos, *symbols;
extern gfx_t *arcade_gfx[], *last_obj;
extern struct MyFileRequest freq;
extern uint8_t *back;
extern char numero[17][3], *controls[];
extern struct team_disk *teamlist;
extern struct teamstats_disk DatiCampionato[];
extern struct EasyStruct easy;
extern UBYTE colore_team[3], highlight_team[3];
extern struct SoundInfo *menusound[];
extern struct championship_disk campionato;

// Moduli generici...

void bltchunkybitmap(uint8_t *, int, int, uint8_t *, int, int, int, int, int, int);
void bltanimobj(struct MChunky *, uint8_t *, int, int, int);
void rectfill_pattern(uint8_t * b, int x1, int y1, int x2, int y2, unsigned char color, int width);
void rectfill(uint8_t * b, int x1, int y1, int x2, int y2, unsigned char color, int width);
void bitmapScale(struct MyScaleArgs *);
int drawtext(char *, int, int, int, int);
void setfont(struct myfont *);
struct myfont *openfont(char *);
void closefont(struct myfont *);
void freedraw(long, int, int, int, int);
void freepolydraw(long, int, int *);
void draw(long, int, int, int, int);
void polydraw(long, int, int, int, int *);
void bitmapFastScale(struct MyFastScaleArgs *);


extern BOOL os_check_joy(int);
extern void init_joy_config(void);
extern int os_get_joy_button(int);
extern char network_server[];
