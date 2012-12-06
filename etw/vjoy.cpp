#include "touch.h"

static TouchControl *touch = NULL, *replay_touch = NULL;

extern "C" {

#include "eat.h"
extern void SendNetwork(unsigned long joypos);
extern unsigned long NetJoyPos[2];

void touch_init()
{
    extern struct SDL_Window *screen;
    int w, h;
    
    if (touch)
        delete touch;
    if (replay_touch)
        delete replay_touch;

    SDL_RenderGetLogicalSize(SDL_GetRenderer(screen), &w, &h);

    D(bug("Starting touch interface for window size %dx%d\n", w, h));
    
    touch = new TouchControl(screen, "mobile/knob.bmp", "mobile/joystick-base.bmp");

    // only pause key in CPU vs CPU matches and highlights
    if ((p->team[0]->Joystick != -1 ||
         p->team[1]->Joystick != -1) && !highlight) {
        touch->add_button("mobile/red-normal.bmp", "mobile/red-pressed.bmp", WINDOW_WIDTH - 140, WINDOW_HEIGHT - 80, TouchControl::BUTTON_1);
        touch->add_button("mobile/blue-normal.bmp", "mobile/blue-pressed.bmp", WINDOW_WIDTH - 70, WINDOW_HEIGHT - 100, TouchControl::BUTTON_2);
    }

    touch->add_button("mobile/pause-normal.bmp", "mobile/pause-pressed.bmp", WINDOW_WIDTH - 64, 0, TouchControl::BUTTON_3);

    replay_touch = new TouchControl(screen);
    replay_touch->add_button("mobile/slow-normal.bmp", "mobile/slow-pressed.bmp", (WINDOW_WIDTH - 64 * 4) / 2, WINDOW_HEIGHT - 52, TouchControl::BUTTON_1);
    replay_touch->add_button("mobile/rpause-normal.bmp", "mobile/rpause-pressed.bmp", (WINDOW_WIDTH - 64 * 4) / 2 + 64, WINDOW_HEIGHT - 52, TouchControl::BUTTON_2);
    replay_touch->add_button("mobile/stop-normal.bmp", "mobile/stop-pressed.bmp", (WINDOW_WIDTH - 64 * 4) / 2 + 64 * 3, WINDOW_HEIGHT - 52, TouchControl::BUTTON_3);
    // show this button only if we are an highlight
    if (!highlight && !training)
        replay_touch->add_button("mobile/record-normal.bmp", "mobile/record-pressed.bmp", (WINDOW_WIDTH - 64 * 4) / 2 + 64 * 2, WINDOW_HEIGHT - 52, TouchControl::BUTTON_4);
    

    D(bug("Touch interfaces initialized\n"));
}

void draw_touch()
{
    if (replay_mode && replay_touch)
        replay_touch->draw();
    else if (touch)
        touch->draw();
}

#if 0
int display_touched()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_FINGERUP ||
            ev.type == SDL_MOUSEBUTTONUP)
            return TRUE;
    }
    return FALSE;
}
#endif


void hide_vjoy() {
    if (touch) {
        touch->show_joy(false);
        touch->show_button(TouchControl::BUTTON_1, false);
        touch->show_button(TouchControl::BUTTON_2, false);
    }
}
void show_vjoy() {
    if (touch) {
        touch->show_joy(true);
        touch->show_button(TouchControl::BUTTON_1, true);
        touch->show_button(TouchControl::BUTTON_2, true);
    }
}

void check_cpuvscpu_touch()
{
    if (!touch) {
        D(bug("MyReadTouchPort without touch class initialization!"));
        return;
    }
    int res = touch->iteration();
    
    // trigger pause on button_3 press
    if (res & TouchControl::BUTTON_3) {
        if (!network_game) {
            DoPause();
            return;
        }
    }
    if (res & TouchControl::QUIT) {
        SetResult("break");
        final = FALSE;
        quit_game=TRUE;
    }
}
    
int check_replay_touch()
{
    BOOL rc = TRUE;

    if (!replay_touch) {
        D(bug("check_replay_touch without touch class initialization!"));
        return FALSE;
    }

    int res = replay_touch->iteration();

    if (res & TouchControl::BUTTONUP) {
        if (res  & TouchControl::BUTTON_1) {
            if(!slow_motion) {
                MY_CLOCKS_PER_SEC_50<<=2;
                slow_motion=TRUE;
            }
            else {
                slow_motion=FALSE;
                MY_CLOCKS_PER_SEC_50>>=2;
            }
        }
        if (res & TouchControl::BUTTON_2) {
            if (pause_mode)
                pause_mode = FALSE;
            else
                return DoPause();
        }

        if (res & TouchControl::BUTTON_4) {
            SaveReplay();
            p->show_panel = PANEL_HIGHSAVE;
            p->show_time = 200;
        }

        if (res & TouchControl::BUTTON_3)
            rc = FALSE;
    }
    
    return rc;
}

uint32_t MyReadTouchPort(uint32_t l)
{
    uint32_t mask = 0;
    
    if (!touch) {
        D(bug("MyReadTouchPort without touch class initialization!"));
        return 0;
    }

    int res = touch->iteration();

    // if we don't have the ball and we got a touch out of a button
    // we count on the fact that only one touch player can play at once
    // and use the other player controller struct as additional data,
    if (res & TouchControl::FREE_TOUCHDOWN) {
        mask |= JPF_TOUCH_DOWN;
        r_controls[l^1][counter] = touch->touch_x() | (touch->touch_y() << 16);
    }
    // this will allow us to keep the coherence of replays.
    if (res & TouchControl::FREE_TOUCH) {
        mask |= JPF_TOUCH;
        r_controls[l^1][counter] = touch->touch_x() | (touch->touch_y() << 16);
    }

    // at this level we need only the button down event
    if (res & TouchControl::BUTTONDOWN) {
        if (res & TouchControl::BUTTON_1)
            mask |= JPF_BUTTON_RED;
        if (res & TouchControl::BUTTON_2)
            mask |= JPF_BUTTON_BLUE;
    }

    // trigger pause on button_3 press
    if (res & TouchControl::BUTTON_3) {
        if (!network_game) {
            DoPause();
            return 0;
        }
    }

    if (res & TouchControl::RIGHT)
        mask|=JPF_JOY_RIGHT;
    else if(res & TouchControl::LEFT)
        mask|=JPF_JOY_LEFT;

    if (res & TouchControl::DOWN)
        mask|=JPF_JOY_DOWN;
    else if(res & TouchControl::UP)
        mask|=JPF_JOY_UP;

    if (res & TouchControl::QUIT) {
        SetResult("break");
        final = FALSE;
        quit_game=TRUE;
    }

    if(!network_game)
        return mask;
    else {
        SendNetwork(mask);
        return NetJoyPos[l];
    }    
}

}
