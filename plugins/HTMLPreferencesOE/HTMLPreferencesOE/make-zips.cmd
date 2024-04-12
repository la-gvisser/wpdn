@echo off

if not exist Plugin md Plugin
if not exist Plugin\About md Plugin\About
if not exist Plugin\Options md Plugin\Options
if not exist Plugin\CaptureTab md Plugin\CaptureTab
if not exist Plugin\EngineTab md Plugin\EngineTab

pushd ".\Plugin"

rem Base
if exist About.zip del /f /q About.zip
if exist HTMLPreferencesOE-About.zip del /f /q HTMLPreferencesOE-About.zip
pushd About\About
cmd /C 7z a ..\..\About.zip *
popd
ren About.zip HTMLPreferencesOE-About.zip

if exist Options.zip del /f /q Options.zip
if exist HTMLPreferencesOE-Options.zip del /f /q HTMLPreferencesOE-Options.zip
pushd Options\Options
cmd /C 7z a ..\..\Options.zip *
popd
ren Options.zip HTMLPreferencesOE-Options.zip

if exist CaptureTab.zip del /f /q CaptureTab.zip
if exist HTMLPreferencesOE-CaptureTab.zip del /f /q HTMLPreferencesOE-CaptureTab.zip
pushd CaptureTab\CaptureTab
cmd /C 7z a ..\..\CaptureTab.zip *
popd
ren CaptureTab.zip HTMLPreferencesOE-CaptureTab.zip

if exist EngineTab.zip del /f /q EngineTab.zip
if exist HTMLPreferencesOE-EngineTab.zip del /f /q HTMLPreferencesOE-EngineTab.zip
pushd EngineTab\EngineTab
cmd /C 7z a ..\..\EngineTab.zip *
popd
ren EngineTab.zip HTMLPreferencesOE-EngineTab.zip


rem Prefs 1
if exist About.zip del /f /q About.zip
if exist HTMLPrefsOE-1-About.zip del /f /q HTMLPrefsOE-1-About.zip
pushd About\About-1
cmd /C 7z a ..\..\About.zip *
popd
ren About.zip HTMLPrefsOE-1-About.zip

if exist Options.zip del /f /q Options.zip
if exist HTMLPrefsOE-1-Options.zip del /f /q HTMLPrefsOE-1-Options.zip
pushd Options\Options-1
cmd /C 7z a ..\..\Options.zip *
popd
ren Options.zip HTMLPrefsOE-1-Options.zip

if exist CaptureTab.zip del /f /q CaptureTab.zip
if exist HTMLPrefsOE-1-CaptureTab.zip del /f /q HTMLPrefsOE-1-CaptureTab.zip
pushd CaptureTab\CaptureTab-1
cmd /C 7z a ..\..\CaptureTab.zip *
popd
ren CaptureTab.zip HTMLPrefsOE-1-CaptureTab.zip

if exist EngineTab.zip del /f /q EngineTab.zip
if exist HTMLPrefsOE-1-EngineTab.zip del /f /q HTMLPrefsOE-1-EngineTab.zip
pushd EngineTab\EngineTab-1
cmd /C 7z a ..\..\EngineTab.zip *
popd
ren EngineTab.zip HTMLPrefsOE-1-EngineTab.zip


rem Prefs 2
if exist About.zip del /f /q About.zip
if exist HTMLPrefsOE-2-About.zip del /f /q HTMLPrefsOE-2-About.zip
pushd About\About-2
cmd /C 7z a ..\..\About.zip *
popd
ren About.zip HTMLPrefsOE-2-About.zip

if exist Options.zip del /f /q Options.zip
if exist HTMLPrefsOE-2-Options.zip del /f /q HTMLPrefsOE-2-Options.zip
pushd Options\Options-2
cmd /C 7z a ..\..\Options.zip *
popd
ren Options.zip HTMLPrefsOE-2-Options.zip

if exist CaptureTab.zip del /f /q CaptureTab.zip
if exist HTMLPrefsOE-2-CaptureTab.zip del /f /q HTMLPrefsOE-2-CaptureTab.zip
pushd CaptureTab\CaptureTab-2
cmd /C 7z a ..\..\CaptureTab.zip *
popd
ren CaptureTab.zip HTMLPrefsOE-2-CaptureTab.zip

if exist EngineTab.zip del /f /q EngineTab.zip
if exist HTMLPrefsOE-2-EngineTab.zip del /f /q HTMLPrefsOE-2-EngineTab.zip
pushd EngineTab\EngineTab-2
cmd /C 7z a ..\..\EngineTab.zip *
popd
ren EngineTab.zip HTMLPrefsOE-2-EngineTab.zip


rem Prefs 3
if exist About.zip del /f /q About.zip
if exist HTMLPrefsOE-3-About.zip del /f /q HTMLPrefsOE-3-About.zip
pushd About\About-3
cmd /C 7z a ..\..\About.zip *
popd
ren About.zip HTMLPrefsOE-3-About.zip

if exist Options.zip del /f /q Options.zip
if exist HTMLPrefsOE-3-Options.zip del /f /q HTMLPrefsOE-3-Options.zip
pushd Options\Options-3
cmd /C 7z a ..\..\Options.zip *
popd
ren Options.zip HTMLPrefsOE-3-Options.zip

if exist CaptureTab.zip del /f /q CaptureTab.zip
if exist HTMLPrefsOE-3-CaptureTab.zip del /f /q HTMLPrefsOE-3-CaptureTab.zip
pushd CaptureTab\CaptureTab-3
cmd /C 7z a ..\..\CaptureTab.zip *
popd
ren CaptureTab.zip HTMLPrefsOE-3-CaptureTab.zip

if exist EngineTab.zip del /f /q EngineTab.zip
if exist HTMLPrefsOE-3-EngineTab.zip del /f /q HTMLPrefsOE-3-EngineTab.zip
pushd EngineTab\EngineTab-3
cmd /C 7z a ..\..\EngineTab.zip *
popd
ren EngineTab.zip HTMLPrefsOE-3-EngineTab.zip


rem Prefs 4
if exist About.zip del /f /q About.zip
if exist HTMLPrefsOE-4-About.zip del /f /q HTMLPrefsOE-4-About.zip
pushd About\About-4
cmd /C 7z a ..\..\About.zip *
popd
ren About.zip HTMLPrefsOE-4-About.zip

if exist Options.zip del /f /q Options.zip
if exist HTMLPrefsOE-4-Options.zip del /f /q HTMLPrefsOE-4-Options.zip
pushd Options\Options-4
cmd /C 7z a ..\..\Options.zip *
popd
ren Options.zip HTMLPrefsOE-4-Options.zip

if exist CaptureTab.zip del /f /q CaptureTab.zip
if exist HTMLPrefsOE-4-CaptureTab.zip del /f /q HTMLPrefsOE-4-CaptureTab.zip
pushd CaptureTab\CaptureTab-4
cmd /C 7z a ..\..\CaptureTab.zip *
popd
ren CaptureTab.zip HTMLPrefsOE-4-CaptureTab.zip

if exist EngineTab.zip del /f /q EngineTab.zip
if exist HTMLPrefsOE-4-EngineTab.zip del /f /q HTMLPrefsOE-4-EngineTab.zip
pushd EngineTab\EngineTab-4
cmd /C 7z a ..\..\EngineTab.zip *
popd
ren EngineTab.zip HTMLPrefsOE-4-EngineTab.zip


rem Base
popd
