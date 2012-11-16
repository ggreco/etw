cmake_minimum_required (VERSION 2.6)

project(ETW)

find_package (PkgConfig REQUIRED)
pkg_check_modules (SDL2 REQUIRED sdl2)
find_package(GTK2 REQUIRED)

set(sources highdirent.c cpu.c human.c myiff.c specials.c 
         	anim.c crowd.c intro.c os_control.c teamsetup.c 
        	refree.c data.c league.c os_init.c	tables.c 
        	arcade.c speed_tables.c lists.c os_video.c tactics.c 
        	chunkyblitting.c display.c loops.c ball.c teams.c 
        	commento.c etw.c main.c goalkeeper.c freq.c 
        	computer.c etw_locale.c	menu.c radar.c utility.c 
        	config.c font.c menu_config.c replay.c speed_data.c 
        	connection.c generic_video.c menu_data.c sound.c wc.c 
        	control.c gfx.c menu_font.c special.c network.c 
        	highsocket.c prefix.c vjoy.cpp touch.cpp)


link_directories(${SDL2_LIBRARY_DIRS})
add_executable(etw ${sources})

set(LIBS ${LIBS} ${SDL2_LIBRARIES} ${GTK2_LIBRARIES})
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

include_directories("include" ${SDL2_INCLUDE_DIRS} ${GTK2_INCLUDE_DIRS})
