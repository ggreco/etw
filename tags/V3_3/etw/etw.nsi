; etw.nsi
;
; This script is based on example2.nsi.
; 
; To run it from linux do something like that:
;
; /usr/local/cxoffice/bin/wine /usr/local/cxoffice/support/dotwine/fake_windows/Program\ Files/NSIS/makensis.exe etw.nsi
;
; NOTE: the installer script searches the data root in the parent directory.
;--------------------------------

; The name of the installer
Name "Eat The Whistle"

; The file to write
OutFile "etwsetup.exe"

; The default installation directory
InstallDir $PROGRAMFILES\EatTheWhistle

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\ETW" "Install_Dir"

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "ETW (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  SetOverwrite ifnewer ; NOT AN INSTRUCTION, NOT COUNTED IN SKIPPINGS
  
  ; Put file there
  File "..\ETW.exe"
  File "..\SDL.dll"
  File /r "..\arcadesnd"
  File /r "..\catalogs"
  File /r "..\crowd"
  File /r "..\gfx"
  File /r "..\intro"
  File /r "..\menugfx"
  File /r "..\music"
  File /r "..\newgfx"
  File /r "..\saves"
  File /r "..\snd"
  File /r "..\t"
  File /r "..\tct"
  File /r "..\teams"

  IfFileExists "$INSTDIR\etw.cfg" 0 skipcheck

    MessageBox MB_YESNO|MB_ICONQUESTION "Would you like to overwrite existing ETW configuration?" IDNO NoOverwrite ; skipped if file doesn't exist

    skipcheck:
    SetOverwrite on ; NOT AN INSTRUCTION, NOT COUNTED IN SKIPPINGS

  NoOverwrite:

  File "..\etw.cfg" ; skipped if answered no
  SetOverwrite try ; NOT AN INSTRUCTION, NOT COUNTED IN SKIPPINGS


  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\ETW "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ETW" "DisplayName" "Eat The Whistle"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ETW" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ETW" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ETW" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Eat The Whistle"
  CreateShortCut "$SMPROGRAMS\Eat The Whistle\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Eat The Whistle\Play Eat The Whistle.lnk" "$INSTDIR\ETW.exe"
  
SectionEnd

Section "Desktop Shortcut"
  CreateShortCut "$DESKTOP\Eat The Whistle.lnk" "$INSTDIR\ETW.exe"   
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ETW"
  DeleteRegKey HKLM SOFTWARE\ETW

  ; Remove files and uninstaller
  Delete $INSTDIR\ETW.exe
  Delete $INSTDIR\SDL.dll
  Delete $INSTDIR\uninstall.exe

  ; Removing data files
  Delete "$INSTDIR\arcadesnd\*.*"
  Delete "$INSTDIR\catalogs\*.*"
  Delete "$INSTDIR\crowd\*.*"
  Delete "$INSTDIR\gfx\*.*"
  Delete "$INSTDIR\intro\*.*"
  Delete "$INSTDIR\menugfx\*.*"
  Delete "$INSTDIR\music\*.*"
  Delete "$INSTDIR\newgfx\*.*"
  Delete "$INSTDIR\saves\*.*"
  Delete "$INSTDIR\snd\*.*"
  Delete "$INSTDIR\t\*.*"
  Delete "$INSTDIR\tct\*.*"
  Delete "$INSTDIR\teams\*.*"
  RMDir "$INSTDIR\arcadesnd"
  RMDir "$INSTDIR\catalogs"
  RMDir "$INSTDIR\crowd"
  RMDir "$INSTDIR\gfx"
  RMDir "$INSTDIR\intro"
  RMDir "$INSTDIR\menugfx"
  RMDir "$INSTDIR\music"
  RMDir "$INSTDIR\newgfx"
  RMDir "$INSTDIR\saves"
  RMDir "$INSTDIR\snd"
  RMDir "$INSTDIR\t"
  RMDir "$INSTDIR\tct"
  RMDir "$INSTDIR\teams"
  
  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Eat The Whistle\*.*"
  Delete "$DESKTOP\Eat The Whistle.lnk"
  ; Remove directories used
  RMDir "$SMPROGRAMS\EatTheWhistle"
  
  RMDir "$INSTDIR"

SectionEnd
