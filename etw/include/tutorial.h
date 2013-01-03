//
//  tutorial.h
//  etw
//
//  Created by Gabriele Greco on 1/2/13.
//  Copyright (c) 2013 Gabriele Greco. All rights reserved.
//

#ifndef __etw__tutorial__
#define __etw__tutorial__

#include <string>

struct TutorialLine
{
    enum Activation {ByMenuId, BySpecialFunc};
    Activation activation;
    void *data;
    std::string line;
    bool ActivatedByMenu() const { return activation == ByMenuId; }
    bool SpecialFuncMatch() const {
        if (activation != BySpecialFunc)
            return false;
        
        BOOL (*func)() = (BOOL(*)())data;
        return func();
    }
};

#endif /* defined(__etw__tutorial__) */
