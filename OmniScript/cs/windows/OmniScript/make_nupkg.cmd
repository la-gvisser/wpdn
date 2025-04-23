@echo off
set VERSION=2.5.2
if not "%1"=="" set VERSION=%1
echo %VERSION%

if exist %USERPROFILE%\packages\OmniScript.2.5.0.nupkg del /F/ Q %USERPROFILE%\packages\OmniScript.2.5.0.nupkg
if exist %USERPROFILE%\packages\OmniScript.2.5.1.nupkg del /F/ Q %USERPROFILE%\packages\OmniScript.2.5.1.nupkg
if exist %USERPROFILE%\packages\OmniScript.2.5.2.nupkg del /F/ Q %USERPROFILE%\packages\OmniScript.2.5.2.nupkg
if exist %USERPROFILE%\packages\OmniScript.%VERSION%.nupkg del /F/ Q %USERPROFILE%\packages\OmniScript.%VERSION%.nupkg
if exist %USERPROFILE%\.nuget\packages\OmniScript\ rmdir /S/ Q %USERPROFILE%\.nuget\packages\OmniScript

dotnet pack -c Release
dotnet publish -c Release

if exist .\bin\Release\OmniScript.%VERSION%.nupkg (
  copy .\bin\Release\OmniScript.%VERSION%.nupkg %USERPROFILE%\packages\
)
