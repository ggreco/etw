// extern void GriddedWipe(uint8_t Type, struct BitMap *source);
#include "menu.h"

extern void LoadMenuLogo(char *name);
extern BOOL InitMenuFonts(void);
extern void Intro(void);
extern void Outro(void);
extern void RemoveReqs(void);
extern void EnableReqs(void);
extern void FreeMenuFonts(void);
/* AC: _WINUSER_ is for Microsoft Visual C++ _WINUSER_H for MinGW */
extern void blit_scaled_logo();
#if !defined(_WINUSER_) && !defined(_WINUSER_H)
extern void ChangeMenu(int16_t );
#endif
extern void OpenMenuScreen(void);
extern void PlayMenuMusic(void);
extern void StopMenuMusic(void);
extern void FreeMenuMusic(void);
extern void read_menu_config(void);
extern void write_config(char *);
extern BOOL LoadMenuStuff(void);
extern BOOL LoadBack(void);
extern BOOL LoadArcadeBack(void);
extern void FreeMenuStuff(void);
extern void SwapAllTeams(void);
extern void MakeLeague(int);
extern void LoadLeague(void);
extern void LoadArcadeGfx(void);
extern void LoadPLogo(char *);
extern void SaveLeague(void);
extern void InitTable(void);
extern void UpdateLeagueTable(void);
extern void SaveTeams(char *);
extern void LoadTeams(char *);
extern void RedrawButton(struct Button *, uint8_t);
extern void CancelButton(struct Button *);
extern void ClearScores(void);
extern void PrintShadow(int, int, char *, int, struct myfont *);
extern void MyRestoreBack(void);
extern void ClearScores(void);
extern void LoadScores(void);
extern void ShowCredits(void);
extern void AddScore(uint8_t);
extern BOOL FileRequest(struct MyFileRequest *);

extern void CheckCheat(uint8_t);
extern void DrawBox(int16_t);
// Procedure speciali dei menu...

extern BOOL handle_pause(int16_t);
extern BOOL TeamSelection(int16_t);
extern BOOL ArcadeTeamSelection(int16_t);
extern BOOL TeamSettings(int16_t);
extern BOOL TeamSubstitutions(int16_t);
extern BOOL VideoPrefs(int16_t);
extern BOOL SystemPrefs(int16_t);
extern BOOL MobilePrefs(int16_t);
extern BOOL GamePrefs(int16_t);
extern BOOL AudioPrefs(int16_t);
extern BOOL HighSelection(int16_t);
extern BOOL JoyCfg(int16_t);
extern BOOL KeyCfg(int16_t);

extern void SetTeamSettings(int16_t, BOOL);
extern void DisplayTactic(const char*, int, int, const char *n[], int);
extern int16_t StartMatch(int8_t, int8_t);
extern int16_t LastMatchResult();
extern void ScaleGfxObj(gfx_t *, uint8_t *);
extern void UpdateButtonList(void);

extern void StoreButtonList(void);

extern void UpdatePrefs(int8_t);
extern void SetupSpecialEvent(struct Button *);
extern void ClearSelection(void);
extern void ClearMatches(int);
extern void GroupsUpdate(void);
extern void SetupMatches(void);
extern void PlayMatches(int);
extern int16_t ComputerMatch(int8_t, int8_t);
extern int16_t PlayMatch(int8_t, int8_t);
char *ElaboraRisultato(int8_t, int8_t, uint16_t);
extern BOOL GroupsClear(void);
extern void request(const char *);
extern void ViewEliminazioneDiretta(int);
extern BOOL HandleMenuIDCMP(void);
extern void LoadHigh(char *);
extern void SetHighSelection(void);
extern BOOL ChangeMRMenu(int16_t);

extern player *network_player;
extern struct Button teamselection[], teamsettings[], pannelli[], wcp[], league[], mp[], mb[], lb[],
    asb[], cb[], cp[], wcfp[], mr[], scores[], hl[], sprefs_bottoni[];
extern BOOL no_sound, allow_replay, nocpu, saved, wb_game, big, injuries, substitutions,
    arcade, situation, training, use_crowd, use_speaker, use_replay, bookings, free_longpass, offside,
    menu_music, use_speaker, no_sound, use_crowd, special, random_draw, nopari, make_setup,
    game_start, penalties, free_kicks, friendly, arcade_back, arcade_teams,
    final, nointro, killer, savehigh, can_modify, warp, music_playing, golden_gol, newchange, network_game,
    audio_to_fast, use_gfx_scaling, tutorial, use_touch, prefs_changed, reqqing;
extern int32_t Pens[256];
extern int framerate;
extern int32_t t_l, situation_time;
extern uint32_t detail_level, ahimode;
extern struct Match turni[64][32];
extern int arcade_score;

extern int8_t competition, selected_number, wanted_number, actual_team, controllo[], p_control[],
    situation_result[2], strictness, field, duration, field_type, ruolo[];
extern uint8_t totale_giornate;
extern int8_t teamarray[], turno, nteams, daytime, league_pos[64], ppp, ppv, pps;

extern int WINDOW_WIDTH, WINDOW_HEIGHT, oldwidth, oldheight, wanted_width, wanted_height;
extern int radar_position, players, current_menu;
extern const int previous_menu[MENUS];
extern uint8_t control[];
extern char career_file[], *menu_soundname[], localename[], scontri, i_scontri;
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
extern uint8_t colore_team[3], highlight_team[3];
extern struct SoundInfo *menusound[];
extern struct championship_disk campionato;
extern const char *numbers[100];

// from os_video
void os_set_color(int, int, int , int);
int create_anim_context(int, int);
void delete_anim_context();

// Moduli generici...
BOOL ResizeWindow(int, int);
void FullScreen(BOOL);
void bltchunkybitmap(uint8_t *, int, int, uint8_t *, int, int, int, int, int, int);
void bltanimobj(struct MChunky *, uint8_t *, int, int, int);
void rectfill_pattern(uint8_t * b, int x1, int y1, int x2, int y2, unsigned char color, int width);
void rectfill(uint8_t * b, int x1, int y1, int x2, int y2, unsigned char color, int width);
void bitmapScale(struct MyScaleArgs *);
//int drawtext(char *, int, int, int, int);
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
extern void os_flush_events();
extern char network_server[];
extern void initialize_menus();
extern void free_menus();
extern void update_menu_tactic();

// achievement api
void add_achievement(const char *, float);
void show_world_scores();
void show_achievements();
void add_score(int);
void reset_achievements();
void init_game_center();
// ads & inapp purchase
void show_ads(int);
void hide_ads();
int has_full_version();
void buy_full_version();

// tutorial api
void check_tutorial();
void init_tutorial();
