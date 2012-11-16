#include "touch.h"
#include <math.h>

SDL_Texture *TouchControl::
load_bmp(const char *name, int &w, int &h)
{
    SDL_Surface *bmp = SDL_LoadBMP(name);

    if (!bmp) return NULL;

    SDL_Texture *t = SDL_CreateTextureFromSurface(SDL_GetRenderer(screen_), bmp);

    SDL_FreeSurface(bmp);

    if (t) SDL_QueryTexture(t, NULL, NULL, &w, &h);

    return t;
}

int TouchControl::
add_button(const char *normal, const char *pressed, int x, int y)
{
    static int buttons[] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8};
    button b;
    if (!(b.img = load_bmp(normal, b.w, b.h)))
        return 0;
    if (!(b.pressed = load_bmp(pressed, b.w, b.h))) {
        SDL_DestroyTexture(b.img);
        return 0;
    }

    b.id = buttons[buttons_.size()];
    b.x = x;
    b.y = y;
    b.is_pressed = false;

    buttons_.push_back(b);

    return b.id;
}

        
TouchControl::
TouchControl(SDL_Window *screen, const char *knob, const char *base, int w, int h) :
    screen_(screen), visible_(false), fading_(0), 
    distance_(0.0), delta_x_(0.0), delta_y_(0.0),
    screen_w_(w), screen_h_(h)
{
    knob_ = load_bmp(knob, knob_w_, knob_h_);
    joybase_ = load_bmp(base, joyrect_.w, joyrect_.h);
    
    if (screen_w_ < 0 || screen_h_ < 0)
        SDL_GetWindowSize(screen, &screen_w_, &screen_h_);

    activation_.x = 0;
    activation_.y = screen_h_ / 2;
    activation_.w = screen_w_ / 3;
    activation_.h = screen_h_ / 2;
}

TouchControl::
~TouchControl()
{
    SDL_DestroyTexture(knob_);
    SDL_DestroyTexture(joybase_);

    for (BtIt it = buttons_.begin(); it != buttons_.end(); ++it) {
        SDL_DestroyTexture(it->img);
        SDL_DestroyTexture(it->pressed);
    }
}

void TouchControl::
draw()
{
    if (SDL_Renderer *rend = SDL_GetRenderer(screen_)) {
        if (visible_ || fading_ > 0) {
            if (!visible_)
                fading_--;

            SDL_Rect r = {0, 0, knob_w_, knob_h_}, 
                     dest = {center_x_ - knob_w_ / 2, 
                             center_y_ - knob_h_ / 2,
                             knob_w_, knob_h_};

            SDL_RenderCopy(rend, joybase_, NULL, &joyrect_);

            if (dest.x < 0) {
                r.x += -dest.x;
                r.w += dest.x;
                dest.x = 0;
            }
            if (dest.y < 0) {
                r.y += -dest.y;
                r.h += dest.y;
                dest.y = 0;
            }
            if (dest.y < screen_h_) {
                if (dest.y + knob_h_ > screen_h_)
                    r.h -= (dest.y + knob_h_ - screen_h_);

                SDL_RenderCopy(rend, knob_, &r, &dest);
            }
        }

        for (BtIt it = buttons_.begin(); it != buttons_.end(); ++it) {
            SDL_Rect r = {it->x, it->y, it->w, it->h};

            SDL_RenderCopy(rend, it->is_pressed ? it->pressed : it->img, NULL, &r);
        }
    }
}

int TouchControl::
iteration()
{
    uint32_t result = 0;
    SDL_Event e;

    while(SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                return QUIT;

            case SDL_MOUSEBUTTONUP:
                if (visible_) {
                    delta_x_ = delta_y_ = distance_ = 0.0;
                    fading_ = 20;
                    visible_ = false;
                }
                for (BtIt it = buttons_.begin(); it != buttons_.end(); ++it) {
                    if (it->is_pressed) {
                        it->is_pressed = false;
                        result |= it->id;
                        result |= BUTTONUP;
                    }
                }

                break;
            case SDL_MOUSEBUTTONDOWN:
                {
                    int x, y;

                    x = e.button.x;
                    y = e.button.y;

#if 0
                    // landscape?
                    else {
                        x = e.button.y;
                        y = 320 - e.button.x;
                    }
#endif
                    if (button *b = in_button(x, y)) {
                        result |= b->id;
                        result |= BUTTONDOWN;
                        b->is_pressed = true;
                    }
                    if (!visible_)
                        if (in_activation_area(x, y)) {
                            visible_ = 1;
                            center_x_ = x;
                            center_y_ = y;
                            joyrect_.x = center_x_ - joyrect_.w / 2;
                            joyrect_.y = center_y_ - joyrect_.h / 2;
                            if (joyrect_.x < 0)
                                joyrect_.x = 0;
                            if (joyrect_.y < 0)
                                joyrect_.y = 0;
                            if (joyrect_.y + joyrect_.h > screen_h_)
                                joyrect_.y = screen_h_ - joyrect_.h;

                            reference_x_ = center_x_;
                            reference_y_ = center_y_;
                        }                    
                }
                break;
            case SDL_MOUSEMOTION:
                if (visible_) {
                        center_x_ = e.motion.x;
                        center_y_ = e.motion.y;
#if 0
                    if (!landscape_) 
                    else {
                        center_x_ = e.motion.y;
                        center_y_ = 320 - e.motion.x;
                    }
#endif
                    float radius = knob_w_;
                    delta_x_ = center_x_ - reference_x_;
                    delta_y_ = center_y_ - reference_y_;
                    distance_ = sqrtf(delta_x_ * delta_x_ + delta_y_ * delta_y_);

                    if (distance_ > radius) {
                        center_x_ = reference_x_ + (delta_x_ * radius) / distance_;
                        center_y_ = reference_y_ + (delta_y_ * radius) / distance_;
                    }
                }
                break;
        }
    }

    if (distance_ >= (knob_w_ / 2))
        result |= FAST;
    if (distance_ > knob_w_ / 4) {
        if (delta_x_ >= knob_w_ / 5)
            result |= RIGHT;
        else if (delta_x_ <= -(knob_w_ / 5))
            result |= LEFT;
        if (delta_y_ >= knob_h_ / 5)
            result |= DOWN;
        else if (delta_y_ <= -(knob_h_ / 5))
            result |= UP;
    }

    return result;
}


