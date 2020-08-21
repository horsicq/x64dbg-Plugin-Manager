set VS_PATH="C:\Program Files (x86)\Microsoft Visual Studio 12.0"
set SEVENZIP_PATH="C:\Program Files\7-Zip"
set QT_PATH="C:\Qt5.6.3\5.6.3\msvc2013"

set BUILD_NAME=x64plgmnr_win32_portable
set CUR_DRIVE=%CD:~0,2%
set SOURCE_PATH=%~dp0
mkdir %SOURCE_PATH%\build
mkdir %SOURCE_PATH%\build\loader
mkdir %SOURCE_PATH%\release
set /p RELEASE_VERSION=<%SOURCE_PATH%\release_version.txt

set QT_SPEC=win32-msvc2013
call %VS_PATH%\VC\bin\vcvars32.bat
set ZIP_NAME=%BUILD_NAME%_%RELEASE_VERSION%

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

rem rmdir /s /q release\
mkdir %SOURCE_PATH%\release

rem portable
rmdir /s /q %SOURCE_PATH%\release\%BUILD_NAME%\
mkdir %SOURCE_PATH%\release\%BUILD_NAME%
mkdir %SOURCE_PATH%\release\%BUILD_NAME%\platforms

copy %SOURCE_PATH%\build\release\x64plgmnr.exe %SOURCE_PATH%\release\%BUILD_NAME%\
copy %SOURCE_PATH%\build\release\x64plgmnrc.exe %SOURCE_PATH%\release\%BUILD_NAME%\

copy %SOURCE_PATH%\QOpenSSL\libs\win32-msvc\libcrypto-1_1.dll %SOURCE_PATH%\release\%BUILD_NAME%\
copy %SOURCE_PATH%\QOpenSSL\libs\win32-msvc\libssl-1_1.dll %SOURCE_PATH%\release\%BUILD_NAME%\

copy %QT_PATH%\bin\Qt5Widgets.dll %SOURCE_PATH%\release\%BUILD_NAME%\
copy %QT_PATH%\bin\Qt5Gui.dll %SOURCE_PATH%\release\%BUILD_NAME%\
copy %QT_PATH%\bin\Qt5Core.dll %SOURCE_PATH%\release\%BUILD_NAME%\
copy %QT_PATH%\bin\Qt5Network.dll %SOURCE_PATH%\release\%BUILD_NAME%\
copy %QT_PATH%\plugins\platforms\qwindows.dll %SOURCE_PATH%\release\%BUILD_NAME%\platforms\
copy %SOURCE_PATH%\ssl\libeay32.dll %SOURCE_PATH%\release\%BUILD_NAME%\
copy %SOURCE_PATH%\ssl\ssleay32.dll %SOURCE_PATH%\release\%BUILD_NAME%\

copy %VS_PATH%\VC\redist\x86\Microsoft.VC120.CRT\msvcp120.dll %SOURCE_PATH%\release\%BUILD_NAME%\
copy %VS_PATH%\VC\redist\x86\Microsoft.VC120.CRT\msvcr120.dll %SOURCE_PATH%\release\%BUILD_NAME%\

cd %SOURCE_PATH%\release
if exist %BUILD_NAME%_%RELEASE_VERSION%.zip del %BUILD_NAME%_%RELEASE_VERSION%.zip
%SEVENZIP_PATH%\7z.exe a %BUILD_NAME%_%RELEASE_VERSION%.zip %BUILD_NAME%\*
cd %SOURCE_PATH%

cd %SOURCE_PATH%\release\%BUILD_NAME%
ren Example Example1

cd %SOURCE_PATH%

rmdir /s /q %SOURCE_PATH%\release\%BUILD_NAME%\

goto:eof
