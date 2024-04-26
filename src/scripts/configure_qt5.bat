@echo off

REM ### CONFIGURE QT 5.15 FOR FRICTION
REM # Copyright (c) Friction contributors
REM # GPLv3+

set CWD=%cd%
set SRC_DIR=%CWD%\src
set QT_DIR=%SRC_DIR%\qt
set QT_V=5.15.13

set PATH=C:\Python;%ProgramFiles%\CMake\bin;%PATH%

cd "%SRC_DIR%\qt-everywhere-src-%QT_V%"
configure.bat -release -prefix "%QT_DIR%" -mp -platform win32-msvc -opensource -confirm-license -nomake examples -nomake tests -opengl desktop -optimize-size -no-pch -no-glib -no-dbus -no-avx2 -no-avx512 -strip -no-openssl -no-gif -no-wmf -no-ico -schannel -skip qt3d -skip qtcharts -skip qtconnectivity -skip qtdatavis3d -skip qtdoc -skip qtgamepad -skip qtgraphicaleffects -skip qtlocation -skip qtlottie -skip qtnetworkauth -skip qtpurchasing -skip qtquick3d -skip qtquickcontrols -skip qtquickcontrols2 -skip qtquicktimeline -skip qtremoteobjects -skip qtscript -skip qtscxml -skip qtsensors -skip qtserialbus -skip qtserialport -skip qtspeech -skip qtvirtualkeyboard -skip qtwebchannel -skip qtwebengine -skip qtwebglplugin -skip qtwebsockets -skip qtwebview -skip qtxmlpatterns -skip qttools
