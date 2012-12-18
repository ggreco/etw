//
//  GameCenter.m
//  ios
//
//  Created by Brad Jackson on 27/04/12.
//  Copyright (c) 2012 Wildfire Studios. All rights reserved.
//

#import "GameCenter.h"

static UIViewController *gViewColtroller = NULL;
static UIWindow *gWindow = NULL;
static UIView *gView = NULL;

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
    NSString *nid = [NSString stringWithCString:id encoding:NSUTF8StringEncoding];
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
    else
        NSLog(@"Not submitting already earned achievement %@", nid);
}

void add_score(int value)
{
    [[GameCenter getInstance] saveScore:value withIdentifier:@"arcade_results"];
}

void init_game_center()
{
    gWindow = [[UIApplication sharedApplication] keyWindow];
    gView = [gWindow.subviews objectAtIndex:0];
    id nextResponder = [gView nextResponder];
    if ([nextResponder isKindOfClass:[UIViewController class]])
        gViewColtroller = (UIViewController *) nextResponder;
    else
        NSLog(@"Unable to find viewcontroller!");
    
    [[GameCenter getInstance] authenticateLocalPlayer];
}

// SDLUIKitDelegate *gApplicationDelegate = NULL;

@implementation GKMatchmakerViewController (LandscapeOnly)

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return ( interfaceOrientation == UIInterfaceOrientationLandscapeLeft || interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

-(NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskLandscape;
}

- (BOOL)shouldAutorotate {
    
    return NO;
}
@end

@implementation UIApplication (LandscapeOnly)

- (NSUInteger)application:(UIApplication*)application supportedInterfaceOrientationsForWindow:(UIWindow*)window
{
    NSLog(@"Venngo chiam?");
    return UIInterfaceOrientationMaskAllButUpsideDown;
}
@end

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
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
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
    [localPlayer authenticateWithCompletionHandler:^(NSError *error)
    {
        if (localPlayer.isAuthenticated)
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
                         NSLog(@"Achievement %@ completed: %d", achievement.description, achievement.completed);
                     }
                 }
             }];
        }
        
        NSLog(@"Player Authenticated %d", localPlayer.isAuthenticated);
    }
    ];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    /*
    if ([alertView.title isEqualToString: GAME_CENTER_DISABLED])
    {
        // Responding to game center disabled dialog
        switch (buttonIndex)
        {
            case 1:
            {
                // Launch game center app
                [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"gamecenter:"]];
                break;
            }
            default:
                break;
        }
        return;
    }
    
    
    // Responding to rename local player
    Str aliasCStr = [localPlayer.alias UTF8String];
    
	switch (buttonIndex) {
		case 0:
		{
            // Create new
            TBConfig::instance().getProfile(aliasCStr);
            TBConfig::instance().setCurrentProfile(aliasCStr);
			break;
		}
		case 1:
		{
            // Rename
            TBConfig::instance().renameCurrentProfile(aliasCStr);
			break;
		}
		default:
			break;
	}
    
    // Tell TB to use the new profile name
    TumblebugsGuiManager::instance().mainmenuWindow->updateWelcomeText();
     */
}

- (void) getPlayerAlias: (char *)aliasBuf: (int) length
{
    /*
    const char *aliasPtr = [[localPlayer alias] UTF8String];
    strncpy(aliasBuf, aliasPtr, length);
     */
}

- (BOOL) isAuthenticated
{
    return localPlayer.isAuthenticated;
}

- (void) showHighScores:(NSString *) identifier
{
    if (![GameCenter isGameCenterAPIAvailable])
        return;
    
    GKLeaderboardViewController *leaderboardController = [[GKLeaderboardViewController alloc] init];
    if (leaderboardController != nil)
    {
        leaderboardController.leaderboardDelegate = [GameCenter getInstance];
        if ([identifier length] > 0)
        {
            [leaderboardController setCategory:[[[NSString alloc] initWithFormat:@"%@_Arcade_Test", [identifier stringByReplacingOccurrencesOfString:@" " withString:@"_"]] autorelease]];
        }
        leaderboardController.timeScope = GKLeaderboardTimeScopeWeek;
        
        [gViewColtroller presentModalViewController: leaderboardController animated: YES];

       /* UIWindow *mainWindow = [[UIApplication sharedApplication] keyWindow];
        [mainWindow insertSubview:leaderboardController.view aboveSubview:mainWindow]; */
        
        [leaderboardController release];
    }
}

+ (void) saveScore: (int) score withIdentifier:(NSString *) identifier
{
    NSLog(@"Logging score %d with id %@", score, identifier);
    
    GKScore *scoreReporter = [[[GKScore alloc] initWithCategory:identifier] autorelease];
    scoreReporter.value = score;
    
    [scoreReporter reportScoreWithCompletionHandler:^(NSError *error)
     {
        if (error != nil)
        {
            NSLog(@"Error Message: - %@",[error localizedDescription]);
        }
    } ];
    
    [scoreReporter release];
}

- (void) leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController
{
    [gViewColtroller dismissModalViewControllerAnimated: YES];
}

+ (void) leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController
{
    [gViewColtroller dismissModalViewControllerAnimated: YES];
}

- (void)sendAchievement:(GKAchievement *)achievement {
    
    if (achievement.percentComplete >= 100.0)   //Indicates the achievement is done
        achievement.showsCompletionBanner = YES;    //Indicate that a banner should be shown
    else
        achievement.showsCompletionBanner = NO;
    
    [achievement reportAchievementWithCompletionHandler:
     ^(NSError *error) {
         dispatch_async(dispatch_get_main_queue(), ^(void)
                        {
                            if (error == NULL) {
                                NSLog(@"Successfully sent archievement!");
                            } else {
                                NSLog(@"Achievement failed to send... will try again \
                                      later.  Reason: %@", error.localizedDescription);
                            }
                        });
     }];
}
- (void) showAchievements
{
    if (![GameCenter isGameCenterAPIAvailable])
        return;
	
    GKAchievementViewController *achievements = [[GKAchievementViewController alloc] init];
	if (achievements != NULL)
	{
		achievements.achievementDelegate = self;
		[gViewColtroller presentModalViewController: achievements animated: YES];
        
        [achievements release];
	}
}

- (void)achievementViewControllerDidFinish:(GKAchievementViewController *)viewController;
{
	[gViewColtroller dismissModalViewControllerAnimated: YES];
	//[gViewColtroller release];
}

+ (void)achievementViewControllerDidFinish:(GKAchievementViewController *)viewController;
{
	[gViewColtroller dismissModalViewControllerAnimated: YES];
	//[gViewColtroller release];
}

+(BOOL)isGameCenterNotificationUp
{
    NSArray *windows = [[UIApplication sharedApplication] windows];
    for(UIWindow *win in windows)
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

- (void)listSubviewsOfView:(UIView *)view 
{
    
    // Get the subviews of the view
    NSArray *subviews = [view subviews];
    
    // Return if there are no subviews
    if ([subviews count] == 0) return;
    
    for (UIView *subview in subviews) {
        
        NSLog(@"%@", subview);
        
        // List the subviews of subview
        [self listSubviewsOfView:subview];
    }
}



@end

