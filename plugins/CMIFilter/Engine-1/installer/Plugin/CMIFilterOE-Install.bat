@echo OFF
net stop omniengine
if NOT "%ProgramFiles(x86)%" == "" call :X64INSTALL
if     "%ProgramFiles(x86)%" == "" call :Win32INSTALL
net start omniengine
goto :EOF

:Win32INSTALL
echo 32-bit OS detected
call :installomnipeek32   "%ProgramFiles%" "%~dp0"
call :installomniengine32 "%ProgramFiles%" "%~dp0"
goto :EOF

:x64INSTALL
call :installomnipeek32   "%ProgramFiles(x86)%" "%~dp0"
call :installomniengine32 "%ProgramFiles(x86)%" "%~dp0"
call :installomniengine64 "%ProgramW6432%" "%~dp0"
goto :EOF

:installomnipeek32
if EXIST "%~1\WildPackets\OmniPeek" xcopy "%~2CMIFilterOE\CMIFilterOE.he" "%~1\WildPackets\OmniPeek\" /yrq
if EXIST "%~1\WildPackets\OmniPeek" xcopy "%~2CMIFilterOE\Win32\CMIFilterOE.dll" "%~1\WildPackets\OmniPeek\" /yrq
goto :EOF

:installomniengine32
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2CMIFilterOE\CMIFilterOE.he" "%~1\WildPackets\OmniEngine\" /yrq
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2CMIFilterOE\Win32\CMIFilterOE.dll" "%~1\WildPackets\OmniEngine\" /yrq
goto :EOF

:installomniengine64
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2CMIFilterOE\CMIFilterOE.he" "%~1\WildPackets\OmniEngine\" /yrq
if EXIST "%~1\WildPackets\OmniEngine" xcopy "%~2CMIFilterOE\x64\CMIFilterOE.dll" "%~1\WildPackets\OmniEngine\" /yrq
goto :EOF
