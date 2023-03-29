# enve2d

This is a community fork of [enve](https://github.com/MaurycyLiebner/enve). A 2d animation application.

A lot of changes are currently happening under the hood, so don't expect anything usable until our first [release](https://github.com/enve2d/enve2d/milestone/1).

Features currently present will be checked, if they are broken or not fully implemented a review will be done:

* If they are broken check if a fix is feasible, else remove or disable
* If they are not fully implemented check if this is a feature worth implementing, else remove or disable

The goal is to end up with a [v1.0.0](https://github.com/enve2d/enve2d/milestone/7) release in the not too distant future.

See our issue [tracker](https://github.com/enve2d/enve2d/issues) for more information.

# License

Copyright &copy; 2023 enve2d developers

Copyright &copy; 2016-2022 Maurycy Liebner

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

# Build

Currently only macOS and Linux is supported.

## Requirements

* automake
* autoconf
* python *(v2)*
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
    * WebEngineWidgets
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

General instructions for building enve2d on Linux *(tested on Ubuntu Jammy)*.

### Ubuntu

```
sudo apt install -y \
build-essential \
autoconf \
automake \
cmake \
python2 \
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
qttools5-dev-tools \
qtwebengine5-dev
```

Clone the repository, this might take a while.

```
git clone --recurse-submodules https://github.com/enve2d/enve2d
```

Build a custom version of ``libtcmalloc``:

```
cd enve2d/src/gperftools
./autogen.sh
./configure --disable-shared
make -j4
```

Build ``skia``:

Note that python2 is required. ``/usr/bin/python`` should be a symlink to the python2 binary on your system.

On Ubuntu run:

```
sudo ln -sf /usr/bin/python2 /usr/bin/python
```

Now build ``skia`` (might take a while):

```
cd enve2d/src/skia
python tools/git-sync-deps
bin/gn gen out/build --args='is_official_build=true is_debug=false extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false'
ninja -C out/build -j4 skia
```

Now you are ready to build ``enve2d``:

```
cd enve2d
mkdir build
cd build
```

Note that if you are not using Ubuntu you will need to set paths for ``qscintilla``:

```
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DQSCINTILLA_LIBARIES="path/to/libraries" -DQSCINTILLA_INCLUDES="path/to/includes" ..
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
cp src/app/enve2d .
cp src/core/libenve2dcore.so.0 .
./enve2d
```

Install:

```
make DESTDIR=<SOME_DIR> install 
```

or make a DEB package:

```
make package
```

## macOS

These instructions asume you have macOS High Sierra (10.13) or greater with Qt 5.12.12 *(through the official online installer)* and macports installed running on an Intel CPU.

### Dependencies

Install the following packages if not already installed:

```
sudo port install libmypaint ffmpeg ninja cmake automake autoconf
```

### Get the source

Clone this repository, this might take a while.

```
git clone --recurse-submodules https://github.com/enve2d/enve2d
```

### Build libtcmalloc

We need to build a custom version of ``libtcmalloc``:

```
cd enve2d/src/gperftools
./autogen.sh
./configure --disable-shared
make -j4
```

### Build skia

First make sure that ``python`` in ``PATH`` points to a Python 2 binary.

Now we need to build ``skia`` (might take a while).

```
cd enve2d/src/skia
python tools/git-sync-deps
bin/gn gen out/build --args='is_official_build=true is_debug=false extra_cflags=["-Wno-error"] skia_use_system_expat=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false'
ninja -C out/build -j4 skia
```

### Build quazip

Download [quazip](https://github.com/stachenov/quazip/archive/refs/tags/v1.4.tar.gz), then extract and move to ``enve2d/quazip``.

```
cd enve2d/quazip
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$HOME/Qt/5.12.12/clang_64 .. && make
```

### Build qscintilla

Download [qscintilla](https://www.riverbankcomputing.com/static/Downloads/QScintilla/2.13.4/QScintilla_src-2.13.4.tar.gz), then extract and move to ``enve2d/qscintilla``.

```
cd enve2d/qscintilla/src
$HOME/Qt/5.12.12/clang_64/bin/qmake && make
```

### Build enve2d

```
cd enve2d
mkdir build && cd build
cmake \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_PREFIX_PATH=/$HOME/Qt/5.12.12/clang_64 \
-DQUAZIP_INCLUDE_DIRS=`pwd`/quazip/quazip \
-DQUAZIP_LIBRARIES_DIRS=`pwd`/quazip/build/quazip \
-DQUAZIP_LIBRARIES=quazip1-qt5 \
-DQSCINTILLA_INCLUDE_DIRS=`pwd`/qscintilla/src \
-DQSCINTILLA_LIBRARIES_DIRS=`pwd`/qscintilla/src \
-DQSCINTILLA_LIBRARIES=qscintilla2_qt5 \
..
make -j4
$HOME/Qt/5.12.12/clang_64/bin/macdeployqt src/app/enve2d.app
```
