if not exist Plugin md Plugin
if not exist Plugin\CaptureTab md Plugin\CaptureTab
if not exist Plugin\Options md Plugin\Options
pushd Plugin\CaptureTab
cmd /C 7z a ..\FolderAdapterOE-CaptureTab.zip *
popd
pushd Plugin\Options
cmd /C 7z a ..\FolderAdapterOE-Options.zip *
popd
