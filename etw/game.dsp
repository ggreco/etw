# Microsoft Developer Studio Project File - Name="game" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=game - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "game.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "game.mak" CFG="game - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "game - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "game - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "game - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp2 /MD /W2 /GX /O2 /I "C:\SDL-1.2.3\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "WIN" /D "DEBUG_DISABLED" /D "CD_VERSION" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /i "C:\SDL-1.1.8\include" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 SDL.lib SDLmain.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sock32.lib /nologo /subsystem:windows /machine:I386 /libpath:"C:\SDL-1.1.6\lib"

!ELSEIF  "$(CFG)" == "game - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /Gm /GX /ZI /Od /I "C:\SDL-1.2.3\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "WIN" /D "CD_VERSION" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /i "c:\SDL-1.1.8\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no /map /nodefaultlib

!ENDIF 

# Begin Target

# Name "game - Win32 Release"
# Name "game - Win32 Debug"
# Begin Source File

SOURCE=.\anim.c
# End Source File
# Begin Source File

SOURCE=.\arbitro.c
# End Source File
# Begin Source File

SOURCE=.\arcade.c
# End Source File
# Begin Source File

SOURCE=.\chunkyblitting.c
# End Source File
# Begin Source File

SOURCE=.\commento.c
# End Source File
# Begin Source File

SOURCE=.\computer.c
# End Source File
# Begin Source File

SOURCE=.\config.c
# End Source File
# Begin Source File

SOURCE=.\connection.c
# End Source File
# Begin Source File

SOURCE=.\control.c
# End Source File
# Begin Source File

SOURCE=.\cpu.c
# End Source File
# Begin Source File

SOURCE=.\crowd.c
# End Source File
# Begin Source File

SOURCE=.\data.c
# End Source File
# Begin Source File

SOURCE=.\dati_vel.c
# End Source File
# Begin Source File

SOURCE=.\display.c
# End Source File
# Begin Source File

SOURCE=.\etw.c
# End Source File
# Begin Source File

SOURCE=.\etw_locale.c
# End Source File
# Begin Source File

SOURCE=.\font.c
# End Source File
# Begin Source File

SOURCE=.\freq.c
# End Source File
# Begin Source File

SOURCE=.\generic_video.c
# End Source File
# Begin Source File

SOURCE=.\gfx.c
# End Source File
# Begin Source File

SOURCE=.\Highdirent.c
# End Source File
# Begin Source File

SOURCE=.\highsocket.c
# End Source File
# Begin Source File

SOURCE=.\human.c
# End Source File
# Begin Source File

SOURCE=.\intro.c
# End Source File
# Begin Source File

SOURCE=.\league.c
# End Source File
# Begin Source File

SOURCE=.\lists.c
# End Source File
# Begin Source File

SOURCE=.\loops.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\menu.c
# End Source File
# Begin Source File

SOURCE=.\menu_config.c
# End Source File
# Begin Source File

SOURCE=.\menu_data.c
# End Source File
# Begin Source File

SOURCE=.\menu_font.c
# End Source File
# Begin Source File

SOURCE=.\myiff.c
# End Source File
# Begin Source File

SOURCE=.\network.c
# End Source File
# Begin Source File

SOURCE=.\os_control.c
# End Source File
# Begin Source File

SOURCE=.\os_init.c
# End Source File
# Begin Source File

SOURCE=.\os_video.c
# End Source File
# Begin Source File

SOURCE=.\palla.c
# End Source File
# Begin Source File

SOURCE=.\portiere.c
# End Source File
# Begin Source File

SOURCE=.\radar.c
# End Source File
# Begin Source File

SOURCE=.\replay.c
# End Source File
# Begin Source File

SOURCE=.\sound.c
# End Source File
# Begin Source File

SOURCE=.\special.c
# End Source File
# Begin Source File

SOURCE=.\specials.c
# End Source File
# Begin Source File

SOURCE=.\squadre.c
# End Source File
# Begin Source File

SOURCE=.\tables.c
# End Source File
# Begin Source File

SOURCE=.\tactics.c
# End Source File
# Begin Source File

SOURCE=.\teams.c
# End Source File
# Begin Source File

SOURCE=.\utility.c
# End Source File
# Begin Source File

SOURCE=.\velocita_g.c
# End Source File
# Begin Source File

SOURCE=.\wc.c
# End Source File
# Begin Source File

SOURCE="..\..\Sdl-1.2.3\lib\SDL.lib"
# End Source File
# Begin Source File

SOURCE="..\..\Sdl-1.2.3\lib\SDLmain.lib"
# End Source File
# End Target
# End Project
