// BannerExampleViewController.m
#import "GADBannerView.h"
#import "GADBannerViewDelegate.h"

extern UIView *gView;
extern UIViewController *gViewColtroller;
extern void init_controllers();

@interface MyBanner : GADBannerView <GADBannerViewDelegate> {}
@end

static MyBanner *banner = nil;

void buy_full_version() {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"itms://itunes.apple.com/app/eat-the-whistle/id581355601"]];    
}

int has_full_version() {
    return 0;
}

extern void DoPause();
extern BOOL game_start, pause_mode;

@implementation MyBanner

- (void)adViewWillPresentScreen:(GADBannerView *)bannerView {
    if (game_start && !pause_mode)
        DoPause();
}
/*
- (void)adViewDidDismissScreen:(GADBannerView *)bannerView {
    NSLog(@"Did dismiss screen");
}
- (void)adViewWillDismissScreen:(GADBannerView *)bannerView {
    NSLog(@"Will dismiss screen");
}
*/
- (void)adViewWillLeaveApplication:(GADBannerView *)bannerView {
    if (game_start && !pause_mode)
        DoPause();
}

@end
void show_ads(int ontop) {
    if (!banner) {
        NSLog(@"Creating banner object");
        if (!gViewColtroller)
            init_controllers();
        
        // Create a view of the standard size at the top of the screen.
        // Available AdSize constants are explained in GADAdSize.h.
        banner = [[MyBanner alloc] initWithAdSize:kGADAdSizeBanner];
        
        [banner setDelegate:banner];
        // Specify the ad's "unit identifier." This is your AdMob Publisher ID.
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
            banner.adUnitID = @"a15137065c6c467";
        else
            banner.adUnitID = @"a151370265aa576";
        
        // Let the runtime know which UIViewController to restore after taking
        // the user wherever the ad goes and add it to the view hierarchy.
        banner.rootViewController = gViewColtroller;
    }
    
    NSLog(@"Displaying banner object ontop:%d.", ontop);
    GADRequest *request = [GADRequest request];
#ifndef DEBUG_DISABLED
    request.testDevices =  [NSArray arrayWithObjects:GAD_SIMULATOR_ID, nil];
#endif
    [gView addSubview:banner];
    [banner loadRequest:request];
    
    // start with an animation
    CGSize AdSize = kGADAdSizeBanner.size;
    
    CGRect frame = banner.frame;
    if (ontop)
        frame.origin.y = -AdSize.height;
    else
        frame.origin.y = gViewColtroller.view.bounds.size.height;
    
    frame.origin.x = (gViewColtroller.view.bounds.size.width - AdSize.width) / 2 ;
    
    banner.frame = frame;
    
    [UIView beginAnimations:nil context:nil];
    [UIView setAnimationDuration:0.5];
    [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
    
    frame = banner.frame;
    
    if (ontop)
        frame.origin.y = 0.0f;
    else
        frame.origin.y = gViewColtroller.view.bounds.size.height - AdSize.height;
        
    banner.frame = frame;
    [UIView commitAnimations];
}

void hide_ads() {
    if (banner) {
        NSLog(@"Destroying banner object");
        [UIView animateWithDuration:0.5
                              delay:0.1
                            options: UIViewAnimationCurveEaseOut
                         animations:^
         {
             CGRect frame = banner.frame;
             frame.origin.y = -50.0f;
             banner.frame = frame;
         }
         completion:^(BOOL finished)
         {
             [banner setDelegate:nil];
             [banner removeFromSuperview];
             banner = nil;
         }];
    }
}

