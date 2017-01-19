// BannerExampleViewController.m
#import "GADBannerView.h"
#import "GADBannerViewDelegate.h"
#import <iAd/iAd.h>

extern UIView *gView;
extern UIViewController *gViewColtroller;
extern void init_controllers();

@interface myBanner : NSObject <GADBannerViewDelegate, ADBannerViewDelegate>
@property (nonatomic) BOOL show_ads;
@property (strong, nonatomic) ADBannerView *ibanner;
@property (strong, nonatomic) GADBannerView *gbanner;
@end

static myBanner *vbanner = nil;

void buy_full_version() {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"itms://itunes.apple.com/app/eat-the-whistle/id581355601"]];    
}

int has_full_version() {
    return 0;
}

extern void DoPause();
extern BOOL game_start, pause_mode;

@implementation myBanner

-(id)init {
// admob allocation
    NSLog(@"Creating google banner object");
    if (!gViewColtroller)
        init_controllers();
    
    // Create a view of the standard size at the top of the screen.
    // Available AdSize constants are explained in GADAdSize.h.
    self.gbanner = [[GADBannerView alloc] initWithAdSize:kGADAdSizeBanner];
    
    [self.gbanner setDelegate:self];
    // Specify the ad's "unit identifier." This is your AdMob Publisher ID.
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
        self.gbanner.adUnitID = @"a15137065c6c467";
    else
        self.gbanner.adUnitID = @"a151370265aa576";
    
    self.gbanner.hidden = TRUE;
    // Let the runtime know which UIViewController to restore after taking
    // the user wherever the ad goes and add it to the view hierarchy.
    self.gbanner.rootViewController = gViewColtroller;
    [gView addSubview:self.gbanner];
    
// iad allocation, only on iphone
    if (UI_USER_INTERFACE_IDIOM() != UIUserInterfaceIdiomPad) {
        NSLog(@"Creating apple banner object");
        self.ibanner = [[ADBannerView alloc] initWithFrame:CGRectZero];
        self.ibanner.delegate = self;
//    [self.ibanner sizeFromBannerContentSizeIdentifier:ADBannerContentSizeIdentifier320x50];
        self.ibanner.hidden = TRUE;
        [gView addSubview:self.ibanner];
    }
    else {
        self.ibanner = nil;
        [self.gbanner loadRequest:[GADRequest request]];
    }
    self.show_ads = TRUE;
    
    return self;
}
// *** Google related methods

- (void)adViewWillPresentScreen:(GADBannerView *)bannerView {
    if (game_start && !pause_mode)
        DoPause();
}

- (void)adViewWillLeaveApplication:(GADBannerView *)bannerView {
    if (game_start && !pause_mode)
        DoPause();
}

// Called before ad is shown, good time to show the add
- (void)adViewDidReceiveAd:(GADBannerView *)view
{
    NSLog(@"Admob load");
    if (self.ibanner)
        self.ibanner.hidden = TRUE;
    self.gbanner.hidden = !self.show_ads;
}

// An error occured
- (void)adView:(GADBannerView *)view didFailToReceiveAdWithError:(GADRequestError *)error
{
    NSLog(@"Admob error: %@", error);
    self.gbanner.hidden = TRUE;
}

// **** iAd related methods
- (void)bannerViewWillLoadAd:(ADBannerView *)banner
{
    NSLog(@"iAd load");
    self.gbanner.hidden = TRUE;
    self.ibanner.hidden = !self.show_ads;
}
// Called when an error occured
- (void)bannerView:(ADBannerView *)banner didFailToReceiveAdWithError:(NSError *)error
{
    NSLog(@"iAd error: %@", error);
    self.ibanner.hidden = TRUE;
    [self.gbanner loadRequest:[GADRequest request]];
}

-(void)dealloc {
    NSLog(@"Freeing ads");
    if (self.gbanner) {
        [self.gbanner setDelegate:nil];
        [self.gbanner removeFromSuperview];
        [self.gbanner release];
        self.gbanner = nil;
    }
    if (self.ibanner) {
        self.ibanner.delegate = nil;
        [self.ibanner removeFromSuperview];
        [self.ibanner release];
        self.ibanner = nil;
    }
    [super dealloc];
}


- (void)showAds:(int)ontop {
    self.show_ads = TRUE;
    
    NSLog(@"Displaying banner object ontop:%d.", ontop);
    
    CGSize AdSize = kGADAdSizeBanner.size;

    
    CGRect frame = self.gbanner.frame;
    frame.origin.x = (gViewColtroller.view.bounds.size.width - AdSize.width) / 2 ;
    
    if (ontop)
        frame.origin.y = 0.0f;
    else
        frame.origin.y = gViewColtroller.view.bounds.size.height - AdSize.height;
    
    self.gbanner.frame = frame;
    if (self.ibanner)
        self.ibanner.frame = frame;
}
@end

void show_ads(int ontop) {
    if (!vbanner)
        vbanner = [[myBanner alloc] init];
    
    [vbanner showAds:ontop];
}

void hide_ads() {
    if (vbanner) {
        [vbanner release];
        vbanner = nil;
    }
    /*
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
             [banner release];
             banner = nil;
         }];
    }
     */
}

