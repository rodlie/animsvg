# Friction

[![Windows](https://github.com/friction2d/friction/actions/workflows/windows.yml/badge.svg)](https://github.com/friction2d/friction/actions/workflows/windows.yml?query=branch%3Amain) [![Linux](https://github.com/friction2d/friction/actions/workflows/linux.yml/badge.svg)](https://github.com/friction2d/friction/actions/workflows/linux.yml?query=branch%3Amain) ![GitHub commits since latest release](https://img.shields.io/github/commits-since/friction2d/friction/latest)

[Friction](https://friction.graphics) is a powerful and versatile motion graphics application that allows you to create vector and raster animations for web and video with ease.

![Screenshot of Friction 1.0.0 Beta 1](https://friction.graphics/assets/screenshots/100/friction-100b1-screenshot.png?v=1)

## Contribute

We accept any contributions, big or small. Before submitting a PR it's recommended that you communicate with the developers first (in [issues](https://github.com/friction2d/friction/issues) or [discussions](https://github.com/friction2d/friction/discussions)).

It's always preferred to submit PR's against the `main` branch.

## Branches and versions

Friction uses `X.Y.Z` version numbers and `vX.Y` branches.

* `X` = Major
* `Y` = Minor
* `Z` = Patch

Branch `main` is always the current branch for the next `X` or `Y` release.

A new stable branch is cut from `main` on each `X` or `Y` release and is maintained until a new stable branch is created. Patch (`Z`) releases comes from the parent stable branch (`vX.Y`).

Important fixes added to `main` will be backported to active stable branches when possible (and within reason).

## Build

Generic build instructions.

### Requirements on Linux

* pkg-config
* ninja
* python3
* cmake *(3.12+)*
* clang *(7+)*
* Qt *(5.15.x)*
    * Gui
    * Widgets
    * OpenGL
    * Multimedia
    * Qml
    * Xml
* qscintilla
* ffmpeg *(4.2.x)*
    * libavformat
    * libavcodec
    * libavutil
    * libswscale
    * libswresample
* libunwind
* expat
* harfbuzz
* freetype
* fontconfig
* libjpeg-turbo
* libpng
* libwebp
* zlib
* icu

### Requirements on Windows

* [Visual Studio 2017 Build Tools](https://aka.ms/vs/15/release/vs_buildtools.exe)
* `git` must be available in `PATH`
* `cmake` must be available in `PATH`
* `python3` must be available in `PATH`
* [LLVM](https://github.com/llvm/llvm-project/releases/download/llvmorg-15.0.7/LLVM-15.0.7-win64.exe) installed to default location
* Qt 5.15.14 libraries and headers in `friction\sdk`
  * See [configure_qt5.bat](src/scripts/configure_qt5.bat) and [build_qt5.bat](src/scripts/build_qt5.bat)
* QScintilla 2.14.1 libraries and headers in `friction\sdk`
  * See [build_qscintilla.bat](src/scripts/build_qscintilla.bat)
* FFmpeg 4.2.9 libraries and headers in `friction\sdk`
  * See [build_mxe_ffmpeg.sh](src/scripts/build_mxe_ffmpeg.sh) and [mxe](https://github.com/friction2d/mxe)

Binary SDK available [here](https://github.com/friction2d/friction-sdk/releases).

### Get

```
git clone --recurse-submodules https://github.com/friction2d/friction
```

Or download a release tarball *(friction-VERSION.tar.bz2)*.

### Build on Linux

```
mkdir build
cd build
cmake -G Ninja \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=/usr \
-DCMAKE_CXX_COMPILER=clang++ \
-DCMAKE_C_COMPILER=clang \
..
```
Note that on some configurations you may need to specify `qscintilla` paths:
```
-DQSCINTILLA_INCLUDE_DIRS=<PATH_TO_QSCINTILLA_INCLUDE_DIR> \
-DQSCINTILLA_LIBRARIES_DIRS=<PATH_TO_LIBS> \
-DQSCINTILLA_LIBRARIES=<QSCINTILLA_LIBRARY_NAME> \
```

Now build:

```
cmake --build . --config Release
```

### Build on Windows

All requirements must be installed in the correct folders, this is an example and should be adjusted to fit your environment.

```
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\Common7\Tools\VsDevCmd.bat"

set SDK_DIR=%cd%\sdk
set PATH=C:\Python;%ProgramFiles%\CMake\bin;%ProgramFiles%\Git\bin;%SDK_DIR%\bin;%PATH%

mkdir build
cd build

cmake -G "Visual Studio 15 2017" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%SDK_DIR% ..

cmake --build . --config Release
```

## License

Copyright &copy; Friction contributors.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

**This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the [GNU General Public License](LICENSE.md) for more details.**
