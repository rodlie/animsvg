@echo off

REM ### BUILD FRICTION ON WINDOWS
REM # Copyright (c) Friction contributors
REM # GPLv3+

set OPT=%1
set REL=OFF
if "%OPT%" == "release" (
    set REL=ON
)

set CWD=%cd%
set SDK_DIR=%CWD%\sdk
set SDK_VERSION=20240805

set PATH=%SDK_DIR%\bin;%PATH%

git submodule update -i --recursive

set BRANCH=
for /f %%I in ('git rev-parse --abbrev-ref HEAD 2^> NUL') do set BRANCH=%%I

set COMMIT=
for /f %%i in ('git rev-parse --short^=8 HEAD') do set COMMIT=%%i

if not exist "sdk\" ( 
    curl -OL "https://github.com/friction2d/friction-sdk/releases/download/%SDK_VERSION%/friction-sdk-%SDK_VERSION%-windows-x64.7z"
    7z x friction-sdk-%SDK_VERSION%-windows-x64.7z
)

if exist "build\" (
    @RD /S /Q build
)
mkdir build

cd "%CWD%\build"
mkdir output

cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%SDK_DIR% -DBUILD_ENGINE=OFF -DFRICTION_OFFICIAL_RELEASE=%REL% -DWIN_DEPLOY=ON -DGIT_COMMIT=%COMMIT% -DGIT_BRANCH=%BRANCH% ..
set /p VERSION=<version.txt
cmake --build . --config Release

if "%REL%" == "OFF" (
    set VERSION="%VERSION%-%COMMIT%"
)

set OUTPUT_DIR="%CWD%\build\output\friction-%VERSION%"
mkdir "%OUTPUT_DIR%"
mkdir "%OUTPUT_DIR%\audio"
mkdir "%OUTPUT_DIR%\platforms"

copy "%CWD%\build\src\core\release\frictioncore.dll" "%OUTPUT_DIR%\"
copy "%CWD%\build\src\ui\release\frictionui.dll" "%OUTPUT_DIR%\"
copy "%CWD%\build\src\app\release\friction.exe" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\svgo-win.exe" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\skia.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\Qt5Core.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\Qt5Gui.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\Qt5Multimedia.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\Qt5Network.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\Qt5OpenGL.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\Qt5PrintSupport.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\Qt5Qml.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\Qt5Svg.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\Qt5Widgets.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\Qt5Xml.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\qscintilla2_qt5.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\plugins\audio\qtaudio_wasapi.dll" "%OUTPUT_DIR%\audio\"
copy "%SDK_DIR%\plugins\platforms\qwindows.dll" "%OUTPUT_DIR%\platforms\"
copy "%SDK_DIR%\bin\avcodec-58.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\avdevice-58.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\avformat-58.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\avutil-56.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\swresample-3.dll" "%OUTPUT_DIR%\"
copy "%SDK_DIR%\bin\swscale-5.dll" "%OUTPUT_DIR%\"

cd "%CWD%\build\output"

7z a -mx9 friction-%VERSION%-windows-x64.7z friction-%VERSION%
