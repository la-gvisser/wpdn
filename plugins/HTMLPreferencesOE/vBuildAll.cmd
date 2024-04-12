call C:\VisualStudio14\VC\bin\vcvars32.bat

p4 edit .\HTMLPreferencesOE\Plugin\About\...
p4 edit .\HTMLPreferencesOE\Plugin\CaptureTab\...
p4 edit .\HTMLPreferencesOE\Plugin\EngineTab\...
p4 edit .\HTMLPreferencesOE\Plugin\Options\...

cmd /C start /wait vBuildWebPages.py

if NOT EXIST x64 md x64

del x64\*.dll

echo Build Version 1
del HTMLPreferencesOE\x64\Release\HTMLPreferencesOE.dll

set cl=/DOE_SERIAL#1
msbuild.exe /nologo /t:rebuild /p:Configuration=Release /p:Platform=x64 HTMLPreferencesOE\HTMLPreferencesOE.vcxproj
copy HTMLPreferencesOE\x64\Release\HTMLPreferencesOE.dll x64\HTMLPrefsOE-1.dll


echo Build Version 2
del HTMLPreferencesOE\x64\Release\HTMLPreferencesOE.dll

set cl=/DOE_SERIAL#2 /DNO_ENGINE_TAB
msbuild.exe /nologo /t:rebuild /p:Configuration=Release /p:Platform=x64 HTMLPreferencesOE\HTMLPreferencesOE.vcxproj
copy HTMLPreferencesOE\x64\Release\HTMLPreferencesOE.dll x64\HTMLPrefsOE-2.dll


echo Build Version 3
del HTMLPreferencesOE\x64\Release\HTMLPreferencesOE.dll

set cl=/DOE_SERIAL#3
msbuild.exe /nologo /t:rebuild /p:Configuration=Release /p:Platform=x64 HTMLPreferencesOE\HTMLPreferencesOE.vcxproj
copy HTMLPreferencesOE\x64\Release\HTMLPreferencesOE.dll x64\HTMLPrefsOE-3.dll


echo Build Version 4
del HTMLPreferencesOE\x64\Release\HTMLPreferencesOE.dll

set cl=/DOE_SERIAL#4 /DNO_ENGINE_TAB
msbuild.exe /nologo /t:rebuild /p:Configuration=Release /p:Platform=x64 HTMLPreferencesOE\HTMLPreferencesOE.vcxproj
copy HTMLPreferencesOE\x64\Release\HTMLPreferencesOE.dll x64\HTMLPrefsOE-4.dll
