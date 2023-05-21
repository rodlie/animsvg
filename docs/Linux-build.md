# Build Friction on GNU/Linux

Generic build instructions for GNU/Linux.

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
* libunwind

### Ubuntu *(as an example)*

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

```
git clone --recurse-submodules https://github.com/friction2d/friction
```

*You may drop the use of ``--recurse-submodules`` and replace ``src/gperftools`` and ``src/skia`` with tarballs available on [SourceForge](https://sourceforge.net/projects/friction/files/source/).*

## Build libtcmalloc

```
cd src/gperftools
./autogen.sh
./configure --disable-shared
make -j4
```

## Build skia


```
cd src/skia
```

### Sync

Skia requires several third-party git modules (4GB+), syncing them will take a while. We have skia minimal tarballs available on [SourceForge](https://sourceforge.net/projects/friction/files/source/) if you want to save disk space and bandwidth. Else:

```
python3 tools/git-sync-deps
```

### Build

```
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
cmake -G Ninja \
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
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
```

Now build:

```
cmake --build .
```

## Install Friction

```
make install
```

Add ``DESTDIR=<SOME_TEMP_DIR>`` if you want to install to a different/temporarily folder, else it will install to ``DCMAKE_INSTALL_PREFIX``.

*It's not recommended that you install Friction this way as an end user.*

## Package Friction

The recommended way to install Friction is to package it after build, depending on your distribution select either `DEB` or `RPM`.

### DEB

```
cpack -G DEB
```

### RPM

```
cpack -G RPM
```

You can now install and maintain Friction with the system package manager.
