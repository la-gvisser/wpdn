@echo OFF
echo CMIFilterOE Install
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
call :installomnipeek64   "%ProgramFiles%"      "%~dp0"
call :installomniengine32 "%ProgramFiles(x86)%" "%~dp0"
call :installomniengine64 "%ProgramFiles%"      "%~dp0"
goto :EOF

:installomnipeek32
if NOT EXIST "%~1\Savvius\OmniPeek" goto :EOF
echo Installing CMIFilterOE 32-bit to %~1\Savvius\OmniPeek
if NOT EXIST "%~1\Savvius\OmniPeek\Engine Plugins" mkdir "%~1\Savvius\OmniPeek\Engine Plugins"
if EXIST "%~1\Savvius\OmniPeek\Engine Plugins" xcopy "%~2CMIFilterOE\CMIFilterOE.he" "%~1\Savvius\OmniPeek\Engine Plugins\" /yrq
if EXIST "%~1\Savvius\OmniPeek\Engine Plugins" xcopy "%~2CMIFilterOE\Win32\CMIFilterOE.dll" "%~1\Savvius\OmniPeek\Engine Plugins\" /yrq
goto :EOF

:installomnipeek64
if NOT EXIST "%~1\Savvius\OmniPeek" goto :EOF
echo Installing CMIFilterOE 64-bit to %~1\Savvius\OmniPeek
if NOT EXIST "%~1\Savvius\OmniPeek\Engine Plugins\" mkdir "%~1\Savvius\OmniPeek\Engine Plugins\"
if EXIST "%~1\Savvius\OmniPeek\Engine Plugins" xcopy "%~2CMIFilterOE\CMIFilterOE.he" "%~1\Savvius\OmniPeek\Engine Plugins\" /yrq
if EXIST "%~1\Savvius\OmniPeek\Engine Plugins" xcopy "%~2CMIFilterOE\x64\CMIFilterOE.dll" "%~1\Savvius\OmniPeek\Engine Plugins\" /yrq
goto :EOF

:installomniengine32
if NOT EXIST "%~1\Savvius\Capture Engine" goto :EOF
echo Installing CMIFilterOE 32-bit to %~1\Savvius\Capture Engine
if NOT EXIST "%~1\Savvius\Capture Engine\Plugins" mkdir "%~1\Savvius\Capture Engine\Plugins"
if EXIST "%~1\Savvius\Capture Engine\Plugins" xcopy "%~2CMIFilterOE\CMIFilterOE.he" "%~1\Savvius\Capture Engine\Plugins\" /yrq
if EXIST "%~1\Savvius\Capture Engine\Plugins" xcopy "%~2CMIFilterOE\Win32\CMIFilterOE.dll" "%~1\Savvius\Capture Engine\Plugins\" /yrq
goto :EOF

:installomniengine64
if NOT EXIST "%~1\Savvius\Capture Engine" goto :EOF
echo Installing CMIFilterOE 64-bit to %~1\Savvius\Capture Engine
if NOT EXIST "%~1\Savvius\Capture Engine\Plugins" mkdir "%~1\Savvius\Capture Engine\Plugins"
if EXIST "%~1\Savvius\Capture Engine" xcopy "%~2CMIFilterOE\CMIFilterOE.he" "%~1\Savvius\Capture Engine\Plugins\" /yrq
if EXIST "%~1\Savvius\Capture Engine" xcopy "%~2CMIFilterOE\x64\CMIFilterOE.dll" "%~1\Savvius\Capture Engine\Plugins\" /yrq
goto :EOF
