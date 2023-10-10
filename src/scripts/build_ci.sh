#!/bin/bash
#
# Friction - https://friction.graphics
#
# Copyright (c) Friction contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# Friction CI (Ubuntu)

set -e -x

CI=${CI:-0}
APT=${APT:-0}
PC=${PC:-""}

if [ "${APT}" = 1 ]; then
sudo apt update -y
sudo apt install -y \
curl \
git \
clang \
build-essential \
libtool \
autoconf \
automake \
cmake \
python3 \
ninja-build \
tree \
libfontconfig1-dev \
libfreetype-dev \
libavcodec-dev \
libavformat-dev \
libavutil-dev \
libqscintilla2-qt5-dev \
libqt5opengl5-dev \
libqt5svg5-dev \
libswresample-dev \
libswscale-dev \
libunwind-dev \
qtbase5-dev-tools \
qtbase5-dev \
qtdeclarative5-dev-tools \
qtdeclarative5-dev \
qtmultimedia5-dev \
qttools5-dev-tools \
libexpat1-dev \
libfreetype-dev \
libjpeg-turbo8-dev \
libpng-dev \
libwebp-dev \
zlib1g-dev \
libicu-dev \
libharfbuzz-dev
fi

if [ "${CI}" = 1 ]; then
    git submodule update -i
fi

if [ "${PC}" != "" ]; then
    export PKG_CONFIG_PATH="${PC}:${PKG_CONFIG_PATH}"
fi

CWD=`pwd`
MKJOBS=${MKJOBS:-4}
COMMIT=`git rev-parse --short HEAD`
BRANCH=`git rev-parse --abbrev-ref HEAD`

if [ ! -f "${CWD}/src/gperftools/.libs/libtcmalloc.a" ]; then
    cd ${CWD}/src/gperftools
    ./autogen.sh
    CC=clang CXX=clang++ ./configure --disable-shared
    make -j${MKJOBS}
fi

if [ ! -f "${CWD}/src/skia/out/build/libskia.a" ]; then
    cd ${CWD}/src/skia
    if [ ! -f "bin/gn" ]; then
        (cd bin ; python3 fetch-gn)
    fi
    bin/gn gen out/build --args='is_official_build=true is_debug=false cc="clang" cxx="clang++" extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=true skia_use_system_freetype2=true skia_use_system_libjpeg_turbo=true skia_use_system_libpng=true skia_use_system_libwebp=true skia_use_system_zlib=true skia_use_system_icu=true skia_use_system_harfbuzz=true skia_use_dng_sdk=false'
    ninja -C out/build -j${MKJOBS} skia
fi

cd ${CWD}
rm -rf build || true
mkdir build
cd build

cmake -G Ninja \
-DUSE_SKIA_SYSTEM_LIBS=ON \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=/usr \
-DCMAKE_CXX_COMPILER=clang++ \
-DCMAKE_C_COMPILER=clang \
-DGIT_COMMIT=${COMMIT} \
-DGIT_BRANCH=${BRANCH} \
..
cmake --build .
cpack -G DEB
