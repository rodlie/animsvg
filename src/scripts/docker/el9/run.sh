#!/bin/bash
#
# Friction - https://github.com/friction2d/friction
#
# Copyright (c) Friction developers
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
g++ --version
cmake --version
python --version
qmake-qt5 --version || qmake --version
env
uname -a
cat /etc/os-release

SNAP=${SNAP:-1}
REL=${REL:-0}
MKJOBS=${MKJOBS:-4}

FRICTION_ROOT="/friction2d"
FRICTION_DIR="${FRICTION_ROOT}/friction"
FRICTION_SRC_DIR="${FRICTION_DIR}/src"
FRICTION_SNAP="/snapshots"
FRICTION_DIST="${FRICTION_SNAP}/distfiles"

QUAZIP_V="1.4"
QSCINTILLA_V="2.13.4"
FFMPEG_V="4.4.4"
UNWIND_V="1.6.2"
GPERF_V="e590eba"
SKIA_V="4fcb5c225a"

SF_NET_SRC="https://sourceforge.net/projects/friction/files/source"
SF_NET_SNAP="https://sourceforge.net/projects/friction/files/snapshots"

export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:${PKG_CONFIG_PATH}"

if [ ! -d "${FRICTION_ROOT}" ]; then
    mkdir -p ${FRICTION_ROOT}
fi

if [ ! -d "${FRICTION_DIST}" ]; then
    mkdir -p ${FRICTION_DIST}
fi

if [ ! -d "${FRICTION_SRC_DIR}" ]; then
    cd ${FRICTION_ROOT}
    git clone https://github.com/friction2d/friction
fi

if [ ! -f "${FRICTION_SRC_DIR}/quazip-${QUAZIP_V}/build/quazip/libquazip1-qt5.a" ]; then
    if [ -d "${FRICTION_SRC_DIR}/quazip-${QUAZIP_V}" ]; then
        rm -rf ${FRICTION_SRC_DIR}/quazip-${QUAZIP_V}
    fi
    if [ ! -f "${FRICTION_DIST}/quazip.tar.gz" ]; then
        curl -L -k "https://github.com/stachenov/quazip/archive/refs/tags/v${QUAZIP_V}.tar.gz" --output ${FRICTION_DIST}/quazip.tar.gz
    fi
    cd ${FRICTION_SRC_DIR}
    tar xf ${FRICTION_DIST}/quazip.tar.gz
    cd quazip-${QUAZIP_V}
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF .. && make
fi

if [ ! -f "${FRICTION_SRC_DIR}/QScintilla_src-${QSCINTILLA_V}/src/libqscintilla2_qt5.a" ]; then
    if [ -d "${FRICTION_SRC_DIR}/QScintilla_src-${QSCINTILLA_V}" ]; then
        rm -rf ${FRICTION_SRC_DIR}/QScintilla_src-${QSCINTILLA_V}
    fi
    if [ ! -f "${FRICTION_DIST}/qscintilla.tar.gz" ]; then
        curl -L -k "https://www.riverbankcomputing.com/static/Downloads/QScintilla/${QSCINTILLA_V}/QScintilla_src-${QSCINTILLA_V}.tar.gz" --output ${FRICTION_DIST}/qscintilla.tar.gz
    fi
    cd ${FRICTION_SRC_DIR}
    tar xf ${FRICTION_DIST}/qscintilla.tar.gz
    cd QScintilla_src-${QSCINTILLA_V}/src
    qmake-qt5 CONFIG+=staticlib && make
fi

if [ ! -f "/usr/local/lib/pkgconfig/libavcodec.pc" ]; then
    if [ -d "${FRICTION_SRC_DIR}/ffmpeg-${FFMPEG_V}" ]; then
        rm -rf ${FRICTION_SRC_DIR}/ffmpeg-${FFMPEG_V}
    fi
    if [ ! -f "${FRICTION_DIST}/ffmpeg.tar.xz" ]; then
        curl -L -k "https://ffmpeg.org/releases/ffmpeg-${FFMPEG_V}.tar.xz" --output ${FRICTION_DIST}/ffmpeg.tar.xz
    fi
    cd ${FRICTION_SRC_DIR}
    tar xf ${FRICTION_DIST}/ffmpeg.tar.xz
    cd ffmpeg-${FFMPEG_V}
    CFLAGS="-fPIC" ./configure --pkg-config-flags="--static" --disable-shared --enable-gpl --enable-version3 --enable-runtime-cpudetect --prefix=/usr/local
    make -j${MKJOBS}
    make install
fi

if [ ! -f "/usr/local/lib/pkgconfig/libunwind.pc" ]; then
    if [ -d "${FRICTION_SRC_DIR}/libunwind-${UNWIND_V}" ]; then
        rm -rf ${FRICTION_SRC_DIR}/libunwind-${UNWIND_V}
    fi
    if [ ! -f "${FRICTION_DIST}/libunwind.tar.gz" ]; then
        curl -L -k "http://download.savannah.nongnu.org/releases/libunwind/libunwind-${UNWIND_V}.tar.gz" --output ${FRICTION_DIST}/libunwind.tar.gz
    fi
    cd ${FRICTION_SRC_DIR}
    tar xf ${FRICTION_DIST}/libunwind.tar.gz
    cd libunwind-${UNWIND_V}
    ./configure --disable-shared --disable-minidebuginfo --disable-tests --prefix=/usr/local
    make -j${MKJOBS}
    make install
fi

if [ ! -f "${FRICTION_SRC_DIR}/gperftools/.libs/libtcmalloc.a" ]; then
    if [ ! -f "${FRICTION_DIST}/gperftools.tar.xz" ]; then
        curl -k -L "${SF_NET_SRC}/gperftools-${GPERF_V}.tar.xz/download" --output ${FRICTION_DIST}/gperftools.tar.xz
    fi
    if [ -d "${FRICTION_SRC_DIR}/gperftools-${GPERF_V}" ]; then
        rm -rf ${FRICTION_SRC_DIR}/gperftools-${GPERF_V}
    fi
    if [ -d "${FRICTION_SRC_DIR}/gperftools" ]; then
        rm -rf ${FRICTION_SRC_DIR}/gperftools
    fi
    cd ${FRICTION_SRC_DIR}
    tar xf ${FRICTION_DIST}/gperftools.tar.xz
    mv gperftools-${GPERF_V} gperftools
    cd gperftools
    ./autogen.sh
    ./configure --disable-shared
    make -j${MKJOBS}
fi

if [ ! -f "${FRICTION_SRC_DIR}/skia/out/build/libskia.a" ]; then
    if [ ! -f "${FRICTION_DIST}/skia.tar.xz" ]; then
        curl -k -L "${SF_NET_SRC}/skia-${SKIA_V}-minimal.tar.xz/download" --output ${FRICTION_DIST}/skia.tar.xz
    fi
    if [ -d "${FRICTION_SRC_DIR}/skia-${SKIA_V}" ]; then
        rm -rf ${FRICTION_SRC_DIR}/skia-${SKIA_V}
    fi
    if [ -d "${FRICTION_SRC_DIR}/skia" ]; then
        rm -rf ${FRICTION_SRC_DIR}/skia
    fi
    cd ${FRICTION_SRC_DIR}
    tar xf ${FRICTION_DIST}/skia.tar.xz
    mv skia-${SKIA_V} skia
    cd skia
    bin/gn gen out/build --args='is_official_build=true is_debug=false extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false'
    ninja -C out/build -j${MKJOBS} skia
fi

cd ${FRICTION_DIR}

COMMIT=`git rev-parse --short HEAD`
VERSION=`cat ${FRICTION_DIR}/CMakeLists.txt | sed '/friction2d VERSION/!d;s/)//' | awk '{print $3}'`
TIMESTAMP=${TIMESTAMP:-`date +%Y%m%d`}
YEAR=${YEAR:-`date +%Y`}
MONTH=${MONTH:-`date +%m`}
DISTRO_ID="el" #`cat /etc/os-release | sed '/^ID=/!d;s/ID=//;s/"//g'`
DISTRO_VERSION="9" #`cat /etc/os-release | sed '/^VERSION_ID=/!d;s/VERSION_ID=//;s/"//g'`
DISTRO_PRETTY="Enterprise Linux 9" #`cat /etc/os-release | sed '/^PRETTY_NAME=/!d;s/PRETTY_NAME=//;s/"//g'`

if [ -d "${FRICTION_DIR}/build" ]; then
    rm -rf ${FRICTION_DIR}/build
fi

mkdir build
cd build

cmake -G Ninja \
-DCMAKE_BUILD_TYPE=Release \
-DQUAZIP_INCLUDE_DIRS=${FRICTION_SRC_DIR}/quazip-${QUAZIP_V}/quazip \
-DQUAZIP_LIBRARIES_DIRS=${FRICTION_SRC_DIR}/quazip-${QUAZIP_V}/build/quazip \
-DQUAZIP_LIBRARIES=quazip1-qt5 \
-DQSCINTILLA_INCLUDE_DIRS=${FRICTION_SRC_DIR}/QScintilla_src-${QSCINTILLA_V}/src \
-DQSCINTILLA_LIBRARIES_DIRS=${FRICTION_SRC_DIR}/QScintilla_src-${QSCINTILLA_V}/src \
-DQSCINTILLA_LIBRARIES=qscintilla2_qt5 ..
cmake --build .
strip -s src/app/friction
strip -s src/core/libfrictioncore.so.${VERSION}
cpack -G RPM

PKG="friction-${DISTRO_ID}-${DISTRO_VERSION}.rpm"
if [ "${REL}" = 1 ]; then
    PKG="friction-${VERSION}-${DISTRO_ID}${DISTRO_VERSION}.rpm"
elif [ "${SNAP}" = 1 ]; then
    PKG="friction-${TIMESTAMP}-${VERSION}-${COMMIT}-${DISTRO_ID}${DISTRO_VERSION}.rpm"
fi
mv friction.rpm ${PKG}

if [ ! -d "${FRICTION_SNAP}/${YEAR}/${MONTH}" ]; then
    mkdir -p ${FRICTION_SNAP}/${YEAR}/${MONTH}
fi

cp ${PKG} ${FRICTION_SNAP}/${YEAR}/${MONTH}/

(cd ${FRICTION_SNAP} ;
    #ln -sf ${YEAR}/${MONTH}/${PKG} friction-latest-${DISTRO_ID}${DISTRO_VERSION}.rpm
    echo "* [Latest download for ${DISTRO_PRETTY}](https://sourceforge.net/projects/friction/files/snapshots/${YEAR}/${MONTH}/${PKG}/download)" > friction-latest-${DISTRO_ID}${DISTRO_VERSION}.md
)
