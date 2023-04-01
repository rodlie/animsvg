#!/bin/bash
#
# enve2d - https://github.com/enve2d
#
# Copyright (c) enve2d developers
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

set -e -x

APT=${APT:-0}
PYSYM=${PYSYM:-0}

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
qtwebengine5-dev
fi

git submodule update -i --recursive

CWD=`pwd`

if [ "${PYSYM}" = 1 ]; then
    sudo ln -sf /usr/bin/python2 /usr/bin/python
fi

cd ${CWD}/src/gperftools
./autogen.sh
./configure --disable-shared
make -j2

cd ${CWD}/src/skia
python tools/git-sync-deps || true
bin/gn gen out/build --args='is_official_build=true is_debug=false extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false'
ninja -C out/build -j2 skia

cd ${CWD}
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
make -j2
make package
make DESTDIR=`pwd`/enve2d install
tree enve2d
