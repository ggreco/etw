// this is a fix for OSX
#define MoveTo MyMoveTo

extern struct Giocatore_Disk Riserve[2][12];
extern UBYTE goal_array[GA_SIZE],goal_minute[GA_SIZE],goal_team[GA_SIZE],team_a,team_b,TotaleRiserve[2],NumeroTattiche;
extern char team_name[2][16],fieldname[24],palette[24],shirt[2][24];
extern Oggetto *pezzi_porte[4],*bonus[MAX_ARCADE_ON_FIELD];
extern BYTE need_release[MAX_PLAYERS],arcade_team[2],starting_team;
extern int8_t Table[],strictness,current_field,slowdown[];
extern struct RastPort Sq1RP,Sq2RP,PallaRP,*ProgressRP,ArcadeRP;
extern BOOL quit_game,no_sound,soft_scroll,pause_mode,nosync,final,full_replay,killer,
	start_replay,replay_mode,use_replay,allow_replay,nocpu,teams_swapped,first_kickoff,
	arcade,situation,training,use_crowd,slow_motion,use_speaker,replay_looped,highlight,
	substitutions,bookings,injuries,window_opened,screen_opened,nopari,free_longpass,
	no_record,penalties,free_kicks,arcade_teams,first_half,extratime,left_sel,right_sel,
	friendly,golden_gol,use_key0,use_key1,joyonly,audio2fast,use_offside,
	newpitches,network_game, game_start;
extern char control[4],*controls[CONTROLS],tipo_porta, spk_basename[64];
extern LONG Pens[256];
extern long framerate, time_length, t_l, game_status, urgent_status, situation_time, basepri;
extern WORD velocita_x[3][10][8],cambio_x[8],inversione_x[8][8],arcade_frame_sequence[];
extern WORD velocita_y[3][10][8],cambio_y[8],inversione_y[8][8];
extern BYTE prontezza[10],opposto[],scroll_type,situation_result[2];
extern WORD avanzamento_x[],avanzamento_y[],quota_mod_x[],quota_mod_y[];
extern WORD sin_table[256],cos_table[256],swaps;
extern UWORD counter;
extern WORD av_palla_x[8][12],av_palla_y[8][12],velocita_scivolata_x[],velocita_scivolata_y[];
extern Partita *p;
extern Pallone *pl;
extern struct Animazione Animation[],ArbAnim[],PortAnim[],GLAnim[];
extern WORD field_x,field_y,porte_x[],players,field_x_limit,field_y_limit;
extern BYTE joy_opposto[],dir_pred[],dir_next[],CambioDirezione[8][8],player_type[4],role[4];
extern long WINDOW_WIDTH, WINDOW_HEIGHT, wanted_sound;
extern struct SoundInfo *sound[];
extern char *soundname[],*Tattiche[];
extern Oggetto *object_list[];
extern struct DOggetto *c_list[];
extern int totale_lista,totale_lista_c;
extern struct SignalSemaphore listsem;
extern ULONG detail_level,*r_controls[MAX_PLAYERS],ahimode;
extern struct Rect ingombri[];
extern struct DOggetto peoples[];
extern GfxObj *last_obj,*pause_gfx;
extern mytimer StartGameTime,EndTime,ideal;
extern AnimObj *ports,*replay,*arcade_anim,*goal_banner;
extern GuardaLinee *guardalinee;
extern struct MyFastScaleArgs *scaling;
extern int font_width,font_height,result_width,FIXED_SCALING_WIDTH,FIXED_SCALING_HEIGHT;

extern void MakeRef(UBYTE *,int, int);
extern Tattica *LoadTactic(char *);
extern void FreeTactic(Tattica *);
extern void InvertTactic(Tattica *);
extern Partita *SetupSquadre(void);
extern void LiberaPartita(Partita *);
extern WORD FindDirection(WORD,WORD,WORD,WORD);
extern ULONG ReadKeyPort(ULONG port);
extern ULONG ReadNetworkPort(ULONG port);
extern void ChangeControlled(Squadra *,WORD);
extern void HandleControlled(int);
extern void HandleControlledJ2B(int);
extern void HandleControlledJoyPad(int);
extern void HandleCPU(int);
extern void HandleRealCPU(Giocatore *);
extern void HandleArbitro(void);
extern void HandleGuardalinee(void);
extern void HandleExtras(void);
extern void PostHandleBall(void);
extern void HandleBall(void);
extern void HandlePortiere(int);
extern void HandleRimessa(Giocatore *);
extern void DoSpecials(Giocatore *);
extern void CheckKeys(void);
extern void CheckActive(void);

// main

extern void SetResult(char *,...);

// squadre

extern void ReadSquadra(FILE *,struct Squadra_Disk *);
extern void DisponiSquadra(Squadra *,int ,BOOL );
extern void DisponiPortiere(Squadra *,int ,BOOL );
extern void MakeResult(void);
extern FILE *OpenTeam(char *);
extern void SwapTeams(void);
extern void ChangePlayer(struct Giocatore_Disk *,Giocatore *);
extern BOOL NumeroDiverso(struct Squadra *,char);
extern void GL_Fuori(int);

// control

extern void MoveNonControlled(void);
extern WORD GetJoyDirection(ULONG );
extern void NoPlayerControl(Giocatore *);
extern void CheckChange(Giocatore *);
extern ULONG (*MyReadPort0)(ULONG);
extern ULONG (*MyReadPort1)(ULONG);
extern void UpdatePortStatus(void);

// Special

extern void EseguiEsultanza(int);
extern void CPUShot(Giocatore *);
extern void EseguiCross(Giocatore *);
extern void (*LongPass)(Giocatore *,ULONG);
extern void FreePass(Giocatore *,ULONG);
extern void TargetedPass(Giocatore *,ULONG);
extern void Tira(Giocatore *);
extern void Passaggio(Giocatore *);
extern void Passaggio2(Giocatore *,char);
extern void PassaggioB(Giocatore *);
extern void RimessaLaterale(Giocatore *);
extern void PunizioneCorner(Giocatore *);
extern void UpdateCornerLine(void);
extern void ColpoDiTesta(Giocatore *);
extern void PreparaBarriera(char);
extern BOOL IsOffside(Giocatore *);

// Utils

// Aggiungo e rimuovo oggetti alla lista degli oggetti presente sul campo

extern Giocatore *TrovaPiuVicino(Squadra *s,WORD,WORD);
extern void MoveTo(Giocatore *,WORD,WORD);
extern WORD FindDirection32(WORD, WORD, WORD ,WORD );
extern WORD CanScore(Giocatore *);
extern Giocatore *TrovaPiuVicinoGiocatore(Giocatore *);
extern Giocatore *TrovaPiuVicinoDirGiocatore(Giocatore *);

extern void LiberaListe(void);
extern BOOL AggiungiLista(Oggetto *);
extern void RimuoviLista(Oggetto *);
extern BOOL AggiungiCLista(struct DOggetto *);
extern void RimuoviCLista(struct DOggetto *);
extern void VuotaCLista(void);


extern void LoadKeyDef(int, char *);
extern void SaveKeyDef(int, char *);
extern void DoPause(void);
// extern char GetTable(void); e' una ,macro ora
extern void DoFlash(void);
extern void UpdateBallSpeed(void);
extern void UpdateShotHeight(void);
extern UBYTE FindDirection256(WORD, WORD, WORD ,WORD );
extern BOOL AllowRotation(Giocatore *,WORD);
extern void RimuoviComandoSquadra(char , BYTE );
extern void EseguiDopoComando(Giocatore *);
extern void SetComando(Giocatore *,BYTE,BYTE,BYTE);
extern BOOL InArea(BYTE,WORD,WORD);
extern BOOL InAnyArea(WORD,WORD);
extern WORD IndirizzaTiro(Giocatore *,ULONG );
extern void SetShotSpeed(Giocatore *,WORD);

extern void init_crowd(void);
extern void free_crowd(void);
extern BOOL crowd2memory(void);
extern BOOL speaker2memory(void);
extern void init_speaker(void);
extern void free_speaker(void);
extern struct SoundInfo *handle_crowd(void);
extern struct SoundInfo *handle_speaker(void);

// Rimbalzi

extern void RimbalzoCasuale(void);

// Fonts

extern void PrintNames(void);
extern void PrintSmall(bitmap, char *, char );
extern void PrintVerySmall(bitmap, char *, char,int );
extern void LoadLogo(char *);
extern void LoadPLogo(char *);
extern BOOL InitFonts(void);
extern void FreeFonts(void);
extern void ShowPanel(void);
extern void PrintBottom(char *);
extern void DrawR(void);
extern void DrawPause(void);
extern void ShowFinal(void);

// Suoni

extern void PlayIfNotPlaying(int);
extern BOOL CaricaSuoni(void);
extern void LiberaSuoni(void);
extern void os_start_audio(void);
extern void os_stop_audio(void);
// Radar

extern BOOL big;
extern WORD radar_position;

extern void (*HandleRadar)(void);
extern void HandleRadarBig(void);
extern void HandleRadarLittle(void);
extern void ResizeRadar(void);

// Loops

extern void MainLoop(void);
extern void OldMainLoop(void);
extern void (*HandleSquadra0)(int);
extern void (*HandleSquadra1)(int);
extern void (*HandleRadar)(void);
extern GfxObj *background;
extern WORD n_limit,o_limit,s_limit,e_limit;

// Config

extern void read_config(void);
extern void OpenTheScreen(void);

// Replay

extern void HandleReplay(void);
extern BOOL AllocReplayBuffers(void);
extern void FreeReplayBuffers(void);
extern void SaveReplay(void);
extern void CheckPortiere(int);
extern void StopTime(void);
extern void RestartTime(void);
extern void LoadHighlight(void);
extern void RestartGame(void);


#define SPrintf sprintf	

// Arcade

void RemoveArcadeEffect(Giocatore *,UBYTE);
void GetArcadeEffect(Giocatore *,Oggetto *);
void HandleArcade(void);

// Da os_video.c

void AdjustSDLPalette(void);
long obtain_pen(char,char,char);
void lock_pen(int);
void release_pen(long);
void os_set_color(int,int,int,int);
void close_graphics(void);
BOOL window_open(void);
void os_resize(void);
void os_set_window_frame(void);
void os_wait(void);
int os_wait_end_pause(void);
int os_get_screen_width(void);
int os_get_screen_height(void);
BOOL os_create_dbuffer(void);
int os_get_inner_width(void);
int os_get_inner_height(void);
void os_load_palette(ULONG *);
void os_free_dbuffer(void);
BOOL os_lock_bitmap(void);
void os_unlock_bitmap(void);

// Da os_control.c

void set_controls(void);
void free_joyports(void);
void os_getevent(void);

// Da os_init.c

void init_system(void);
void os_delay(int);
int os_avail_mem(void);
void SetCrowd(int);
void os_init_timer(void);
void os_free_timer(void);
void os_audio2fast(void);

#ifdef CD_VERSION
void UrgentSpeaker(int);
void StartSpoken(void);
#endif

#ifdef DEMOVERSION
void WaitOrKey(int);
#endif

