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

set -e -x

source /opt/rh/llvm-toolset-7/enable
clang -v

CWD=`pwd`
SDK=${SDK:-"/opt/friction"}
SRC=${SDK}/src
DIST=${DIST:-"/mnt"}
JOBS=${JOBS:-4}

NINJA_V=1.11.1
GN_V=82d673ac
SKIA_V=4fcb5c225a

NINJA_BIN=${SDK}/bin/ninja
GN_BIN=${SDK}/bin/gn
SKIA_DIR=${SDK}/skia
SKIA_LIB=${SKIA_DIR}/out/build/libskia.a

export PATH="${SDK}/bin:${PATH}"

if [ ! -d "${SDK}" ]; then
    mkdir -p "${SDK}/lib"
    mkdir -p "${SDK}/bin"
    (cd "${SDK}"; ln -sf lib lib64)
fi

if [ ! -d "${SRC}" ]; then
    mkdir -p "${SRC}"
fi

# ninja
if [ ! -f "${NINJA_BIN}" ]; then
    cd ${SRC}
    NINJA_SRC=ninja-${NINJA_V}
    rm -rf ${NINJA_SRC} || true
    tar xf ${DIST}/${NINJA_SRC}.tar.gz
    cd ${NINJA_SRC}
    ./configure.py --bootstrap
    cp -a ninja ${NINJA_BIN}
fi # ninja

# gn
if [ ! -f "${GN_BIN}" ]; then
    cd ${SRC}
    GN_SRC=gn-${GN_V}
    rm -rf ${GN_SRC} || true
    tar xf ${DIST}/${GN_SRC}.tar.xz
    cd ${GN_SRC}
    python build/gen.py
    ${NINJA_BIN} -C out
    cp -a out/gn ${GN_BIN}
fi # gn

# skia
if [ ! -f "${SKIA_LIB}" ]; then
    cd ${SRC}
    SKIA_SRC=skia-${SKIA_V}
    rm -rf ${SKIA_SRC} || true
    rm -rf ${SKIA_DIR} || true
    tar xf ${DIST}/${SKIA_SRC}.tar.xz
    mv ${SKIA_SRC} ${SKIA_DIR}
    cd ${SKIA_DIR}
    ${GN_BIN} gen out/build --args='is_official_build=true is_debug=false cc="clang" cxx="clang++" extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false skia_use_dng_sdk=false'
    ${NINJA_BIN} -C out/build -j${JOBS} skia
fi # skia

echo "SKIA DONE"
