@echo off
echo ---------------------------------------------------
echo ----- %0 -----------------------
echo ----- %CD%  
echo ---------------------------------------------------
rem =================================================================
rem
rem This simple batch file does the following:
rem
rem [1] Creates a self extractor of the Plugin Installer and the
rem     plugin in Release
rem
rem Usage: 
rem
rem	c:> MakeSelfExtractor.bat <plugin>
rem example: MakeSelfExtractor GoogleMap.dll
rem 
rem =================================================================

rem -------------------------------
rem 	Variables - please adjust 
rem  	according to your config
rem -------------------------------
setlocal

set PLUGIN_NAME=%1

set ICON_FILE=Inst.ico
set MESSAGE_PRODUCT_NAME=%PLUGIN_NAME%

set WINZIP_APP="C:\Program Files\WinZip\wzzip.exe"
set WINZIP_SE_APP="C:\Program Files\WinZip Self-Extractor\Wzipse32.exe"
set PLUGIN_INSTALLER="..\bin\PluginInstaller.exe"
set REPL_TEXT="..\bin\replText.exe"


rem -------------------------------
rem 	FOLDER CHECK 
rem -------------------------------

if "%PLUGIN_NAME%" == "" goto NO_ARGS

if not exist "Release" goto RE_DIR_NOT_PRESENT
if not exist "Release\%PLUGIN_NAME%.dll" goto PL_NOT_PRESENT

if not exist %PLUGIN_INSTALLER% goto PI_NOT_PRESENT

if not exist %REPL_TEXT% goto RT_NOT_PRESENT

if not exist %ICON_FILE% goto ICON_FILE_MISSING

if not exist  %WINZIP_APP% goto WINZIP_MISSING

if not exist  %WINZIP_SE_APP% goto WINZIP_SE_MISSING

rem -------------------------------
rem 	SetUp Message File 
rem -------------------------------
echo Creating message file...
if exist message.txt del /f /q message.txt
copy /Y  message-src.txt  message.txt

%REPL_TEXT% /xy .\message.txt "[PRODUCT_NAME]" "%PLUGIN_NAME%"

rem -------------------------------
rem 	Actual Batch Commands
rem -------------------------------

echo ------------------------------------------------
echo Cleaning out existing zip files with same name: %PLUGIN_NAME%
if not exist "bin" md "bin"
if exist bin\%PLUGIN_NAME%Installer.zip del /f /q bin\%PLUGIN_NAME%Installer.zip
if exist bin\%PLUGIN_NAME%Installer.exe del /f /q bin\%PLUGIN_NAME%Installer.exe

echo ------------------------------------------------
echo Creating Zip file for Self Extractor...

%WINZIP_APP% bin\%PLUGIN_NAME%Installer.zip -a %PLUGIN_INSTALLER%
%WINZIP_APP% bin\%PLUGIN_NAME%Installer.zip -a "Release\%PLUGIN_NAME%.dll"

If Not ErrorLevel 1 Goto NoError
    Goto  ERROR_ZIPPING
:NoError

echo ------------------------------------------------
echo Creating Self Extractor File...

%WINZIP_SE_APP% bin\%PLUGIN_NAME%Installer.zip -setup -t "message.txt" -i "%ICON_FILE%" -le -c "PluginInstaller.exe"
If Not ErrorLevel 1 Goto NoSEError
    Goto  ERROR_SELF_EXTRACT
:NoSEError

echo ------------------------------------------------
echo Deleting Zip File for Self Extractor...
del /F /Q bin\%PLUGIN_NAME%Installer.zip

echo ALL DONE! :)
goto END

rem -----------------------------------------------------------------------------------------
rem -----------------------------------------------------------------------------------------

:NO_ARGS
echo %0 ERROR:  Command line argument missing:  Plugin Name
echo %0 ERROR:    Please see script and try again.
echo %0 Creates a self extractor of the Plugin Installer
echo %0 Usage: 
echo %0     MakeSelfExtractor.bat 'Plugin Name'
echo %0 example: MakeSelfExtractor GoogleMap
@echo 
goto END_OF_ERRORS

:RE_DIR_NOT_PRESENT
echo %0 ERROR:  Release directories not present!!!
echo %0 ERROR:      Please build the plugin and try again
@echo 
goto END_OF_ERRORS

:PL_NOT_PRESENT
echo %0 ERROR:  %1.dll is not present!!!
echo %0 ERROR:      Please build the plugin and try again
@echo 
goto END_OF_ERRORS

:PI_NOT_PRESENT
echo %0 ERROR:  PluginInstaller.exe is not present!!!
echo %0 ERROR:      Please see this script and try again
@echo 
goto END_OF_ERRORS

:RT_NOT_PRESENT
echo %0 ERROR:  replText.exe is not present!!!
echo %0 ERROR:      Please see this script and try again
@echo 
goto END_OF_ERRORS

:ICON_FILE_MISSING
echo %0 ERROR:  Icon File %ICON_FILE% missing!!!!  --- Cannot build online image.
echo %0 ERROR:    Please see this script and try again.
@echo 
goto END_OF_ERRORS

:WINZIP_MISSING
echo %0 ERROR:  WinZip missing from this machine!!!!  --- Cannot build online image.
echo %0 ERROR:    Please see this script and try again.
@echo 
goto END_OF_ERRORS

:WINZIP_SE_MISSING
echo %0 ERROR:  WinZip Self Extractor missing from this machine!!!!  --- Cannot build online image.
echo %0 ERROR:    Please see this script and try again.
@echo 
goto END_OF_ERRORS

:ERROR_ZIPPING
echo %0 ERROR:  WinZip encountered an error creating zip file!!!!  --- Cannot create zip file.
echo %0 ERROR:    Please see this script and try again.
@echo 
goto END_OF_ERRORS

:ERROR_SELF_EXTRACT
echo %0 ERROR:  WinZip encountered an error creating self-extract file!!!!  --- Cannot create self-extractor file.
echo %0 ERROR:    Please see this script and try again.
@echo 
goto END_OF_ERRORS

:END_OF_ERRORS

:END
echo ----- %0 BATCHFILE END ---------------------
