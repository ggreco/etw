//
//  GameCenter.m
//  ios
//
//  Created by Brad Jackson on 27/04/12.
//  Copyright (c) 2012 Wildfire Studios. All rights reserved.
//

#import "GameCenter.h"
#include "mydebug.h"

#ifdef MACOSX
NSViewController *gViewColtroller = NULL;
static NSWindow *gWindow = NULL;
NSView *gView = NULL;
#else
UIViewController *gViewColtroller = NULL;
static UIWindow *gWindow = NULL;
UIView *gView = NULL;

double display_width_inches = 2, display_height_inches = 1;

UIViewController *find_vc(UIView *root)
{
    id nextResponder = [root nextResponder];
    if ([nextResponder isKindOfClass:[UIViewController class]])
        return (UIViewController*)nextResponder;
    for (UIView *v in [root subviews]) {
        UIViewController *vc = find_vc(v);
        if (vc) return vc;
    }
    return nil;
}

void initialize_dpi() {
    double scale = [[UIScreen mainScreen] scale];
    double ppi = scale * ((UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) ? 132 : 163);
    display_width_inches = ([[UIScreen mainScreen] bounds].size.width * scale) / ppi;
    display_height_inches = ([[UIScreen mainScreen] bounds].size.height * scale) / ppi;
}

#endif


void init_controllers()
{
#ifdef MACOSX
    gWindow = [[NSApplication sharedApplication] keyWindow];
    gView = [gWindow contentView];
    gViewColtroller = [gWindow contentViewController];
/*    id nextResponder = [gView nextResponder];
    if ([nextResponder isKindOfClass:[NSViewController class]])
        gViewColtroller = (NSViewController *) nextResponder;
    else
        NSLog(@"Unable to find viewcontroller!");*/
#else
    gWindow = [[UIApplication sharedApplication] keyWindow];
//    gView = [([(gWindow.subviews[0]) subviews][0]) subviews][0];
////    id nextResponder = [gView nextResponder];
//    if ([nextResponder isKindOfClass:[UIViewController class]])
//        gViewColtroller = (UIViewController *) nextResponder;
    if (!(gViewColtroller = find_vc(gWindow)))
        NSLog(@"Unable to find viewcontroller!");
#endif
}

#ifndef ADMOB
// needed only in ad-supported versions
void hide_ads() {}
void show_ads(int i) {}
void buy_full_version() {}
int has_full_version() { return 1; }
#endif

const char *get_lang_id()
{
    NSString * language = [[NSLocale preferredLanguages] objectAtIndex:0];

    return [language UTF8String];
}

void show_world_scores()
{
    [[GameCenter getInstance] showHighScores:@"arcade_results"];
}

void reset_achievements()
{
    [[GameCenter getInstance] resetAchievements];
}

void show_achievements()
{
    [[GameCenter getInstance] showAchievements];
}

void add_achievement(const char *id, float percent)
{
    BOOL isnew = NO;
    NSString *nid = [NSString stringWithFormat:@"grp.%@",[NSString stringWithUTF8String:id]];
    GKAchievement* achievement = [[GameCenter getInstance].achievementCache objectForKey: nid];
    
    if (achievement == NULL) {
        achievement = [[GKAchievement alloc] initWithIdentifier:nid];
        isnew = YES;
    }
    
    if (achievement && achievement.percentComplete < 100.0f) {
        achievement.percentComplete += percent;
    
        [[GameCenter getInstance] sendAchievement:achievement];
        
        if (isnew)
            [[GameCenter getInstance].achievementCache setObject:achievement forKey:nid];
    }
    else {
        D(NSLog(@"Not submitting already earned achievement %@", nid));
    }
}

void add_score(int value)
{
    if (value > 0)
        [[GameCenter getInstance] saveScore:value withIdentifier:@"grp.arcade_results"];
}

void init_game_center()
{
    if (!gWindow)
        init_controllers();
    
    [[GameCenter getInstance] authenticateLocalPlayer];
}

// SDLUIKitDelegate *gApplicationDelegate = NULL;

@implementation GameCenter

NSString *const GAME_CENTER_DISABLED = @"Game Center Disabled";

// Public stuff

- (void) resetAchievements
{
    self.achievementCache= NULL;
	[GKAchievement resetAchievementsWithCompletionHandler: ^(NSError *error)
     {
         NSLog(error == nil ? @"Achievements resetted" : @"Unable to reset achievements!");
		 //[self callDelegateOnMainThread: @selector(achievementResetResult:) withArg: NULL error: error];
     }];
}

+ (GameCenter *)getInstance
{
	static GameCenter *gameCenterInstance;
	
	@synchronized(self)
    {
		if (!gameCenterInstance)
        {
			// This is never freed.  Is a singleton a leak?
			gameCenterInstance = [[[GameCenter alloc] init] retain];
		}
	}
	
	return gameCenterInstance;
}

- (id)init
{	
    if ((self = [super init]))
    {
        attemptedLogin = NO;
        self.achievementCache = nil;
	}
    
	return self;
}

+ (BOOL)isGameCenterAPIAvailable
{
    // Check for presence of GKLocalPlayer class.
    BOOL localPlayerClassAvailable = (NSClassFromString(@"GKLocalPlayer")) != nil;
    
    // The device must be running iOS 4.1 or later.
    NSString *reqSysVer = @"4.1";
#ifdef MACOSX
	NSString *currSysVer = @"10.11";
#else
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
#endif
    BOOL osVersionSupported = ([currSysVer compare: reqSysVer options: NSNumericSearch] != NSOrderedAscending);
    
    return (localPlayerClassAvailable && osVersionSupported);
}

- (void) authenticateLocalPlayer
{
    if (![GameCenter isGameCenterAPIAvailable])
        return;
    
    attemptedLogin = YES;

    // See page 28, should keep a local copy of player to compare after returning from background
    
    localPlayer = [GKLocalPlayer localPlayer];
#ifndef MACOSX
    [localPlayer setAuthenticateHandler:(^(UIViewController* viewcontroller, NSError *error)
#else
    [localPlayer authenticateWithCompletionHandler:^(NSError *error)
#endif
    {
        if (error != nil) {
            NSLog(@"Error initializing game center: %@", error);
        }
        else if (localPlayer.isAuthenticated)
        {
            printf("game center started!\n");
             //[GKNotificationBanner showBannerWithTitle:@"Title" message:@"Message" completionHandler:^{}];
            //[GameCenter showHighScores:@""];
            [GKAchievement loadAchievementsWithCompletionHandler: ^(NSArray *scores, NSError *error)
             {
                 if (error == nil) {
                     self.achievementCache = [NSMutableDictionary dictionaryWithCapacity: [scores count]];
                     
                     for (GKAchievement* achievement in scores) {
                         // work with achievement here, store it in your cache or smith
                         [self.achievementCache setObject: achievement forKey:achievement.identifier];
                     }
                     D(NSLog(@"Obtained %lu achievements, partially/totally completed", (unsigned long)[scores count]));
                 }
             }];
        }
#ifndef MACOSX
        else if(viewcontroller)
        {
            [gViewColtroller presentViewController: viewcontroller animated: YES completion:nil];
//            [self presentViewController:viewcontroller]; //present the login form
        }
#endif

        D(NSLog(@"Player Authenticated %d", localPlayer.isAuthenticated));
    }
#ifndef MACOSX
     )
#endif
    ];
}


- (BOOL) isAuthenticated
{
    return localPlayer.isAuthenticated;
}

- (void) showHighScores:(NSString *) identifier
{
    if (![GameCenter isGameCenterAPIAvailable])
        return;
    
    GKGameCenterViewController* leaderboardController = [[GKGameCenterViewController alloc] init];
    if (leaderboardController != nil)
    {
        leaderboardController.gameCenterDelegate = [GameCenter getInstance];
        if ([identifier length] > 0)
        {
            [leaderboardController setLeaderboardIdentifier:[[[NSString alloc] initWithFormat:@"%@_Arcade_Test", [identifier stringByReplacingOccurrencesOfString:@" " withString:@"_"]] autorelease]];
        }
        leaderboardController.leaderboardTimeScope = GKLeaderboardTimeScopeWeek;
        leaderboardController.viewState = GKGameCenterViewControllerStateLeaderboards;

        [gViewColtroller presentViewController: leaderboardController animated: YES completion:nil];

        [leaderboardController release];
    }
}

- (void) saveScore: (int) score withIdentifier:(NSString *) identifier
{
    D(NSLog(@"Logging score %d with id %@", score, identifier));
    
    GKScore *scoreReporter = [[[GKScore alloc] initWithLeaderboardIdentifier:identifier] autorelease];
    scoreReporter.value = score;

    [GKScore reportScores:@[scoreReporter] withCompletionHandler:^(NSError *error) {
        if (error != nil) {
            D(NSLog(@"Error Message: - %@",[error localizedDescription]));
        }
        else {
            D(NSLog(@"Successfully added score"));
        }
    } ];
}

- (void) gameCenterViewControllerDidFinish:(GKGameCenterViewController *)viewController
{
    [gViewColtroller dismissViewControllerAnimated:YES completion:nil];
}

+ (void) gameCenterViewControllerDidFinish:(GKGameCenterViewController *)viewController
{
    [gViewColtroller dismissViewControllerAnimated:YES completion:nil];
}

- (void)sendAchievement:(GKAchievement *)achievement {
    
    if (achievement.percentComplete >= 100.0)   //Indicates the achievement is done
        achievement.showsCompletionBanner = YES;    //Indicate that a banner should be shown
    else
        achievement.showsCompletionBanner = NO;
    
    NSArray *achievements = [NSArray arrayWithObjects:achievement, nil];
    [GKAchievement reportAchievements:achievements withCompletionHandler:^(NSError *error) {
        dispatch_async(dispatch_get_main_queue(), ^(void)
                       {
                           if (error == nil) {
                               D(NSLog(@"Successfully sent archievement %@!", achievement.description));
                           } else {
                               D(NSLog(@"Achievement %@ failed to send... will try again \
                                       later.  Reason: %@", achievement.description, error.localizedDescription));
                           }
                       });
    }];
}
                                         
- (void) showAchievements
{
    if (![GameCenter isGameCenterAPIAvailable])
        return;
	
    GKGameCenterViewController* achievements = [[GKGameCenterViewController alloc] init];
    if (achievements != nil)
    {
        achievements.gameCenterDelegate = [GameCenter getInstance];
        achievements.viewState = GKGameCenterViewControllerStateAchievements;
        [gViewColtroller presentViewController: achievements animated: YES completion:nil];
        
        [achievements release];
    }
}

+(BOOL)isGameCenterNotificationUp
{
#ifdef MACOSX
    NSArray *windows = [[NSApplication sharedApplication] windows];
    for(NSWindow *win in windows)
#else
    NSArray *windows = [[UIApplication sharedApplication] windows];
    for(UIWindow *win in windows)
#endif
    {
        NSArray *winSubViews = [win subviews];
        if([winSubViews count] == 1)
        {
            
            Class gcNotificationClass = NSClassFromString(@"GKGameEventView");
            if(gcNotificationClass && ([[winSubViews objectAtIndex:0] isKindOfClass:gcNotificationClass]))
            {
                exit(15);
                return YES;
            }
        }
    }
    return NO;
}

#ifdef MACOSX
- (void)listSubviewsOfView:(NSView *)view
#else
- (void)listSubviewsOfView:(UIView *)view
#endif
{
    
    // Get the subviews of the view
    NSArray *subviews = [view subviews];
    
    // Return if there are no subviews
    if ([subviews count] == 0) return;

#ifdef MACOSX
    for (NSView *subview in subviews) {
#else
    for (UIView *subview in subviews) {
#endif
        
        D(NSLog(@"%@", subview));
        
        // List the subviews of subview
        [self listSubviewsOfView:subview];
    }
}



@end

