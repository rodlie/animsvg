# macOS

These instructions assume you have macOS High Sierra (10.13) or greater with Qt 5.12.12 *(through the official online installer)* and macports installed running on an Intel CPU.

**NOTE! macOS is not supported anymore**

## Dependencies

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

Install the following packages if not already installed:

```
sudo port install libmypaint ffmpeg ninja cmake automake autoconf libtool pkgconfig
```

## Get the source

Clone this repository, this might take a while.

```
git clone --recurse-submodules https://github.com/friction2d/friction
```

## Build libtcmalloc

We need to build a custom version of ``libtcmalloc``:

```
cd friction/src/gperftools
./autogen.sh
./configure --disable-shared
make -j4
```

## Build skia

Now we need to build ``skia`` (might take a while).

```
cd friction/src/skia
python3 tools/git-sync-deps
bin/gn gen out/build --args='is_official_build=true is_debug=false extra_cflags=["-Wno-error"] skia_use_system_expat=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false'
ninja -C out/build -j4 skia
```

## Build quazip

Download [quazip](https://github.com/stachenov/quazip/archive/refs/tags/v1.4.tar.gz), then extract and move to ``friction/quazip``.

```
cd friction/quazip
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$HOME/Qt/5.12.12/clang_64 .. && make
```

## Build qscintilla

Download [qscintilla](https://www.riverbankcomputing.com/static/Downloads/QScintilla/2.13.4/QScintilla_src-2.13.4.tar.gz), then extract and move to ``friction/qscintilla``.

```
cd friction/qscintilla/src
$HOME/Qt/5.12.12/clang_64/bin/qmake && make
```

## Build Friction

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
