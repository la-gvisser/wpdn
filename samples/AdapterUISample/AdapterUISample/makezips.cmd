if not exist Plugin md Plugin
if not exist Plugin\Options md Plugin\Options
pushd Plugin\Options
cmd /C 7z a ..\AdapterUISample-Options.zip *
popd
