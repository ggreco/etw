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

#ifdef MACOSX
@interface GameCenter : NSObject <GKGameCenterControllerDelegate, NSTextViewDelegate> {
    GKLocalPlayer *localPlayer;
    NSArray *descriptions;
    BOOL attemptedLogin;
}
#else
@interface GameCenter : NSObject <GKGameCenterControllerDelegate, UIAlertViewDelegate> {
    GKLocalPlayer *localPlayer;
    NSArray *descriptions;
    BOOL attemptedLogin;
}
#endif

@property (strong,atomic) NSMutableDictionary *achievementCache;

+ (GameCenter *)getInstance;
- (void) authenticateLocalPlayer;
- (BOOL) isAuthenticated;
- (void) showHighScores:(NSString *) identifier;
- (void) showAchievements;
- (void) resetAchievements;
//- (void) getPlayerAlias: (char *)aliasBuf: (int) length;
- (void) sendAchievement:(GKAchievement*) achievement;
- (void) saveScore: (int) score withIdentifier:(NSString *) rawIdentifier;
+ (BOOL) isGameCenterAPIAvailable;

@end
