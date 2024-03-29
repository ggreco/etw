#include "touch.h"
#include <math.h>
extern "C" {
#include "eat.h"
}

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

void TouchControl::
add_button(const char *normal, const char *pressed, int x, int y, uint32_t id, int wanted_width, int wanted_height)
{
    button b;
    if (!(b.img = load_bmp(normal, b.ow, b.oh))) {
        D(bug("Unable to create button %d, image %s not found\n", id, normal));
        return;
    }
    if (!(b.pressed = load_bmp(pressed, b.ow, b.oh))) { 
        D(bug("Unable to create button %d, selected image %s not found\n", id, pressed));
        SDL_DestroyTexture(b.img);
        return;
    }

    if (wanted_width <= 0)
        b.w = b.ow;
    if (wanted_height <= 0)
        b.h = b.oh;

    b.id = id;
    b.x = x;
    b.y = y;
    b.is_pressed = false;
    b.visible = true;

    buttons_.push_back(b);
}

       
TouchControl::
TouchControl(SDL_Window *screen, const char *knob, const char *base, const char *freetouch, double scaling) :
    screen_(screen), knob_(NULL), joybase_(NULL), free_(NULL),
    visible_(false), fading_(0), freetouch_on_screen_(0),
    distance_(0.0), delta_x_(0.0), delta_y_(0.0)
{
    if (knob)
        knob_ = load_bmp(knob, knob_orig_w_, knob_orig_h_);
    if (base)
        joybase_ = load_bmp(base, joyrect_orig_w_, joyrect_orig_h_);
    if (freetouch)
        free_ = load_bmp(freetouch, freerect_.w, freerect_.h);

    // we scale only if really needed
    if (scaling < 0.95 || scaling > 1.05) {
        joyrect_.w = (double)joyrect_orig_w_ * scaling;
        joyrect_.h = (double)joyrect_orig_h_ * scaling;
        knob_w_ = (double)knob_orig_w_ * scaling;
        knob_h_ = (double)knob_orig_h_ * scaling;
    }
    else {
        joyrect_.w = joyrect_orig_w_;
        joyrect_.h = joyrect_orig_h_;
        knob_w_ = knob_orig_w_;
        knob_h_ = knob_orig_h_;
    }

    SDL_RenderGetLogicalSize(SDL_GetRenderer(screen), &screen_w_, &screen_h_);

    if (knob_ && joybase_) {
        activation_.x = 0;
        activation_.y = screen_h_ / 2;
        activation_.w = screen_w_ / 3;
        activation_.h = screen_h_ / 2;
        show_joy_ = true;
    }
    else { // no joystick in this screen
        activation_.x = activation_.y = activation_.w = activation_.h = -1;
        show_joy_ = false;
    }
}

TouchControl::
~TouchControl()
{
    if (knob_)
        SDL_DestroyTexture(knob_);
    if (joybase_)
        SDL_DestroyTexture(joybase_);
    if (free_)
        SDL_DestroyTexture(free_);

    for (BtIt it = buttons_.begin(); it != buttons_.end(); ++it) {
        SDL_DestroyTexture(it->img);
        SDL_DestroyTexture(it->pressed);
    }
}

void TouchControl::
draw()
{
    if (SDL_Renderer *rend = SDL_GetRenderer(screen_)) {
        // visible and fading will never be not null if we don't have a knob & a joybase
        if (freetouch_on_screen_ > 0 && free_) {
            SDL_Rect dest = freerect_;
            int delta;
            // first we enlarge
            if (freetouch_on_screen_ >= 20)
                delta = freetouch_on_screen_ - 20;
            else // and then we shrink twice as fast
                delta = (20 - freetouch_on_screen_) * 2;
            
            dest.x += delta/2;
            dest.y += delta/2;
            dest.w -= delta;
            dest.h -= delta;
            freetouch_on_screen_--;

            SDL_RenderCopy(rend, free_, NULL, &dest);
        }

        if ((visible_ || fading_ > 0) && show_joy_) {
            if (!visible_)
                fading_--;

            SDL_Rect r = {0, 0, knob_orig_w_, knob_orig_h_}, 
                     dest = {center_x_ - knob_w_ / 2, 
                             center_y_ - knob_h_ / 2,
                             knob_w_, knob_h_};

            SDL_RenderCopy(rend, joybase_, NULL, &joyrect_);

#if 0
            if (dest.x < 0) {
                r.x += -dest.x;
                r.w += dest.x;
                dest.x = 0;
                dest.w = r.w;
            }
            if (dest.y < 0) {
                r.y += -dest.y;
                r.h += dest.y;
                dest.y = 0;
                dest.h = r.h;
            }
            if (dest.y < screen_h_) {
                if (dest.y + knob_h_ > screen_h_) {
                    r.h -= (dest.y + knob_h_ - screen_h_);
                    dest.h = r.h;
                }
                
            }
#endif
            SDL_RenderCopy(rend, knob_, &r, &dest);
        }

        for (BtIt it = buttons_.begin(); it != buttons_.end(); ++it) {
            if (!it->visible)
                continue;

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
#ifdef ANDROID
            case SDL_KEYDOWN:
                if (e.key.keysym.scancode == SDL_SCANCODE_AC_BACK) {
                    D(bug("Received android back button during game\n"));
                    return BACK; 
                }
                break;
#endif 
            case SDL_WINDOWEVENT:
                switch(e.window.event) {
                    case SDL_WINDOWEVENT_MINIMIZED:
                        D(bug("Received a minimize event during game\n"));
                        return MINIMIZED;
                    case SDL_WINDOWEVENT_RESTORED:
                        SDL_RenderSetLogicalSize(SDL_GetRenderer(screen_), WINDOW_WIDTH, WINDOW_HEIGHT);
                        D(bug("Received a restore event during game\n"));
                        return RESTORED;
                    case SDL_WINDOWEVENT_CLOSE:
                        return QUIT;
                }
                break;
            case SDL_FINGERUP:
                if (visible_ && e.tfinger.fingerId == joyfinger_) {
                    delta_x_ = delta_y_ = distance_ = 0.0;
                    fading_ = 20;
                    visible_ = false;
                    joyfinger_ = -1;
                }
                else { 
                    bool found = false;

                    
                    if (FreeTouch *ft = find_touch(e.tfinger.fingerId)) {
                        found = true;
                        int x = e.tfinger.x * (float)screen_w_,
                            y = e.tfinger.y * (float)screen_h_;
                        
                        if (abs(ft->start_x - x) < 20 &&
                            abs(ft->start_y - y) < 20) {
                            touch_x_ = x;
                            touch_y_ = y;
                            result |= FREE_TOUCH;
                            freetouch_on_screen_ = 25; // show the touch trace for half second
                            freerect_.x = touch_x_ - freerect_.w / 2;
                            freerect_.y = touch_y_ - freerect_.h / 2;
                        }
                        else {
                            result |= FREE_SWIPE;
                            touch_x_ = x - ft->start_x;
                            touch_y_ = y - ft->start_y;
                            D(bug("Swipe detected from %d,%d to %d,%d\n", ft->start_x, ft->start_y, x, y));
                        }
                        
                        touches_.erase((int)e.tfinger.touchId);
                    }
                    
                    for (BtIt it = buttons_.begin(); it != buttons_.end(); ++it) {
                        if (it->is_pressed && e.tfinger.fingerId == it->finger) {
                            it->is_pressed = false;
                            it->finger = -1;
                            result |= it->id;
                            // I'm not sure it's correct to emit buttonup here, we can have multiple button
                            // presses/releases in the same event cycle with multitouch
                            result |= BUTTONUP;
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        D(bug("Found fingerup for id %d with no previous finger down?\n", e.tfinger.fingerId));
                    }
                }
                break;
            case SDL_FINGERDOWN:
                {
                    int x = e.tfinger.x * (float)screen_w_,
                        y = e.tfinger.y * (float)screen_h_;
                    bool found = false;

                    if (button *b = in_button(x, y)) {
                        
                        b->is_pressed = true;
                        b->finger = e.tfinger.fingerId;
                        found = true;
                    }
                    else if (!visible_ && show_joy_ &&
                             in_activation_area(x, y)) {
                            visible_ = true;
                            joyfinger_ = e.tfinger.fingerId;
                            center_x_ = x;
                            center_y_ = y;
                            joyrect_.x = center_x_ - joyrect_.w / 2;
                            joyrect_.y = center_y_ - joyrect_.h / 2;
                            if (joyrect_.x < 20)
                                joyrect_.x = 20;
                            if (joyrect_.y < 0)
                                joyrect_.y = 0;
                            if (joyrect_.y + joyrect_.h > (screen_h_ - 15))
                                joyrect_.y = screen_h_ - joyrect_.h - 15;

                            reference_x_ = center_x_;
                            reference_y_ = center_y_;
                            found = true;
                    }      

                    if (!found) {
                        add_touch(e.tfinger.fingerId, x, y);
                        touch_x_ = x;
                        touch_y_ = y;
                        result |= FREE_TOUCHDOWN;
                    }
                }
                break;
            case SDL_FINGERMOTION:
                if (visible_ && e.tfinger.fingerId == joyfinger_) {
                    int x = e.tfinger.x * (float)screen_w_,
                        y = e.tfinger.y * (float)screen_h_;
                    center_x_ = x;
                    center_y_ = y;

                    
                    float radius = knob_w_;
                    delta_x_ = center_x_ - reference_x_;
                    delta_y_ = center_y_ - reference_y_;
                    distance_ = sqrtf(delta_x_ * delta_x_ + delta_y_ * delta_y_);

                    if (distance_ > radius) {
                        center_x_ = reference_x_ + (delta_x_ * radius) / distance_;
                        center_y_ = reference_y_ + (delta_y_ * radius) / distance_;
                    }
                }
                else if (FreeTouch *f = find_touch(e.tfinger.fingerId)) {
                    // handle free touch moves
                    f->move(e.tfinger.x * (float)screen_w_,
                            e.tfinger.y * (float)screen_h_);
                }
                break;
        }
    }

    if (visible_) {
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
    }


    for (BtVec::const_iterator it = buttons_.begin(); it != buttons_.end(); ++it) {
        if (!it->visible)
            continue;
        if (it->is_pressed) {
            result |= it->id;
            result |= BUTTONDOWN;
        }
    }
    return result;
}


