@echo OFF
echo FTPCaptureOE Install
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
if EXIST "%~1\WildPackets\OmniPeek"   echo Installing FTPCaptureOE to %~1\WildPackets\OmniPeek
if EXIST "%~1\WildPackets\OmniPeek"   xcopy "%~2FTPCaptureOE\FTPCaptureOE.he" "%~1\WildPackets\OmniPeek\" /yrq
if EXIST "%~1\WildPackets\OmniPeek"   xcopy "%~2FTPCaptureOE\Win32\FTPCaptureOE.dll" "%~1\WildPackets\OmniPeek\" /yrq
goto :EOF

:installomniengine32
if EXIST "%~1\WildPackets\OmniEngine" echo Installing FTPCaptureOE to %~1\WildPackets\OmniEngine
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2FTPCaptureOE\FTPCaptureOE.he" "%~1\WildPackets\OmniEngine\" /yrq
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2FTPCaptureOE\Win32\FTPCaptureOE.dll" "%~1\WildPackets\OmniEngine\" /yrq
goto :EOF

:installomniengine64
if EXIST "%~1\WildPackets\OmniEngine" echo Installing FTPCaptureOE to %~1\WildPackets\OmniEngine
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2FTPCaptureOE\FTPCaptureOE.he" "%~1\WildPackets\OmniEngine\" /yrq
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2FTPCaptureOE\x64\FTPCaptureOE.dll" "%~1\WildPackets\OmniEngine\" /yrq
goto :EOF