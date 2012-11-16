#include "touch.h"

static TouchControl *touch = NULL;

extern "C" {

#include "eat.h"
extern void SendNetwork(unsigned long joypos);
extern unsigned long NetJoyPos[2];

void init_touch(SDL_Window *win)
{
    int w, h;
    if (touch)
        delete touch;

    SDL_GetWindowSize(win, &w, &h);

    D(bug("Starting touch interface for window size %dx%d\n", w, h));
    
    touch = new TouchControl(win, "mobile/knob.bmp", "mobile/joystick-base.bmp", WINDOW_WIDTH, WINDOW_HEIGHT);

    touch->add_button("mobile/red-normal.bmp", "mobile/red-pressed.bmp", WINDOW_WIDTH - 140, WINDOW_HEIGHT - 80);
    touch->add_button("mobile/blue-normal.bmp", "mobile/blue-pressed.bmp", WINDOW_WIDTH - 70, WINDOW_HEIGHT - 100);
    touch->add_button("mobile/pause-normal.bmp", "mobile/pause-pressed.bmp", WINDOW_WIDTH - 66, 2);

    D(bug("Touch interface initialized\n"));
}

void draw_touch()
{
    if (touch)
        touch->draw();
}

uint32_t MyReadTouchPort(uint32_t l)
{
    uint32_t mask = 0;

    if (!touch) {
        extern SDL_Window *screen;
        init_touch(screen);
    }

    int res = touch->iteration();

    if (res & TouchControl::BUTTON_1)
        mask |= JPF_BUTTON_RED;
    if (res & TouchControl::BUTTON_2)
        mask |= JPF_BUTTON_BLUE;

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