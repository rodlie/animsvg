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

CI=${CI:-0}
APT=${APT:-0}
SNAP=${SNAP:-0}
DOCKER=${DOCKER:-0}
REL=${REL:-0}
SKIA_SYNC=${SKIA_SYNC:-1}
PC=${PC:-""}

SF_NET_SRC="https://sourceforge.net/projects/friction/files/source"
SF_NET_SNAP="https://sourceforge.net/projects/friction/files/snapshots"

SKIA_GIT="4fcb5c225a"
GPERF_GIT="4df0b85"

SKIA_TAR=${SKIA_TAR:-0}
GPERF_TAR=${GPERF_TAR:-0}

FRICTION_DIST=/snapshots/distfiles

if [ "${SKIA_TAR}" = 1 ]; then
    SKIA_SYNC=0
fi

if [ "${APT}" = 1 ]; then
sudo apt update -y
sudo apt install -y \
curl \
git \
build-essential \
libtool \
autoconf \
automake \
cmake \
python3 \
python-is-python3 \
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
qttools5-dev-tools
fi

if [ "${CI}" = 1 ]; then
    git submodule update -i --recursive
fi

if [ "${PC}" != "" ]; then
    export PKG_CONFIG_PATH="${PC}:${PKG_CONFIG_PATH}"
fi

CWD=`pwd`
MKJOBS=${MKJOBS:-4}
COMMIT=`git rev-parse --short HEAD`
VERSION=`cat ${CWD}/CMakeLists.txt | sed '/friction2d VERSION/!d;s/)//' | awk '{print $3}'`
TIMESTAMP=${TIMESTAMP:-`date +%Y%m%d`}
YEAR=${YEAR:-`date +%Y`}
MONTH=${MONTH:-`date +%m`}
DAY=${DAY:-`date +%d`}
DISTRO_ID=`cat /etc/os-release | sed '/^ID=/!d;s/ID=//;s/"//g'`
DISTRO_VERSION=`cat /etc/os-release | sed '/^VERSION_ID=/!d;s/VERSION_ID=//;s/"//g'`
DISTRO_PRETTY=`cat /etc/os-release | sed '/^PRETTY_NAME=/!d;s/PRETTY_NAME=//;s/"//g'`

IS_SNAP="OFF"
if [ "${SNAP}" = 1 ]; then
    VERSION="${YEAR}.${MONTH}.${DAY}"
    IS_SNAP="ON"
fi

if [ ! -f "${CWD}/src/gperftools/.libs/libtcmalloc.a" ]; then
    if [ "${GPERF_TAR}" = 1 ]; then
        rm -rf ${CWD}/src/gperftools || true
        if [ -f "${FRICTION_DIST}/gperftools.tar.xz" ]; then
            tar xf ${FRICTION_DIST}/gperftools.tar.xz
        else
            curl -k -L "${SF_NET_SRC}/gperftools-${GPERF_GIT}.tar.xz/download" --output ${CWD}/gperftools.tar.xz
            tar xf ${CWD}/gperftools.tar.xz
        fi
        mv gperftools-${GPERF_GIT} ${CWD}/src/gperftools
    fi
    cd ${CWD}/src/gperftools
    ./autogen.sh
    ./configure --disable-shared
    make -j${MKJOBS}
fi

if [ ! -f "${CWD}/src/skia/out/build/libskia.a" ]; then
    if [ "${SKIA_TAR}" = 1 ]; then
        rm -rf ${CWD}/src/skia || true
        if [ -f "${FRICTION_DIST}/skia.tar.xz" ]; then
            tar xf ${FRICTION_DIST}/skia.tar.xz
        else
            curl -k -L "${SF_NET_SRC}/skia-${SKIA_GIT}-minimal.tar.xz/download" --output ${CWD}/skia.tar.xz
            tar xf ${CWD}/skia.tar.xz
        fi
        mv skia-${SKIA_GIT} ${CWD}/src/skia
    fi
    cd ${CWD}/src/skia
    if [ "${SKIA_SYNC}" = 1 ]; then
        python3 tools/git-sync-deps || true
    fi
    bin/gn gen out/build --args='is_official_build=true is_debug=false extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false'
    ninja -C out/build -j${MKJOBS} skia
fi

cd ${CWD}
rm -rf build || true
mkdir build
cd build

cmake \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=/usr \
-DGIT_COMMIT=${COMMIT} \
-DSNAPSHOT=${IS_SNAP} \
-DSNAPSHOT_VERSION_MAJOR=${YEAR} \
-DSNAPSHOT_VERSION_MINOR=${MONTH} \
-DSNAPSHOT_VERSION_PATCH=${DAY} \
..

make -j${MKJOBS}
cpack -G DEB

if [ "${CI}" = 1 ]; then
    make DESTDIR=`pwd`/friction install
    tree friction
fi

PKG="friction-${DISTRO_ID}-${DISTRO_VERSION}.deb"
if [ "${REL}" = 1 ]; then
    PKG="friction-${VERSION}-${DISTRO_ID}-${DISTRO_VERSION}.deb"
elif [ "${SNAP}" = 1 ]; then
    PKG="friction-${TIMESTAMP}-${COMMIT}-${DISTRO_ID}-${DISTRO_VERSION}.deb"
fi
mv friction.deb ${PKG}

if [ "${DOCKER}" = 1 ]; then
    if [ ! -d "/snapshots/${YEAR}/${MONTH}" ]; then
        mkdir -p /snapshots/${YEAR}/${MONTH}
    fi
    cp ${PKG} /snapshots/${YEAR}/${MONTH}/
    (cd /snapshots ;
        echo "* [Latest download for ${DISTRO_PRETTY}](https://sourceforge.net/projects/friction/files/snapshots/${YEAR}/${MONTH}/${PKG}/download)" > friction-latest-${DISTRO_ID}-${DISTRO_VERSION}.md
    )
fi
