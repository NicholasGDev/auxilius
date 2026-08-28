; Auxilius — NSIS Installer Script
; Generates a Windows .exe installer with Add/Remove Programs registration
; Requires: NSIS 3.x (https://nsis.sourceforge.io)
; Build: makensis installer\windows\auxilius.nsi

Unicode True

!define APP_NAME        "Auxilius"
!ifndef APP_VERSION
  !define APP_VERSION     "0.1.0"
!endif
!define APP_PUBLISHER   "NicholasGDev"
!define APP_URL         "https://github.com/NicholasGDev/auxilius"
!define APP_DESCRIPTION "Zeus Retail Evolution — Developer Toolbox"
!define UNINSTALL_KEY   "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
!define INSTALL_REG_KEY "Software\${APP_PUBLISHER}\${APP_NAME}"

Name "${APP_NAME} ${APP_VERSION}"
OutFile "..\..\dist\auxilius-setup-${APP_VERSION}.exe"
InstallDir "$LOCALAPPDATA\${APP_NAME}"
InstallDirRegKey HKCU "${INSTALL_REG_KEY}" "InstallDir"
RequestExecutionLevel user    ; no UAC — installs per-user
BrandingText "${APP_NAME} v${APP_VERSION} — ${APP_PUBLISHER}"

; ── UI ────────────────────────────────────────────────────────────────────────
!include "MUI2.nsh"

!define MUI_ABORTWARNING
!define MUI_ICON          "..\..\resources\icon.ico"
!define MUI_UNICON        "..\..\resources\icon.ico"
!define MUI_HEADERIMAGE
!define MUI_BGCOLOR       "1E1E1E"
!define MUI_TEXTCOLOR     "CCCCCC"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "PortugueseBR"

!include "FileFunc.nsh"

; ── Install ───────────────────────────────────────────────────────────────────
Section "Auxilius (obrigatório)" SecMain
  SectionIn RO

  SetOutPath "$INSTDIR"

  ; Copy all app files
  File /r "..\..\dist\win-unpacked\*.*"

  ; Write install location to registry
  WriteRegStr HKCU "${INSTALL_REG_KEY}" "InstallDir" "$INSTDIR"
  WriteRegStr HKCU "${INSTALL_REG_KEY}" "Version"    "${APP_VERSION}"

  ; Add/Remove Programs entry
  WriteRegStr   HKCU "${UNINSTALL_KEY}" "DisplayName"          "${APP_NAME} — ${APP_DESCRIPTION}"
  WriteRegStr   HKCU "${UNINSTALL_KEY}" "DisplayVersion"       "${APP_VERSION}"
  WriteRegStr   HKCU "${UNINSTALL_KEY}" "Publisher"            "${APP_PUBLISHER}"
  WriteRegStr   HKCU "${UNINSTALL_KEY}" "URLInfoAbout"         "${APP_URL}"
  WriteRegStr   HKCU "${UNINSTALL_KEY}" "InstallLocation"      "$INSTDIR"
  WriteRegStr   HKCU "${UNINSTALL_KEY}" "UninstallString"      '"$INSTDIR\Uninstall.exe"'
  WriteRegStr   HKCU "${UNINSTALL_KEY}" "QuietUninstallString" '"$INSTDIR\Uninstall.exe" /S'
  WriteRegStr   HKCU "${UNINSTALL_KEY}" "DisplayIcon"          "$INSTDIR\Auxilius.exe,0"
  WriteRegDWORD HKCU "${UNINSTALL_KEY}" "NoModify"             1
  WriteRegDWORD HKCU "${UNINSTALL_KEY}" "NoRepair"             1

  ; Estimate install size (KB)
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKCU "${UNINSTALL_KEY}" "EstimatedSize" "$0"

  ; Write uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Start Menu shortcut
  CreateDirectory "$SMPROGRAMS\${APP_NAME}"
  CreateShortcut  "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" \
                  "$INSTDIR\Auxilius.exe" "" "$INSTDIR\Auxilius.exe" 0
  CreateShortcut  "$SMPROGRAMS\${APP_NAME}\Desinstalar ${APP_NAME}.lnk" \
                  "$INSTDIR\Uninstall.exe"

  ; Desktop shortcut (optional — user can delete)
  CreateShortcut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\Auxilius.exe" \
                 "" "$INSTDIR\Auxilius.exe" 0

SectionEnd

; ── Uninstall ─────────────────────────────────────────────────────────────────
Section "Uninstall"

  ; Remove app files
  RMDir /r "$INSTDIR"

  ; Remove Start Menu shortcuts
  RMDir /r "$SMPROGRAMS\${APP_NAME}"

  ; Remove Desktop shortcut
  Delete "$DESKTOP\${APP_NAME}.lnk"

  ; Remove registry entries
  DeleteRegKey HKCU "${UNINSTALL_KEY}"
  DeleteRegKey HKCU "${INSTALL_REG_KEY}"

SectionEnd


