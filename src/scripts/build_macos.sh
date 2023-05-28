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

CWD=`pwd`

SNAP=1 #${SNAP:-0}
REL=0 #${REL:-0}
MKJOBS=${MKJOBS:-4}

COMMIT=`git rev-parse --short HEAD`
VERSION=`cat ${CWD}/CMakeLists.txt | sed '/friction2d VERSION/!d;s/)//' | awk '{print $3}'`

TIMESTAMP=${TIMESTAMP:-`date +%Y%m%d`}
YEAR=${YEAR:-`date +%Y`}
MONTH=${MONTH:-`date +%m`}
DAY=${DAY:-`date +%d`}

IS_SNAP="OFF"
if [ "${SNAP}" = 1 ]; then
    VERSION="${YEAR}.${MONTH}.${DAY}"
    IS_SNAP="ON"
fi

OSX=${OSX:-10.13}
QT=${QT:-"${HOME}/Qt/5.12.12/clang_64"}

QUAZIP=${QUAZIP:-"${CWD}/quazip"}
QUAZIP_LIB=${QUAZIP_LIB:-"quazip1-qt5"}

QSCINTILLA=${QSCINTILLA:-"${CWD}/qscintilla"}
QSCINTILLA_LIB=${QSCINTILLA_LIB:-"qscintilla2_qt5"}

export PATH="/opt/local/bin:${PATH}"
export PKG_CONFIG_PATH="/opt/local/lib/pkgconfig"

cd ${CWD}
if [ -d "build" ]; then
    rm -rf build
fi
mkdir build && cd build

cmake -G Ninja \
-DSNAPSHOT=${IS_SNAP} \
-DSNAPSHOT_VERSION_MAJOR=${YEAR} \
-DSNAPSHOT_VERSION_MINOR=${MONTH} \
-DSNAPSHOT_VERSION_PATCH=${DAY} \
-DGIT_COMMIT=${COMMIT} \
-DCMAKE_OSX_DEPLOYMENT_TARGET=${OSX} \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_PREFIX_PATH=${QT} \
-DQUAZIP_INCLUDE_DIRS=${QUAZIP}/quazip \
-DQUAZIP_LIBRARIES_DIRS=${QUAZIP}/build/quazip \
-DQUAZIP_LIBRARIES=${QUAZIP_LIB} \
-DQSCINTILLA_INCLUDE_DIRS=${QSCINTILLA}/src \
-DQSCINTILLA_LIBRARIES_DIRS=${QSCINTILLA}/src \
-DQSCINTILLA_LIBRARIES=${QSCINTILLA_LIB} \
..
cmake --build .

${QT}/bin/macdeployqt src/app/friction.app
mkdir dmg
mv src/app/friction.app dmg/

VOLNAME="Friction"
DMG="friction"
if [ "${REL}" = 1 ]; then
    VOLNAME="${VOLNAME} ${VERSION}"
    DMG="${DMG}-${VERSION}-macOS${OSX}"
elif [ "${SNAP}" = 1 ]; then
    VOLNAME="${VOLNAME} ${TIMESTAMP}-${COMMIT}"
    DMG="${DMG}-${TIMESTAMP}-${COMMIT}-macOS${OSX}"
fi

hdiutil create -volname "${VOLNAME}" -srcfolder dmg -ov -format UDBZ ${DMG}.dmg
