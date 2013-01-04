//
//  tutorial.cpp
//  etw
//
//  Created by Gabriele Greco on 1/2/13.
//  Copyright (c) 2013 Gabriele Greco. All rights reserved.
//

extern "C" {
#include "menu.h"
#include "etw_locale.h"

    extern BOOL MyEasyRequest(void*, struct EasyStruct *, void*);
    extern void StopTime();
    extern void RestartTime();
    extern void hide_vjoy();
    extern void show_vjoy();
}

#include "tutorial.h"

#define SZ(x) (sizeof(x)/sizeof(x[0]))

bool running_tutorial_line = false;

static bool training_session(void)
{
    extern BOOL pause_mode;
    static time_t last_training_tip = 0;
    
    if (!training || !game_start || pause_mode)
        return false;
    
    time_t now = time(NULL);
    
    if (now > (last_training_tip + 4)) {
        last_training_tip = now;
        return true;
    }
    
    return false;
}

static TutorialLine lines[] = {
    {TutorialLine::ByMenuId, (void*)MENU_MAIN_MENU, "Welcome to the Eat The Whistle Tutorial!\n\nWe'll guide you to discover the game features and mechanics.\n\nYou can deactivate this tutorial in any moment through the audio video settings."},
#ifdef IPHONE
    {TutorialLine::ByMenuId, (void*)MENU_SCORES, "Eat the whistle supports Game Center for achievements and scores. Please remember that you need to log in to Game Center in order to record your achievements and compete with your friends."},
#endif
    {TutorialLine::ByMenuId, (void*)MENU_GAME_START, "Eat the Whistle has two different game modes: Simulation and Arcade.\n\nBefore trying one of them you'll better practice a bit with the controls using the practice option."},
    {TutorialLine::ByMenuId, (void*)MENU_ARCADE, "Arcade mode is an original football variation played in a speedball-like arena, with pickable bonuses, and crazy teams. Try to win the arcade challenge or the whistle cup if you can!"},
    {TutorialLine::ByMenuId, (void*)MENU_SIMULATION, "Simulation mode will offer you an old-style soccer game experience, you can play a league, a cup or a friendly match!"},
    {TutorialLine::ByMenuId, (void*)MENU_TRAINING, "Now play a training session.\n\nThis will let you learn the basics and practice the game controls."},
    {TutorialLine::ByMenuId, (void*)MENU_TEAM_SELECTION, "This menu let you choose the team you want to use, an human team is identified with a blue background and the letter p, a computer controller one with a light gray background and the letter c.\n\nthe number of teams you have to select may vary: 1 for training, 2 for a friendly match, 3+ for a league..."},
    {TutorialLine::ByMenuId, (void*)MENU_ARCADE_SELECTION, "This menu let you choose the arcade team you want to use, an human team is marked with a blue p, a computer controller one with the light gray letter c.\n\nThe number of teams you have to select may vary: 1 for the challenge, 2 for a friendly match, 8 for the whistle tour."},
    {TutorialLine::ByMenuId, (void*)MENU_TEAM_SETTINGS, "This screen let you change your team tactic or the 11 initial players before a match.\nThe stars near the player name are a global valutation of the abilities of your players."},
    {TutorialLine::ByMenuId, (void*)MENU_MOBILE_PREFS, "This menu let you change audio-video settings, you can also disable this tutorial if you want."},
    {TutorialLine::ByMenuId, (void*)MENU_HIGHLIGHT, "During a match you can record an highlight using the red circle icon during the replay, through this menu you can view an highlight or remove it."},
    {TutorialLine::ByMenuId, (void*)MENU_CHALLENGE, "The Arcade Challenge is the ultimate Eat The Whistle... challenge, you will have to win against all the arcade teams, except the one you have chosen, and a secret team to complete it."},
    {TutorialLine::BySpecialFunc, (void*)training_session, "Welcome to the Eat The Whistle training session, we will guide you through the basics of the game control system."},
#ifdef MOBILE_VERSION
    {TutorialLine::BySpecialFunc, (void*)training_session, "You can pause the game, change your tactic or view replay or leave the game in any moment through the pause menu (icon on the top right corner of the touchscreen)"},
    {TutorialLine::BySpecialFunc, (void*)training_session, "You have the control of the player with a blue triangle on his head, you can move it with the virtual joystick that appears as soon as you put a finger on the lower left part of the touchscreen."},
    {TutorialLine::BySpecialFunc, (void*)training_session, "The virtual joystick has two different speed for each direction, if you move the red ball more distant from the center the player will run faster but you'll have a less responsive control if you want to change his direction."},
    {TutorialLine::BySpecialFunc, (void*)training_session, "When the ball is in possession of your team you can use the BLUE button to pass the ball, tap it for a short pass, keep it pressed for a long pass. You can also pass the ball simply touching with a finger the man you want to pass to!"},
    {TutorialLine::BySpecialFunc, (void*)training_session, "You can shoot with the RED button, the joystick position when you RELEASE the button will decide the direction and the type of shot, experiment various joystick positions in a training session to find all of them!"},
    {TutorialLine::BySpecialFunc, (void*)training_session, "When the ball is in possession of the other team you can change the player you control touching another player, alternatively by tapping the blue button you can activate the nearest player to the ball position."},
    {TutorialLine::BySpecialFunc, (void*)training_session, "Tapping the RED button when the other team has the ball possession will make the active player try to get the ball with a sliding tackle toward the ball, but be careful to not commit a foul!"},
    {TutorialLine::BySpecialFunc, (void*)training_session, "Pressing the RED button when the ball is in no possession will perform a special action that will change depending from the ball height and your position, an head shot, a bicycle kick, a tackle... anything can happens if you tap it with the right timing and in a good position!"},
#endif
};

void tutorial_line(int i)
{
    bool stopped = false;
    extern void *hwin;
    extern BOOL time_stopped, pause_mode;
    struct EasyStruct easy = {0};
    running_tutorial_line = true;
    easy.es_TextFormat = lines[i].line.c_str();
    easy.es_GadgetFormat = msg_82;
    
    if (game_start) {
        if (!time_stopped) {
            StopTime();
            stopped = true;
        }
        hide_vjoy();
    }
    
    MyEasyRequest(hwin, &easy, NULL);
    
    if (game_start && !pause_mode)
        show_vjoy();
    
    if (stopped) {
        extern uint32_t ideal;
        RestartTime();
        ideal = Timer() - 1;
    }
    
    running_tutorial_line = false;
    lines[i].seen = true;
}

extern "C" void
init_tutorial()
{
    for (int i = 0; i < SZ(lines); ++i)
        lines[i].seen = false;
}


extern "C" void
check_tutorial()
{
    if (running_tutorial_line || reqqing || !tutorial)
        return;
    
    for (int i = 0; i < SZ(lines); ++i) {
        if (lines[i].seen)
            continue;
        
        if (lines[i].ActivatedByMenu() &&
            current_menu == (int)lines[i].data) {
            tutorial_line(i);
        }
        else if (lines[i].SpecialFuncMatch())
            tutorial_line(i);
    }
}

