LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../android-project/jni/SDL
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/include $(LOCAL_PATH)/../android-project/jni/SDL_mixer

LOCAL_CFLAGS := -DANDROID 
# Add your application source files here...
LOCAL_SRC_FILES := SDL_android_main.cpp \
	highdirent.c cpu.c human.c myiff.c specials.c \
   	anim.c crowd.c intro.c os_control.c teamsetup.c \
   	refree.c data.c league.c os_init.c	tables.c \
   	arcade.c speed_tables.c lists.c os_video.c tactics.c \
   	chunkyblitting.c display.c loops.c ball.c teams.c \
   	commento.c etw.c main.c goalkeeper.c freq.c \
   	computer.c etw_locale.c	menu.c radar.c utility.c \
   	config.c font.c menu_config.c replay.c speed_data.c \
   	connection.c generic_video.c menu_data.c sound.c wc.c \
   	control.c gfx.c menu_font.c special.c network.c \
   	highsocket.c vjoy.cpp touch.cpp world.c \
    tutorial.cpp substitutions.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -llog -landroid

include $(BUILD_SHARED_LIBRARY)
