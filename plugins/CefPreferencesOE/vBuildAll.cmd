rem call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"
call C:\VisualStudio14\VC\bin\vcvars32.bat

p4 edit .\CefPreferencesOE\Plugin\About\...
p4 edit .\CefPreferencesOE\Plugin\CaptureTab\...
p4 edit .\CefPreferencesOE\Plugin\EngineTab\...
p4 edit .\CefPreferencesOE\Plugin\Options\...

cmd /C start /wait vBuildWebPages.py

if NOT EXIST x64 md x64

del x64\*.dll

echo Build Version 1
del CefPreferencesOE\x64\Release\CefPreferencesOE.dll

set cl=/DOE_SERIAL#1
msbuild.exe /nologo /t:rebuild /p:Configuration=Release /p:Platform=x64 CefPreferencesOE\CefPreferencesOE.vcxproj
copy CefPreferencesOE\x64\Release\CefPreferencesOE.dll x64\CefPrefsOE-1.dll


echo Build Version 2
del CefPreferencesOE\x64\Release\CefPreferencesOE.dll

set cl=/DOE_SERIAL#2 /DNO_ENGINE_TAB
msbuild.exe /nologo /t:rebuild /p:Configuration=Release /p:Platform=x64 CefPreferencesOE\CefPreferencesOE.vcxproj
copy CefPreferencesOE\x64\Release\CefPreferencesOE.dll x64\CefPrefsOE-2.dll


echo Build Version 3
del CefPreferencesOE\x64\Release\CefPreferencesOE.dll

set cl=/DOE_SERIAL#3
msbuild.exe /nologo /t:rebuild /p:Configuration=Release /p:Platform=x64 CefPreferencesOE\CefPreferencesOE.vcxproj
copy CefPreferencesOE\x64\Release\CefPreferencesOE.dll x64\CefPrefsOE-3.dll


echo Build Version 4
del CefPreferencesOE\x64\Release\CefPreferencesOE.dll

set cl=/DOE_SERIAL#4 /DNO_ENGINE_TAB
msbuild.exe /nologo /t:rebuild /p:Configuration=Release /p:Platform=x64 CefPreferencesOE\CefPreferencesOE.vcxproj
copy CefPreferencesOE\x64\Release\CefPreferencesOE.dll x64\CefPrefsOE-4.dll

:END
