cmake_minimum_required (VERSION 2.6)
project(ETWEditor)
set(VERSION "0.1")

find_package (PkgConfig REQUIRED)
pkg_check_modules (GTK2 REQUIRED gtk+-2.0)
pkg_check_modules (GLIB REQUIRED glib-2.0)

set(CMAKE_BUILD_TYPE Debug)
#SET(CMAKE_BUILD_TYPE distribution)
#SET(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3")
#SET(CMAKE_C_FLAGS_DISTRIBUTION "-O3")

# necessario per l'icona
if (WIN32)
# lo script di check di openal non funziona bene in un cross-environment
    ENABLE_LANGUAGE(RC)
    set(ADDITIONAL_SOURCES icon.rc)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gstabs -mms-bitfields")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gstabs -mms-bitfields")
else ()
    if (APPLE)
        message(STATUS "OSX compilation detected, gtk libraries: ${GTK2_LIBRARIES}.")
        configure_file(Info.plist.in "${PROJECT_BINARY_DIR}/Info.plist")        
        set(CARBON_LIBRARY "-framework Carbon")
        MARK_AS_ADVANCED(CARBON_LIBRARY)
        message(STATUS "Carbon lib: ${CARBON_LIBRARY}")
        set(ADDITIONAL_LIBS ${CARBON_LIBRARY} ${ADDITIONAL_LIBS})
    else ()
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGDK_PIXBUF_DISABLE_DEPRECATED")
    endif()
# kludge necessario perche' cmake al momento non sa della separazione di gdk-pixbuf
    find_path(GDK_PIXBUF_INCLUDE_DIRS NAMES gdk-pixbuf/gdk-pixbuf.h PATHS
            /usr/include/gdk-pixbuf-2.0
            /usr/include)
    find_path(GDK_CONFIG_DIRS NAMES gdkconfig.h PATHS
            /usr/include/gtk-2.0/gdk
            /usr/lib/gtk-2.0/include)
    if (GDK_PIXBUF_INCLUDE_DIRS)
         set(GTK2_INCLUDE_DIRS "${GDK_PIXBUF_INCLUDE_DIRS}" "${GTK2_INCLUDE_DIRS}")
# Remove duplicates when cmake will be fixed
    endif (GDK_PIXBUF_INCLUDE_DIRS)
    if (GDK_CONFIG_DIRS)
        set(GTK2_INCLUDE_DIRS "${GDK_CONFIG_DIRS}" "${GTK2_INCLUDE_DIRS}")
# Remove duplicates when cmake will be fixed
    endif (GDK_CONFIG_DIRS)

    list(REMOVE_DUPLICATES GTK2_INCLUDE_DIRS)
    message(STATUS "GTK search paths: ${GTK2_INCLUDE_DIRS}")
endif ()

if (GTK2_LIBRARY_DIRS)
    link_directories("${GTK2_LIBRARY_DIRS}")
endif()

include_directories(${GTK2_INCLUDE_DIRS} oogtk ${GLIB_INCLUDE_DIRS})

add_executable(etwed WIN32 editor.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GTK2_CFLAGS_OTHER}")
target_link_libraries(etwed ${GTK2_LIBRARIES} ${ADDITIONAL_LIBS})


