#include "preinclude.h"
#include "eat.h"
#include "network.h"

SDL_Joystick *joy[4] = {NULL, NULL, NULL, NULL};
char joybuttons[2];
uint8_t joycfg_buttons[2][8];
char num_joys=0;
BOOL has_joystick=FALSE;
extern uint32_t NetJoyPos[];

extern void ResizeWin(SDL_Event *);

uint32_t ReadNetworkPort(uint32_t l)
{
//    fprintf(stderr, "Read network %ld: %ld\n", l, NetJoyPos[l]);
    return NetJoyPos[l];
}

uint32_t MyReadJoyPort(uint32_t l)
{    
//    extern struct joy_config joycfg[];
    int xc,yc;
    uint32_t mask=0;

    if(l > 1 || !joy[l])
        return 0L;

    if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][0]))
        mask=JPF_BUTTON_RED;

    if(joybuttons[l]>1)
    {
        if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][1]))
            mask|=JPF_BUTTON_BLUE;

        if(joybuttons[l]>5)
        {
            if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][2]))
                mask|=JPF_BUTTON_YELLOW;
            if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][3]))
                mask|=JPF_BUTTON_GREEN;
            if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][5]))
                mask|=JPF_BUTTON_FORWARD;
            if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][4]))
                mask|=JPF_BUTTON_REVERSE;

            if(joybuttons[l]>6)
            {
                if(SDL_JoystickGetButton(joy[l],joycfg_buttons[l][6]))
                    mask|=JPF_BUTTON_PLAY;
            }
        }
    }

    xc=SDL_JoystickGetAxis(joy[l],0);
    yc=SDL_JoystickGetAxis(joy[l],1);

    if(xc>3000)
        mask|=JPF_JOY_RIGHT;
    else if(xc<-3000)
        mask|=JPF_JOY_LEFT;

    if(yc>3000)
        mask|=JPF_JOY_DOWN;
    else if(yc<-3000)
        mask|=JPF_JOY_UP;

    if(!network_game)
        return mask;
    else {
        SendNetwork(mask);
        return NetJoyPos[l];
    }
}


void free_joyports(void)
{
    int i;

    for(i=0;i<num_joys;i++)
    {
        if(!joy[i])
            num_joys++;
        else {
            SDL_JoystickClose(joy[i]);
            joy[i] = NULL;
        }
    }

    has_joystick=FALSE;
    num_joys=0;
}


BOOL os_check_joy(int i)
{
    SDL_Joystick *j;

    if((j=SDL_JoystickOpen(i))) {
        SDL_JoystickClose(j);
        return TRUE;
    }

    return FALSE;
}

int os_get_joy_button(int i)
{
    SDL_Joystick *j;

    
    SDL_JoystickEventState(SDL_ENABLE);

    if ((j=SDL_JoystickOpen(i))) {
        int k=-1;
        BOOL ok=FALSE;
        SDL_Event e;

        while(SDL_PollEvent(&e));

        while(!ok)
        {
            SDL_WaitEvent(&e);

            if(e.type==SDL_JOYBUTTONDOWN)
            {
                k=e.jbutton.button;
                ok=TRUE;
            }

            if(e.type==SDL_KEYUP)
                ok=TRUE;
        }

        SDL_JoystickClose(j);

        SDL_JoystickEventState(SDL_DISABLE);

        return k;
    }

    SDL_JoystickEventState(SDL_DISABLE);

    return -1;
}

static BOOL open_joystick(int joyid)
{
    if ((joy[joyid] = SDL_JoystickOpen(joyid))) {
        num_joys++;
        has_joystick=TRUE;

        D(bug("Opened Joystick %ld for TOUCH\n",joyid));
        D(bug("Name: %s\n", SDL_JoystickName(joy[joyid])));
        D(bug("Number of Axes: %ld\n", SDL_JoystickNumAxes(joy[joyid])));
        D(bug("Number of Hats: %ld\n", SDL_JoystickNumAxes(joy[joyid])));
        D(bug("Number of Buttons: %ld\n", SDL_JoystickNumButtons(joy[joyid])));
        D(bug("Number of Balls: %ld\n", SDL_JoystickNumBalls(joy[joyid])));
        D(bug("Joystick event manager: %ld\n",SDL_JoystickEventState(SDL_QUERY)));

        if(SDL_JoystickEventState(SDL_QUERY)==SDL_ENABLE)
        {
            D(bug("** Forcing deactivation of joystick event manager\n"));
            SDL_JoystickEventState(SDL_DISABLE);
        }

        joybuttons[joyid]=SDL_JoystickNumButtons(joy[joyid]);
        return TRUE;
    }
    return FALSE;
}

void set_controls(void)
{
    use_touch = use_key0 = use_key1 = FALSE;

    p->team[0]->Joystick=player_type[0];

    if (control[0] == CTRL_TOUCH) {
        use_touch = TRUE;
        open_joystick(p->team[0]->Joystick);
    }
    else if(control[0]>=CTRL_KEY_1)
    {
        use_key0=TRUE;
        /* AC: Or the configuration is saved in automatic, or here it is
        * better not to load, otherwise the settings not still saved are lost
        *
        LoadKeyDef(0,KEY_RED_FILE);*/
    }

    if (control[1] == CTRL_TOUCH) {
        use_touch = TRUE;
    }
    else if(control[1]>=CTRL_KEY_1)
    {
        use_key1=TRUE;
        /* Idem like above
        LoadKeyDef(1,KEY_BLUE_FILE);*/
    }

    if(p->team[0]->Joystick>=0)
    {
        if(control[p->team[0]->Joystick]<CTRL_KEY_1)    {
joy_try_again:
            D(bug("Opening joystick %ld\n",p->team[0]->Joystick));

            if (open_joystick(p->team[0]->Joystick)) {
                D(bug("team 0 has a joystick!"));
            }
            else if (player_type[1] == TYPE_COMPUTER && player_type[0] == 1) {
                // if we are playing single player and we have a single joystick installed
                // let the player use it anyway
                p->team[0]->Joystick = 0;
                goto joy_try_again;
            }
            else {
                // Prevedo fallback a tastiera o ritorno un errore?
                D(bug(" ->Error opening joystick!\n"));
                has_joystick = FALSE;
            }
        }

        if (control[p->team[0]->Joystick]==CTRL_TOUCH ) {
                HandleTeam0=HandleControlledTouch;
                D(bug("Touch for team 0\n"));
        }
        else if(control[p->team[0]->Joystick]==CTRL_JOYPAD ||
            control[p->team[0]->Joystick]==CTRL_KEY_2) {
            HandleTeam0=HandleControlledJoyPad;
            D(bug("Joypad/key2 for team 0\n"));
        }
        else if(control[p->team[0]->Joystick]==CTRL_JOY2B ||
                control[p->team[0]->Joystick]==CTRL_KEY_1) {
            HandleTeam0 = HandleControlledJ2B;
            D(bug("Joy2b/key1 for team 0\n"));
        }
        else {
            HandleTeam0=HandleControlled;
        }
    }
    else {
        HandleTeam0=HandleCPU;

        if(nocpu)
            p->team[0]->Joystick=TYPE_JOYSTICK1;
    }

    if (player_type[1] == player_type[0] && 
        player_type[0] != TYPE_COMPUTER) {
            player_type[1]^=1;
    }

    p->team[1]->Joystick=player_type[1];

    if(p->team[1]->Joystick<0) {
        HandleTeam1=HandleCPU;

        if(nocpu)
            p->team[1]->Joystick=TYPE_JOYSTICK1;
    }
    else {
        if(control[p->team[1]->Joystick]<CTRL_KEY_1) {
            D(bug("Opening joystick %ld\n",p->team[1]->Joystick));

            if (open_joystick(p->team[1]->Joystick)) {
                D(bug("Team 1 has a joystick!"));
            }
            else {
                D(bug(" ->Error opening joystick!\n"));
            }
        }
        
        if (control[p->team[1]->Joystick]==CTRL_TOUCH ) {
            HandleTeam1=HandleControlledTouch;
            D(bug("Touch for team 1\n"));
        }
        else if(control[p->team[1]->Joystick]==CTRL_JOYPAD ||
                control[p->team[1]->Joystick]==CTRL_KEY_2) {
            HandleTeam1=HandleControlledJoyPad;
            D(bug("Joypad/key2 for team 1\n"));
        }
        else if(control[p->team[1]->Joystick]==CTRL_JOY2B ||
                control[p->team[1]->Joystick]==CTRL_KEY_1 ) {
            HandleTeam1=HandleControlledJ2B;
            D(bug("Joy2b/key1 for team 0\n"));
        }
        else {
            HandleTeam1=HandleControlled;
        }
    }
    if (use_touch)
        touch_init();
}


void fake_goal(int idx)
{
    p->team[idx]->Reti++;
    if(!penalties && (p->team[0]->Reti+p->team[1]->Reti)<GA_SIZE )
    {
        int i=p->team[0]->Reti+p->team[1]->Reti - 1;

        goal_array[i]=(rand() % 10) + 2;

        goal_team[i]= idx;

        goal_minute[i]=(rand() % 45) + 1;

        if(extratime)
        {
            goal_minute[i]/=3;

            if(first_half)
                goal_minute[i]+=90;
            else
                goal_minute[i]+=105;
        }
        else if(!first_half)
        {
            goal_minute[i]+=45;
        }

    }
    D(bug("I have signed a GOAL for team %s\n",p->team[idx]->name));
}
void CheckKeys(void)
{
    SDL_Event e;

    while(SDL_PollEvent(&e))
    {
    switch(e.type)
    {
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
        case SDL_JOYAXISMOTION:
            D(bug("Joystick event, I shouldn't take them!!!!\n"));
            break;
        case SDL_QUIT:
            SetResult("break");
            final = FALSE;
            quit_game=TRUE;
            break;

        case SDL_WINDOWEVENT:
            if (e.window.event ==  SDL_WINDOWEVENT_RESIZED) 
                ResizeWin(&e);
            else if (e.window.event == SDL_WINDOWEVENT_MINIMIZED && !pause_mode)
                DoPause();
            break;
        case SDL_KEYDOWN:           
            if(!(p->show_panel&PANEL_CHAT)) {
                switch(e.key.keysym.sym)
                {
#ifndef DEBUG_DISABLED

                    // I - for debug, it shows the men positions

                    case SDLK_i:
                        {
                            team_t *s=p->team[1];
                            int i;

                            for(i=0;i<10;i++)
                            {
                                D(bug("G: %ld A: %ld S: %ld C: %ld\n",i+2,s->players[i].AnimType,s->players[i].Special,s->players[i].Comando));

                                if(s->players[i].world_x<0 || s->players[i].world_y<0 ||
                                        s->players[i].world_x>11000 || s->players[i].world_y>4200 )
                                {
                                    int j;
                                    player_t *g=&s->players[i];

                                    D(bug("Parameters: Sp:%ld Cmd:%ld Dir:%ld Anim:%ld Spd:%ld Sect:%ld\n",
                                                g->Special,g->Comando,g->dir,g->AnimType,g->ActualSpeed,g->sector));

                                    for(j=0;j<(SECTORS+SPECIALS);j++)
                                    {
                                        D(bug("S:%ld - without: X:%ld Y:%ld - with: X:%ld Y:%ld\n",j,
                                                    g->team->tactic->Position[0][i][j].x,g->team->tactic->Position[0][i][j].y,
                                                    g->team->tactic->Position[1][i][j].x,g->team->tactic->Position[1][i][j].y));
                                    }
                                }
                            }
                        }
                        break;
                    case SDLK_1:
                        PlayBackSound(sound[DOG]);
                        break;
                    case SDLK_2:
                        PlayBackSound(sound[CRASH]);
                        break;
                    case SDLK_3:
                        PlayBackSound(sound[BOOU]);
                        break;
                    case SDLK_4:
                    case SDLK_5:
                    case SDLK_6:
                        if (pl->gioc_palla) {
                            player_t *g = pl->gioc_palla;
                            pl->InGioco=FALSE;
                            FermaPalla();
                            p->referee.Comando=FISCHIA_FALLO;
                            TogliPalla();
                      
                            if (e.key.keysym.sym == SDLK_6) {
                                DoSpecialAnim(g,GIOCATORE_INFORTUNIO);
                                g->Ammonito |= 4;
                                g->ActualSpeed=0;

                                if(injuries) {
                                    StopTime();

                                    p->player_injuried=g;

                                    if(g->team->Joystick>=0)
                                        p->show_panel=PANEL_PLAY_INJURIED;

                                    p->show_time=200;
                                }
                                break;
                            }
                            else if (e.key.keysym.sym == SDLK_4) {
                                p->team[g->SNum]->Ammonizioni++;
                                p->referee.Comando=AMMONIZIONE;
                            }
                            else if (e.key.keysym.sym == SDLK_5) {
                                p->referee.Comando=ESPULSIONE;
                                p->team[g->SNum]->Espulsioni++;
                            }               

                            p->referee.Tick=-50;
                            p->show_panel|=PANEL_TIME|PANEL_CARD;
                            p->referee.Argomento=g->GNum+g->SNum*10;
                        }
                        break;
                    case SDLK_w:
                        // W - end time!
                        EndTime=Timer();
                        D(bug("Forcing end of time!\n"));
                        break;

                        // D - create a foul
                    case SDLK_d:
                        pl->InGioco=FALSE;
                        FermaPalla();

                        p->referee.Comando=FISCHIA_FALLO;
                        TogliPalla();
                        if(p->team[0]->Possesso)
                        {
                            pl->sq_palla=p->team[0];
                            p->team[1]->Falli++;
                        }
                        else
                        {
                            pl->sq_palla=p->team[1];
                            p->team[0]->Falli++;
                        }
                        break;

                        /* AC: make a fake goal for team 0. */
                    case SDLK_g:
                        fake_goal(0);
                        break;
                    case SDLK_j:
                        fake_goal(1);
                        break;
#endif
                    case SDLK_r:
                        // R - replay
                        if(!replay_mode&&!start_replay)
                            start_replay=TRUE;
                        else
                        {
                            if(!slow_motion)
                            {
                                MY_CLOCKS_PER_SEC_50<<=2;
                                slow_motion=TRUE;
                            }
                            else
                            {
                                slow_motion=FALSE;
                                MY_CLOCKS_PER_SEC_50>>=2;
                            }
                        }
                        break;
                        // W - enables/disables the radar
                    case SDLK_s:
                        if(detail_level&USA_RADAR)
                            detail_level&=(~USA_RADAR);
                        else
                            detail_level|=USA_RADAR;
                        break;
                        // L - shows stats
                        /* There was an overlap with SDLK_RETURN, used in the keyboard RED definition.
                         */
                    case SDLK_h:
                        p->show_panel|=PANEL_CHAT;
                        p->show_time=120;
                        break;
                    case SDLK_l:
                        p->show_panel|=PANEL_STATS;
                        p->show_time=80;
                        break;

                        // X - change position/dimensions of the radar
                    case SDLK_x:
                        /* With the new default keyboard configuration, this control
                         * isn't needed anymore
                        if(use_key1)
                            break;
                        */

                        radar_position++;
                        ResizeRadar();
                        break;

#ifdef USE_ROLEPLAY

                    // Left Alt, role play enable/disabled for team 1
                    case SDLK_LALT:
                        p->team[1]->gioco_ruolo = (p->team[1]->gioco_ruolo ? FALSE : TRUE);
                        break;
                    // Left Shift, changes active player team 1 if role play
                    case SDLK_LSHIFT:
                        if(p->team[1]->gioco_ruolo)
                        {
                            char num=p->team[1]->attivo->GNum;

                            if(num<9)
                                num++;
                            else
                                num=0;

                            p->team[1]->gioco_ruolo=FALSE;
                            ChangeControlled(p->team[1],num);
                            p->team[1]->gioco_ruolo=TRUE;
                        }
                        break;
                    // Right Alt, role play enable/disabled for team 0
                    case SDLK_RALT:
                        p->team[0]->gioco_ruolo = (p->team[0]->gioco_ruolo ? FALSE : TRUE);
                        break;
                    // Right Shift, changes active player team 0 if role play
                    case SDLK_RSHIFT:
                        if(p->team[0]->gioco_ruolo)
                        {
                            char num=p->team[0]->attivo->GNum;

                            if(num<9)
                                num++;
                            else
                                num=0;

                            p->team[0]->gioco_ruolo=FALSE;
                            ChangeControlled(p->team[0],num);
                            p->team[0]->gioco_ruolo=TRUE;
                        }
                        break;
#endif

                        // P - pause
                    case SDLK_p:
                        if(!replay_mode && p->show_panel&PANEL_REPLAY && friendly)
                           RestartGame();
                        else if (pause_mode)
                           pause_mode = FALSE;
                        else
                           DoPause();
                        break;
                        // DEL - visual on the ball
                    case SDLK_DELETE:
                        scroll_type=0;
                        break;

                        // Q - quit game
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        {
                            extern BOOL CheckQuit();
                            BOOL stop_time = FALSE;

                            if (!time_stopped) {
                                StopTime();
                                stop_time = TRUE;
                            }
                            if (CheckQuit()) {
                                SetResult("break");
                                final = FALSE;

                                quit_game=TRUE;
                            }

                            if (stop_time) {
                                RestartTime();
                                ideal = Timer() - 1;
                            }
                        }
                        break;

                        // BARRA - change visual
                    case SDLK_SPACE:
                        if(!replay_mode) {
                            /* With the new default keyboard configuration, this control
                            * isn't needed anymore
                            if(!use_key1) {
                            */
                                if(scroll_type<23)
                                    scroll_type++;
                                else
                                    scroll_type=0;
                            //}
                            //                            ChangeControlled(p->team[0],selected);
                        }
                        else {
                            SaveReplay();
                        }
                        break;
                    default:
                        // not handled keys...
                        break;
                }
            }
            else {
                if( (e.key.keysym.sym>=SDLK_a && e.key.keysym.sym<=SDLK_z) ||
                    (e.key.keysym.sym>=SDLK_0 && e.key.keysym.sym<=SDLK_9) ||
                     e.key.keysym.sym==SDLK_SPACE ) {
                    size_t l=strlen(outgoing_chat);

                    if(l<(sizeof(outgoing_chat)-1)) {
                        outgoing_chat[l]=toupper(e.key.keysym.sym);
                        outgoing_chat[l+1]=0;
                    }
                    p->show_time=150;
                }
                switch(e.key.keysym.sym) {
                    case SDLK_BACKSPACE:
                        if(!*outgoing_chat)
                            p->show_panel&=(~PANEL_CHAT);
                        else {
                            p->show_time=120;
                            outgoing_chat[strlen(outgoing_chat)-1]=0;
                        }
                        break;
                    case SDLK_RETURN:
                        if(network_game)
                            SendChatMessage();

                        *outgoing_chat=0;
                        p->show_panel&=(~PANEL_CHAT);
                        break;
                    case SDLK_ESCAPE:
                        *outgoing_chat=0;
                        p->show_panel&=(~PANEL_CHAT);
                        break;
                    default:
                        // not handled keys...
                        break;
                }
            }
        }
    }
}


SDL_Scancode query[]=
{
    /* Red player */
    SDL_SCANCODE_UP,SDL_SCANCODE_RIGHT,SDL_SCANCODE_DOWN,SDL_SCANCODE_LEFT,
    SDL_SCANCODE_BACKSPACE,SDL_SCANCODE_RETURN,
    SDL_SCANCODE_PAGEUP,SDL_SCANCODE_PAGEDOWN,
    SDL_SCANCODE_RSHIFT,SDL_SCANCODE_END,
    /* Blue player */
// Query1
    /* AC: There was an overlap with the keys used by the game.
     * At the moment the new definitions have been chosen "randomly"
     */
    SDL_SCANCODE_S,SDL_SCANCODE_C,SDL_SCANCODE_X,SDL_SCANCODE_Z, // dir: up->clockwise (s->c->x->z)
    SDL_SCANCODE_V,SDL_SCANCODE_B,
    SDL_SCANCODE_J,SDL_SCANCODE_M,SDL_SCANCODE_LSHIFT,SDL_SCANCODE_TAB
};

void UpdatePortStatus(void)
{
    if (has_joystick) {
        SDL_JoystickUpdate();
        SDL_PumpEvents();
    }
}

uint32_t ReadKeyPort(uint32_t port)
{
//    D(fprintf(stderr,"R:%d ", port));
    register uint32_t v=0;
    register const uint8_t *keys;
    register SDL_Scancode *q;

    keys=SDL_GetKeyboardState(NULL);

    q= (port) ? &query[10] : query;

// Movements

    if(keys[q[0]]==SDL_PRESSED)
        v|=JPF_JOY_UP;
    else if(keys[q[2]]==SDL_PRESSED)
        v|=JPF_JOY_DOWN;

    if(keys[q[1]]==SDL_PRESSED)
        v|=JPF_JOY_RIGHT;
    else if(keys[q[3]]==SDL_PRESSED)
        v|=JPF_JOY_LEFT;

// Buttons!

    if(keys[q[4]]==SDL_PRESSED)
        v|=JPF_BUTTON_RED;
    
    if(keys[q[5]]==SDL_PRESSED)
        v|=JPF_BUTTON_BLUE;

    if(keys[q[6]]==SDL_PRESSED)
        v|=JPF_BUTTON_YELLOW;

    if(keys[q[7]]==SDL_PRESSED)
        v|=JPF_BUTTON_GREEN;

    if(keys[q[8]]==SDL_PRESSED)
        v|=JPF_BUTTON_FORWARD;

    if(keys[q[9]]==SDL_PRESSED)
        v|=JPF_BUTTON_REVERSE;

    if(!network_game)
        return v;
    else {
        SendNetwork(v);
        return NetJoyPos[port];
    }
}

void LoadKeyDef(int port,char *file)
{
    FILE *f;

    if ((f=fopen(file,"r"))) {
        char buffer[120];
        int i=0;
        SDL_Scancode *q;

        q= (port) ? &query[10] : query;

        D(bug("Loading custom key definition...\n"));

        while(fgets(buffer,119,f)) {
            /* AC: Upper limit was 9 and not '9'... */
            if(buffer[0]<'0' || buffer[0]>'9') {
                continue;
            }

            if(i>=9) {
                D(bug("Too much definitions!\n"));
                break;
            }

            q[i]=atoi(buffer);

            i++;
        }

        fclose(f);
    }
}

void SaveKeyDef(int port,char *file)
{
    FILE *f;

    if ((f=fopen(file,"w")))
    {
        char buffer[120];
        int i=0;
        SDL_Scancode *q;

        q= (port) ? &query[10] : query;

        D(bug("Saving custom key definition...\n"));

        while(i < 10)
        {
            sprintf(buffer,"%d\n",q[i]);
            fputs(buffer,f);
            i++;
        }

        fclose(f);
    }
}

