set VS_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community"
set QT_PATH="C:\Qt\5.12.5\msvc2017"
set SEVENZIP_PATH="C:\Program Files\7-Zip"
set CUR_DRIVE=%CD:~0,2%
set SOURCE_PATH=%~dp0
set /p RELEASE_VERSION=<release_version.txt
set BUILD_NAME=x64plgmnr_win32_portable
set QMAKE=%QT_PATH%\bin\qmake.exe

set QT_SPEC=win32-msvc
call %VS_PATH%\VC\Auxiliary\Build\vcvars32.bat

%CUR_DRIVE%
cd %SOURCE_PATH%

cd %SOURCE_PATH%\gui_source
%QT_PATH%\bin\qmake.exe gui_source.pro -r -spec %QT_SPEC% "CONFIG+=release"

nmake Makefile.Release clean
nmake
del Makefile
del Makefile.Release
del Makefile.Debug

cd %SOURCE_PATH%

cd %SOURCE_PATH%\console_source
%QT_PATH%\bin\qmake.exe console_source.pro -r -spec %QT_SPEC% "CONFIG+=release"

nmake Makefile.Release clean
nmake
del Makefile
del Makefile.Release
del Makefile.Debug

cd %SOURCE_PATH%

cd %SOURCE_PATH%\winloader_source

Rc.exe /v rsrc.rc
Cvtres.exe /v rsrc.res
cl.exe /c winloader.cpp /D_USING_V110_SDK71_ /GS- /Oi-
link.exe /SUBSYSTEM:WINDOWS winloader.obj rsrc.res user32.lib kernel32.lib shell32.lib /NODEFAULTLIB /SAFESEH:NO /SUBSYSTEM:WINDOWS,5.01 /ENTRY:entry /OUT:%SOURCE_PATH%\build\release\winloader.exe
if exist winloader.obj del winloader.obj
if exist rsrc.res del rsrc.res
if exist rsrc.obj del rsrc.obj

cd %SOURCE_PATH%

rem rmdir /s /q release\
mkdir %SOURCE_PATH%\release

rem portable
rmdir /s /q %SOURCE_PATH%\release\%BUILD_NAME%\
mkdir %SOURCE_PATH%\release\%BUILD_NAME%
mkdir %SOURCE_PATH%\release\%BUILD_NAME%\base
mkdir %SOURCE_PATH%\release\%BUILD_NAME%\base\platforms

copy %SOURCE_PATH%\build\release\winloader.exe %SOURCE_PATH%\release\%BUILD_NAME%\x64plgmnr.exe
copy %SOURCE_PATH%\build\release\x64plgmnr.exe %SOURCE_PATH%\release\%BUILD_NAME%\base\
copy %SOURCE_PATH%\build\release\x64plgmnrc.exe %SOURCE_PATH%\release\%BUILD_NAME%\base\

copy %SOURCE_PATH%\QOpenSSL\libs\win32-msvc\libcrypto-1_1.dll %SOURCE_PATH%\release\%BUILD_NAME%\base\
copy %SOURCE_PATH%\QOpenSSL\libs\win32-msvc\libssl-1_1.dll %SOURCE_PATH%\release\%BUILD_NAME%\base\

copy %QT_PATH%\bin\Qt5Widgets.dll %SOURCE_PATH%\release\%BUILD_NAME%\base\
copy %QT_PATH%\bin\Qt5Gui.dll %SOURCE_PATH%\release\%BUILD_NAME%\base\
copy %QT_PATH%\bin\Qt5Core.dll %SOURCE_PATH%\release\%BUILD_NAME%\base\
copy %QT_PATH%\bin\Qt5Network.dll %SOURCE_PATH%\release\%BUILD_NAME%\base\
copy %QT_PATH%\plugins\platforms\qwindows.dll %SOURCE_PATH%\release\%BUILD_NAME%\base\platforms\

copy %VS_PATH%\VC\Redist\MSVC\14.16.27012\x86\Microsoft.VC141.CRT\msvcp140.dll %SOURCE_PATH%\release\%BUILD_NAME%\base\
copy %VS_PATH%\VC\Redist\MSVC\14.16.27012\x86\Microsoft.VC141.CRT\vcruntime140.dll %SOURCE_PATH%\release\%BUILD_NAME%\base\

cd %SOURCE_PATH%\release
if exist %BUILD_NAME%_%RELEASE_VERSION%.zip del %BUILD_NAME%_%RELEASE_VERSION%.zip
%SEVENZIP_PATH%\7z.exe a %BUILD_NAME%_%RELEASE_VERSION%.zip %BUILD_NAME%\*
cd %SOURCE_PATH%

cd %SOURCE_PATH%

rmdir /s /q %SOURCE_PATH%\release\%BUILD_NAME%\

goto:eof
