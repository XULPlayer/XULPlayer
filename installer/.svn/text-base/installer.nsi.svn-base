;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NSIS installer script for XULPlayer;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

!define PRODUCT_NAME "XULPlayer"
!define VER_MAJOR   "0.5"
!define VER_MINOR   "pre8"
!define PRODUCT_VERSION "${VER_MAJOR}-${VER_MINOR}"
!define PRODUCT_GROUP ""
!define PRODUCT_PUBLISHER "XULPlayer Team"
!define PRODUCT_WEB_SITE "http://xulplayer.sourceforge.net/install"
!define PRODUCT_DIR_REGKEY "Software\${PRODUCT_NAME}"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_ID "${PRODUCT_NAME}-${VER_MAJOR}"
!define MAIN_APP "$INSTDIR\xulplayer.exe"
!define BIN_PATH "..\release"

;;;;;;;;;;;;;;;;;;;;;;;;;
; General configuration ;
;;;;;;;;;;;;;;;;;;;;;;;;;

Name "${PRODUCT_GROUP} ${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile xulplayer-${PRODUCT_VERSION}.exe
InstallDir "$PROGRAMFILES\XULPlayer"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" "InstallDir"
!ifdef NSIS_LZMA_COMPRESS_WHOLE
SetCompressor lzma
!else
SetCompressor /SOLID lzma
!endif

SpaceTexts           none
ShowInstDetails      show
ShowUninstDetails    show
BrandingText         "${PRODUCT_NAME} ${__DATE__}"
Caption              "$(^Name)"
UninstallCaption     "$(^Name) uninstaller"

SetOverwrite ifnewer
CRCCheck on

InstType "Recommended"
InstType "Full"
InstType "Minimum"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; NSIS Modern User Interface configuration ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

!include             MUI.nsh
!include             ZipDLL.nsh
!include             NSDialogs.nsh
!include             BlueHelper.nsh

${VerCmp}

; MUI Settings
  !define MUI_ABORTWARNING
  !define MUI_ICON "installer.ico"
  !define MUI_UNICON "installer.ico"
  !define MUI_COMPONENTSPAGE_SMALLDESC

; Installer pages
  ; Welcome page
    !define MUI_WELCOMEPAGE_TITLE_3LINES
    !insertmacro MUI_PAGE_WELCOME
  ; License page
    !insertmacro MUI_PAGE_LICENSE "license.txt"
  ; Components page
    !insertmacro MUI_PAGE_COMPONENTS

  ; Directory page
    !insertmacro MUI_PAGE_DIRECTORY

  ; Instfiles page
    !insertmacro MUI_PAGE_INSTFILES
  ; Finish page
    !define MUI_FINISHPAGE_RUN "${MAIN_APP}"
    !define MUI_FINISHPAGE_RUN_NOTCHECKED
;    !define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
 ;   !define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
    !define MUI_FINISHPAGE_LINK "Visit Homepage"
    !define MUI_FINISHPAGE_LINK_LOCATION "{PRODUCT_WEB_SITE}"
    !define MUI_FINISHPAGE_NOREBOOTSUPPORT
    !insertmacro MUI_PAGE_FINISH

; Uninstaller pages
    !insertmacro MUI_UNPAGE_CONFIRM
    !insertmacro MUI_UNPAGE_COMPONENTS
    !insertmacro MUI_UNPAGE_INSTFILES
    !insertmacro MUI_UNPAGE_FINISH

; Language files
  !insertmacro MUI_LANGUAGE "English" # first language is the default language
  ;!insertmacro MUI_LANGUAGE "SimpChinese"

!insertmacro MUI_RESERVEFILE_LANGDLL

; Reserve files
  !insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Var UninstallLog

!macro OpenUninstallLog
  FileOpen $UninstallLog "$INSTDIR\uninstall.log" w
  FileSeek $UninstallLog 0 END
!macroend

!macro CloseUninstallLog
  FileClose $UninstallLog
  SetFileAttributes "$INSTDIR\uninstall.log" HIDDEN
!macroend

!macro InstallFile FILEREGEX
  File "${FILEREGEX}"
  !define Index 'Line${__LINE__}'
  FindFirst $0 $1 "$INSTDIR\${FILEREGEX}"
  StrCmp $0 "" "${Index}-End"
  "${Index}-Loop:"
    StrCmp $1 "" "${Index}-End"
    FileWrite $UninstallLog "$1$\r$\n"
    FindNext $0 $1
    Goto "${Index}-Loop"
  "${Index}-End:"
  !undef Index
!macroend

!macro InstallFolder FOLDER
  File /r "${FOLDER}"
  Push "${FOLDER}"
  Call InstallFolderInternal
!macroend

Function StrStr
/*After this point:
  ------------------------------------------
  $R0 = SubString (input)
  $R1 = String (input)
  $R2 = SubStringLen (temp)
  $R3 = StrLen (temp)
  $R4 = StartCharPos (temp)
  $R5 = TempStr (temp)*/
 
  ;Get input from user
  Exch $R0
  Exch
  Exch $R1
  Push $R2
  Push $R3
  Push $R4
  Push $R5
 
  ;Get "String" and "SubString" length
  StrLen $R2 $R0
  StrLen $R3 $R1
  ;Start "StartCharPos" counter
  StrCpy $R4 0
 
  ;Loop until "SubString" is found or "String" reaches its end
  loop:
    ;Remove everything before and after the searched part ("TempStr")
    StrCpy $R5 $R1 $R2 $R4
 
    ;Compare "TempStr" with "SubString"
    StrCmp $R5 $R0 done
    ;If not "SubString", this could be "String"'s end
    IntCmp $R4 $R3 done 0 done
    ;If not, continue the loop
    IntOp $R4 $R4 + 1
    Goto loop
  done:
 
/*After this point:
  ------------------------------------------
  $R0 = ResultVar (output)*/
 
  ;Remove part before "SubString" on "String" (if there has one)
  StrCpy $R0 $R1 `` $R4
 
  ;Return output to user
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Exch $R0
FunctionEnd

Function InstallFolderInternal
  Pop $9
  !define Index 'Line${__LINE__}'
  FindFirst $0 $1 "$INSTDIR\$9\*"
  StrCmp $0 "" "${Index}-End"
  "${Index}-Loop:"
    StrCmp $1 "" "${Index}-End"
    StrCmp $1 "." "${Index}-Next"
    StrCmp $1 ".." "${Index}-Next"
    IfFileExists "$9\$1\*" 0 "${Index}-Write"
      Push $0
      Push $9
      Push "$9\$1"
      Call InstallFolderInternal
      Pop $9
      Pop $0
      Goto "${Index}-Next"
    "${Index}-Write:"
    FileWrite $UninstallLog "$9\$1$\r$\n"
    "${Index}-Next:"
    FindNext $0 $1
    Goto "${Index}-Loop"
  "${Index}-End:"
  !undef Index
FunctionEnd


;;;;;;;;;;;;;;;;;;;;;;
; Installer sections ;
;;;;;;;;;;;;;;;;;;;;;;

Section "${PRODUCT_NAME} (required)" SEC01
  SectionIn 1 2 3 RO
  SetShellVarContext all
  SetOutPath "$INSTDIR"

  !insertmacro OpenUninstallLog

  !insertmacro InstallFile ${BIN_PATH}\xulplayer.exe
  !insertmacro InstallFile ${BIN_PATH}\application.ini
  !insertmacro InstallFile ${BIN_PATH}\changelog.txt
  CreateDirectory $INSTDIR\playlists

  !insertmacro InstallFolder ${BIN_PATH}\chrome
  !insertmacro InstallFolder ${BIN_PATH}\bin
  !insertmacro InstallFolder ${BIN_PATH}\defaults

  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" \
    "${PRODUCT_WEB_SITE}"
  FileWrite $UninstallLog "${PRODUCT_NAME}.url$\r$\n"

  !insertmacro CloseUninstallLog
SectionEnd

!define XULRUNNER_PACKAGE "xulrunner-1.9.0.6.en-US.win32.zip"

Section "XULRunner" SEC01a
/*
  SectionIn 2
  ClearErrors
  NSISdl::download http://releases.mozilla.org/pub/mozilla.org/xulrunner/releases/1.9.0.6/runtimes/${XULRUNNER_PACKAGE} "$TEMP\${XULRUNNER_PACKAGE}"
  IfErrors 0 dounzip
  MessageBox MB_OK "Unable to download XULRunner package. $(^Name) might not be able to run without it. You can install Firefox 3 or manually download XULRunner and extract the files to the folder of XULPlayer."
  Goto skip
  dounzip:
  !insertmacro ZIPDLL_EXTRACT "$TEMP\${XULRUNNER_PACKAGE}" "$INSTDIR" "<ALL>"
  skip:
  Delete "$TEMP\${XULRUNNER_PACKAGE}"
  IfFileExists "$INSTDIR\xulrunner\xulrunner.exe" +2 0
  MessageBox MB_OK "XULRunner is not installed. $(^Name) might not be able to run without it.$\nThe solution is to install Firefox 3 or manually download XULRunner and extract the files to the folder of XULPlayer."
*/

  !insertmacro InstallFolder ${BIN_PATH}\..\..\xulrunner

SectionEnd

!define ENCODER_PACKAGE "xulplayer-encoders-pack-090304.zip"

Section "Video Encoders" SEC01b
  SectionIn 1 2
  ClearErrors
  NSISdl::download http://xulplayer.googlecode.com/files/${ENCODER_PACKAGE} "$TEMP\${ENCODER_PACKAGE}"
  IfErrors 0 dounzip
  MessageBox MB_OK "With encoders, you can not perform transcoding with XULPlayer."
  Goto skip
  dounzip:
  !insertmacro ZIPDLL_EXTRACT "$TEMP\${ENCODER_PACKAGE}" "$INSTDIR" "<ALL>"
  skip:
  Delete "$TEMP\${ENCODER_PACKAGE}"
SectionEnd

/*
Section "Flash Player Plugin" SEC01c
  SectionIn 2
  ClearErrors
  NSISdl::download http://fpdownload.macromedia.com/get/flashplayer/xpi/current/flashplayer-win.xpi "$TEMP\flashplayer.zip"
  IfErrors 0 dounzip
  MessageBox MB_OK "Unable to download Flash plugin."
  Goto skip
  dounzip:
  !insertmacro ZIPDLL_EXTRACT "$TEMP\flashplayer.zip" "$INSTDIR\bin" "NPSWF32.dll"
  skip:
  Delete "$TEMP\flashplayer.zip"
SectionEnd
*/

Section "Start Menu Shortcut" SEC02a
  SectionIn 1 2 3
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME} Web Site.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\xulplayer.exe"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Associate Playable Files.lnk" "$INSTDIR\FileAssociate.exe"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall ${PRODUCT_NAME}.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Desktop Shortcut" SEC02b
  SectionIn 1 2
  CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" \
    "$INSTDIR\xulplayer.exe" ""
SectionEnd

; Installer section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} \
    "The ${PRODUCT_NAME} itself"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01a} \
    "The XUL application runtime environment. You don't need to install it if Firefox 3 is installed already."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01b} \
    "Video encoders required by XULPlayer in order to perform transcoding for devices"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02a} \
    "Adds icons to your start menu for easy access"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02b} \
    "Adds icon to your desktop for easy access"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function .onInit
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "$(^Name)") i .r1 ?e'
	Pop $0
	StrCmp $0 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION "XULPlayer installer is already running"
		Quit
		
  ReadRegStr $R0  ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" \
  "UninstallString"
  StrCmp $R0 "" done
 
  MessageBox MB_YESNO|MB_ICONEXCLAMATION \
  "${PRODUCT_NAME} has already been installed. $\nDo you want to remove \
  the previous version before installing $(^Name) ?" \
  IDNO done
  
  ;Run the uninstaller
  ;uninst:
    ClearErrors
    ExecWait '$R0 _?=$INSTDIR' ;Do not copy the uninstaller to a temp file
  done:
  ;!insertmacro MUI_LANGDLL_DISPLAY

  ;detect Firefox 3
  ReadRegStr $0 HKLM "Software\Mozilla\Mozilla Firefox" "CurrentVersion"
  ${VerCmp} $0 $0 "3.0"
  IntCmp $0 0 hasff 0 hasff
  IntOp $1 ${SF_SELECTED} | ${SF_RO}
  SectionSetFlags ${SEC01a} $1
  ;SectionSetFlags ${SEC01b} $1
  goto doneff

  hasff:
  ;check flash player plugin
  IfFileExists "$windir\system32\NPSWF32.dll" doneff
  IfFileExists "$windir\SysWOW64\NPSWF32.dll" doneff
  SectionSetFlags ${SEC01b} ${SF_SELECTED}

  doneff:
  
  ClearErrors

FunctionEnd

Function .onInstSuccess
FunctionEnd

Function .onGUIEnd
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "InstallDir" $INSTDIR
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "Version" "${PRODUCT_VERSION}"
FunctionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninstall.exe"

  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" \
    "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" \
    "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" \
    "DisplayIcon" "$INSTDIR\xulplayer.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" \
    "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" \
    "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" \
    "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

;;;;;;;;;;;;;;;;;;;;;;;;
; Uninstaller sections ;
;;;;;;;;;;;;;;;;;;;;;;;;

; TrimNewlines (copied from NSIS documentation)
; input, top of stack  (e.g. whatever$\r$\n)
; output, top of stack (replaces, with e.g. whatever)
; modifies no other variables.

Function un.TrimNewlines
 Exch $R0
 Push $R1
 Push $R2
 StrCpy $R1 0
 
 loop:
   IntOp $R1 $R1 - 1
   StrCpy $R2 $R0 1 $R1
   StrCmp $R2 "$\r" loop
   StrCmp $R2 "$\n" loop
   IntOp $R1 $R1 + 1
   IntCmp $R1 0 no_trim_needed
   StrCpy $R0 $R0 $R1
 
 no_trim_needed:
   Pop $R2
   Pop $R1
   Exch $R0
FunctionEnd

Function un.RemoveEmptyDirs
  Pop $9
  !define Index 'Line${__LINE__}'
  FindFirst $0 $1 "$INSTDIR$9*"
  StrCmp $0 "" "${Index}-End"
  "${Index}-Loop:"
    StrCmp $1 "" "${Index}-End"
    StrCmp $1 "." "${Index}-Next"
    StrCmp $1 ".." "${Index}-Next"
      Push $0
      Push $1
      Push $9
      Push "$9$1\"
      Call un.RemoveEmptyDirs
      Pop $9
      Pop $1
      Pop $0
    "${Index}-Remove:"
    RMDir "$INSTDIR$9$1"
    "${Index}-Next:"
    FindNext $0 $1
    Goto "${Index}-Loop"
  "${Index}-End:"
  FindClose $0
  !undef Index
FunctionEnd

Section "Uninstall" SEC91
  SectionIn 1 2 3 RO
  SetShellVarContext all

  RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"

  FileOpen $UninstallLog "$INSTDIR\uninstall.log" r
  UninstallLoop:
    ClearErrors
    FileRead $UninstallLog $R0
    IfErrors UninstallEnd
    Push $R0
    Call un.TrimNewLines
    Pop $R0
    Delete "$INSTDIR\$R0"
    Goto UninstallLoop
  UninstallEnd:
  FileClose $UninstallLog

  Delete "$INSTDIR\uninstall.log"
  Delete "$INSTDIR\uninstall.exe"
  Push "\"
  Call un.RemoveEmptyDirs
  RMDir /r "$INSTDIR"

  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

; Uninstaller section descriptions
!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC91} \
    "Uninstall $(PRODUCT_NAME) and all its components"
!insertmacro MUI_UNFUNCTION_DESCRIPTION_END

;Function un.onUninstSuccess
;  HideWindow
;  MessageBox MB_ICONINFORMATION|MB_OK \
;    "$(^Name) was successfully removed from your computer."
;FunctionEnd

Function un.onInit
  ;!insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd
