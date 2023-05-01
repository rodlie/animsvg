# Friction

2D animation software for Linux.

See our issue [tracker](https://github.com/friction2d/friction/issues) or [forum](https://github.com/friction2d/friction/discussions) for more information.

We are working on our first release (0.9.0), see progress [here](https://github.com/friction2d/friction/milestone/1).

## System requirements

* Minimum screen resolution of 1280 x 800 
* OpenGL 3.3 compatible graphics card and driver

## Downloads

### Releases

No releases are currently available.

### Snapshots

Snapshots for selected platforms will soon be available.

## FAQ

### is this a fork?

Yes, this is a fork of [enve](https://github.com/MaurycyLiebner/enve).

# License

Friction is copyright &copy; Friction [developers](https://github.com/friction2d/friction/graphs/contributors)

[enve](https://github.com/MaurycyLiebner/enve) is copyright &copy; 2016-2022 Maurycy Liebner

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

**This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the [GNU General Public License](LICENSE.md) for more details.**

# Build

Generic build instructions for supported platforms.

## Requirements

* pkg-config
* automake
* autoconf
* python3
* ninja
* cmake
* Qt5 *(5.12+)*
    * Gui
    * Concurrent
    * Widgets
    * OpenGL
    * Multimedia
    * Qml
    * Xml
    * Svg
* libmypaint
* quazip
* qscintilla
* ffmpeg
    * libavformat
    * libavcodec
    * libavutil
    * libswscale
    * libswresample
* libunwind *(Linux)*


## Linux

General instructions for building on Linux *(tested on Ubuntu Jammy)*.

### Ubuntu

```
sudo apt install -y \
build-essential \
git \
libtool \
autoconf \
automake \
cmake \
python3 \
python-is-python3 \
ninja-build \
libfontconfig1-dev \
libfreetype-dev \
libavcodec-dev \
libavformat-dev \
libavutil-dev \
libmypaint-dev \
libqscintilla2-qt5-dev \
libqt5opengl5-dev \
libqt5svg5-dev \
libquazip5-dev \
libswresample-dev \
libswscale-dev \
libunwind-dev \
qt5-image-formats-plugins \
libqt5multimedia5-plugins \
qtbase5-dev-tools \
qtbase5-dev \
qtdeclarative5-dev-tools \
qtdeclarative5-dev \
qtmultimedia5-dev \
qttools5-dev-tools
```

### Get the source

Clone the repository, this might take a while.

```
git clone --recurse-submodules https://github.com/friction2d/friction
```

### Build libtcmalloc

Build a custom version of ``libtcmalloc``:

```
cd friction/src/gperftools
./autogen.sh
./configure --disable-shared
make -j4
```

### Build skia

Build a custom version of ``skia``:

```
cd friction/src/skia
python3 tools/git-sync-deps
bin/gn gen out/build --args='is_official_build=true is_debug=false extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false'
ninja -C out/build -j4 skia
```

### Build Friction

```
cd friction
mkdir build
cd build
```

Note that if you are not using Ubuntu you will need to set paths for ``qscintilla`` and ``quazip``:

```
cmake \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=/usr \
-DQUAZIP_INCLUDE_DIRS=<PATH_TO_QUAZIP_INCLUDE_FILES> \
-DQUAZIP_LIBRARIES_DIRS=<PATH_TO_LIBS> \
-DQUAZIP_LIBRARIES=<QUAZIP_LIBRARY_NAME> \
-DQSCINTILLA_INCLUDE_DIRS=<PATH_TO_QSCINTILLA_INCLUDE_DIR> \
-DQSCINTILLA_LIBRARIES_DIRS=<PATH_TO_LIBS> \
-DQSCINTILLA_LIBRARIES=<QSCINTILLA_LIBRARY_NAME> \
..
```

else just use:

```
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
```

Now build:

```
make -j4
```

Run:

```
cp src/app/friction .
cp src/core/libfrictioncore.so.0 .
./friction
```

Install:

```
make DESTDIR=<SOME_TEMP_DIR> install 
```

or make a DEB package:

```
make package
```

## macOS

**macOS is currently unsupported until issue #16 has been fixed.**

These instructions assume you have macOS High Sierra (10.13) or greater with Qt 5.12.12 *(through the official online installer)* and macports installed running on an Intel CPU.

### Dependencies

Install the following packages if not already installed:

```
sudo port install libmypaint ffmpeg ninja cmake automake autoconf libtool pkgconfig
```

### Get the source

Clone this repository, this might take a while.

```
git clone --recurse-submodules https://github.com/friction2d/friction
```

### Build libtcmalloc

We need to build a custom version of ``libtcmalloc``:

```
cd friction/src/gperftools
./autogen.sh
./configure --disable-shared
make -j4
```

### Build skia

Now we need to build ``skia`` (might take a while).

```
cd friction/src/skia
python3 tools/git-sync-deps
bin/gn gen out/build --args='is_official_build=true is_debug=false extra_cflags=["-Wno-error"] skia_use_system_expat=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false'
ninja -C out/build -j4 skia
```

### Build quazip

Download [quazip](https://github.com/stachenov/quazip/archive/refs/tags/v1.4.tar.gz), then extract and move to ``friction/quazip``.

```
cd friction/quazip
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$HOME/Qt/5.12.12/clang_64 .. && make
```

### Build qscintilla

Download [qscintilla](https://www.riverbankcomputing.com/static/Downloads/QScintilla/2.13.4/QScintilla_src-2.13.4.tar.gz), then extract and move to ``friction/qscintilla``.

```
cd friction/qscintilla/src
$HOME/Qt/5.12.12/clang_64/bin/qmake && make
```

### Build Friction

```
cd friction
mkdir build && cd build
cmake \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_PREFIX_PATH=$HOME/Qt/5.12.12/clang_64 \
-DQUAZIP_INCLUDE_DIRS=`pwd`/../quazip/quazip \
-DQUAZIP_LIBRARIES_DIRS=`pwd`/../quazip/build/quazip \
-DQUAZIP_LIBRARIES=quazip1-qt5 \
-DQSCINTILLA_INCLUDE_DIRS=`pwd`/../qscintilla/src \
-DQSCINTILLA_LIBRARIES_DIRS=`pwd`/../qscintilla/src \
-DQSCINTILLA_LIBRARIES=qscintilla2_qt5 \
..
make -j4
$HOME/Qt/5.12.12/clang_64/bin/macdeployqt src/app/friction.app
```
