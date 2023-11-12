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

# Deploy Friction on Ubuntu.

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
MKJOBS=${MKJOBS:-4}
CLANG=${CLANG:-1}

if [ "${REL}" = 0 ] && [ "${SNAP}" = 0 ]; then
    echo "REL or SNAP?"
    exit 1
fi

if [ "${REL}" = 1 ]; then
    SNAP=0
elif [ "${SNAP}" = 1 ]; then
    REL=0
fi

DISTRO_ID=`cat /etc/os-release | sed '/^ID=/!d;s/ID=//;s/"//g'`
if [ "${DISTRO_ID}" != "ubuntu" ]; then
    echo "UNSUPPORTED"
    exit 1
fi

DISTRO_VERSION_ID=`cat /etc/os-release | sed '/^VERSION_ID=/!d;s/VERSION_ID=//;s/"//g'`
DID="${DISTRO_ID}${DISTRO_VERSION_ID}"

FRICTION_ROOT="/friction.graphics"
FRICTION_DIR="${FRICTION_ROOT}/friction"
FRICTION_SRC_DIR="${FRICTION_DIR}/src"
FRICTION_OUT_DIR="/snapshots"

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

GPERF_V="4df0b85"
SKIA_V="5ae542b872"

SF_NET_SRC="https://sourceforge.net/projects/friction/files/source"

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
    git clone https://github.com/friction2d/friction
    if [ "${FRICTION_BRANCH}" != "" ]; then
        (cd friction; git checkout ${FRICTION_BRANCH})
    fi
    if [ "${FRICTION_COMMIT}" != "" ]; then
        (cd friction; git checkout ${FRICTION_COMMIT})
    fi
    if [ "${FRICTION_TAG}" != "" ]; then
        (cd friction; git checkout tags/${FRICTION_TAG})
    fi
    (cd friction; git submodule update -i docs)
fi

if [ ! -f "${FRICTION_SRC_DIR}/gperftools/.libs/libtcmalloc.a" ]; then
    if [ ! -f "${FRICTION_DIST}/gperftools-${GPERF_V}.tar.xz" ]; then
        curl -k -L "${SF_NET_SRC}/gperftools-${GPERF_V}.tar.xz/download" --output ${FRICTION_DIST}/gperftools-${GPERF_V}.tar.xz
    fi
    if [ -d "${FRICTION_SRC_DIR}/gperftools-${GPERF_V}" ]; then
        rm -rf ${FRICTION_SRC_DIR}/gperftools-${GPERF_V}
    fi
    if [ -d "${FRICTION_SRC_DIR}/gperftools" ]; then
        rm -rf ${FRICTION_SRC_DIR}/gperftools
    fi
    cd ${FRICTION_SRC_DIR}
    tar xf ${FRICTION_DIST}/gperftools-${GPERF_V}.tar.xz
    mv gperftools-${GPERF_V} gperftools
    cd gperftools
    if [ -f "${FRICTION_DIST}/gperftools-${GPERF_V}-build-${DID}${COMPILER}.tar.xz" ]; then
        tar xf ${FRICTION_DIST}/gperftools-${GPERF_V}-build-${DID}${COMPILER}.tar.xz
    else
        ./autogen.sh
        ./configure --disable-shared
        make -j${MKJOBS}
        tar cvvfJ ${FRICTION_DIST}/gperftools-${GPERF_V}-build-${DID}${COMPILER}.tar.xz .
    fi
fi

if [ ! -f "${FRICTION_SRC_DIR}/skia/out/build/libskia.a" ]; then
    if [ ! -f "${FRICTION_DIST}/skia-${SKIA_V}.tar.xz" ]; then
        curl -k -L "${SF_NET_SRC}/skia-${SKIA_V}.tar.xz/download" --output ${FRICTION_DIST}/skia-${SKIA_V}.tar.xz
    fi
    if [ -d "${FRICTION_SRC_DIR}/skia-${SKIA_V}" ]; then
        rm -rf ${FRICTION_SRC_DIR}/skia-${SKIA_V}
    fi
    if [ -d "${FRICTION_SRC_DIR}/skia" ]; then
        rm -rf ${FRICTION_SRC_DIR}/skia
    fi
    cd ${FRICTION_SRC_DIR}
    tar xf ${FRICTION_DIST}/skia-${SKIA_V}.tar.xz
    mv skia-${SKIA_V} skia
    cd skia
    if [ -f "${FRICTION_DIST}/skia-${SKIA_V}-build-${DID}${COMPILER}.tar.xz" ]; then
        tar xf ${FRICTION_DIST}/skia-${SKIA_V}-build-${DID}${COMPILER}.tar.xz
    else
        if [ ! -f "bin/gn" ]; then
            (cd bin ; python3 fetch-gn)
        fi
        bin/gn gen out/build --args='is_official_build=true is_debug=false cc="clang" cxx="clang++" extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=true skia_use_system_freetype2=true skia_use_system_libjpeg_turbo=true skia_use_system_libpng=true skia_use_system_libwebp=true skia_use_system_zlib=true skia_use_system_icu=true skia_use_system_harfbuzz=true skia_use_dng_sdk=false'
        ninja -C out/build -j${MKJOBS} skia
        tar cvvfJ ${FRICTION_DIST}/skia-${SKIA_V}-build-${DID}${COMPILER}.tar.xz out
    fi
fi

cd ${FRICTION_DIR}

COMMIT=`git rev-parse --short HEAD`
BRANCH=`git rev-parse --abbrev-ref HEAD`
VERSION="dev"

if [ -d "${FRICTION_DIR}/build" ]; then
    rm -rf ${FRICTION_DIR}/build
fi

mkdir build
cd build

CMAKE_EXTRA="-DUSE_SKIA_SYSTEM_LIBS=ON"
if [ "${CLANG}" = 1 ]; then
    CMAKE_EXTRA="${CMAKE_EXTRA} -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang"
fi

if [ "${REL}" = 1 ]; then
    cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DFRICTION_OFFICIAL_RELEASE=ON \
    ${CMAKE_EXTRA} ..
    VERSION=`cat version.txt`
else
    cmake -G Ninja \
    -DGIT_COMMIT=${COMMIT} \
    -DGIT_BRANCH=${BRANCH} \
    -DCMAKE_BUILD_TYPE=Release \
    ${CMAKE_EXTRA} ..
    VERSION=`cat version.txt`
    VERSION="${VERSION}-dev-${COMMIT}"
fi
cmake --build .

PKG_EXT="deb"
cpack -G DEB

OPKG="friction.${PKG_EXT}"
PKG="friction-${VERSION}-${DID}${COMPILER}.${PKG_EXT}"
mv ${OPKG} ${PKG}

FRICTION_PKG_DIR="${FRICTION_OUT_DIR}/${VERSION}"
if [ ! -d "${FRICTION_PKG_DIR}" ]; then
    mkdir -p ${FRICTION_PKG_DIR}
fi

cp ${PKG} ${FRICTION_PKG_DIR}/

echo "BUILD DONE: ${FRICTION_PKG_DIR}/${PKG}"
