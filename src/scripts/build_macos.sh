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

CI=${CI:-0}
SNAP=${SNAP:-0}
REL=${REL:-0}
CWD=`pwd`
SDK=${SDK:-"/opt/enve2d"}
MKJOBS=${MKJOBS:-4}
COMMIT=`git rev-parse --short HEAD`
VERSION=`cat ${CWD}/CMakeLists.txt | sed '/enve2dproject VERSION/!d;s/)//' | awk '{print $3}'`
TIMESTAMP=${TIMESTAMP:-`date +%Y%m%d%H%M`}

QT=${QT:-"${SDK}/qt/5.12.12/clang_64"}

QUAZIP=${QUAZIP:-"${SDK}/quazip-build"}
QUAZIP_LIB=${QUAZIP_LIB:-"quazip1-qt5"}

QSCINTILLA=${QSCINTILLA:-"${SDK}/qscintilla-build"}
QSCINTILLA_LIB=${QSCINTILLA_LIB:-"qscintilla2_qt5"}

if [ "${CI}" = 1 ]; then
    QT_DOWNLOAD="https://sourceforge.net/projects/enve2d/files/sdk/mac/20230401/enve2d-mac-qt512.tar.bz2/download"
    SKIA_DOWNLOAD="https://sourceforge.net/projects/enve2d/files/sdk/mac/20230401/enve2d-mac-skia-build.tar.bz2/download"
    QUAZIP_DOWNLOAD="https://sourceforge.net/projects/enve2d/files/sdk/mac/20230401/enve2d-mac-quazip-build.tar.bz2/download"
    GPERFTOOLS_DOWNLOAD="https://sourceforge.net/projects/enve2d/files/sdk/mac/20230401/enve2d-mac-gperftools-build.tar.bz2/download"
    QSCINTILLA_DOWNLOAD="https://sourceforge.net/projects/enve2d/files/sdk/mac/20230401/enve2d-mac-qscintilla-build.tar.bz2/download"
    PORTS_BIN_DOWNLOAD="https://sourceforge.net/projects/enve2d/files/sdk/mac/20230401/enve2d-macports-bin.tar.bz2/download"
    PORTS_LIB_DOWNLOAD="https://sourceforge.net/projects/enve2d/files/sdk/mac/20230401/enve2d-macports-lib.tar.bz2/download"
    PORTS_LIBEXEC_DOWNLOAD="https://sourceforge.net/projects/enve2d/files/sdk/mac/20230401/enve2d-macports-libexec.tar.bz2/download"
    PORTS_SHARE_DOWNLOAD="https://sourceforge.net/projects/enve2d/files/sdk/mac/20230401/enve2d-macports-share.tar.bz2/download"
    PORTS_INCLUDE_DOWNLOAD="https://sourceforge.net/projects/enve2d/files/sdk/mac/20230401/enve2d-macports-include.tar.bz2/download"
    if [ ! -d "${SDK}" ]; then
        sudo mkdir -p ${SDK}
        sudo chmod 777 ${SDK}
    fi
    if [ ! -d "${QT}" ]; then
        rm -rf ${SDK}/qt || true
        curl -k -L ${QT_DOWNLOAD} --output qt.tar.bz2
        mkdir -p ${SDK}/qt
        tar xf qt.tar.bz2 -C ${SDK}/qt/
    fi
    if [ ! -d "${SDK}/skia-build" ]; then
        curl -k -L ${SKIA_DOWNLOAD} --output skia.tar.bz2
        tar xf skia.tar.bz2 -C ${SDK}/
    fi
    if [ ! -d "${QUAZIP}" ]; then
        curl -k -L ${QUAZIP_DOWNLOAD} --output quazip.tar.bz2
        tar xf quazip.tar.bz2 -C ${SDK}/
    fi
    if [ ! -d "${SDK}/gperftools-build" ]; then
        curl -k -L ${GPERFTOOLS_DOWNLOAD} --output gperftools.tar.bz2
        tar xf gperftools.tar.bz2 -C ${SDK}/
    fi
    if [ ! -d "${QSCINTILLA}" ]; then
        curl -k -L ${QSCINTILLA_DOWNLOAD} --output qscintilla.tar.bz2
        tar xf qscintilla.tar.bz2 -C ${SDK}/
    fi
    if [ ! -d "/opt/local" ]; then
        sudo mkdir -p /opt/local
        sudo chmod 777 /opt/local
        curl -k -L ${PORTS_BIN_DOWNLOAD} --output bin.tar.bz2
        curl -k -L ${PORTS_LIB_DOWNLOAD} --output lib.tar.bz2
        curl -k -L ${PORTS_LIBEXEC_DOWNLOAD} --output libexec.tar.bz2
        curl -k -L ${PORTS_SHARE_DOWNLOAD} --output share.tar.bz2
        curl -k -L ${PORTS_INCLUDE_DOWNLOAD} --output include.tar.bz2
        tar xf bin.tar.bz2 -C /opt/local/
        tar xf lib.tar.bz2 -C /opt/local/
        tar xf libexec.tar.bz2 -C /opt/local/
        tar xf share.tar.bz2 -C /opt/local/
        tar xf include.tar.bz2 -C /opt/local/
    fi
    if [ ! -d "${CWD}/src/gperftools/.libs" ]; then
        rm -rf ${CWD}/src/gperftools
        (cd ${CWD}/src ; ln -sf ${SDK}/gperftools-build gperftools)
    fi
    if [ ! -d "${CWD}/src/skia/out/build" ]; then
        rm -rf ${CWD}/src/skia
        (cd ${CWD}/src ; ln -sf ${SDK}/skia-build skia)
    fi
fi

export PATH="/opt/local/bin:${PATH}"
export PKG_CONFIG_PATH="/opt/local/lib/pkgconfig"

cd ${CWD}
rm -rf build || true
mkdir build
cd build

cmake \
-DENVE2D_GIT=${COMMIT} \
-DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_PREFIX_PATH=${QT} \
-DQUAZIP_INCLUDE_DIRS=${QUAZIP}/quazip \
-DQUAZIP_LIBRARIES_DIRS=${QUAZIP}/build/quazip \
-DQUAZIP_LIBRARIES=${QUAZIP_LIB} \
-DQSCINTILLA_INCLUDE_DIRS=${QSCINTILLA}/src \
-DQSCINTILLA_LIBRARIES_DIRS=${QSCINTILLA}/src \
-DQSCINTILLA_LIBRARIES=${QSCINTILLA_LIB} \
..

make -j${MKJOBS}
${QT}/bin/macdeployqt src/app/enve2d.app
mkdir dmg
mv src/app/enve2d.app dmg/
cp ${CWD}/LICENSE.md ${CWD}/README.md dmg/

VOLNAME="enve2d"
DMG="enve2d"
if [ "${REL}" = 1 ]; then
    VOLNAME="${VOLNAME} ${VERSION}"
    DMG="${DMG}-${VERSION}-macOS"
elif [ "${SNAP}" = 1 ]; then
    VOLNAME="${VOLNAME} ${VERSION}-${COMMIT}"
    DMG="${DMG}-${TIMESTAMP}-${VERSION}-${COMMIT}-macOS"
fi

hdiutil create -volname "${VOLNAME}" -srcfolder dmg -ov -format UDBZ ${DMG}.dmg
