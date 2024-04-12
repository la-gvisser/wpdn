@echo off

if not exist Plugin md Plugin
if not exist Plugin\About md Plugin\About
if not exist Plugin\Options md Plugin\Options
if not exist Plugin\CaptureTab md Plugin\CaptureTab
if not exist Plugin\EngineTab md Plugin\EngineTab

pushd ".\Plugin"

rem Base
if exist About.zip del /f /q About.zip
if exist CefPreferencesOE-About.zip del /f /q CefPreferencesOE-About.zip
pushd About\About
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\About.zip *
popd
ren About.zip CefPreferencesOE-About.zip

if exist Options.zip del /f /q Options.zip
if exist CefPreferencesOE-Options.zip del /f /q CefPreferencesOE-Options.zip
pushd Options\Options
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\Options.zip *
popd
ren Options.zip CefPreferencesOE-Options.zip

if exist CaptureTab.zip del /f /q CaptureTab.zip
if exist CefPreferencesOE-CaptureTab.zip del /f /q CefPreferencesOE-CaptureTab.zip
pushd CaptureTab\CaptureTab
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\CaptureTab.zip *
popd
ren CaptureTab.zip CefPreferencesOE-CaptureTab.zip

if exist EngineTab.zip del /f /q EngineTab.zip
if exist CefPreferencesOE-EngineTab.zip del /f /q CefPreferencesOE-EngineTab.zip
pushd EngineTab\EngineTab
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\EngineTab.zip *
popd
ren EngineTab.zip CefPreferencesOE-EngineTab.zip


rem Prefs 1
if exist About.zip del /f /q About.zip
if exist CefPrefsOE-1-About.zip del /f /q CefPrefsOE-1-About.zip
pushd About\About-1
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\About.zip *
popd
ren About.zip CefPrefsOE-1-About.zip

if exist Options.zip del /f /q Options.zip
if exist CefPrefsOE-1-Options.zip del /f /q CefPrefsOE-1-Options.zip
pushd Options\Options-1
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\Options.zip *
popd
ren Options.zip CefPrefsOE-1-Options.zip

if exist CaptureTab.zip del /f /q CaptureTab.zip
if exist CefPrefsOE-1-CaptureTab.zip del /f /q CefPrefsOE-1-CaptureTab.zip
pushd CaptureTab\CaptureTab-1
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\CaptureTab.zip *
popd
ren CaptureTab.zip CefPrefsOE-1-CaptureTab.zip

if exist EngineTab.zip del /f /q EngineTab.zip
if exist CefPrefsOE-1-EngineTab.zip del /f /q CefPrefsOE-1-EngineTab.zip
pushd EngineTab\EngineTab-1
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\EngineTab.zip *
popd
ren EngineTab.zip CefPrefsOE-1-EngineTab.zip


rem Prefs 2
if exist About.zip del /f /q About.zip
if exist CefPrefsOE-2-About.zip del /f /q CefPrefsOE-2-About.zip
pushd About\About-2
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\About.zip *
popd
ren About.zip CefPrefsOE-2-About.zip

if exist Options.zip del /f /q Options.zip
if exist CefPrefsOE-2-Options.zip del /f /q CefPrefsOE-2-Options.zip
pushd Options\Options-2
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\Options.zip *
popd
ren Options.zip CefPrefsOE-2-Options.zip

if exist CaptureTab.zip del /f /q CaptureTab.zip
if exist CefPrefsOE-2-CaptureTab.zip del /f /q CefPrefsOE-2-CaptureTab.zip
pushd CaptureTab\CaptureTab-2
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\CaptureTab.zip *
popd
ren CaptureTab.zip CefPrefsOE-2-CaptureTab.zip

if exist EngineTab.zip del /f /q EngineTab.zip
if exist CefPrefsOE-2-EngineTab.zip del /f /q CefPrefsOE-2-EngineTab.zip
pushd EngineTab\EngineTab-2
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\EngineTab.zip *
popd
ren EngineTab.zip CefPrefsOE-2-EngineTab.zip


rem Prefs 3
if exist About.zip del /f /q About.zip
if exist CefPrefsOE-3-About.zip del /f /q CefPrefsOE-3-About.zip
pushd About\About-3
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\About.zip *
popd
ren About.zip CefPrefsOE-3-About.zip

if exist Options.zip del /f /q Options.zip
if exist CefPrefsOE-3-Options.zip del /f /q CefPrefsOE-3-Options.zip
pushd Options\Options-3
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\Options.zip *
popd
ren Options.zip CefPrefsOE-3-Options.zip

if exist CaptureTab.zip del /f /q CaptureTab.zip
if exist CefPrefsOE-3-CaptureTab.zip del /f /q CefPrefsOE-3-CaptureTab.zip
pushd CaptureTab\CaptureTab-3
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\CaptureTab.zip *
popd
ren CaptureTab.zip CefPrefsOE-3-CaptureTab.zip

if exist EngineTab.zip del /f /q EngineTab.zip
if exist CefPrefsOE-3-EngineTab.zip del /f /q CefPrefsOE-3-EngineTab.zip
pushd EngineTab\EngineTab-3
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\EngineTab.zip *
popd
ren EngineTab.zip CefPrefsOE-3-EngineTab.zip


rem Prefs 4
if exist About.zip del /f /q About.zip
if exist CefPrefsOE-4-About.zip del /f /q CefPrefsOE-4-About.zip
pushd About\About-4
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\About.zip *
popd
ren About.zip CefPrefsOE-4-About.zip

if exist Options.zip del /f /q Options.zip
if exist CefPrefsOE-4-Options.zip del /f /q CefPrefsOE-4-Options.zip
pushd Options\Options-4
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\Options.zip *
popd
ren Options.zip CefPrefsOE-4-Options.zip

if exist CaptureTab.zip del /f /q CaptureTab.zip
if exist CefPrefsOE-4-CaptureTab.zip del /f /q CefPrefsOE-4-CaptureTab.zip
pushd CaptureTab\CaptureTab-4
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\CaptureTab.zip *
popd
ren CaptureTab.zip CefPrefsOE-4-CaptureTab.zip

if exist EngineTab.zip del /f /q EngineTab.zip
if exist CefPrefsOE-4-EngineTab.zip del /f /q CefPrefsOE-4-EngineTab.zip
pushd EngineTab\EngineTab-4
cmd /C "C:\Program Files\7-Zip\7z.exe" a ..\..\EngineTab.zip *
popd
ren EngineTab.zip CefPrefsOE-4-EngineTab.zip


rem Base
popd
