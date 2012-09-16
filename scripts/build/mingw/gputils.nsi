# gputils.nsi - NSIS installer script for gputils
#
# Copyright (c) 2004-2008 Borut Razem
#
# This file is part of gputils.
#
#  This software is provided 'as-is', without any express or implied
#  warranty.  In no event will the authors be held liable for any damages
#  arising from the use of this software.
#
#  Permission is granted to anyone to use this software for any purpose,
#  including commercial applications, and to alter it and redistribute it
#  freely, subject to the following restrictions:
#
#  1. The origin of this software must not be misrepresented; you must not
#     claim that you wrote the original software. If you use this software
#     in a product, an acknowledgment in the product documentation would be
#     appreciated but is not required.
#  2. Altered source versions must be plainly marked as such, and must not be
#     misrepresented as being the original software.
#  3. This notice may not be removed or altered from any source distribution.
#
#  Borut Razem
#  borut.razem@siol.net
#
# How to run NSIS installer:
#
#   "c:/Program Files/NSIS/makensis" -DVER_MAJOR=<VER_MAJOR> -DVER_MINOR=<VER_MINOR> -DVER_REVISION=<VER_REVISION> -DVER_BUILD=0 -DDASH_RELEASE="" gputils.nsi
#   replace <VER_XXX> with the appropriate values, for example for gputils 0.13.6:
#   <VER_MAJOR> = 0
#   <VER_MINOR> = 13
#   <VER_REVISION> = 6
#
# How to create WIN32 release setup.exe package on Windows+cygwin or Windows+mingw32 machine
#
# - download the release src package gputils-x.xx.x.tar.gz to a clean directory
#   (referenced as PKGDIR in continuation).
# - copy GPUTILS_SVN_SNAPSHOT/scripts/build/mingw/make-mingw to the PKGDIR
#   directory (GPUTILS_SVN_SNAPSHOT is the gputils Subversion snapshot directory).
# - copy GPUTILS_SVN_SNAPSHOT/scripts/build/mingw/gputils.nsi to the PKGDIR
#   directory
# - execute make-mingw from the PKGDIR directory using cygwin bash console
#   CC="gcc -mno-cygwin" ./make-mingw --email=<BUILDER_EMAIL> --name="<BUILDER_NAME>" --source=<SOURCE_PACKAGE_NAME> --save-temps
# - or execute make-mingw from the PKGDIR directory using MYSYS bash console
#   CC=gcc ./make-mingw --email=<BUILDER_EMAIL> --name="<BUILDER_NAME>" --source=<SOURCE_PACKAGE_NAME> --save-temps
#   where:
#   <BUILDER_EMAIL>       - builder'e e-mail address
#   <BUILDER_NAME>        - builder's name and surname
#   <SOURCE_PACKAGE_NAME> - gputils source package name, for example gputils-x.xx.x.tar.gz
# - The setup file gputils-x.xx.x.exe is created in PKGDIR/dist/gputils directory.
#   Upload it to gputils download repository at sourceforge.net
#
# For debugging define -DGPUTILS.DEBUG command line option

;--------------------------------
; Debugging Macros

!ifdef GPUTILS.DEBUG
  Var GPUTILS.FunctionName
  Var GPUTILS.StrStack0
  Var GPUTILS.StrStack1
  Var GPUTILS.StrStack2
  Var GPUTILS.StrStack3
  Var GPUTILS.StrStack4

!define GPUTILS.PushStr "!insertmacro MACRO_GPUTILS_PushStr"
!macro MACRO_GPUTILS_PushStr NAME
  StrCpy $GPUTILS.StrStack4 $GPUTILS.StrStack3
  StrCpy $GPUTILS.StrStack3 $GPUTILS.StrStack2
  StrCpy $GPUTILS.StrStack2 $GPUTILS.StrStack1
  StrCpy $GPUTILS.StrStack1 $GPUTILS.StrStack0
  StrCpy $GPUTILS.StrStack0 $GPUTILS.FunctionName
  StrCpy $GPUTILS.FunctionName "${NAME}"
!macroend

!define GPUTILS.PopStr "!insertmacro MACRO_GPUTILS_PopStr"
!macro MACRO_GPUTILS_PopStr
  StrCpy $GPUTILS.FunctionName $GPUTILS.StrStack0
  StrCpy $GPUTILS.StrStack0 $GPUTILS.StrStack1
  StrCpy $GPUTILS.StrStack1 $GPUTILS.StrStack2
  StrCpy $GPUTILS.StrStack2 $GPUTILS.StrStack3
  StrCpy $GPUTILS.StrStack3 $GPUTILS.StrStack4
!macroend
!endif

!define DebugMsg "!insertmacro MACRO_GPUTILS_DebugMsg"
!macro MACRO_GPUTILS_DebugMsg MSG
  !ifdef GPUTILS.DEBUG
    MessageBox MB_OK "*** $GPUTILS.FunctionName: ${MSG} ***"
  !endif
!macroend

!define Function "!insertmacro MACRO_GPUTILS_Function"
!macro MACRO_GPUTILS_Function NAME
  Function "${NAME}"
  !ifdef GPUTILS.DEBUG
    ${GPUTILS.PushStr} ${NAME}
  !endif
!macroend

!define FunctionEnd "!insertmacro MACRO_GPUTILS_FunctionEnd"
!macro MACRO_GPUTILS_FunctionEnd
  !ifdef GPUTILS.DEBUG
    ${GPUTILS.PopStr}
  !endif
  FunctionEnd
!macroend

!define Section "!insertmacro MACRO_GPUTILS_Section"
!macro MACRO_GPUTILS_Section NAME ID
  Section "${NAME}" "${ID}"
  !ifdef GPUTILS.DEBUG
    ${GPUTILS.PushStr} "${NAME}"
  !endif
!macroend

!define UnselectedSection "!insertmacro MACRO_GPUTILS_UnselectedSection"
!macro MACRO_GPUTILS_UnselectedSection NAME ID
  Section /o ${NAME} ${ID}
  !ifdef GPUTILS.DEBUG
    ${GPUTILS.PushStr} "${NAME}"
  !endif
!macroend

!define SectionEnd "!insertmacro MACRO_GPUTILS_SectionEnd"
!macro MACRO_GPUTILS_SectionEnd
  !ifdef GPUTILS.DEBUG
    ${GPUTILS.PopStr}
  !endif
  SectionEnd
!macroend


!define PRODUCT_NAME "gputils"

; Version
!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
  !define PRODUCT_VERSION "${VER_MAJOR}.${VER_MINOR}.${VER_REVISION}"
!else
  !define PRODUCT_VERSION "XX.XX"
!endif

SetCompressor /SOLID lzma

!define GPUTILS_ROOT "."

!define DEV_ROOT "${GPUTILS_ROOT}"

InstType "Full (Bin, Hdr, Lkr, Doc, HTML help)"
InstType "Compact (Bin, Doc)"

;--------------------------------
; Configuration

!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define UNINST_ROOT_KEY HKLM
!define ROOT_KEY HKLM

;--------------------------------
; Header Files

!include "MUI2.nsh"
!include WordFunc.nsh
!include StrFunc.nsh
${StrStr}
${UnStrStr}

;--------------------------------
; Functions

!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
  !insertmacro VersionCompare
!endif

;--------------------------------
; Variables

Var GPUTILS.PathToRemove

;--------------------------------
; Configuration

; MUI Settings
!define MUI_ABORTWARNING

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE "${GPUTILS_ROOT}\COPYING.txt"

; Uninstall/reinstall page
!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
Page custom GPUTILS.PageReinstall GPUTILS.PageLeaveReinstall
!endif

; StartMenu page
!define MUI_STARTMENUPAGE_DEFAULTFOLDER ${PRODUCT_NAME}
!define MUI_STARTMENUPAGE_REGISTRY_ROOT ${UNINST_ROOT_KEY}
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "NSIS:StartMenuDir"
!define MUI_STARTMENUPAGE_NODISABLE
Var MUI_STARTMENUPAGE_VARIABLE
!insertmacro MUI_PAGE_STARTMENU Application $MUI_STARTMENUPAGE_VARIABLE

; Components page
!define MUI_COMPONENTSPAGE_SMALLDESC
!insertmacro MUI_PAGE_COMPONENTS

; Directory page
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE "GPUTILS.InstFilesLeave"
!insertmacro MUI_PAGE_INSTFILES

${Function} GPUTILS.InstFilesLeave
  ; Remove old path if reinstallation
  ${If} $GPUTILS.PathToRemove != ""
    ${DebugMsg} "removing path $GPUTILS.PathToRemove"
    Push $GPUTILS.PathToRemove
    Call GPUTILS.RemoveFromPath
  ${EndIf}
${FunctionEnd}

; Finish page - add to path
!define MUI_FINISHPAGE_TEXT "Confirm the checkbox if you want to add GPUTILS binary directory to the PATH environment variable"
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Add $INSTDIR\bin to the PATH"
!define MUI_FINISHPAGE_SHOWREADME_FUNCTION GPUTILS.AddBinToPath
!define MUI_FINISHPAGE_SHOWREADME
!define MUI_FINISHPAGE_BUTTON "Next"
!insertmacro MUI_PAGE_FINISH

; Finish page - reboot
!insertmacro MUI_PAGE_FINISH

${Function} GPUTILS.AddBinToPath
  ; Add new path
  ${DebugMsg} "adding path $INSTDIR\bin"
  Push "$INSTDIR\bin"
  Call GPUTILS.AddToPath
${FunctionEnd}

; Uninstaller pages
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
BrandingText ""
OutFile "${PRODUCT_NAME}-${PRODUCT_VERSION}${DASH_RELEASE}.exe"
InstallDir "$PROGRAMFILES\gputils"

${Function} .onInit
  ${DebugMsg} "Pre INSTDIR = $INSTDIR"

!ifndef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
  ; Old unistallation method
  ; Uninstall the old version, if present
  ReadRegStr $R0 ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  StrCmp $R0 "" inst

  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
  "$(^Name) is already installed. $\n$\nClick 'OK' to remove the \
  previous version or 'Cancel' to cancel this upgrade." \
  IDOK uninst
  Abort

uninst:
  ; Run the uninstaller
  ClearErrors
  ExecWait '$R0'

  Goto done
inst:

  ; Install the new version
  MessageBox MB_YESNO|MB_ICONQUESTION "This will install $(^Name). Do you wish to continue?" IDYES +2
  Abort

done:
!else
  ; If the registry key exists it is an uninstallation or reinstallation:
  ;  take the old installation directory
  Push $R0

  ReadRegStr $R0 ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallLocation"
  ${IfNot} ${Errors}
    StrCpy $INSTDIR $R0
    StrCpy $GPUTILS.PathToRemove "$INSTDIR\bin"
  ${EndIf}

  Pop $R0
!endif
  ${DebugMsg} "Post INSTDIR = $INSTDIR"
${FunctionEnd}

${Function} un.onInit

  ${DebugMsg} "Pre INSTDIR = $INSTDIR"

  Push $R0
  ReadRegStr $R0 ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallLocation"
  ${IfNot} ${Errors}
    StrCpy $INSTDIR $R0
  ${EndIf}
  Pop $R0

  ${DebugMsg} "Post INSTDIR = $INSTDIR"

${FunctionEnd}

${Section} -Common SECCOMMON
  SetOutPath "$INSTDIR"
  File "${GPUTILS_ROOT}\COPYING.txt"
  File "${GPUTILS_ROOT}\ChangeLog.txt"
  File "${GPUTILS_ROOT}\README.txt"
  File "${GPUTILS_ROOT}\AUTHORS.txt"
  File "${GPUTILS_ROOT}\DISTRIBUTION.txt"
  File "${GPUTILS_ROOT}\NEWS.txt"
${SectionEnd}

${Section} "gputils application files" SEC01
  SectionIn 1 2 RO
  SetOutPath "$INSTDIR\bin"
  File "${GPUTILS_ROOT}\bin\gpasm.exe"
  File "${GPUTILS_ROOT}\bin\gpdasm.exe"
  File "${GPUTILS_ROOT}\bin\gplink.exe"
  File "${GPUTILS_ROOT}\bin\gplib.exe"
  File "${GPUTILS_ROOT}\bin\gpstrip.exe"
  File "${GPUTILS_ROOT}\bin\gpvc.exe"
  File "${GPUTILS_ROOT}\bin\gpvo.exe"
${SectionEnd}

${Section} "gputils header files" SEC02
  SectionIn 1
  SetOutPath "$INSTDIR\header"
  File "${GPUTILS_ROOT}\header\*.inc"

  ; Set GPUTILS_HEADER_PATH environment variable
  ${DebugMsg} "defining environment variable GPUTILS_HEADER_PATH=$INSTDIR\header"
  Push "GPUTILS_HEADER_PATH"  ; name
  Push "$INSTDIR\header"      ; value
  Call GPUTILS.WriteEnvStr
${SectionEnd}

${Section} "gputils lkr files" SEC03
  SectionIn 1
  SetOutPath "$INSTDIR\lkr"
  File "${GPUTILS_ROOT}\lkr\*.lkr"

  ; Set GPUTILS_LKR_PATH environment variable
  ${DebugMsg} "defining environment variable GPUTILS_LKR_PATH=$INSTDIR\lkr"
  Push "GPUTILS_LKR_PATH"  ; name
  Push "$INSTDIR\lkr"      ; value
  Call GPUTILS.WriteEnvStr
${SectionEnd}

${Section} "gputils documentation" SEC04
  SectionIn 1 2
  SetOutPath "$INSTDIR\doc"
  File "${GPUTILS_ROOT}\doc\gputils.pdf"
${SectionEnd}

${Section} "gputils devices HTML help" SEC05
  SectionIn 1 2
  SetOutPath "$INSTDIR\doc\html-help"
;  File "${GPUTILS_ROOT}}\doc\html-help\*.html"
  File "${GPUTILS_ROOT}\doc\html-help\*.css"
${SectionEnd}

;--------------------------------
;Descriptions

;Language strings
LangString DESC_SEC01 ${LANG_ENGLISH} "gputils application files"
LangString DESC_SEC02 ${LANG_ENGLISH} "gputils header files"
LangString DESC_SEC03 ${LANG_ENGLISH} "gputils lkr files"
LangString DESC_SEC04 ${LANG_ENGLISH} "gputils documentation"
LangString DESC_SEC05 ${LANG_ENGLISH} "gputils devices HTML help"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} $(DESC_SEC01)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} $(DESC_SEC02)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC03} $(DESC_SEC03)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} $(DESC_SEC04)
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} $(DESC_SEC05)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
;--------------------------------

${Section} -Icons SECICONS
!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE"
  CreateShortCut "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\Uninstall gputils.lnk" "$INSTDIR\uninstall.exe"
  CreateShortCut "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\Documentation.lnk" "$INSTDIR\doc\gputils.pdf"
  CreateShortCut "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\Devices HTML help.lnk" "$INSTDIR\doc\html-help\index.html"
  CreateShortCut "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\README.lnk" "$INSTDIR\README.TXT"
  CreateShortCut "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\Change Log.lnk" "$INSTDIR\ChangeLog.txt"
  CreateShortCut "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\GPL 2 License.lnk" "$INSTDIR\COPYING.txt"
!insertmacro MUI_STARTMENU_WRITE_END
${SectionEnd}

${Section} -INI SECINI
  WriteIniStr "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\gputils on the Web.url "InternetShortcut" "URL" "http://gputils.sourceforge.net/"
${SectionEnd}

${Section} -PostInstall SECPOSTINSTALL
  WriteRegStr ${ROOT_KEY} "Software\${PRODUCT_NAME}" "" $INSTDIR
!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
  WriteRegDword ${ROOT_KEY} "Software\${PRODUCT_NAME}" "VersionMajor" "${VER_MAJOR}"
  WriteRegDword ${ROOT_KEY} "Software\${PRODUCT_NAME}" "VersionMinor" "${VER_MINOR}"
  WriteRegDword ${ROOT_KEY} "Software\${PRODUCT_NAME}" "VersionRevision" "${VER_REVISION}"
  WriteRegDword ${ROOT_KEY} "Software\${PRODUCT_NAME}" "VersionBuild" "${VER_BUILD}"
!endif

  WriteRegExpandStr ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegExpandStr ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "gputils.sourceforge.net"
  WriteRegStr ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "http://gputils.sourceforge.net/"
  WriteRegStr ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "HelpLink" "http://gputils.sourceforge.net/"
  WriteRegStr ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLUpdateInfo" "http://gputils.sourceforge.net/"

  WriteUninstaller "$INSTDIR\uninstall.exe"
${SectionEnd}


;;;; Uninstaller code ;;;;

${Section} Uninstall SECUNINSTALL
  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_STARTMENUPAGE_VARIABLE

  ${DebugMsg} "removing path $INSTDIR\bin"
  Push "$INSTDIR\bin"
  Call un.GPUTILS.RemoveFromPath

; Clean the registry
  DeleteRegKey ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey ${ROOT_KEY} "Software\${PRODUCT_NAME}"

  Delete "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\GPL 2 License.lnk"
  Delete "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\Change Log.lnk"
  Delete "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\README.lnk"
  Delete "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\Documentation.lnk"
  Delete "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\Devices HTML help.lnk"
  Delete "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\Uninstall gputils.lnk"
  Delete "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE\gputils on the Web.url"

  RMDir "$SMPROGRAMS\$MUI_STARTMENUPAGE_VARIABLE"

  ; Remove environment variables
  ${DebugMsg} "removing environment variable GPUTILS_HEADER_PATH"
  Push "GPUTILS_HEADER_PATH"  ; name
  Call un.GPUTILS.DeleteEnvStr
  ${DebugMsg} "removing environment variable GPUTILS_LKR_PATH"
  Push "GPUTILS_LKR_PATH"     ; name
  Call un.GPUTILS.DeleteEnvStr

  Delete "$INSTDIR\bin\gpasm.exe"
  Delete "$INSTDIR\bin\gpdasm.exe"
  Delete "$INSTDIR\bin\gplink.exe"
  Delete "$INSTDIR\bin\gplib.exe"
  Delete "$INSTDIR\bin\gpstrip.exe"
  Delete "$INSTDIR\bin\gpvc.exe"
  Delete "$INSTDIR\bin\gpvo.exe"
  RMDir "$INSTDIR\bin"

  Delete "$INSTDIR\lkr\*.lkr"
  RMDir "$INSTDIR\lkr"

  Delete "$INSTDIR\header\*.inc"
  RMDir "$INSTDIR\header"

  Delete "$INSTDIR\doc\*.pdf"
  RMDir "$INSTDIR\doc"

  Delete "$INSTDIR\doc\html-help\*.html"
  Delete "$INSTDIR\doc\html-help\*.css"
  RMDir "$INSTDIR\doc\html-help"

  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\ChangeLog.txt"
  Delete "$INSTDIR\README.txt"
  Delete "$INSTDIR\AUTHORS.txt"
  Delete "$INSTDIR\COPYING.txt"
  Delete "$INSTDIR\DISTRIBUTION.txt"
  Delete "$INSTDIR\NEWS.txt"
  RMDir "$INSTDIR"

  SetAutoClose true
${SectionEnd}

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Path Manipulation functions                                                 ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

!verbose 3
!include "WinMessages.nsh"
!verbose 4

!ifdef ALL_USERS
  !define WriteEnvStr_RegKey \
     'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
!else
  !define WriteEnvStr_RegKey 'HKCU "Environment"'
!endif

; WriteEnvStr - Writes an environment variable
; Note: Win9x systems requires reboot
;
; Example:
;  Push "HOMEDIR"           ; name
;  Push "C:\New Home Dir\"  ; value
;  Call WriteEnvStr

${Function} GPUTILS.WriteEnvStr
  Exch $1 ; $1 has environment variable value
  Exch
  Exch $0 ; $0 has environment variable name
  Push $2

  Call GPUTILS.IsNT
  Pop $2
  ${If} $2 != 1
    ; Not on NT
    StrCpy $2 $WINDIR 2 ; Copy drive of windows (c:)
    FileOpen $2 "$2\autoexec.bat" a
    Push $2
    Push "SET $0=$1"
    Call AppendToHandle
    FileClose $2
    SetRebootFlag true
  ${Else}
    WriteRegExpandStr ${WriteEnvStr_RegKey} $0 $1
    SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  ${EndIf}

  Pop $2
  Pop $1
  Pop $0
${FunctionEnd}

; un.GPUTILS.DeleteEnvStr - Removes an environment variable
; Note: Win9x systems requires reboot
;
; Example:
;  Push "HOMEDIR"           # name
;  Call un.GPUTILS.DeleteEnvStr

${Function} un.GPUTILS.DeleteEnvStr
  Exch $0 ; $0 now has the name of the variable
  Push $1
  Push $2
  Push $3
  Push $4
  Push $5

  Call un.GPUTILS.IsNT
  Pop $1
  ${If} $1 != 1
    ; Not on NT
    StrCpy $1 $WINDIR 2
    FileOpen $1 "$1\autoexec.bat" r
    GetTempFileName $4
    FileOpen $2 $4 w
    StrCpy $0 "SET $0="
    SetRebootFlag true

    DeleteEnvStr_dosLoop:
      FileRead $1 $3
      StrLen $5 $0
      StrCpy $5 $3 $5
      StrCmp $5 $0 DeleteEnvStr_dosLoop
      StrCmp $5 "" DeleteEnvStr_dosLoopEnd
      FileWrite $2 $3
      Goto DeleteEnvStr_dosLoop

    DeleteEnvStr_dosLoopEnd:
      FileClose $2
      FileClose $1
      StrCpy $1 $WINDIR 2
      Delete "$1\autoexec.bat"
      CopyFiles /SILENT $4 "$1\autoexec.bat"
      Delete $4
  ${Else}
    DeleteRegValue ${WriteEnvStr_RegKey} $0
    SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  ${EndIf}

  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Pop $0
${FunctionEnd}

; AddToPath - Adds the given dir to the search path.
;        Input - head of the stack
;        Note - Win9x systems requires reboot

${Function} GPUTILS.AddToPath
  Exch $0
  Push $1
  Push $2
  Push $3
  Push $4

  ; don't add if the path doesn't exist
  ${If} ${FileExists} $0
    Call GPUTILS.IsNT
    Pop $4
    ${If} $4 != 1
      ; Not on NT: read PATH from environment variable
      ReadEnvStr $1 PATH
    ${Else}
      ; On NT: read PATH from registry
      ReadRegStr $1 HKCU "Environment" "PATH"
    ${EndIf}

    ${StrStr} $2 "$1;" "$0;"
    ${If} $2 == ""
      ${StrStr} $2 "$1;" "$0\;"
      ${If} $2 == ""
        GetFullPathName /SHORT $3 $0
        ${StrStr} $2 "$1;" "$3;"
        ${If} $2 == ""
          ${StrStr} $2 "$1;" "$03\;"
          ${If} $2 == ""
            ${If} $4 != 1
              ; Not on NT
              StrCpy $1 $WINDIR 2
              FileOpen $1 "$1\autoexec.bat" a
              FileSeek $1 -1 END
              FileReadByte $1 $2
              ${If} $2 = 26        ; DOS EOF
                FileSeek $1 -1 END ; write over EOF
              ${Endif}
              ${DebugMsg} "adding line $\r$\nSET PATH=%PATH%;$3$\r$\n"
              FileWrite $1 "$\r$\nSET PATH=%PATH%;$3$\r$\n"
              FileClose $1
              ${DebugMsg} "SetRebootFlag true"
              SetRebootFlag true
            ${Else}
              ;System PATH variable is at:
              ;HKLM "/SYSTEM/CurrentControlSet/Control/Session Manager/Environment" "Path"
              ReadRegStr $1 HKCU "Environment" "PATH"
              StrCpy $2 $1 1 -1  ; copy last char
              ${If} $2 == ";"    ; if last char == ;
                StrCpy $1 $1 -1  ; remove last char
              ${Endif}
              ${If} $1 != ""
                StrCpy $0 "$1;$0"
              ${Endif}
              WriteRegExpandStr HKCU "Environment" "PATH" $0
              SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
            ${Endif}
          ${Endif}
        ${Endif}
      ${Endif}
    ${Endif}
  ${EndIf}

  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Pop $0
${FunctionEnd}

; RemoveFromPath - Remove a given dir from the path
;     Input: head of the stack

!macro GPUTILS.RemoveFromPath un
${Function} ${un}GPUTILS.RemoveFromPath
  Exch $0
  Push $1
  Push $2
  Push $3
  Push $4
  Push $5
  Push $6

  IntFmt $6 "%c" 26 ; DOS EOF

  Call ${un}GPUTILS.IsNT
  Pop $1
  ${If} $1 != 1
    ; Not on NT
    StrCpy $1 $WINDIR 2
    FileOpen $1 "$1\autoexec.bat" r
    GetTempFileName $4
    FileOpen $2 $4 w
    GetFullPathName /SHORT $0 $0
    StrCpy $0 "SET PATH=%PATH%;$0"

  nextLine:
    ; copy all lines except the line containing "SET PATH=%PATH%;$0"
    ; from autoexec.bat to the temporary file
    ClearErrors
    FileRead $1 $3
    ${IfNot} ${Errors}
      StrCpy $5 $3 1 -1 ; read last char
      ${If} $5 == $6    ; if DOS EOF
        StrCpy $3 $3 -1 ; remove DOS EOF so we can compare
      ${EndIf}
      ${If} $3 != "$0$\r$\n"
        ${AndIf} $3 != "$0$\n"
        ${AndIf} $3 != "$0"
        FileWrite $2 $3
        Goto nextLine
      ${Else}
        ; This is the line I'm looking for:
        ; don't copy it
        ${DebugMsg} "removing line $0"
        ${DebugMsg} "SetRebootFlag true"
        SetRebootFlag true
        Goto nextLine
      ${EndIf}
    ${EndIf}

    FileClose $2
    FileClose $1
    StrCpy $1 $WINDIR 2
    Delete "$1\autoexec.bat"
    CopyFiles /SILENT $4 "$1\autoexec.bat"
    Delete $4
  ${Else}
    ;System PATH variable is at:
    ;HKLM "/SYSTEM/CurrentControlSet/Control/Session Manager/Environment" "Path"
    ReadRegStr $1 HKCU "Environment" "PATH"
    StrCpy $5 $1 1 -1 ; copy last char
    ${If} $5 != ";"   ; if last char != ;
      StrCpy $1 "$1;" ; append ;
    ${EndIf}
    Push $1
    Push "$0;"
    Call ${un}StrStr  ; Find `$0;` in $1
    Pop $2            ; pos of our dir
    ${If} $2 != ""
      ; it is in path:
      ; $0 - path to add
      ; $1 - path var
      StrLen $3 "$0;"
      StrLen $4 $2
      StrCpy $5 $1 -$4   ; $5 is now the part before the path to remove
      StrCpy $6 $2 "" $3 ; $6 is now the part after the path to remove
      StrCpy $3 $5$6

      StrCpy $5 $3 1 -1  ; copy last char
      ${If} $5 == ";"    ; if last char == ;
        StrCpy $3 $3 -1  ; remove last char
      ${EndIf}
      ${If} $3 != ""
        ; New PATH not empty: update the registry
        WriteRegExpandStr HKCU "Environment" "PATH" $3
      ${Else}
        ; New PATH empty: remove from the registry
        DeleteRegValue HKCU "Environment" "PATH"
      ${EndIf}
      SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
    ${Endif}
  ${Endif}

  Pop $6
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Pop $0
${FunctionEnd}
!macroend
!insertmacro GPUTILS.RemoveFromPath ""
!insertmacro GPUTILS.RemoveFromPath "un."

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;            Utility Functions            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; AppendToHandle
;
; Example:
;  Push $1                ; file handle
;  Push "Line to append"  ; line
;  Call AppendToHandle

!macro AppendToHandle
Function AppendToHandle
  Exch $1 ; $1 line
  Exch
  Exch $0 ; $0 file handle
  Push $2 ; Last char EOF

  FileSeek $0 -1 END
  FileReadByte $0 $2
  IntCmp $2 26 0 +2 +2 ; DOS EOF
    FileSeek $0 -1 END ; write over EOF
  FileWrite $0 "$\r$\n$1"

  Pop $2
  Pop $1
  Pop $0
FunctionEnd
!macroend
!insertmacro AppendToHandle

; IsNT
; no input
; output, top of the stack = 1 if NT or 0 if not
;
; Usage:
;   Call IsNT
;   Pop $R0
;  ($R0 at this point is 1 or 0)

!macro GPUTILS.IsNT un
${Function} ${un}GPUTILS.IsNT
  Push $R0
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  ${If} $R0 == ""
    ; we are not NT.
    Pop $R0
    Push 0
  ${Else}
    ; NT!!!
    Pop $R0
    Push 1
  ${EndIf}
${FunctionEnd}
!macroend
!insertmacro GPUTILS.IsNT ""
!insertmacro GPUTILS.IsNT "un."

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Uninstall/Reinstall page functions                                         ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

!ifdef VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD

Var ReinstallPageCheck

${Function} GPUTILS.PageReinstall

  ReadRegStr $R0 ${ROOT_KEY} "Software\${PRODUCT_NAME}" ""

  ${If} $R0 == ""
    ReadRegStr $R0 ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
    ${If} $R0 == ""
      Abort
    ${EndIf}
  ${EndIf}

  ReadRegDWORD $R0 ${ROOT_KEY} "Software\${PRODUCT_NAME}" "VersionMajor"
  ReadRegDWORD $R1 ${ROOT_KEY} "Software\${PRODUCT_NAME}" "VersionMinor"
  ReadRegDWORD $R2 ${ROOT_KEY} "Software\${PRODUCT_NAME}" "VersionRevision"
  ReadRegDWORD $R3 ${ROOT_KEY} "Software\${PRODUCT_NAME}" "VersionBuild"
  StrCpy $R0 $R0.$R1.$R2.$R3

  ${VersionCompare} ${VER_MAJOR}.${VER_MINOR}.${VER_REVISION}.${VER_BUILD} $R0 $R0
  ${If} $R0 == 0
    StrCpy $R1 "${PRODUCT_NAME} ${PRODUCT_VERSION} is already installed. Select the operation you want to perform and click Next to continue."
    StrCpy $R2 "Add/Reinstall components"
    StrCpy $R3 "Uninstall ${PRODUCT_NAME}"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose the maintenance option to perform."
    StrCpy $R0 "2"
  ${ElseIf} $R0 == 1
    StrCpy $R1 "An older version of ${PRODUCT_NAME} is installed on your system. It's recommended that you uninstall the current version before installing. Select the operation you want to perform and click Next to continue."
    StrCpy $R2 "Uninstall before installing"
    StrCpy $R3 "Do not uninstall"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose how you want to install ${PRODUCT_NAME}."
    StrCpy $R0 "1"
  ${ElseIf} $R0 == 2
    StrCpy $R1 "A newer version of ${PRODUCT_NAME} is already installed! It is not recommended that you install an older version. If you really want to install this older version, it's better to uninstall the current version first. Select the operation you want to perform and click Next to continue."
    StrCpy $R2 "Uninstall before installing"
    StrCpy $R3 "Do not uninstall"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose how you want to install ${PRODUCT_NAME}."
    StrCpy $R0 "1"
  ${Else}
    Abort
  ${EndIf}

  nsDialogs::Create /NOUNLOAD 1018

  ${NSD_CreateLabel} 0 0 100% 24u $R1
  Pop $R1

  ${NSD_CreateRadioButton} 30u 50u -30u 8u $R2
  Pop $R2
  ${NSD_OnClick} $R2 GPUTILS.PageReinstallUpdateSelection

  ${NSD_CreateRadioButton} 30u 70u -30u 8u $R3
  Pop $R3
  ${NSD_OnClick} $R3 GPUTILS.PageReinstallUpdateSelection

  ${If} $ReinstallPageCheck != 2
    SendMessage $R2 ${BM_SETCHECK} ${BST_CHECKED} 0
  ${Else}
    SendMessage $R3 ${BM_SETCHECK} ${BST_CHECKED} 0
  ${EndIf}

  nsDialogs::Show

${FunctionEnd}

${Function} GPUTILS.PageReinstallUpdateSelection

  Pop $R1

  ${NSD_GetState} $R2 $R1

  ${If} $R1 == ${BST_CHECKED}
    StrCpy $ReinstallPageCheck 1
  ${Else}
    StrCpy $ReinstallPageCheck 2
  ${EndIf}

${FunctionEnd}

${Function} GPUTILS.PageLeaveReinstall

  ${NSD_GetState} $R2 $R1

  ${DebugMsg} "R0 = $R0, R1 = $R1, R2 = $R2"

  ${If} $R0 == 1
    ${AndIf} $R1 != 1
    Goto reinst_done
  ${EndIf}

  ${If} $R0 == 2
    ${AndIf} $R1 == 1
    Goto reinst_done
  ${EndIf}

  ReadRegStr $R1 ${UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"

  ;Run uninstaller
  HideWindow

  ${If} $R0 == "2"
    ; Uninstall only: uninstaller should be removed
    ClearErrors
    ; ExecWait doesn't wait if _?=$INSTDIR is not defined!
    ExecWait '$R1'
    Quit
  ${Else}
    ; Uninstal & Reinstall: uninstaller will be rewritten
    ClearErrors
    ; ExecWait doesn't wait if _?=$INSTDIR is not defined!
    ExecWait '$R1 _?=$INSTDIR'
  ${EndIf}

  BringToFront

reinst_done:

${FunctionEnd}

!endif # VER_MAJOR & VER_MINOR & VER_REVISION & VER_BUILD
