package org.ggsoft.etw;

import org.libsdl.app.SDLActivity; 

import android.app.*;
import android.content.*;
import android.view.*;
import android.widget.AbsoluteLayout;
import android.widget.RelativeLayout;
import android.os.*;
import android.util.Log;
import android.graphics.*;
import android.text.method.*;
import android.text.*;
import android.media.*;
import android.hardware.*;
import android.content.*;

import java.lang.*;
import android.content.res.AssetManager;
import android.util.DisplayMetrics;
import com.google.ads.*;
import org.ggsoft.etw.util.*;
/**
    ETWGame
*/
public class ETWGame extends SDLActivity {

    private static AdView adview = null;
    
    static final String SKU_FULL_VERSION = "full_version";
    // (arbitrary) request code for the purchase flow
    static final int RC_REQUEST = 10001;
    private static native void load(AssetManager mgr);
    private static native void setInches(float w, float h);
    private static AssetManager mgr;
    private static Handler uiHandler;
    private static boolean full_version = false;
    private static boolean no_inapp_billing = false;
    private static IabHelper mHelper;
	private static final int SDLViewID = 1;

    // Setup
    public void setContentView(View view) {
        uiHandler = new Handler();
        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);
        mHelper = new IabHelper(this, "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAnwdMlRY6RLi0B0u0XKF6vMW3Oiysc8tkvRQ/EJOAGN8ZMyNtn9dssVHkJ/U559WmX1z9OpOa6MYIpsB/JyKrE+5SSGvAi2wEoSZaX2EcOGcVKReRzmfdXQPUQVj57Jkch3pri2z+olDmqjaAA1WeWmhTgyESwZqcXY0nU8Z9MuRwKiygmWKFArquvfj0IX+A7T6Pe92kVAZrKRQTsBb6J6kGxqdGbPS4tKXLdgau85937y8K+/yFv2ZKZabpUDA3HxxY/MNdJOUWQGwgaL+0N7OSpElD9dP3ZFUSr5nkinZpY3uezAnUnA41xBkJqeGhU9lIKvIuPkABoC/nT5K9WwIDAQAB");
        mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
            public void onIabSetupFinished(IabResult result) {
                if (!result.isSuccess()) {
                    // Oh noes, there was a problem.
                    no_inapp_billing = true;
                    Log.e("ETW", "Problem setting up In-app Billing: " + result);
                }            
                else {
                    // Hooray, IAB is fully set up!  
                    Log.v("ETW", "App billing setup OK, querying inventory.");
                    mHelper.queryInventoryAsync(mGotInventoryListener);
               }
            }
        });

        mLayout = new RelativeLayout(this);
        view.setId(SDLViewID);
        mLayout.addView(view);
        super.setContentView(mLayout);

        mgr = getResources().getAssets();
        load(mgr);
      
        setInches((float)dm.widthPixels/(float)dm.xdpi, (float)dm.heightPixels/(float)dm.ydpi);
    }

    // Listener that's called when we finish querying the items and subscriptions we own
    IabHelper.QueryInventoryFinishedListener mGotInventoryListener = new IabHelper.QueryInventoryFinishedListener() {
        public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
            if (result.isFailure()) {
                Log.e("ETW", "Failed to query inventory: " + result);
                return;
            }

            Log.d("ETW", "Query inventory was successful.");
            
            /*
             * Check for items we own. Notice that for each purchase, we check
             * the developer payload to see if it's correct! See
             * verifyDeveloperPayload().
             */
            
            // Do we have the premium upgrade?
            Purchase premiumPurchase = inventory.getPurchase(SKU_FULL_VERSION);
            full_version = (premiumPurchase != null && verifyDeveloperPayload(premiumPurchase));
            Log.v("ETW", "User is " + (full_version ? "PREMIUM" : "NOT PREMIUM"));
        }
    };
    
    static void buy_full_version() {
         if (no_inapp_billing) {
             Log.v("ETW", "Purchase procedure not available");
             return;
         }

         Log.v("ETW", "Purchase procedure started");
         String payload = "ETW".concat("full");

         mHelper.launchPurchaseFlow(mSingleton, SKU_FULL_VERSION, RC_REQUEST, 
                                    mPurchaseFinishedListener, payload); 
    }
        
    static IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener() {
        public void onIabPurchaseFinished(IabResult result, Purchase purchase) {
            Log.v("ETW", "Purchase finished: " + result + ", purchase: " + purchase);
            if (result.isFailure()) {
                Log.e("ETW", "Failed purchase!");
                return;
            }
            if (!verifyDeveloperPayload(purchase)) {
                Log.e("ETW", "Error purchasing. Authenticity verification failed.");
                return;
            }

            Log.v("ETW", "Purchase successful.");

            if (purchase.getSku().equals(SKU_FULL_VERSION)) {
                // bought the premium upgrade!
                Log.v("ETW", "Purchase is premium upgrade. Congratulating user.");
                alert("Thank you for upgrading to premium!");
                full_version = true;
            }
        }
    };

    static void alert(String message) {
        AlertDialog.Builder bld = new AlertDialog.Builder(mSingleton);
        bld.setMessage(message);
        bld.setNeutralButton("OK", null);
        Log.d("ETW", "Showing alert dialog: " + message);
        bld.create().show();
    }

    static boolean has_full_version() {
        return full_version;
    }

    static boolean verifyDeveloperPayload(Purchase p) {
        String orig = "ETW".concat("full");
        String payload = p.getDeveloperPayload();
//        Log.v("ETW", "Verifying that " + orig + " is equal to " + payload + " result:" + (payload == orig));
        return payload.equals(orig);
    }

    protected void onDestroy() {
        super.onDestroy();
        Log.v("SDL", "onDestroy()");
        // killing billing services
        if (mHelper != null) mHelper.dispose();
        mHelper = null;

        // Send a quit message to the application
        SDLActivity.nativeQuit();

        // Now wait for the SDL thread to quit
        if (mSDLThread != null) {
            try {
                mSDLThread.join();
            } catch(Exception e) {
                Log.v("SDL", "Problem stopping thread: " + e);
            }
            mSDLThread = null;

            //Log.v("SDL", "Finished waiting for SDL thread");
        }
    }

    public static void showAds(final boolean ontop) {
        uiHandler.post(new Runnable() {
            @Override
            public void run() {
                if (full_version || adview != null) { return; }

                adview = new AdView(mSingleton, AdSize.BANNER, "f048b8abe5da43c7");
//                TextView adview = new TextView(mSingleton);
//                adview.setText("Prova di testo");
                /*
                DisplayMetrics dm = new DisplayMetrics();
                mSingleton.getWindowManager().getDefaultDisplay().getMetrics(dm);
                AdSize testSize = AdSize.createAdSize(AdSize.BANNER, mSingleton);
                int px = (dm.widthPixels - testSize.getWidthInPixels(mSingleton)) / 2;
                int py = dm.heightPixels - testSize.getHeightInPixels(mSingleton);
                AbsoluteLayout.LayoutParams params = new AbsoluteLayout.LayoutParams(
                    testSize.getWidthInPixels(mSingleton), testSize.getHeightInPixels(mSingleton),
                    px, py);

                Log.v("ETW", "Positioning ads at " + px + "," + py);
*/

                /*
                FrameLayout.LayoutParams params =new FrameLayout.LayoutParams(
                                    FrameLayout.LayoutParams.FILL_PARENT, 
                                    FrameLayout.LayoutParams.FILL_PARENT, android.view.Gravity.BOTTOM|android.view.Gravity.CENTER_HORIZONTAL);
                */
            
                adview.setId(2);
                RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(
                                                        RelativeLayout.LayoutParams.WRAP_CONTENT, 
                                                        RelativeLayout.LayoutParams.WRAP_CONTENT);


                if (ontop) {
                    params.addRule(RelativeLayout.ALIGN_PARENT_TOP);
                    params.addRule(RelativeLayout.CENTER_HORIZONTAL);                    
                } else {
                    params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
                    params.addRule(RelativeLayout.CENTER_HORIZONTAL);
                    params.addRule(RelativeLayout.ABOVE, SDLViewID);
                }

                AdRequest re = new AdRequest();
//                re.addTestDevice(AdRequest.TEST_EMULATOR);
                adview.loadAd(re);
                mLayout.addView(adview, params);
                Log.v("ETW", "Showing ads, ontop:" + ontop);
            }
        });
    }
    public static void hideAds() {
        uiHandler.post(new Runnable() {
            @Override
            public void run() {
                if (adview == null) {  return; }

                mLayout.removeView(adview);
                adview.destroy();
                adview = null;

                Log.v("ETW", "Hiding ads");
            }
        });
    }
}

