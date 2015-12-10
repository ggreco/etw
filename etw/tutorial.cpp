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

    extern BOOL MyEasyRequest(struct EasyStruct *);
    extern void StopTime();
    extern void RestartTime();
    extern void hide_vjoy();
    extern void show_vjoy();
    extern BOOL pause_mode;
    extern uint32_t ideal;
    extern BOOL time_stopped;
}

#include "tutorial.h"
#include <time.h>

#define SZ(x) (sizeof(x)/sizeof(x[0]))


bool running_tutorial_line = false;

static bool free_kicks_session() {
    return (training && game_start && free_kicks);
}
static bool penalties_session() {
    return (training && game_start && penalties);
}

static bool played_training = false;

static bool match_without_training()
{
    if (!game_start || pause_mode || training || played_training)
        return false;

    return true;
}

//#define CREATE_CATALOG

#ifdef CREATE_CATALOG

#include <string>
#include <map>
std::string strip_cr(const char*src)
{
    std::string result;
    while (*src) {
        if (*src == '\n')
            result.append("\\n");
        else
            result.push_back(*src);
        ++src;
    }
    return result;
}
struct ct {
    ct(const char *l, const char *o) {
        if (l)
            local = strip_cr(l);
        if (o)
            base = strip_cr(o);
    }
    ct() {}
    std::string local;
    std::string base;
};
std::map<int, ct> ct_lines;

extern "C" void ct_insert(int id, const char *n, const char *o)
{
    ct_lines[id] = ct(n, o);
}

extern "C" void ct_flush()
{
    FILE *f = fopen("language.ct", "w");
    if (!f)
        return;
    fprintf(f, "## version $VER: etw.catalog 1.0 (XX.XX.XX)\n"
               "## codeset X\n"
               "## language X\n");
    for (std::map<int, ct>::const_iterator it = ct_lines.begin(); it != ct_lines.end(); ++it) 
        fprintf(f, "msg_%d\n%s\n; %s\n", it->first, it->second.local.c_str(), it->second.base.c_str());
    
    fclose(f);
}
#endif
static bool training_session(void)
{
    static time_t last_training_tip = 0;
    
    if (!training || !game_start || pause_mode || free_kicks || penalties)
        return false;
    
    time_t now = ::time(NULL);
    
    // a tutorial every 10 seconds
    
    if (now > (last_training_tip + 9)) {
        played_training = true;
        last_training_tip = now;
        return true;
    }
    
    return false;
}

static TutorialLine lines[] = {
    {TutorialLine::ByMenuId, (void*)MENU_MAIN_MENU, msg_251},
#ifdef IPHONE
    {TutorialLine::ByMenuId, (void*)MENU_SCORES, msg_238},
#endif
    {TutorialLine::ByMenuId, (void*)MENU_GAME_START, msg_237},
    {TutorialLine::ByMenuId, (void*)MENU_ARCADE, msg_235},
    {TutorialLine::ByMenuId, (void*)MENU_SIMULATION, msg_241},
    {TutorialLine::ByMenuId, (void*)MENU_TRAINING, msg_239},
    {TutorialLine::ByMenuId, (void*)MENU_TEAM_SELECTION, msg_248},
    {TutorialLine::ByMenuId, (void*)MENU_ARCADE_SELECTION, msg_247},
    {TutorialLine::ByMenuId, (void*)MENU_TEAM_SETTINGS, msg_249},
    {TutorialLine::ByMenuId, (void*)MENU_MOBILE_PREFS, msg_246},
    {TutorialLine::ByMenuId, (void*)MENU_HIGHLIGHT, msg_236},
    {TutorialLine::ByMenuId, (void*)MENU_CHALLENGE, msg_244},
    {TutorialLine::BySpecialFunc, (void*)training_session, msg_252},
#ifdef MOBILE_VERSION
    {TutorialLine::BySpecialFunc, (void*)training_session, msg_256},
    {TutorialLine::BySpecialFunc, (void*)training_session, msg_258},
    {TutorialLine::BySpecialFunc, (void*)training_session, msg_245},
    {TutorialLine::BySpecialFunc, (void*)training_session, msg_254},
    {TutorialLine::BySpecialFunc, (void*)training_session, msg_257},
    {TutorialLine::BySpecialFunc, (void*)training_session, msg_253},
    {TutorialLine::BySpecialFunc, (void*)training_session, msg_243},
    {TutorialLine::BySpecialFunc, (void*)training_session, msg_240},
    {TutorialLine::BySpecialFunc, (void*)free_kicks_session, msg_250},
#endif
    {TutorialLine::BySpecialFunc, (void*)penalties_session, msg_242},
    {TutorialLine::BySpecialFunc, (void*)match_without_training, msg_255},
};

void tutorial_line(int i)
{
    bool stopped = false;
    struct EasyStruct easy = {0};
    running_tutorial_line = true;
    easy.es_TextFormat = lines[i].line;
    easy.es_GadgetFormat = msg_82;
    
    if (game_start) {
        if (!time_stopped) {
            StopTime();
            stopped = true;
        }
        hide_vjoy();
        // remove previous tutorial requester?
        ScreenSwap();
    }
    
    MyEasyRequest(&easy);
    
    if (game_start && !pause_mode)
        show_vjoy();
    
    if (stopped) {
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
            current_menu == (int)(size_t)lines[i].data) {
            tutorial_line(i);
        }
        else if (lines[i].SpecialFuncMatch())
            tutorial_line(i);
    }
}

