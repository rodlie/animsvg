# Friction

[Friction](https://friction.graphics) is a powerful and versatile motion graphics application that allows you to create stunning vector and raster animations for web and video platforms with ease.

## Contribute

We accept any contributions (in form of a PR). Before submitting a PR it's recommended that you communicate with the developers first (in [issues](https://github.com/friction2d/friction/issues) or [discussions](https://github.com/friction2d/friction/discussions)). Someone might already be working on the same feature/issue, or for some reason the feature is not wanted.

It's always preferred to submit PR's against the `main` branch. If your feature is experimental, a new branch could be made available for further development before entering `main`.

## Branches and versions

Friction uses `X.Y.Z` version numbers and `X.Y` branches.

* `X` = Major
* `Y` = Minor
* `Z` = Patch

Branch `main` is always the current branch for the next `X` or `Y` release. The `main` branch should not be considered stable, but usable.

A new stable branch is cut from `main` on each `X` or `Y` release and is maintained until a new stable branch is created.

The next `Z` release usually comes from the latest stable branch (`vX.Y`).

Features under development should be kept in it's own branch until ready to be merged with `main`.

## Build

Generic build instructions.

### Requirements on Linux

* pkg-config
* ninja
* python3
* cmake *(3.12+)*
* clang *(7+)*
* Qt *(5.15)*
    * Gui
    * Widgets
    * OpenGL
    * Multimedia
    * Qml
    * Xml
* qscintilla
* ffmpeg *(4.2)*
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
* Qt 5.15.13 libraries and headers in `friction\src\qt`
  * See [configure_qt5.bat](src/scripts/configure_qt5.bat) and [build_qt5.bat](src/scripts/build_qt5.bat)
* QScintilla 2.14.1 libraries and headers in `friction\src\qscintilla`
  * See [build_qscintilla.bat](src/scripts/build_qscintilla.bat)
* FFmpeg 4.2.9 libraries and headers in `friction\src\ffmpeg`
  * See [build_mxe_ffmpeg.sh](src/scripts/build_mxe_ffmpeg.sh) and [mxe](https://github.com/friction2d/mxe)

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

All requirements must be installed in the correct folders, this is an example and should be adjusted to fit your enviroment.

```
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\Common7\Tools\VsDevCmd.bat"

set QT_DIR=%cd%\src\qt
set PATH=C:\Python;%ProgramFiles%\CMake\bin;%ProgramFiles%\Git\bin;%QT_DIR%\bin;%PATH%

mkdir build
cd build

cmake -G "Visual Studio 15 2017" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QT_DIR%

cmake --build . --config Release
```

## License

Copyright &copy; Friction [contributors](https://github.com/friction2d/friction/graphs/contributors).

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

**This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the [GNU General Public License](LICENSE.md) for more details.**
