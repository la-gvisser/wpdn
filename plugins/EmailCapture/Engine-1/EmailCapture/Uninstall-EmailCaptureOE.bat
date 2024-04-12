@echo OFF
echo %cd%
if NOT "%ProgramFiles(x86)%" == "" call :X64UNINSTALL
if     "%ProgramFiles(x86)%" == "" call :Win32UNINSTALL
goto :EOF

:Win32UNINSTALL
echo in Win32 uninstall
call :uninstallomnipeek32   "%ProgramFiles%"
call :uninstallomniengine32 "%ProgramFiles%"
goto :EOF

:X64UNINSTALL
echo in x64 uninstall
call :uninstallomnipeek32   "%ProgramFiles(x86)%"
call :uninstallomniengine32 "%ProgramFiles(x86)%"
call :uninstallomniengine64 "%ProgramFiles%"
goto :EOF

:uninstallomnipeek32
echo in installomnipeek32 %~1
if EXIST "%~1\WildPackets\OmniPeek\EmailCaptureOE.he"   del "%~1\WildPackets\OmniPeek\EmailCaptureOE.he"
if EXIST "%~1\WildPackets\OmniPeek\EmailCaptureOE.dll"  del "%~1\WildPackets\OmniPeek\EmailCaptureOE.dll"
if EXIST "%~1\WildPackets\OmniPeek" call :uninstallcommon32 "%~1\Common Files\WildPackets"
goto :EOF

:uninstallomniengine32
echo in installomniengine32 %~1
if EXIST "%~1\WildPackets\OmniEngine\EmailCaptureOE.he"  del "%~1\WildPackets\OmniEngine\EmailCaptureOE.he"
if EXIST "%~1\WildPackets\OmniEngine\EmailCaptureOE.dll" del "%~1\WildPackets\OmniEngine\EmailCaptureOE.dll"
if EXIST "%~1\WildPackets\OmniEngine" call :uninstallcommon32 "%~1\Common Files\WildPackets"
goto :EOF

:uninstallomniengine64
echo in installomniengine64 %~1
if EXIST "%~1\WildPackets\OmniEngine\EmailCaptureOE.he"  del "%~1\WildPackets\OmniEngine\EmailCaptureOE.he"
if EXIST "%~1\WildPackets\OmniEngine\EmailCaptureOE.dll" del "%~1\WildPackets\OmniEngine\EmailCaptureOE.dll"
if EXIST "%~1\WildPackets\OmniEngine" call :uninstallcommon64 "%~1\Common Files\WildPackets"
goto :EOF

:uninstallcommon32
if EXIST "%~1\mimepp.dll"   del "%~1\mimepp.dll"
goto :EOF

:uninstallcommon64
if EXIST "%~1\mimepp64.dll" del "%~1\mimepp64.dll"
goto :EOF
