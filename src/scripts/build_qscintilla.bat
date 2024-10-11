@echo off

REM ### BUILD QSCINTILLA FOR FRICTION
REM # Copyright (c) Ole-André Rodlie and contributors
REM # GPLv3+

set CWD=%cd%
set SRC_DIR=%CWD%\src
set QT_DIR=%SRC_DIR%\qt
set QSCINTILLA_DIR=%SRC_DIR%\qscintilla

set PATH=%ProgramFiles%\CMake\bin;%QT_DIR%\bin;%PATH%

cd "%QSCINTILLA_DIR%\src"
%QT_DIR%\bin\qmake.exe CONFIG+=release
nmake
