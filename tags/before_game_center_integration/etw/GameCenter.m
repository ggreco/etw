//
//  GameCenter.m
//  ios
//
//  Created by Brad Jackson on 27/04/12.
//  Copyright (c) 2012 Wildfire Studios. All rights reserved.
//

#import "GameCenter.h"

void init_game_center()
{
    [[GameCenter getInstance] authenticateLocalPlayer];
}

typedef struct SDL_uikitopenglview SDL_uikitopenglview;

/* Define the SDL window structure, corresponding to toplevel windows */
struct SDL_Window
{
    const void *magic;
    Uint32 id;
    char *title;
    int x, y;
    int w, h;
    int min_w, min_h;
    Uint32 flags;
    
    /* Stored position and size for windowed mode */
    SDL_Rect windowed;
    
    SDL_DisplayMode fullscreen_mode;
    
    float brightness;
    Uint16 *gamma;
    Uint16 *saved_gamma;        /* (just offset into gamma) */
    
    SDL_Surface *surface;
    SDL_bool surface_valid;
    
    void *shaper;
    void *data;
    void *driverdata;
    
    SDL_Window *prev;
    SDL_Window *next;
};

struct SDL_WindowData
{
    UIWindow *uiwindow;
    SDL_uikitopenglview *view;
    SDL_uikitviewcontroller *viewcontroller;
};
typedef struct SDL_WindowData SDL_WindowData;

SDL_uikitviewcontroller *gViewColtroller = NULL;
UIWindow *gWindow = NULL;
SDL_uikitopenglview *gView = NULL;
// SDLUIKitDelegate *gApplicationDelegate = NULL;

void SetUIViewController(SDL_Window *aWindow)
{
    SDL_WindowData *data = (SDL_WindowData *)aWindow->driverdata;
    gViewColtroller = data->viewcontroller;
    gWindow = data->uiwindow;
    gView = data->view;
    // gApplicationDelegate = [SDLUIKitDelegate sharedAppDelegate];
}

@implementation GameCenter

NSString *const GAME_CENTER_DISABLED = @"Game Center Disabled";

// Public stuff

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

+ (void) showHighScores:(NSString *) identifier
{
    if (![GameCenter isGameCenterAPIAvailable])
    {
        return;
    }
    
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

+ (void) showAchievements
{
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

