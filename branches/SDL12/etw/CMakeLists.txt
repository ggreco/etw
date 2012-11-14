cmake_minimum_required (VERSION 2.6)

project(ETW)

find_package(SDL REQUIRED)
find_package(GTK2 REQUIRED)

set(sources highdirent.c cpu.c human.c myiff.c specials.c 
         	anim.c crowd.c intro.c os_control.c squadre.c 
        	arbitro.c data.c league.c os_init.c	tables.c 
        	arcade.c dati_vel.c lists.c os_video.c tactics.c 
        	chunkyblitting.c display.c loops.c palla.c teams.c 
        	commento.c etw.c main.c portiere.c freq.c 
        	computer.c etw_locale.c	menu.c radar.c utility.c 
        	config.c font.c menu_config.c replay.c velocita_g.c 
        	connection.c generic_video.c menu_data.c sound.c wc.c 
        	control.c gfx.c menu_font.c special.c network.c 
        	highsocket.c prefix.c)

add_executable(etw ${sources})

set(LIBS ${LIBS} ${SDL_LIBRARY} ${GTK2_LIBRARIES})
target_link_libraries(etw ${LIBS})
set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS CD_VERSION)
if (APPLE)
    set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS MACOSX)
else(APPLE)
    if (WIN32)
        set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS WIN)
    else(WIN32)
        set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS LINUX ENABLE_BINRELOC)
    endif(WIN32)
endif(APPLE)

include_directories("include" ${SDL_INCLUDE_DIR} ${GTK2_INCLUDE_DIRS})
