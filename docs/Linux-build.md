# Linux

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

## Get the source

Clone the repository, this might take a while.

```
git clone --recurse-submodules https://github.com/friction2d/friction
```

## Build libtcmalloc

Build a custom version of ``libtcmalloc``:

```
cd friction/src/gperftools
./autogen.sh
./configure --disable-shared
make -j4
```

## Build skia

Build a custom version of ``skia``:

```
cd friction/src/skia
python3 tools/git-sync-deps
bin/gn gen out/build --args='is_official_build=true is_debug=false extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false'
ninja -C out/build -j4 skia
```

## Build Friction

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
