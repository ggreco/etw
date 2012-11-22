#ifndef TOUCH_CONTROL_H

#define TOUCH_CONTROL_H

#include "SDL.h"
#include <vector>
class TouchControl
{  
        struct button {
            int x, y, w, h, id;
            SDL_Texture *img, *pressed;
            bool is_pressed;
            int finger;
        };

        typedef std::vector<button> BtVec;
        typedef BtVec::iterator BtIt;

        BtVec buttons_;
        SDL_Window *screen_;
        SDL_Texture *knob_, *joybase_;
        int knob_w_, knob_h_;
        bool visible_;
        int joyfinger_;
        int fading_;
        int center_y_, center_x_, screen_w_, screen_h_;
        int reference_y_, reference_x_;
        int touch_x_, touch_y_;
        SDL_Rect joyrect_, activation_;
        float distance_, delta_x_, delta_y_;
        bool landscape_;

        button *in_button(int x, int y) const {
            for (BtVec::const_iterator it = buttons_.begin(); it != buttons_.end(); ++it)
                if (x >= it->x && y >= it->y &&
                    x <= (it->x + it->w) &&
                    y <= (it->y + it->h))
                    return const_cast<button *>(&(*it));
            
            return NULL;
        }

        bool in_activation_area(int x, int y) const {
            x -= activation_.x;
            y -= activation_.y;

            return (x >= 0 && y >= 0 && x <= activation_.w && y <= activation_.h);
        }
    public:
        enum Result {NOTHING = 0, 
                     LEFT = 0x1,
                     RIGHT = 0x2,
                     UP = 0x4,
                     DOWN = 0x8,
                     BUTTON_1 = 0x10,
                     BUTTON_2 = 0x20,
                     BUTTON_3 = 0x40,
                     BUTTON_4 = 0x80,
                     BUTTON_5 = 0x100,
                     BUTTON_6 = 0x200,
                     BUTTON_7 = 0x400,
                     BUTTON_8 = 0x800,
                     FAST = 0x1000,
                     BUTTONDOWN = 0x2000,
                     BUTTONUP = 0x4000,
                     FREE_TOUCH = 0x8000,
                     QUIT = 0xff0000};

        TouchControl(SDL_Window *screen, const char *knob = NULL, const char *base = NULL);
        ~TouchControl();
        void activation(int x, int y, int w, int h) {
            activation_.x = x; activation_.y = y;
            activation_.w = w; activation_.h = h;
        }
        int touch_x() const { return touch_x_; }
        int touch_y() const { return touch_y_; }
        int add_button(const char *normal, const char *pressed, int x, int y);
        int iteration();
        void draw();
        void landscape(bool flag) { landscape_ = flag; }
        bool landscape() const { return landscape_; }
        SDL_Texture *load_bmp(const char*, int&, int&);
};

#endif
