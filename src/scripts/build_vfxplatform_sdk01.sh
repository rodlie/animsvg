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

source /opt/rh/llvm-toolset-7.0/enable
clang -v

SDK=${SDK:-"/opt/friction"}
SRC=${SDK}/src
DIST=${DIST:-"/mnt"}
MKJOBS=${MKJOBS:-32}

NINJA_V=1.11.1
#GN_V=82d673ac
UNWIND_V=1.4.0
#GPERF_V=4df0b85
#SKIA_V=4c434dbee3

NINJA_BIN=${SDK}/bin/ninja
#GN_BIN=${SDK}/bin/gn

#GPERF_DIR=${SDK}/gperftools
#GPERF_LIB=${GPERF_DIR}/.libs/libtcmalloc.a

#SKIA_DIR=${SDK}/skia
#SKIA_LIB=${SKIA_DIR}/out/build/libskia.a

STATIC_CFLAGS="-fPIC"
DEFAULT_CFLAGS="-I${SDK}/include"
DEFAULT_LDFLAGS="-L${SDK}/lib"
COMMON_CONFIGURE="--prefix=${SDK}"
SHARED_CONFIGURE="${COMMON_CONFIGURE} --enable-shared --disable-static"
STATIC_CONFIGURE="${COMMON_CONFIGURE} --disable-shared --enable-static"
DEFAULT_CONFIGURE="${SHARED_CONFIGURE}"

export PATH="${SDK}/bin:${PATH}"
export PKG_CONFIG_PATH="${SDK}/lib/pkgconfig"
export LD_LIBRARY_PATH="${SDK}/lib:${LD_LIBRARY_PATH}"

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
# if [ ! -f "${GN_BIN}" ]; then
#     cd ${SRC}
#     GN_SRC=gn-${GN_V}
#     rm -rf ${GN_SRC} || true
#     tar xf ${DIST}/${GN_SRC}.tar.xz
#     cd ${GN_SRC}
#     python build/gen.py
#     ${NINJA_BIN} -C out
#     cp -a out/gn ${GN_BIN}
# fi # gn

# skia
# if [ ! -f "${SKIA_LIB}" ]; then
#     cd ${SRC}
#     SKIA_SRC=skia-${SKIA_V}
#     rm -rf ${SKIA_SRC} || true
#     rm -rf ${SKIA_DIR} || true
#     tar xf ${DIST}/${SKIA_SRC}.tar.xz
#     mv ${SKIA_SRC} ${SKIA_DIR}
#     cd ${SKIA_DIR}
#     ${GN_BIN} gen out/build --args='is_official_build=true is_debug=false cc="clang" cxx="clang++" extra_cflags=["-Wno-error"] target_os="linux" target_cpu="x64" skia_use_system_expat=false skia_use_system_freetype2=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_zlib=false skia_use_system_icu=false skia_use_system_harfbuzz=false skia_use_dng_sdk=false'
#     ${NINJA_BIN} -C out/build -j${MKJOBS} skia
# fi # skia

# libunwind
if [ ! -f "${SDK}/lib/pkgconfig/libunwind.pc" ]; then
    cd ${SRC}
    UNWIND_SRC=libunwind-${UNWIND_V}
    rm -rf ${UNWIND_SRC} || true
    tar xf ${DIST}/linux/${UNWIND_SRC}.tar.gz
    cd ${UNWIND_SRC}
    CC=clang CXX=clang++ ./configure ${DEFAULT_CONFIGURE} --disable-minidebuginfo --disable-tests
    make -j${MKJOBS}
    make install
fi # libunwind

# gperftools
# if [ ! -f "${GPERF_LIB}" ]; then
#     cd ${SRC}
#     GPERF_SRC=gperftools-${GPERF_V}
#     rm -rf ${GPERF_SRC} || true
#     rm -rf ${GPERF_DIR} || true
#     tar xf ${DIST}/${GPERF_SRC}.tar.xz
#     mv ${GPERF_SRC} ${GPERF_DIR}
#     cd ${GPERF_DIR}
#     ./autogen.sh
#     CC=clang CXX=clang++ \
#     CFLAGS="${DEFAULT_CFLAGS}" \
#     CXXFLAGS="${DEFAULT_CFLAGS}" \
#     LDFLAGS="${DEFAULT_LDFLAGS} -lunwind" \
#     ./configure ${STATIC_CONFIGURE} --enable-libunwind
#     make -j${MKJOBS}
# fi # gperftools

echo "SDK PART 1 DONE"
