@echo off

REM # BUILD SKIA FOR FRICTION
REM # Copyright (c) Friction contributors
REM # GPLv3+

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\Common7\Tools\VsDevCmd.bat" %*

set PATH=%ProgramFiles%\Python311;%ProgramFiles%\Git\bin;%PATH%

python.exe tools\git-sync-deps
bin\gn.exe gen out/build --args="is_official_build=true is_debug=false extra_cflags=[\"-Wno-error\",\"/MD\",\"/O2\"] target_os=\"windows\" host_os=\"win\" current_os=\"win\" target_cpu=\"x64\" clang_win=\"C:\Program Files\LLVM\" cc=\"clang-cl\" cxx=\"clang-cl\" skia_use_system_expat=false skia_use_system_icu=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_harfbuzz=false" --ide=vs
cd out\build
msbuild all.sln
