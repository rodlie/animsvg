# Build Friction on Windows

Instructions for building Friction on Windows *(10)*.

## Requirements

* [Visual Studio 2017 Community](https://visualstudio.microsoft.com/vs/older-downloads/)
* [LLVM](https://github.com/llvm/llvm-project/releases/download/llvmorg-15.0.7/LLVM-15.0.7-win64.exe)
* [CMake](https://cmake.org/files/v3.26/cmake-3.26.4-windows-x86_64.msi)
* [Python](https://www.python.org/ftp/python/3.11.3/python-3.11.3-amd64.exe)
* [Qt 5.12.12](https://download.qt.io/archive/qt/5.12/5.12.12/qt-opensource-windows-x86-5.12.12.exe)


## Get the source

```
git clone --recurse-submodules https://github.com/friction2d/friction
```

## Get ffmpeg

[Download](https://sourceforge.net/projects/friction/files/source/friction-ffmpeg-444-bin.7z/download) a precompiled version of ffmpeg *(you can build it yourself, see our [mxe](https://github.com/friction2d/mxe) repo)*.

Extract and move the `ffmpeg` folder to `friction\src\`.

## Setup

Start the `Qt 5.12.12 (MSVC 2017 64-bit)` console and run

```
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
```

*Note: Make sure `python` and `cmake` is in `PATH`*

## Build skia

```
cd friction\src\skia
python tools/git-sync-deps
bin\gn.exe gen out/build --args="is_official_build=true is_debug=false extra_cflags=[\"-Wno-error\",\"/MD\",\"/O2\"] target_os=\"windows\" host_os=\"win\" current_os=\"win\" target_cpu=\"x64\" clang_win=\"C:\Program Files\LLVM\" cc=\"clang-cl\" cxx=\"clang-cl\" skia_use_system_expat=false skia_use_system_icu=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_harfbuzz=false" --ide=vs
```
```
cd out\build
msbuild all.sln
```

## Build qscintilla

Download [qscintilla](https://www.riverbankcomputing.com/static/Downloads/QScintilla/2.13.4/QScintilla_src-2.13.4.tar.gz) then extract and move to ``friction\src\``.

```
cd friction\src\qscintilla\src
qmake CONFIG+=release
nmake
```

## Build Friction

```
cd friction
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A x64 -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```
