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
clang++ --version
cmake --version
python3 --version
qmake-qt5 --version || qmake --version
env
cat /etc/os-release

REL=${REL:-0}
MKJOBS=${MKJOBS:-4}

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

export CC=clang
export CXX=clang++

if [ ! -d "${FRICTION_ROOT}" ]; then
    mkdir -p ${FRICTION_ROOT}
fi

if [ ! -d "${FRICTION_SRC_DIR}" ]; then
    cd ${FRICTION_ROOT}
    git clone --recurse-submodules https://github.com/friction2d/friction
    if [ "${FRICTION_BRANCH}" != "" ]; then
        (cd friction; git checkout ${FRICTION_BRANCH})
    fi
    if [ "${FRICTION_COMMIT}" != "" ]; then
        (cd friction; git checkout ${FRICTION_COMMIT})
    fi
    if [ "${FRICTION_TAG}" != "" ]; then
        (cd friction; git checkout tags/${FRICTION_TAG})
    fi
    #(cd friction; git submodule update -i docs)
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

CMAKE_EXTRA="-DCMAKE_BUILD_TYPE=Release -DUSE_SKIA_SYSTEM_LIBS=ON"
CMAKE_EXTRA="${CMAKE_EXTRA} -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang"

if [ "${REL}" = 1 ]; then
    cmake -G Ninja \
    -DFRICTION_OFFICIAL_RELEASE=ON \
    ${CMAKE_EXTRA} ..
    VERSION=`cat version.txt`
else
    cmake -G Ninja \
    -DGIT_COMMIT=${COMMIT} \
    -DGIT_BRANCH=${BRANCH} \
    ${CMAKE_EXTRA} ..
    VERSION=`cat version.txt`
    VERSION="${VERSION}-dev-${COMMIT}"
fi
cmake --build .

PKG_EXT="deb"
cpack -G DEB

OPKG="friction.${PKG_EXT}"
PKG="friction-${VERSION}-${DID}.${PKG_EXT}"
mv ${OPKG} ${PKG}

FRICTION_PKG_DIR="${FRICTION_OUT_DIR}/${VERSION}"
if [ ! -d "${FRICTION_PKG_DIR}" ]; then
    mkdir -p ${FRICTION_PKG_DIR}
fi

cp ${PKG} ${FRICTION_PKG_DIR}/

echo "BUILD DONE: ${FRICTION_PKG_DIR}/${PKG}"
