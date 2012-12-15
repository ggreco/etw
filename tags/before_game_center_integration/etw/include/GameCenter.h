//
//  GameCenter.h
//  ios
//
//  Created by Brad Jackson on 27/04/12.
//  Copyright (c) 2012 Wildfire Studios. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

#include "SDL.h"

@interface SDL_uikitviewcontroller : UIViewController {
@private
    SDL_Window *window;
}
@end

@interface GameCenter : NSObject <GKLeaderboardViewControllerDelegate, GKAchievementViewControllerDelegate, UIAlertViewDelegate> {
    GKLocalPlayer *localPlayer;
    NSArray *descriptions;
    BOOL attemptedLogin;
}

+ (GameCenter *)getInstance;
- (void) authenticateLocalPlayer;
- (BOOL) isAuthenticated;
+ (void) showHighScores:(NSString *) identifier;
+ (void) showAchievements;
- (void) getPlayerAlias: (char *)aliasBuf: (int) length;

+ (void) saveScore: (int) score withIdentifier:(NSString *) rawIdentifier;
+ (BOOL) isGameCenterAPIAvailable;

@end
