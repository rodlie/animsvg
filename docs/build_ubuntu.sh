#!/bin/bash
#set -e -x

APT=${APT:-0}

if [ "${APT}" = 1 ]; then
sudo apt update -y
sudo apt install -y \
build-essential \
autoconf \
automake \
cmake \
python2 \
ninja-build \
tree \
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
qtwebengine5-dev \
libqt5xmlpatterns5-dev || exit 1
fi

git submodule update -i --recursive || exit 1

CWD=`pwd`

sudo ln -sf /usr/bin/python2 /usr/bin/python

cd ${CWD}/src/gperftools
./autogen.sh
./configure --disable-static || exit 1
make -j2 || exit 1

cd ${CWD}/src/skia
python tools/git-sync-deps
bin/gn gen out/build --args='is_official_build=true is_debug=false extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false' || exit 1
ninja -C out/build -j2 skia || exit 1

cd ${CWD}
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr .. || exit 1
make -j2 || exit 1
make DESTDIR=`pwd`/enve2d install || exit 1
tree enve2d
