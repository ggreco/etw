#ifndef TOUCH_CONTROL_H

#define TOUCH_CONTROL_H

#include "SDL.h"
#include <map>
#include <vector>

class TouchControl
{
    struct FreeTouch {
        FreeTouch() : fid(-1) {}
        FreeTouch(int i, int x, int y) : fid(i), start_x(x), start_y(y) {}
        int start_x, start_y;
        std::vector< std::pair<int, int> > coords;
        int fid;
        void move(int x, int y) { coords.push_back(std::make_pair(x, y)); }
    };
    
    typedef  std::map<int, FreeTouch> TouchMap;
    TouchMap touches_;
    
    struct button {
        int x, y, w, h;
        uint32_t id;
        SDL_Texture *img, *pressed;
        bool is_pressed;
        int finger;
        bool visible;
    };
    
    typedef std::vector<button> BtVec;
    typedef BtVec::iterator BtIt;
    
    BtVec buttons_;
    SDL_Window *screen_;
    SDL_Texture *knob_, *joybase_, *free_;
    int knob_w_, knob_h_;
    bool visible_;
    int joyfinger_;
    int fading_;
    int center_y_, center_x_, screen_w_, screen_h_;
    int reference_y_, reference_x_;
    int touch_x_, touch_y_;
    SDL_Rect joyrect_, activation_, freerect_;
    int freetouch_on_screen_;
    float distance_, delta_x_, delta_y_;
    bool show_joy_;
    
    button *in_button(int x, int y) const {
        for (BtVec::const_iterator it = buttons_.begin(); it != buttons_.end(); ++it) {
            if (!it->visible)
                continue;
            
            if (x >= it->x && y >= it->y &&
                x <= (it->x + it->w) &&
                y <= (it->y + it->h))
                return const_cast<button *>(&(*it));
        }
        
        return NULL;
    }
    
    bool in_activation_area(int x, int y) const {
        x -= activation_.x;
        y -= activation_.y;
        
        return (x >= 0 && y >= 0 && x <= activation_.w && y <= activation_.h);
    }
    
    // add a new touch, replace the previous one if stuck
    void add_touch(int i, int x, int y) {
        FreeTouch touch(i, x, y);
        touches_[i] = touch;
    }
    FreeTouch *find_touch(int fid) {
        TouchMap::iterator it = touches_.find(fid);
        
        if (it != touches_.end())
            return &it->second;
        else
            return NULL;
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
        FREE_TOUCHDOWN = 0x10000,
        MINIMIZED = 0x20000,
        RESTORED = 0x40000,
        FREE_SWIPE = 0x80000,
        QUIT = 0xf00000};
    
    TouchControl(SDL_Window *screen, const char *knob = NULL, const char *base = NULL, const char *touch = NULL);
    ~TouchControl();
    void activation(int x, int y, int w, int h) {
        activation_.x = x; activation_.y = y;
        activation_.w = w; activation_.h = h;
    }
    int touch_x() const { return touch_x_; }
    int touch_y() const { return touch_y_; }
    void add_button(const char *normal, const char *pressed, int x, int y, uint32_t id);
    int iteration();
    void draw();
    
    // joystick may be only shown if we have the images for it!
    void show_button(uint32_t id, bool val) {
        for (BtVec::iterator it = buttons_.begin(); it != buttons_.end(); ++it)
            if (it->id == id) {
                it->visible = val;
                if (!val)
                    it->pressed = false;
            }
    }
    
    void show_joy(bool val) {
        // if it's already visible we do not hide it since it means the player is
        // keeping is finger pressed on the screen while using another finger to pass
        show_joy_ = val && joybase_ && knob_;
    
        if (!val) {
            visible_ = false;
            fading_ = 0;
        }
    }
    
    SDL_Texture *load_bmp(const char*, int&, int&);
};

#endif
