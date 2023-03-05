# enve2d

This is a community fork of [enve](https://github.com/MaurycyLiebner/enve).

## Build

## Requirements

* automake
* autoconf
* python2
* ninja
* cmake
* Qt5
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
* libavformat
* libavcodec
* libavutil
* libswscale
* libswresample

## Linux

```
git clone --recurse-submodules https://github.com/enve2d/enve2d
```

```
cd enve2d/src/gperftools
./autogen.sh
./configure --disable-static
make -j4
```

```
cd enve2d/src/skia
python tools/git-sync-deps
bin/gn gen out/build --args='is_official_build=true is_debug=false extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false'
ninja -C out/build -j4 skia
```

```
cd enve2d
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```
