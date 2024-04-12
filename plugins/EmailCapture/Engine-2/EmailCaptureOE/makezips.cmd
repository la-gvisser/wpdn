if not exist Plugin md Plugin
if not exist Plugin\About md Plugin\About
if not exist Plugin\CaptureTab md Plugin\CaptureTab
if not exist Plugin\EngineTab md Plugin\EngineTab
if not exist Plugin\Options md Plugin\Options
pushd Plugin\About
cmd /C 7z a ..\EmailCaptureOE-About.zip *
cd ..\CaptureTab
cmd /C 7z a ..\EmailCaptureOE-CaptureTab.zip *
cd ..\EngineTab
cmd /C 7z a ..\EmailCaptureOE-EngineTab.zip *
cd ..\Options
cmd /C 7z a ..\EmailCaptureOE-Options.zip *
popd
