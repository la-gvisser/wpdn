@echo off
echo ---------------------------------------------------
echo ----- %0 -----------------------
echo ----- %CD%  
echo ---------------------------------------------------
rem =================================================================
rem
rem This simple batch file does the following:
rem
rem [1] Creates a self extractor of the Savvius Engine Analysis Module Wizard Installer 
rem
rem Usage: 
rem
rem	c:> MakeSelfExtractor.bat <Engine Version>
rem example: MakeSelfExtractor 11
rem 
rem =================================================================

rem -------------------------------
rem 	Variables - please adjust 
rem  	according to your config
rem -------------------------------
setlocal

set ENG_VERSION=%1

set ICON_FILE=Inst.ico
set MESSAGE_PRODUCT_NAME=%ENG_VERSION%

set WINZIP_APP="%ProgramFiles%\WinZip\wzzip.exe"
set WINZIP_SE_APP="%ProgramFiles(x86)%\WinZip Self-Extractor\Wzipse32.exe"


rem -------------------------------
rem 	FOLDER CHECK 
rem -------------------------------

if "%ENG_VERSION%" == "" goto NO_ARGS

if not exist "..\WizardInstaller" goto WI_DIR_NOT_PRESENT
if not exist "..\WizardInstaller\Release" goto WI_DIR_NOT_PRESENT
if not exist "..\OmniEngine" goto PP_DIR_NOT_PRESENT

if not exist %ICON_FILE%  goto ICON_FILE_MISSING

if exist %WINZIP_APP% goto WIN32HOST
rem set WINZIP_APP="%ProgramFiles%\WinZip\WINZIP64.exe"
set WINZIP_APP="%ProgramFiles(x86)%\WinZip\wzzip.exe"
set WINZIP_SE_APP="%ProgramFiles(x86)%\WinZip Self-Extractor\Wzipse32.exe"
rem ps set WINZIP_SE_APP="%ProgramFiles(x86)%\WinZip Self-Extractor\WINZIPSE.exe"
if not exist %WINZIP_APP% goto WINZIP_MISSING
:WIN32HOST
if not exist %WINZIP_SE_APP% goto WINZIP_SE_MISSING

rem -------------------------------
rem 	SetUp Message File 
rem -------------------------------
echo Creating message file...
if exist message.txt del /f /q message.txt
copy /Y  message-src.txt  message.txt

repltext.exe /xy .\message.txt "[PRODUCT_NAME]" "Engine Plugin Wizard %ENG_VERSION%"

rem -------------------------------
rem 	Actual Batch Commands
rem -------------------------------

echo ------------------------------------------------
echo Cleaning out existing zip files with same name: OmniEngine%ENG_VERSION%
if exist bin\WizardInstaller%ENG_VERSION%.zip del /f /q bin\WizardInstaller%ENG_VERSION%.zip
if exist bin\WizardInstaller%ENG_VERSION%.exe del /f /q bin\WizardInstaller%ENG_VERSION%.exe

echo ------------------------------------------------
echo Creating Zip file for Self Extractor...

if not exist "bin" md "bin"
pushd "."
cd ".."
%WINZIP_APP% BuildInstaller\bin\WizardInstaller%ENG_VERSION%.zip -P -a "OmniEngine\1033\*.*"
%WINZIP_APP% BuildInstaller\bin\WizardInstaller%ENG_VERSION%.zip -P -a "OmniEngine\HTML\1033\*.*"
%WINZIP_APP% BuildInstaller\bin\WizardInstaller%ENG_VERSION%.zip -P -a "OmniEngine\Images\*.*"
%WINZIP_APP% BuildInstaller\bin\WizardInstaller%ENG_VERSION%.zip -P -a "OmniEngine\Scripts\1033\*.*"
%WINZIP_APP% BuildInstaller\bin\WizardInstaller%ENG_VERSION%.zip -P -a "OmniEngine\Templates\1033\*.*"
%WINZIP_APP% BuildInstaller\bin\WizardInstaller%ENG_VERSION%.zip -P -a "OmniEngine\default.14.0.vcxproj"
%WINZIP_APP% BuildInstaller\bin\WizardInstaller%ENG_VERSION%.zip -P -a "OmniEngine\OmniEngine%ENG_VERSION%.ico"
%WINZIP_APP% BuildInstaller\bin\WizardInstaller%ENG_VERSION%.zip -P -a "OmniEngine\OmniEngine%ENG_VERSION%.vsdir"
%WINZIP_APP% BuildInstaller\bin\WizardInstaller%ENG_VERSION%.zip -P -a "OmniEngine\OmniEngine%ENG_VERSION%.14.0.vsz"
cd "WizardInstaller\Release"
%WINZIP_APP% ..\..\BuildInstaller\bin\WizardInstaller%ENG_VERSION%.zip -P -a "WizardInstaller.exe"
popd
If Not ErrorLevel 1 Goto NoError
    Goto  ERROR_ZIPPING
:NoError

echo ------------------------------------------------
echo Creating Self Extractor File...

%WINZIP_SE_APP% bin\WizardInstaller%ENG_VERSION%.zip -setup -t "message.txt" -i "%ICON_FILE%" -le -c "WizardInstaller.exe"
If Not ErrorLevel 1 Goto NoSEError
    Goto  ERROR_SELF_EXTRACT
:NoSEError

echo ------------------------------------------------
echo Deleting Zip File for Self Extractor...
rem ps pause
del /F /Q bin\WizardInstaller%ENG_VERSION%.zip

echo ALL DONE! :)
goto END

rem -----------------------------------------------------------------------------------------
rem -----------------------------------------------------------------------------------------

:WI_DIR_NOT_PRESENT
echo %0 ERROR:  WizardInstaller%1 directories not present!!!
echo %0 ERROR:      Please check directory names and try again
@echo 
goto END_OF_ERRORS

:PP_DIR_NOT_PRESENT
echo %0 ERROR:  OmniEngine%1 directories not present!!!
echo %0 ERROR:      Please check directory names and try again
@echo 
goto END_OF_ERRORS

:NO_ARGS
echo %0 ERROR:  Command line argument missing:  API version number
echo %0 ERROR:    Please see script and try again.
echo %0 Creates a self extractor of the Peek Analysis Module Wizard Installer
echo %0 Usage: 
echo %0     MakeSelfExtractor.bat 'API Version'
echo %0 example: MakeSelfExtractor 10
@echo 
goto END_OF_ERRORS

:ICON_FILE_MISSING
echo %0 ERROR:  Icon File %ICON_FILE% missing!!!!  --- Cannot build online image.
echo %0 ERROR:    Please see script and try again.
@echo 
goto END_OF_ERRORS

:WINZIP_MISSING
echo %0 ERROR:  WinZip missing from this machine!!!!  --- Cannot build online image.
echo %0 ERROR:    Please see script and try again.
@echo 
goto END_OF_ERRORS

:WINZIP_SE_MISSING
echo %0 ERROR:  WinZip Self Extractor missing from this machine!!!!  --- Cannot build online image.
echo %0 ERROR:    Please see script and try again.
@echo 
goto END_OF_ERRORS

:ERROR_ZIPPING
echo %0 ERROR:  WinZip encountered an error creating zip file!!!!  --- Cannot create zip file.
echo %0 ERROR:    Please see script and try again.
@echo 
goto END_OF_ERRORS

:ERROR_SELF_EXTRACT
echo %0 ERROR:  WinZip encountered an error creating self-extract file!!!!  --- Cannot create self-extractor file.
echo %0 ERROR:    Please see script and try again.
@echo 
goto END_OF_ERRORS

:END_OF_ERRORS

:END
echo ----- %0 BATCHFILE END ---------------------
