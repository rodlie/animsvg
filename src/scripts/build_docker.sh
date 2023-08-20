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

# Deploy Friction on Ubuntu, Fedora, RHEL.

set -e -x
g++ --version
clang++ --version
cmake --version
python --version
qmake-qt5 --version || qmake --version
env
cat /etc/os-release

SNAP=${SNAP:-0}
REL=${REL:-0}
MKJOBS=${MKJOBS:-8}
CLANG=${CLANG:-1}

if [ "${REL}" = 0 ] && [ "${SNAP}" = 0 ]; then
    echo "REL or SNAP?"
    exit 1
fi

TIMESTAMP=${TIMESTAMP:-`date +%Y%m%d`}
YEAR=${YEAR:-`date +%Y`}
MONTH=${MONTH:-`date +%m`}
DAY=${DAY:-`date +%d`}

if [ "${REL}" = 1 ]; then
    SNAP=0
elif [ "${SNAP}" = 1 ]; then
    REL=0
fi

DISTRO_ID=`cat /etc/os-release | sed '/^ID=/!d;s/ID=//;s/"//g'`
if [ "${DISTRO_ID}" = "fedora" ]; then
    DISTRO_ID="fc"
elif [ "${DISTRO_ID}" = "ubuntu" ]; then
    DISTRO_ID="ubuntu"
else
    DISTRO_ID="el"
fi

if [ "${DISTRO_ID}" != "ubuntu" ]; then
    # ffmpeg with clang forces latomic, that won't work
    # an easy check in configure should do, but for now just disable clang
    # not that important anyway, as long as skia is built with clang we are ok
    CLANG=0
fi

DISTRO_VERSION_ID=`cat /etc/os-release | sed '/^VERSION_ID=/!d;s/VERSION_ID=//;s/"//g'`
if [ "${DISTRO_ID}" = "fc" ]; then
    DISTRO_VERSION="${DISTRO_VERSION_ID}"
    DISTRO_PRETTY="Fedora Linux ${DISTRO_VERSION}"
elif [ "${DISTRO_ID}" = "ubuntu" ]; then
    DISTRO_VERSION="${DISTRO_VERSION_ID}"
    DISTRO_PRETTY="Ubuntu Linux ${DISTRO_VERSION}"
else
    DISTRO_VERSION="${DISTRO_VERSION_ID:0:1}"
    DISTRO_PRETTY="Enterprise Linux ${DISTRO_VERSION}"
fi

DID="${DISTRO_ID}${DISTRO_VERSION}"

FRICTION_ROOT="/friction.graphics"
FRICTION_DIR="${FRICTION_ROOT}/friction"
FRICTION_SRC_DIR="${FRICTION_DIR}/src"
FRICTION_OUT_DIR="/snapshots"

if [ "${REL}" = 1 ]; then
    FRICTION_OUT_DIR="/releases"
fi

FRICTION_DIST="${FRICTION_OUT_DIR}/distfiles"
FRICTION_BRANCH=${FRICTION_BRANCH:-""}
FRICTION_COMMIT=${FRICTION_COMMIT:-""}
FRICTION_TAG=${FRICTION_TAG:-""}

if [ "${FRICTION_BRANCH}" != "" ]; then
    FRICTION_COMMIT=""
    FRICTION_TAG=""
elif [ "${FRICTION_COMMIT}" != "" ]; then
    FRICTION_BRANCH=""
    FRICTION_TAG=""
elif [ "${FRICTION_TAG}" != "" ]; then
    FRICTION_BRANCH=""
    FRICTION_COMMIT=""
fi

#QUAZIP_V="1.4"
QSCINTILLA_V="2.13.4"
FFMPEG_V="4.4.4"
UNWIND_V="1.6.2"
GPERF_V="4df0b85"
SKIA_V="4fcb5c225a"

SF_NET_SRC="https://sourceforge.net/projects/friction/files/source"
SF_NET_SNAP="https://sourceforge.net/projects/friction/files/snapshots"
QSCINTILLA_URL="https://www.riverbankcomputing.com/static/Downloads/QScintilla"
FFMPEG_URL="https://ffmpeg.org/releases"
UNWIND_URL="http://download.savannah.nongnu.org/releases/libunwind"

if [ "${DISTRO_ID}" != "ubuntu" ]; then
    export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:${PKG_CONFIG_PATH}"
fi

COMPILER=""
if [ "${CLANG}" = 1 ]; then
    #COMPILER="-clang"
    export CC=clang
    export CXX=clang++
fi

if [ ! -d "${FRICTION_ROOT}" ]; then
    mkdir -p ${FRICTION_ROOT}
fi

if [ ! -d "${FRICTION_DIST}" ]; then
    mkdir -p ${FRICTION_DIST}
fi

if [ ! -d "${FRICTION_SRC_DIR}" ]; then
    cd ${FRICTION_ROOT}
    git clone --recurse-submodules="src/plugins/shaders" https://github.com/friction2d/friction
    if [ "${FRICTION_BRANCH}" != "" ]; then
        (cd friction; git checkout ${FRICTION_BRANCH})
    fi
    if [ "${FRICTION_COMMIT}" != "" ]; then
        (cd friction; git checkout ${FRICTION_COMMIT})
    fi
    if [ "${FRICTION_TAG}" != "" ]; then
        (cd friction; git checkout tags/${FRICTION_TAG})
    fi
    #(cd friction; git submodule update -i)
fi

#if [ ! -f "${FRICTION_SRC_DIR}/quazip-${QUAZIP_V}/build/quazip/libquazip1-qt5.a" ]; then
#    if [ -d "${FRICTION_SRC_DIR}/quazip-${QUAZIP_V}" ]; then
#        rm -rf ${FRICTION_SRC_DIR}/quazip-${QUAZIP_V}
#    fi
#    if [ ! -f "${FRICTION_DIST}/quazip.tar.gz" ]; then
#        curl -L -k "https://github.com/stachenov/quazip/archive/refs/tags/v${QUAZIP_V}.tar.gz" --output ${FRICTION_DIST}/quazip.tar.gz
#    fi
#    cd ${FRICTION_SRC_DIR}
#    tar xf ${FRICTION_DIST}/quazip.tar.gz
#    cd quazip-${QUAZIP_V}
#    if [ -f "${FRICTION_DIST}/quazip-build-${DID}.tar.xz" ]; then
#        tar xf ${FRICTION_DIST}/quazip-build-${DID}.tar.xz
#    else
#        mkdir build && cd build
#        cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF .. && make
#        cd ..
#        tar cvvfJ ${FRICTION_DIST}/quazip-build-${DID}.tar.xz build
#    fi
#fi

if [ "${DISTRO_ID}" != "ubuntu" ]; then

if [ ! -f "${FRICTION_SRC_DIR}/QScintilla_src-${QSCINTILLA_V}/src/libqscintilla2_qt5.a" ]; then
    if [ -d "${FRICTION_SRC_DIR}/QScintilla_src-${QSCINTILLA_V}" ]; then
        rm -rf ${FRICTION_SRC_DIR}/QScintilla_src-${QSCINTILLA_V}
    fi
    if [ ! -f "${FRICTION_DIST}/qscintilla.tar.gz" ]; then
        curl -L -k "${QSCINTILLA_URL}/${QSCINTILLA_V}/QScintilla_src-${QSCINTILLA_V}.tar.gz" --output ${FRICTION_DIST}/qscintilla.tar.gz
    fi
    cd ${FRICTION_SRC_DIR}
    tar xf ${FRICTION_DIST}/qscintilla.tar.gz
    cd QScintilla_src-${QSCINTILLA_V}
    if [ -f "${FRICTION_DIST}/qscintilla-build-${DID}${COMPILER}.tar.xz" ]; then
        tar xf ${FRICTION_DIST}/qscintilla-build-${DID}${COMPILER}.tar.xz
    else
        cd src
        if [ "${CLANG}" = 1 ]; then
            qmake-qt5 CONFIG+=staticlib QMAKE_CXX=clang++ QMAKE_CC=clang QMAKE_LINK=clang++
        else
            qmake-qt5 CONFIG+=staticlib
        fi
        make
        cd ..
        tar cvvfJ ${FRICTION_DIST}/qscintilla-build-${DID}${COMPILER}.tar.xz src
    fi
fi

HAS_LOCAL_CACHE=0
if [ -f "${FRICTION_DIST}/local-build-${DID}${COMPILER}.tar.xz" ]; then
    HAS_LOCAL_CACHE=1
    tar xf ${FRICTION_DIST}/local-build-${DID}${COMPILER}.tar.xz -C /usr/
fi

if [ ! -f "/usr/local/lib/pkgconfig/libavcodec.pc" ]; then
    if [ -d "${FRICTION_SRC_DIR}/ffmpeg-${FFMPEG_V}" ]; then
        rm -rf ${FRICTION_SRC_DIR}/ffmpeg-${FFMPEG_V}
    fi
    if [ ! -f "${FRICTION_DIST}/ffmpeg.tar.xz" ]; then
        curl -L -k "${FFMPEG_URL}/ffmpeg-${FFMPEG_V}.tar.xz" --output ${FRICTION_DIST}/ffmpeg.tar.xz
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
        curl -L -k "${UNWIND_URL}/libunwind-${UNWIND_V}.tar.gz" --output ${FRICTION_DIST}/libunwind.tar.gz
    fi
    cd ${FRICTION_SRC_DIR}
    tar xf ${FRICTION_DIST}/libunwind.tar.gz
    cd libunwind-${UNWIND_V}
    ./configure --disable-shared --disable-minidebuginfo --disable-tests --prefix=/usr/local
    make -j${MKJOBS}
    make install
fi

if [ "${HAS_LOCAL_CACHE}" = 0 ]; then
    (cd /usr ; tar cvvfJ ${FRICTION_DIST}/local-build-${DID}${COMPILER}.tar.xz local)
fi

fi # [ "${DISTRO_ID}" != "ubuntu" ]

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
    if [ -f "${FRICTION_DIST}/gperftools-build-${DID}${COMPILER}.tar.xz" ]; then
        tar xf ${FRICTION_DIST}/gperftools-build-${DID}${COMPILER}.tar.xz
    else
        if [ "${DISTRO_ID}" = "fc" ]; then
            autoupdate
        fi
        ./autogen.sh
        ./configure --disable-shared
        make -j${MKJOBS}
        tar cvvfJ ${FRICTION_DIST}/gperftools-build-${DID}${COMPILER}.tar.xz .
    fi
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
    if [ -f "${FRICTION_DIST}/skia-build-${DID}${COMPILER}.tar.xz" ]; then
        tar xf ${FRICTION_DIST}/skia-build-${DID}${COMPILER}.tar.xz
    else
        bin/gn gen out/build --args='is_official_build=true is_debug=false cc="clang" cxx="clang++" extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false skia_use_dng_sdk=false'
        ninja -C out/build -j${MKJOBS} skia
        tar cvvfJ ${FRICTION_DIST}/skia-build-${DID}${COMPILER}.tar.xz out
    fi
fi

cd ${FRICTION_DIR}

COMMIT=`git rev-parse --short HEAD`
BRANCH=`git rev-parse --abbrev-ref HEAD`
VERSION="dev"

IS_SNAP="OFF"
if [ "${SNAP}" = 1 ]; then
    VERSION="${YEAR}.${MONTH}.${DAY}"
    IS_SNAP="ON"
fi

if [ -d "${FRICTION_DIR}/build" ]; then
    rm -rf ${FRICTION_DIR}/build
fi

mkdir build
cd build

CMAKE_EXTRA=""
if [ "${DISTRO_ID}" != "ubuntu" ]; then
    CMAKE_EXTRA="-DSTATIC_FFMPEG=ON"
    CMAKE_EXTRA="${CMAKE_EXTRA} -DQSCINTILLA_INCLUDE_DIRS=${FRICTION_SRC_DIR}/QScintilla_src-${QSCINTILLA_V}/src"
    CMAKE_EXTRA="${CMAKE_EXTRA} -DQSCINTILLA_LIBRARIES_DIRS=${FRICTION_SRC_DIR}/QScintilla_src-${QSCINTILLA_V}/src"
    CMAKE_EXTRA="${CMAKE_EXTRA} -DQSCINTILLA_LIBRARIES=qscintilla2_qt5"
fi
if [ "${CLANG}" = 1 ]; then
    CMAKE_EXTRA="${CMAKE_EXTRA} -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang"
fi

if [ "${SNAP}" != 1 ]; then
    cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    ${CMAKE_EXTRA} ..
    VERSION=`cat version.txt`
else
    cmake -G Ninja \
    -DGIT_COMMIT=${COMMIT} \
    -DGIT_BRANCH=${BRANCH} \
    -DSNAPSHOT=${IS_SNAP} \
    -DSNAPSHOT_VERSION_MAJOR=${YEAR} \
    -DSNAPSHOT_VERSION_MINOR=${MONTH} \
    -DSNAPSHOT_VERSION_PATCH=${DAY} \
    -DCMAKE_BUILD_TYPE=Release \
    ${CMAKE_EXTRA} ..
fi
cmake --build .

PKG_EXT="deb"
if [ "${DISTRO_ID}" != "ubuntu" ]; then
    PKG_EXT="rpm"
    cpack -G RPM
else
    cpack -G DEB
fi

PKG="friction-${DID}${COMPILER}.${PKG_EXT}"
if [ "${REL}" = 1 ]; then
    PKG="friction-${VERSION}-${DID}${COMPILER}.${PKG_EXT}"
elif [ "${SNAP}" = 1 ]; then
    PKG="friction-${TIMESTAMP}-${BRANCH}-${COMMIT}-${DID}${COMPILER}.${PKG_EXT}"
fi
mv friction.${PKG_EXT} ${PKG}

FRICTION_PKG_DIR="${FRICTION_OUT_DIR}/${YEAR}/${MONTH}"
if [ "${REL}" = 1 ]; then
    FRICTION_PKG_DIR="${FRICTION_OUT_DIR}/${VERSION}"
fi

if [ ! -d "${FRICTION_PKG_DIR}" ]; then
    mkdir -p ${FRICTION_PKG_DIR}
fi

cp ${PKG} ${FRICTION_PKG_DIR}/

echo "BUILD DONE: ${FRICTION_PKG_DIR}/${PKG}"
