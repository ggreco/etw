cmake_minimum_required (VERSION 2.8.12)

project(ETW)

find_package (PkgConfig REQUIRED)
pkg_check_modules (SDL2 REQUIRED sdl2)
pkg_check_modules (SDL2_MIXER REQUIRED SDL2_mixer)

if (APPLE)
    add_definitions(-DMACOSX)
    set(ADDITIONAL_SOURCES FileReq.m lang.m)
    set(ADDITIONAL_LIBS "-framework Cocoa -framework Foundation")
else(APPLE)
    if (WIN32)
        add_definitions(-DWIN)
    else(WIN32)
        add_definitions(-DLINUX -DENABLE_BINRELOC)
        find_package(GTK2 REQUIRED)
        set(ADDITIONAL_LIBS ${GTK2_LIBRARIES})
        set(ADDITIONAL_HEADERS  ${GTK2_INCLUDE_DIRS})
    endif(WIN32)
endif(APPLE)

set (CMAKE_CXX_STANDARD 17)

set(sources highdirent.c cpu.c human.c myiff.c specials.c 
         	anim.c crowd.c intro.c os_control.c teamsetup.c 
        	refree.c data.c league.c os_init.c	tables.c 
        	arcade.c speed_tables.c lists.c os_video.c tactics.c 
        	chunkyblitting.c display.c loops.c ball.c teams.c 
        	commento.c etw.c main.c goalkeeper.c freq.c 
        	computer.c etw_locale.c	menu.c radar.c utility.c 
        	config.c font.c menu_config.c replay.c speed_data.c 
        	connection.c generic_video.c menu_data.c sound.c wc.c 
        	control.c gfx.cpp menu_font.c special.c network.c 
        	highsocket.c prefix.c vjoy.cpp touch.cpp world.c
            tutorial.cpp substitutions.c stubs.c 
            ${ADDITIONAL_SOURCES})

# comment this for release mode        
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(insight)
link_directories(${SDL2_MIXER_LIBRARY_DIRS} ${SDL2_LIBRARY_DIRS})
add_executable(etw ${sources})

set(LIBS ${LIBS} ${SDL2_MIXER_LIBRARIES} ${SDL2_LIBRARIES} ${ADDITIONAL_LIBS})
target_link_libraries(etw insight ${LIBS})

include_directories("include" ${SDL2_INCLUDE_DIRS} ${ADDITIONAL_HEADERS})
