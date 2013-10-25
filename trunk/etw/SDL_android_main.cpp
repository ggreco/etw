
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
static jmethodID mid_buyFull;
static jmethodID mid_hasFull;
static jclass my_class;

// Start up the SDL app
extern "C" void Java_org_libsdl_app_SDLActivity_nativeInit(JNIEnv* env, jclass cls, jobject obj)
{
    /* This interface could expand with ABI negotiation, calbacks, etc. */
    SDL_Android_Init(env, cls);

    SDL_SetMainReady();

    /* Run the application code! */
    int status;
    char *argv[2];
    argv[0] = strdup("ETW");
    argv[1] = NULL;
 __android_log_write(ANDROID_LOG_INFO, "ETW", "NATIVE INIT COMPLETED");
    status = SDL_main(1, argv);

 /* Do not issue an exit or the whole application will terminate instead of just the SDL thread */
    //exit(status);
}

extern "C" JNIEnv *Android_JNI_GetEnv();

extern "C" int has_full_version() {
    JNIEnv *env = Android_JNI_GetEnv();
    return env->CallStaticBooleanMethod(my_class, mid_hasFull);
}

extern "C" int buy_full_version() {
    JNIEnv *env = Android_JNI_GetEnv();
    env->CallStaticVoidMethod(my_class, mid_buyFull);
}

extern "C" void show_ads(int ontop) {
    JNIEnv *env = Android_JNI_GetEnv();
    env->CallStaticVoidMethod(my_class, mid_showAds, ontop ? true : false);
}

extern "C" void hide_ads() {
    JNIEnv *env = Android_JNI_GetEnv();
    env->CallStaticVoidMethod(my_class, mid_hideAds);
}

extern "C" void Java_org_ggsoft_etw_ETWGame_load(JNIEnv* env, jclass jcls, jobject obj) {
    // get the bindings for my callbacks
     __android_log_write(ANDROID_LOG_INFO, "ETW", "Setting up java JNI bindings...");

    my_class = (jclass)env->NewGlobalRef(jcls);
    mid_showAds = env->GetStaticMethodID(my_class, "showAds", "(Z)V");
    mid_hideAds = env->GetStaticMethodID(my_class, "hideAds", "()V");
    mid_buyFull = env->GetStaticMethodID(my_class, "buy_full_version", "()V");
    mid_hasFull = env->GetStaticMethodID(my_class, "has_full_version", "()Z");
    
     __android_log_write(ANDROID_LOG_INFO, "ETW", "Setting up java asset manager...");
    asset_mgr = AAssetManager_fromJava(env, obj); 
}

extern "C" void Java_org_ggsoft_etw_ETWGame_setInches(JNIEnv* env, jclass jcls, jfloat wi, jfloat hi) {
    display_width_inches = wi;
    display_height_inches = hi;
}
 
#endif /* __ANDROID__ */

/* vi: set ts=4 sw=4 expandtab: */
