package org.ggsoft.etw;

import java.util.Arrays;
import android.app.*;
import android.content.*;
import android.view.*;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;
//import android.widget.TextView;
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
    SDL Activity
*/
public class SDLActivity extends Activity {

    // Keep track of the paused state
    public static boolean mIsPaused = false, mIsSurfaceReady = false, mHasFocus = true;

    // Main components
    protected static SDLActivity mSingleton;
    protected static SDLSurface mSurface;
    protected static View mTextEdit;
    protected static ViewGroup mLayout;

    // This is what SDL runs in. It invokes SDL_main(), eventually
    protected static Thread mSDLThread;

    // Audio
    protected static Thread mAudioThread;
    protected static AudioTrack mAudioTrack;
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

    // Load the .so
    static {
        System.loadLibrary("SDL2");
        //System.loadLibrary("SDL2_image");
        System.loadLibrary("SDL2_mixer");
        //System.loadLibrary("SDL2_ttf");
        System.loadLibrary("main");
    }

    // Setup
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        //Log.v("SDL", "onCreate()");
        super.onCreate(savedInstanceState);
        
        // So we can call stuff from static callbacks
        mSingleton = this;

        // Set up the surface
        mSurface = new SDLSurface(getApplication());
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
        mSurface.setId(1);
        mLayout.addView(mSurface);
        setContentView(mLayout);

        mgr = getResources().getAssets();
        load(mgr);
      
        setInches((float)dm.widthPixels/(float)dm.xdpi, (float)dm.heightPixels/(float)dm.ydpi);
    }

    // Events
    @Override
    protected void onPause() {
        Log.v("SDL", "onPause()");
        super.onPause();
        SDLActivity.handlePause();
    }

    @Override
    protected void onResume() {
        Log.v("SDL", "onResume()");
        super.onResume();
        SDLActivity.handleResume();
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
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        Log.v("SDL", "onWindowFocusChanged(): " + hasFocus);

        SDLActivity.mHasFocus = hasFocus;
        if (hasFocus) {
            SDLActivity.handleResume();
        }
    }

    @Override
    public void onLowMemory() {
        Log.v("SDL", "onLowMemory()");
        super.onLowMemory();
        SDLActivity.nativeLowMemory();
    }

    @Override
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

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        int keyCode = event.getKeyCode();
        // Ignore certain special keys so they're handled by Android
        if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN ||
            keyCode == KeyEvent.KEYCODE_VOLUME_UP ||
            keyCode == KeyEvent.KEYCODE_CAMERA ||
            keyCode == 168 || /* API 11: KeyEvent.KEYCODE_ZOOM_IN */
            keyCode == 169 /* API 11: KeyEvent.KEYCODE_ZOOM_OUT */
            ) {
            return false;
        }
        return super.dispatchKeyEvent(event);
    }

    /** Called by onPause or surfaceDestroyed. Even if surfaceDestroyed
     *  is the first to be called, mIsSurfaceReady should still be set
     *  to 'true' during the call to onPause (in a usual scenario).
     */
    public static void handlePause() {
        if (!SDLActivity.mIsPaused && SDLActivity.mIsSurfaceReady) {
            SDLActivity.mIsPaused = true;
            SDLActivity.nativePause();
            mSurface.enableSensor(Sensor.TYPE_ACCELEROMETER, false);
        }
    }

    /** Called by onResume or surfaceCreated. An actual resume should be done only when the surface is ready.
     * Note: Some Android variants may send multiple surfaceChanged events, so we don't need to resume
     * every time we get one of those events, only if it comes after surfaceDestroyed
     */
    public static void handleResume() {
        if (SDLActivity.mIsPaused && SDLActivity.mIsSurfaceReady && SDLActivity.mHasFocus) {
            SDLActivity.mIsPaused = false;
            SDLActivity.nativeResume();
            mSurface.enableSensor(Sensor.TYPE_ACCELEROMETER, true);
        }
    }


    // Messages from the SDLMain thread
    static final int COMMAND_CHANGE_TITLE = 1;
    static final int COMMAND_UNUSED = 2;
    static final int COMMAND_TEXTEDIT_HIDE = 3;

    protected static final int COMMAND_USER = 0x8000;

    /**
     * This method is called by SDL if SDL did not handle a message itself.
     * This happens if a received message contains an unsupported command.
     * Method can be overwritten to handle Messages in a different class.
     * @param command the command of the message.
     * @param param the parameter of the message. May be null.
     * @return if the message was handled in overridden method.
     */
    protected boolean onUnhandledMessage(int command, Object param) {
        return false;
    }

    /**
     * A Handler class for Messages from native SDL applications.
     * It uses current Activities as target (e.g. for the title).
     * static to prevent implicit references to enclosing object.
     */
    protected static class SDLCommandHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Context context = getContext();
            if (context == null) {
                Log.e("ETW", "error handling message, getContext() returned null");
                return;
            }
            switch (msg.arg1) {
            case COMMAND_CHANGE_TITLE:
                if (context instanceof Activity) {
                    ((Activity) context).setTitle((String)msg.obj);
                } else {
                    Log.e("ETW", "error handling message, getContext() returned no Activity");
                }
                break;
            case COMMAND_TEXTEDIT_HIDE:
                if (mTextEdit != null) {
                    mTextEdit.setVisibility(View.GONE);

                    InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
                    imm.hideSoftInputFromWindow(mTextEdit.getWindowToken(), 0);
                }
                break;

            default:
                if ((context instanceof SDLActivity) && !((SDLActivity) context).onUnhandledMessage(msg.arg1, msg.obj)) {
                    Log.e("ETW", "error handling message, command is " + msg.arg1);
                }
            }
        }
    }

    // Handler for the messages
    Handler commandHandler = new SDLCommandHandler();

    // Send a message from the SDLMain thread
    boolean sendCommand(int command, Object data) {
        Message msg = commandHandler.obtainMessage();
        msg.arg1 = command;
        msg.obj = data;
        return commandHandler.sendMessage(msg);
    }

    // C functions we call
    public static native void nativeInit();
    public static native void nativeLowMemory();
    public static native void nativeQuit();
    public static native void nativePause();
    public static native void nativeResume();
    public static native void onNativeResize(int x, int y, int format);
    public static native void onNativeKeyDown(int keycode);
    public static native void onNativeKeyUp(int keycode);
    public static native void onNativeKeyboardFocusLost();
    public static native void onNativeTouch(int touchDevId, int pointerFingerId,
                                            int action, float x, 
                                            float y, float p);
    public static native void onNativeAccel(float x, float y, float z);
    public static native void onNativeSurfaceChanged();
    public static native void onNativeSurfaceDestroyed();
    public static native void nativeFlipBuffers();

    public static void flipBuffers() {
        SDLActivity.nativeFlipBuffers();
    }

    public static boolean setActivityTitle(String title) {
        // Called from SDLMain() thread and can't directly affect the view
        return mSingleton.sendCommand(COMMAND_CHANGE_TITLE, title);
    }

    public static boolean sendMessage(int command, int param) {
        return mSingleton.sendCommand(command, Integer.valueOf(param));
    }

    public static Context getContext() {
        return mSingleton;
    }

    static class ShowTextInputTask implements Runnable {
        /*
         * This is used to regulate the pan&scan method to have some offset from
         * the bottom edge of the input region and the top edge of an input
         * method (soft keyboard)
         */
        static final int HEIGHT_PADDING = 15;

        public int x, y, w, h;

        public ShowTextInputTask(int x, int y, int w, int h) {
            this.x = x;
            this.y = y;
            this.w = w;
            this.h = h;
        }

        @Override
        public void run() {
            AbsoluteLayout.LayoutParams params = new AbsoluteLayout.LayoutParams(
                    w, h + HEIGHT_PADDING, x, y);

            if (mTextEdit == null) {
                mTextEdit = new DummyEdit(getContext());

                mLayout.addView(mTextEdit, params);
            } else {
                mTextEdit.setLayoutParams(params);
            }

            mTextEdit.setVisibility(View.VISIBLE);
            mTextEdit.requestFocus();

            InputMethodManager imm = (InputMethodManager) getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.showSoftInput(mTextEdit, 0);
        }
    }

    public static boolean showTextInput(int x, int y, int w, int h) {
        // Transfer the task to the main thread as a Runnable
        return mSingleton.commandHandler.post(new ShowTextInputTask(x, y, w, h));
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
                    params.addRule(RelativeLayout.ABOVE, mSurface.getId());
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

    public static Surface getNativeSurface() {
        return SDLActivity.mSurface.getNativeSurface();
    }

    // Audio
    public static int audioInit(int sampleRate, boolean is16Bit, boolean isStereo, int desiredFrames) {
        int channelConfig = isStereo ? AudioFormat.CHANNEL_CONFIGURATION_STEREO : AudioFormat.CHANNEL_CONFIGURATION_MONO;
        int audioFormat = is16Bit ? AudioFormat.ENCODING_PCM_16BIT : AudioFormat.ENCODING_PCM_8BIT;
        int frameSize = (isStereo ? 2 : 1) * (is16Bit ? 2 : 1);
        
        Log.v("SDL", "SDL audio: wanted " + (isStereo ? "stereo" : "mono") + " " + (is16Bit ? "16-bit" : "8-bit") + " " + (sampleRate / 1000f) + "kHz, " + desiredFrames + " frames buffer");
        
        // Let the user pick a larger buffer if they really want -- but ye
        // gods they probably shouldn't, the minimums are horrifyingly high
        // latency already
        desiredFrames = Math.max(desiredFrames, (AudioTrack.getMinBufferSize(sampleRate, channelConfig, audioFormat) + frameSize - 1) / frameSize);
        
        if (mAudioTrack == null) {
            mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRate,
                    channelConfig, audioFormat, desiredFrames * frameSize, AudioTrack.MODE_STREAM);
            
            // Instantiating AudioTrack can "succeed" without an exception and the track may still be invalid
            // Ref: https://android.googlesource.com/platform/frameworks/base/+/refs/heads/master/media/java/android/media/AudioTrack.java
            // Ref: http://developer.android.com/reference/android/media/AudioTrack.html#getState()
            
            if (mAudioTrack.getState() != AudioTrack.STATE_INITIALIZED) {
                Log.e("SDL", "Failed during initialization of Audio Track");
                mAudioTrack = null;
                return -1;
            }
            
            mAudioTrack.play();
        }
       
        Log.v("SDL", "SDL audio: got " + ((mAudioTrack.getChannelCount() >= 2) ? "stereo" : "mono") + " " + ((mAudioTrack.getAudioFormat() == AudioFormat.ENCODING_PCM_16BIT) ? "16-bit" : "8-bit") + " " + (mAudioTrack.getSampleRate() / 1000f) + "kHz, " + desiredFrames + " frames buffer");
        
        return 0;
    }
    
    public static void audioWriteShortBuffer(short[] buffer) {
        for (int i = 0; i < buffer.length; ) {
            int result = mAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {
                    // Nom nom
                }
            } else {
                Log.w("SDL", "SDL audio: error return from write(short)");
                return;
            }
        }
    }
    
    public static void audioWriteByteBuffer(byte[] buffer) {
        for (int i = 0; i < buffer.length; ) {
            int result = mAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {
                    // Nom nom
                }
            } else {
                Log.w("SDL", "SDL audio: error return from write(byte)");
                return;
            }
        }
    }

    public static void audioQuit() {
        if (mAudioTrack != null) {
            mAudioTrack.stop();
            mAudioTrack = null;
        }
    }

    // Input

    /**
     * @return an array which may be empty but is never null.
     */
    public static int[] inputGetInputDeviceIds(int sources) {
        int[] ids = InputDevice.getDeviceIds();
        int[] filtered = new int[ids.length];
        int used = 0;
        for (int i = 0; i < ids.length; ++i) {
            InputDevice device = InputDevice.getDevice(ids[i]);
            if ((device != null) && ((device.getSources() & sources) != 0)) {
                filtered[used++] = device.getId();
            }
        }
        return Arrays.copyOf(filtered, used);
    }
}

/**
    Simple nativeInit() runnable
*/
class SDLMain implements Runnable {
    @Override
    public void run() {
        // Runs SDL_main()
        SDLActivity.nativeInit();

        //Log.v("SDL", "SDL thread terminated");
    }
}


/**
    SDLSurface. This is what we draw on, so we need to know when it's created
    in order to do anything useful. 

    Because of this, that's where we set up the SDL thread
*/
class SDLSurface extends SurfaceView implements SurfaceHolder.Callback, 
    View.OnKeyListener, View.OnTouchListener, SensorEventListener  {

    // Sensors
    protected static SensorManager mSensorManager;
    protected static Display mDisplay;

    // Keep track of the surface size to normalize touch events
    protected static float mWidth, mHeight;

    // Startup    
    public SDLSurface(Context context) {
        super(context);
        getHolder().addCallback(this); 
    
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
        setOnKeyListener(this); 
        setOnTouchListener(this);   

        mDisplay = ((WindowManager)context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        mSensorManager = (SensorManager)context.getSystemService(Context.SENSOR_SERVICE);

        // Some arbitrary defaults to avoid a potential division by zero
        mWidth = 1.0f;
        mHeight = 1.0f;
    }
    
    public Surface getNativeSurface() {
        return getHolder().getSurface();
    }

    // Called when we have a valid drawing surface
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.v("SDL", "surfaceCreated()");
        holder.setType(SurfaceHolder.SURFACE_TYPE_GPU);
    }

    // Called when we lose the surface
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.v("SDL", "surfaceDestroyed()");
        // Call this *before* setting mIsSurfaceReady to 'false'
        SDLActivity.handlePause();
        SDLActivity.mIsSurfaceReady = false;
        SDLActivity.onNativeSurfaceDestroyed();
    }

    // Called when the surface is resized
    @Override
    public void surfaceChanged(SurfaceHolder holder,
                               int format, int width, int height) {
        Log.v("SDL", "surfaceChanged()");

        int sdlFormat = 0x15151002; // SDL_PIXELFORMAT_RGB565 by default
        switch (format) {
        case PixelFormat.A_8:
            Log.v("SDL", "pixel format A_8");
            break;
        case PixelFormat.LA_88:
            Log.v("SDL", "pixel format LA_88");
            break;
        case PixelFormat.L_8:
            Log.v("SDL", "pixel format L_8");
            break;
        case PixelFormat.RGBA_4444:
            Log.v("SDL", "pixel format RGBA_4444");
            sdlFormat = 0x15421002; // SDL_PIXELFORMAT_RGBA4444
            break;
        case PixelFormat.RGBA_5551:
            Log.v("SDL", "pixel format RGBA_5551");
            sdlFormat = 0x15441002; // SDL_PIXELFORMAT_RGBA5551
            break;
        case PixelFormat.RGBA_8888:
            Log.v("SDL", "pixel format RGBA_8888");
            sdlFormat = 0x16462004; // SDL_PIXELFORMAT_RGBA8888
            break;
        case PixelFormat.RGBX_8888:
            Log.v("SDL", "pixel format RGBX_8888");
            sdlFormat = 0x16261804; // SDL_PIXELFORMAT_RGBX8888
            break;
        case PixelFormat.RGB_332:
            Log.v("SDL", "pixel format RGB_332");
            sdlFormat = 0x14110801; // SDL_PIXELFORMAT_RGB332
            break;
        case PixelFormat.RGB_565:
            Log.v("SDL", "pixel format RGB_565");
            sdlFormat = 0x15151002; // SDL_PIXELFORMAT_RGB565
            break;
        case PixelFormat.RGB_888:
            Log.v("SDL", "pixel format RGB_888");
            // Not sure this is right, maybe SDL_PIXELFORMAT_RGB24 instead?
            sdlFormat = 0x16161804; // SDL_PIXELFORMAT_RGB888
            break;
        default:
            Log.v("SDL", "pixel format unknown " + format);
            break;
        }

        mWidth = width;
        mHeight = height;
        SDLActivity.onNativeResize(width, height, sdlFormat);
        Log.v("SDL", "Window size:" + width + "x"+height);

        // Set mIsSurfaceReady to 'true' *before* making a call to handleResume
        SDLActivity.mIsSurfaceReady = true;
        SDLActivity.onNativeSurfaceChanged();


        if (SDLActivity.mSDLThread == null) {
            // This is the entry point to the C app.
            // Start up the C app thread and enable sensor input for the first time

            SDLActivity.mSDLThread = new Thread(new SDLMain(), "SDLThread");
            enableSensor(Sensor.TYPE_ACCELEROMETER, true);
            SDLActivity.mSDLThread.start();
        }
    }

    // unused
    @Override
    public void onDraw(Canvas canvas) {}


    // Key events
    @Override
    public boolean onKey(View  v, int keyCode, KeyEvent event) {
        // let android handle volume by itself
        if (keyCode == KeyEvent.KEYCODE_VOLUME_UP ||
            keyCode == KeyEvent.KEYCODE_VOLUME_DOWN) {
            return false;
        }

        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            //Log.v("SDL", "key down: " + keyCode);
            SDLActivity.onNativeKeyDown(keyCode);
            return true;
        }
        else if (event.getAction() == KeyEvent.ACTION_UP) {
            //Log.v("SDL", "key up: " + keyCode);
            SDLActivity.onNativeKeyUp(keyCode);
            return true;
        }
        
        return false;
    }

    // Touch events
    @Override
    public boolean onTouch(View v, MotionEvent event) {
        {
             final int touchDevId = event.getDeviceId();
             final int pointerCount = event.getPointerCount();
             // touchId, pointerId, action, x, y, pressure
             int actionPointerIndex = (event.getAction() & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT; /* API 8: event.getActionIndex(); */
             int pointerFingerId = event.getPointerId(actionPointerIndex);
             int action = (event.getAction() & MotionEvent.ACTION_MASK); /* API 8: event.getActionMasked(); */

             float x = event.getX(actionPointerIndex) / mWidth;
             float y = event.getY(actionPointerIndex) / mHeight;
             float p = event.getPressure(actionPointerIndex);

             if (action == MotionEvent.ACTION_MOVE && pointerCount > 1) {
                // TODO send motion to every pointer if its position has
                // changed since prev event.
                for (int i = 0; i < pointerCount; i++) {
                    pointerFingerId = event.getPointerId(i);
                    x = event.getX(i) / mWidth;
                    y = event.getY(i) / mHeight;
                    p = event.getPressure(i);
                    SDLActivity.onNativeTouch(touchDevId, pointerFingerId, action, x, y, p);
                }
             } else {
                SDLActivity.onNativeTouch(touchDevId, pointerFingerId, action, x, y, p);
             }
        }
      return true;
   } 

    // Sensor events
    public void enableSensor(int sensortype, boolean enabled) {
        // TODO: This uses getDefaultSensor - what if we have >1 accels?
        if (enabled) {
            mSensorManager.registerListener(this, 
                            mSensorManager.getDefaultSensor(sensortype), 
                            SensorManager.SENSOR_DELAY_GAME, null);
        } else {
            mSensorManager.unregisterListener(this, 
                            mSensorManager.getDefaultSensor(sensortype));
        }
    }
    
    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // TODO
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            float x, y;
            switch (mDisplay.getRotation()) {
                case Surface.ROTATION_90:
                    x = -event.values[1];
                    y = event.values[0];
                    break;
                case Surface.ROTATION_270:
                    x = event.values[1];
                    y = -event.values[0];
                    break;
                case Surface.ROTATION_180:
                    x = -event.values[1];
                    y = -event.values[0];
                    break;
                default:
                    x = event.values[0];
                    y = event.values[1];
                    break;
            }
            SDLActivity.onNativeAccel(-x / SensorManager.GRAVITY_EARTH,
                                      y / SensorManager.GRAVITY_EARTH,
                                      event.values[2] / SensorManager.GRAVITY_EARTH - 1);
        }
    }
    
}

/* This is a fake invisible editor view that receives the input and defines the
 * pan&scan region
 */
class DummyEdit extends View implements View.OnKeyListener {
    InputConnection ic;

    public DummyEdit(Context context) {
        super(context);
        setFocusableInTouchMode(true);
        setFocusable(true);
        setOnKeyListener(this);
    }

    @Override
    public boolean onCheckIsTextEditor() {
        return true;
    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {

        // This handles the hardware keyboard input
        if (event.isPrintingKey()) {
            if (event.getAction() == KeyEvent.ACTION_DOWN) {
                ic.commitText(String.valueOf((char) event.getUnicodeChar()), 1);
            }
            return true;
        }

        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            SDLActivity.onNativeKeyDown(keyCode);
            return true;
        } else if (event.getAction() == KeyEvent.ACTION_UP) {
            SDLActivity.onNativeKeyUp(keyCode);
            return true;
        }

        return false;
    }
        
    //
    @Override
    public boolean onKeyPreIme (int keyCode, KeyEvent event) {
        // As seen on StackOverflow: http://stackoverflow.com/questions/7634346/keyboard-hide-event
        // FIXME: Discussion at http://bugzilla.libsdl.org/show_bug.cgi?id=1639
        // FIXME: This is not a 100% effective solution to the problem of detecting if the keyboard is showing or not
        // FIXME: A more effective solution would be to change our Layout from AbsoluteLayout to Relative or Linear
        // FIXME: And determine the keyboard presence doing this: http://stackoverflow.com/questions/2150078/how-to-check-visibility-of-software-keyboard-in-android
        // FIXME: An even more effective way would be if Android provided this out of the box, but where would the fun be in that :)
        if (event.getAction()==KeyEvent.ACTION_UP && keyCode == KeyEvent.KEYCODE_BACK) {
            if (SDLActivity.mTextEdit != null && SDLActivity.mTextEdit.getVisibility() == View.VISIBLE) {
                SDLActivity.onNativeKeyboardFocusLost();
            }
        }
        return super.onKeyPreIme(keyCode, event);
    }

    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
        ic = new SDLInputConnection(this, true);

        outAttrs.imeOptions = EditorInfo.IME_FLAG_NO_EXTRACT_UI
                | 33554432 /* API 11: EditorInfo.IME_FLAG_NO_FULLSCREEN */;

        return ic;
    }
}

class SDLInputConnection extends BaseInputConnection {

    public SDLInputConnection(View targetView, boolean fullEditor) {
        super(targetView, fullEditor);

    }

    @Override
    public boolean sendKeyEvent(KeyEvent event) {

        /*
         * This handles the keycodes from soft keyboard (and IME-translated
         * input from hardkeyboard)
         */
        int keyCode = event.getKeyCode();
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            if (event.isPrintingKey()) {
                commitText(String.valueOf((char) event.getUnicodeChar()), 1);
            }
            SDLActivity.onNativeKeyDown(keyCode);
            return true;
        } else if (event.getAction() == KeyEvent.ACTION_UP) {

            SDLActivity.onNativeKeyUp(keyCode);
            return true;
        }
        return super.sendKeyEvent(event);
    }

    @Override
    public boolean commitText(CharSequence text, int newCursorPosition) {

        nativeCommitText(text.toString(), newCursorPosition);

        return super.commitText(text, newCursorPosition);
    }

    @Override
    public boolean setComposingText(CharSequence text, int newCursorPosition) {

        nativeSetComposingText(text.toString(), newCursorPosition);

        return super.setComposingText(text, newCursorPosition);
    }

    public native void nativeCommitText(String text, int newCursorPosition);

    public native void nativeSetComposingText(String text, int newCursorPosition);

}

