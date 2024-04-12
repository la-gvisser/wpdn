@echo OFF
echo EmailCaptureOE Install
if NOT "%ProgramFiles(x86)%" == "" call :X64INSTALL
if     "%ProgramFiles(x86)%" == "" call :Win32INSTALL
pause
goto :EOF

:Win32INSTALL
echo 32-bit OS detected
call :installomnipeek32   "%ProgramFiles%" "%~dp0"
call :installomniengine32 "%ProgramFiles%" "%~dp0"
goto :EOF

:X64INSTALL
echo 64-bit OS detected
call :installomnipeek32   "%ProgramFiles(x86)%" "%~dp0"
call :installomniengine32 "%ProgramFiles(x86)%" "%~dp0"
call :installomniengine64 "%ProgramFiles%"      "%~dp0"
goto :EOF

:installomnipeek32
if EXIST "%~1\WildPackets\OmniPeek"   echo Installing EmailCaptureOE to %~1\WildPackets\OmniPeek
if EXIST "%~1\WildPackets\OmniPeek"   xcopy "%~2EmailCaptureOE\EmailCaptureOE.he" "%~1\WildPackets\OmniPeek\" /yrq
if EXIST "%~1\WildPackets\OmniPeek"   xcopy "%~2EmailCaptureOE\Win32\EmailCaptureOE.dll" "%~1\WildPackets\OmniPeek\" /yrq
if EXIST "%~1\WildPackets\OmniPeek"   call :installcommon32 "%~1\Common Files\WildPackets" "%~2"
goto :EOF

:installomniengine32
if EXIST "%~1\WildPackets\OmniEngine" echo Installing EmailCaptureOE to %~1\WildPackets\OmniEngine
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2EmailCaptureOE\EmailCaptureOE.he" "%~1\WildPackets\OmniEngine\" /yrq
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2EmailCaptureOE\Win32\EmailCaptureOE.dll" "%~1\WildPackets\OmniEngine\" /yrq
if EXIST "%~1\WildPackets\OmniEngine" call :installcommon32 "%~1\Common Files\WildPackets" "%~2"
goto :EOF

:installomniengine64
if EXIST "%~1\WildPackets\OmniEngine" echo Installing EmailCaptureOE to %~1\WildPackets\OmniEngine
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2EmailCaptureOE\EmailCaptureOE.he" "%~1\WildPackets\OmniEngine\" /yrq
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2EmailCaptureOE\x64\EmailCaptureOE.dll" "%~1\WildPackets\OmniEngine\" /yrq
if EXIST "%~1\WildPackets\OmniEngine" call :installcommon64 "%~1\Common Files\WildPackets" "%~2"
goto :EOF

:installcommon32
if NOT EXIST "%~1" mkdir "%~1"
if EXIST "%~1" echo Installing mimepp.dll to %~1
if EXIST "%~1" xcopy "%~2EmailCaptureOE\Win32\mimepp.dll" "%~1\" /yrq
goto :EOF

:installcommon64
if NOT EXIST "%~1" mkdir "%~1"
if EXIST "%~1" echo Installing mimepp64.dll to %~1
if EXIST "%~1" xcopy "%~2EmailCaptureOE\x64\mimepp64.dll" "%~1\" /yrq
goto :EOF
