@echo off

rem Create or clean the directories.
if exist build rmdir /S /Q build
if not exist bin mkdir bin
if not exist omniscript mkdir omniscript
if not exist omniscript\Decodes mkdir omniscript\Decodes

if exist bin\RCapture.py del bin\RCapture.py
if exist bin\Forensic.py del bin\Forensic.py
if exist omniscript\omniapi.he del omniscript\omniapi.he
if exist omniscript\*.pyc del omniscript\*.pyc
if exist omniscript\*.dll del omniscript\*.dll
if exist omniscript\*.exe del omniscript\*.exe
if exist omniscript\*.txt del omniscript\*.txt
if exist omniscript\*.xml del omniscript\*.xml
if exist omniscript\Decodes\*.bin del omniscript\Decodes\*.bin
if exist omniscript\Decodes\*.dcd del omniscript\Decodes\*.dcd

if "%1"=="clean" goto :EOF

rem Populate the build directory.
copy RCapture\RCapture.py bin\ > NUL
copy Forensics\Forensics.py bin\ > NUL
copy ..\..\shared\omniapi.he omniscript\ > NUL
copy ..\..\shared\*.txt omniscript\ > NUL
copy ..\..\shared\*.xml omniscript\ > NUL

copy /b ..\..\shared\bin\omniapi.dll omniscript\ > NUL
copy /b ..\..\shared\bin\libeay32.dll omniscript\ > NUL
copy /b ..\..\shared\bin\libheruntime.dll omniscript\ > NUL
copy /b ..\..\shared\bin\peekutil.dll omniscript\ > NUL
copy /b ..\..\shared\bin\ssleay32.dll omniscript\ > NUL
copy /b ..\..\shared\bin\wpxml.dll omniscript\ > NUL
copy /b ..\..\shared\bin\zlibwapi.dll omniscript\ > NUL
copy /b ..\..\shared\pkt2dcd.exe omniscript\ > NUL

copy /b ..\..\..\pkt2dcd\bin\pkt2dcd.exe omniscript > NUL
copy /b ..\..\..\pkt2dcd\bin\Dcodu.dll omniscript > NUL
copy /b ..\..\..\pkt2dcd\bin\Protospecs.dll omniscript > NUL
copy /b ..\..\..\pkt2dcd\bin\Decodes\decodes.bin omniscript\Decodes > NUL

if "%1"=="setup" goto :EOF

rem Build the egg.
setup.py bdist_egg

rem Install the egg.
echo(
@cd dist
C:\Python27\Python.exe C:\Python27\ez_setup.py OmniScript-0.6.4-py2.7.egg
@cd ..

:EOF