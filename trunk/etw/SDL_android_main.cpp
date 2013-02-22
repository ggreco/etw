
#include "SDL_config.h"

#ifdef __ANDROID__

/* Include the SDL main definition header */
#include "SDL_main.h"
/*******************************************************************************
                 Functions called by JNI
*******************************************************************************/
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/configuration.h>
#include <android/log.h>

extern "C" {
    AAssetManager* asset_mgr = NULL;
    double display_width_inches = 3.0;
    double display_height_inches = 2.0;
}

extern "C" const char *
get_lang_id()
{
    if (!asset_mgr) {
        __android_log_write(ANDROID_LOG_INFO, "ETW", "Language not found, no asset manager available");
        return NULL;
    }

    AConfiguration *cfg = AConfiguration_new();
    AConfiguration_fromAssetManager(cfg, asset_mgr);

    static char language[4];
    memset(language, 0, sizeof(language));
    AConfiguration_getLanguage(cfg, language);
    AConfiguration_delete(cfg);

    if (!*language) {
        __android_log_write(ANDROID_LOG_INFO, "ETW", "Language not found in aconfiguration");
        return NULL;
    }

    return language;
}


// Called before SDL_main() to initialize JNI bindings in SDL library
extern "C" void SDL_Android_Init(JNIEnv* env, jclass cls);


static jmethodID mid_showAds;
static jmethodID mid_hideAds;
static jclass my_class;

// Start up the SDL app
extern "C" void Java_org_ggsoft_etw_SDLActivity_nativeInit(JNIEnv* env, jclass cls, jobject obj)
{
    /* This interface could expand with ABI negotiation, calbacks, etc. */
    SDL_Android_Init(env, cls);

    // get the bindings for my callbacks
    my_class = (jclass)env->NewGlobalRef(cls);
    mid_showAds = env->GetStaticMethodID(my_class, "showAds", "()V");
    mid_hideAds = env->GetStaticMethodID(my_class, "hideAds", "()V");
    /* Run the application code! */
    int status;
    char *argv[2];
    argv[0] = strdup("SDL_app");
    argv[1] = NULL;
    status = SDL_main(1, argv);

    /* Do not issue an exit or the whole application will terminate instead of just the SDL thread */
    //exit(status);
}

extern "C" JNIEnv *Android_JNI_GetEnv();

extern "C" void show_ads() {
    JNIEnv *env = Android_JNI_GetEnv();
    env->CallStaticVoidMethod(my_class, mid_showAds);
}

extern "C" void hide_ads() {
    JNIEnv *env = Android_JNI_GetEnv();
    env->CallStaticVoidMethod(my_class, mid_hideAds);
}

extern "C" {
    void Java_org_libsdl_app_SDLActivity_nativeRunAudioThread(JNIEnv* env, jclass cls);
    void Java_org_libsdl_app_SDLActivity_onNativeResize(JNIEnv* env, jclass jcls,
                                    jint width, jint height, jint format);
    void Java_org_libsdl_app_SDLActivity_onNativeKeyDown(JNIEnv* env, jclass jcls, jint keycode);
    void Java_org_libsdl_app_SDLActivity_onNativeKeyUp(JNIEnv* env, jclass jcls, jint keycode);
    void Java_org_libsdl_app_SDLActivity_onNativeTouch(JNIEnv* env, jclass jcls,
                                    jint touch_device_id_in, jint pointer_finger_id_in,
                                    jint action, jfloat x, jfloat y, jfloat p);
    void Java_org_libsdl_app_SDLActivity_onNativeAccel(JNIEnv* env, jclass jcls,
                                    jfloat x, jfloat y, jfloat z);
    void Java_org_libsdl_app_SDLActivity_nativeQuit(JNIEnv* env, jclass cls);
    void Java_org_libsdl_app_SDLActivity_nativeRunAudioThread(JNIEnv* env, jclass cls);
    void Java_org_libsdl_app_SDLActivity_nativePause(JNIEnv* env, jclass cls);
    void Java_org_libsdl_app_SDLActivity_nativeResume(JNIEnv* env, jclass cls);
};

extern "C" void Java_org_ggsoft_etw_SDLActivity_load(JNIEnv* env, jclass jcls, jobject obj)
{
    asset_mgr = AAssetManager_fromJava(env, obj); 
}

extern "C" void Java_org_ggsoft_etw_SDLActivity_setInches(JNIEnv* env, jclass jcls, jfloat wi, jfloat hi)
{
    display_width_inches = wi;
    display_height_inches = hi;
}
                                    
// Resize
extern "C" void Java_org_ggsoft_etw_SDLActivity_onNativeResize(
                                    JNIEnv* env, jclass jcls,
                                    jint width, jint height, jint format)
{
    Java_org_libsdl_app_SDLActivity_onNativeResize(env, jcls, width, height, format);
}
 
// Keydown
extern "C" void Java_org_ggsoft_etw_SDLActivity_onNativeKeyDown(
                                    JNIEnv* env, jclass jcls, jint keycode)
{
    Java_org_libsdl_app_SDLActivity_onNativeKeyDown(env, jcls, keycode);
}
 

// Pause
extern "C" void Java_org_ggsoft_etw_SDLActivity_nativePause(JNIEnv* env, jclass cls) {
    Java_org_libsdl_app_SDLActivity_nativePause(env, cls);
}

// resume
extern "C" void Java_org_ggsoft_etw_SDLActivity_nativeResume(JNIEnv* env, jclass cls) {
    Java_org_libsdl_app_SDLActivity_nativeResume(env, cls);
}

// Keyup
extern "C" void Java_org_ggsoft_etw_SDLActivity_onNativeKeyUp(
                                    JNIEnv* env, jclass jcls, jint keycode)
{
    Java_org_libsdl_app_SDLActivity_onNativeKeyUp(env, jcls, keycode);
}
 
// Touch
extern "C" void Java_org_ggsoft_etw_SDLActivity_onNativeTouch(
                                    JNIEnv* env, jclass jcls,
                                    jint touch_device_id_in, jint pointer_finger_id_in,
                                    jint action, jfloat x, jfloat y, jfloat p)
{
    Java_org_libsdl_app_SDLActivity_onNativeTouch(env, jcls, touch_device_id_in, pointer_finger_id_in, action, x, y, p);
}
 
// Accelerometer
extern "C" void Java_org_ggsoft_etw_SDLActivity_onNativeAccel(
                                    JNIEnv* env, jclass jcls,
                                    jfloat x, jfloat y, jfloat z)
{
     Java_org_libsdl_app_SDLActivity_onNativeAccel(env, jcls, x, y, z);
}
 
// Quit
extern "C" void Java_org_ggsoft_etw_SDLActivity_nativeQuit(
                                    JNIEnv* env, jclass cls)
{
    Java_org_libsdl_app_SDLActivity_nativeQuit(env, cls);
}
 
extern "C" void Java_org_ggsoft_etw_SDLActivity_nativeRunAudioThread(
                                    JNIEnv* env, jclass cls)
{
    Java_org_libsdl_app_SDLActivity_nativeRunAudioThread(env, cls);
}

#endif /* __ANDROID__ */

/* vi: set ts=4 sw=4 expandtab: */
