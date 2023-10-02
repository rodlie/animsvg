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

source /opt/rh/devtoolset-7/enable
gcc -v

CWD=`pwd`
SDK=${SDK:-"/opt/friction"}
SRC=${SDK}/src
DIST=${DIST:-"/mnt/mxe/pkg"}
JOBS=${JOBS:-4}

ZLIB_V=1.2.13
BZIP_V=1.0.8
NASM_V=2.14.02
LAME_V=3.100
VPX_V=1.8.2
OGG_V=1.3.5
VORBIS_V=1.3.7
THEORA_V=1.1.1
XVID_V=1.3.4
LSMASH_V=2.14.5
X264_V=20180806-2245
X265_V=3.5
AOM_V=3.6.1

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

STATIC_CFLAGS="-fPIC"
DEFAULT_CFLAGS="-I${SDK}/include"
DEFAULT_LDFLAGS="-L${SDK}/lib"
COMMON_CONFIGURE="--prefix=${SDK}"
SHARED_CONFIGURE="${COMMON_CONFIGURE} --enable-shared --disable-static"
STATIC_CONFIGURE="${COMMON_CONFIGURE} --disable-shared --enable-static"
DEFAULT_CONFIGURE="${SHARED_CONFIGURE}"

# REMOVE
# libpng.pc  libpng15.pc  png.h  pngconf.h  pnglibconf.h

# nasm
if [ ! -f "${SDK}/bin/nasm" ]; then
    cd ${SRC}
    NASM_SRC=nasm-${NASM_V}
    rm -rf ${NASM_SRC} || true
    tar xf ${DIST}/${NASM_SRC}.tar.xz
    cd ${NASM_SRC}
    ./configure ${COMMON_CONFIGURE}
    make -j${JOBS}
    make install
fi # nasm

# bzip2
if [ ! -f "${SDK}/lib/libbz2.so.1.0" ]; then
    cd ${SRC}
    BZIP_SRC=bzip2-${BZIP_V}
    rm -rf ${BZIP_SRC} || true
    tar xf ${DIST}/${BZIP_SRC}.tar.gz
    cd ${BZIP_SRC}
    make -f Makefile-libbz2_so
    cp -a libbz2.so* ${SDK}/lib/
    (cd ${SDK}/lib ; ln -sf libbz2.so.1.0 libbz2.so)
    cp -a bzlib.h ${SDK}/include/
fi # bzip2

# zlib
if [ ! -f "${SDK}/lib/libz.so" ]; then
    cd ${SRC}
    ZLIB_SRC=zlib-${ZLIB_V}
    rm -rf ${ZLIB_SRC} || true
    tar xf ${DIST}/${ZLIB_SRC}.tar.xz
    cd ${ZLIB_SRC}
    ./configure ${COMMON_CONFIGURE}
    make -j${JOBS}
    make install
fi # zlib

# lame
if [ ! -f "${SDK}/lib/libmp3lame.so" ]; then
    cd ${SRC}
    LAME_SRC=lame-${LAME_V}
    rm -rf ${LAME_SRC} || true
    tar xf ${DIST}/${LAME_SRC}.tar.gz
    cd ${LAME_SRC}
    CFLAGS="${DEFAULT_CFLAGS}" \
    CXXFLAGS="${DEFAULT_CFLAGS}" \
    LDFLAGS="${DEFAULT_LDFLAGS}" \
    ./configure ${DEFAULT_CONFIGURE} --disable-frontend --disable-gtktest --with-fileio=lame --enable-nasm
    make -j${JOBS}
    make install
fi # lame

# libvpx
if [ ! -f "${SDK}/lib/libvpx.so" ]; then
    cd ${SRC}
    VPX_SRC=libvpx-${VPX_V}
    rm -rf ${VPX_SRC} || true
    tar xf ${DIST}/libvpx-v${VPX_V}.tar.gz
    cd ${VPX_SRC}
    CFLAGS="${DEFAULT_CFLAGS}" \
    CXXFLAGS="${DEFAULT_CFLAGS}" \
    LDFLAGS="${DEFAULT_LDFLAGS}" \
    ./configure ${DEFAULT_CONFIGURE} --disable-examples --disable-install-docs
    make -j${JOBS}
    make install
fi # libvpx

# libogg
if [ ! -f "${SDK}/lib/libogg.so" ]; then
    cd ${SRC}
    OGG_SRC=libogg-${OGG_V}
    rm -rf ${OGG_SRC} || true
    tar xf ${DIST}/${OGG_SRC}.tar.gz
    cd ${OGG_SRC}
    CFLAGS="${DEFAULT_CFLAGS}" \
    CXXFLAGS="${DEFAULT_CFLAGS}" \
    LDFLAGS="${DEFAULT_LDFLAGS}" \
    ./configure ${DEFAULT_CONFIGURE}
    make -j${JOBS}
    make install
fi # libogg

# libvorbis
if [ ! -f "${SDK}/lib/libvorbis.so" ]; then
    cd ${SRC}
    VORBIS_SRC=libvorbis-${VORBIS_V}
    rm -rf ${VORBIS_SRC} || true
    tar xf ${DIST}/${VORBIS_SRC}.tar.gz
    cd ${VORBIS_SRC}
    CFLAGS="${DEFAULT_CFLAGS}" \
    CXXFLAGS="${DEFAULT_CFLAGS}" \
    LDFLAGS="${DEFAULT_LDFLAGS}" \
    ./configure ${DEFAULT_CONFIGURE}
    make -j${JOBS}
    make install
fi # libvorbis

# libtheora
if [ ! -f "${SDK}/lib/libtheora.so" ]; then
    cd ${SRC}
    THEORA_SRC=libtheora-${THEORA_V}
    rm -rf ${THEORA_SRC} || true
    tar xf ${DIST}/${THEORA_SRC}.tar.gz
    cd ${THEORA_SRC}
    CFLAGS="${DEFAULT_CFLAGS}" \
    CXXFLAGS="${DEFAULT_CFLAGS}" \
    LDFLAGS="${DEFAULT_LDFLAGS}" \
    ./configure ${DEFAULT_CONFIGURE} --disable-examples --disable-sdltest
    make -j${JOBS}
    make install
fi # libtheora

# xvidcore
if [ ! -f "${SDK}/lib/libxvidcore.so" ]; then
    cd ${SRC}
    rm -rf xvidcore || true
    tar xf ${DIST}/xvidcore-${XVID_V}.tar.gz
    cd xvidcore/build/generic
    CFLAGS="${DEFAULT_CFLAGS}" \
    CXXFLAGS="${DEFAULT_CFLAGS}" \
    LDFLAGS="${DEFAULT_LDFLAGS}" \
    ./configure ${COMMON_CONFIGURE}
    make -j${JOBS}
    make install
fi # xvidcore

# liblsmash
if [ ! -f "${SDK}/lib/liblsmash.so" ]; then
    cd ${SRC}
    LSMASH_SRC=l-smash-${LSMASH_V}
    rm -rf ${LSMASH_SRC} || true
    tar xf ${DIST}/liblsmash-v${LSMASH_V}.tar.gz
    cd ${LSMASH_SRC}
    CFLAGS="${DEFAULT_CFLAGS}" \
    CXXFLAGS="${DEFAULT_CFLAGS}" \
    LDFLAGS="${DEFAULT_LDFLAGS}" \
    ./configure ${DEFAULT_CONFIGURE}
    make -j${JOBS}
    make install
fi # liblsmash

# x264
if [ ! -f "${SDK}/lib/libx264.so" ]; then
    cd ${SRC}
    X264_SRC=x264-snapshot-${X264_V}
    rm -rf ${X264_SRC} || true
    tar xf ${DIST}/${X264_SRC}.tar.bz2
    cd ${X264_SRC}
    CFLAGS="${DEFAULT_CFLAGS}" \
    CXXFLAGS="${DEFAULT_CFLAGS}" \
    LDFLAGS="${DEFAULT_LDFLAGS}" \
    ./configure ${COMMON_CONFIGURE} --enable-shared --disable-lavf --disable-swscale --disable-opencl --disable-cli
    make -j${JOBS}
    make install
fi # x264

# x265
if [ ! -f "${SDK}/lib/libx265.so" ]; then
    cd ${SRC}
    X265_SRC=x265_${X265_V}
    rm -rf ${X265_SRC} || true
    tar xf ${DIST}/${X265_SRC}.tar.gz
    cd ${X265_SRC}/source
    mkdir build && cd build
    CFLAGS="${DEFAULT_CFLAGS}" \
    CXXFLAGS="${DEFAULT_CFLAGS}" \
    LDFLAGS="${DEFAULT_LDFLAGS}" \
    cmake \
    -DCMAKE_INSTALL_PREFIX=${SDK} \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_SHARED=ON \
    -DENABLE_CLI=OFF ..
    make -j${JOBS}
    make install
    rm ${SDK}/lib/libx265.a
fi # x265

# aom
if [ ! -f "${SDK}/lib/libaom.so" ]; then
    cd ${SRC}
    AOM_SRC=libaom-${AOM_V}
    rm -rf ${AOM_SRC} || true
    tar xf ${DIST}/${AOM_SRC}.tar.gz
    cd ${AOM_SRC}
    mkdir build2 && cd build2
    CFLAGS="${DEFAULT_CFLAGS}" \
    CXXFLAGS="${DEFAULT_CFLAGS}" \
    LDFLAGS="${DEFAULT_LDFLAGS}" \
    cmake \
    -DCMAKE_INSTALL_PREFIX=${SDK} \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_NASM=ON \
    -DENABLE_DOCS=OFF \
    -DENABLE_TESTS=OFF \
    -DENABLE_TESTDATA=OFF \
    -DENABLE_TOOLS=OFF \
    -DENABLE_EXAMPLES=OFF \
    -DCONFIG_AV1_HIGHBITDEPTH=0 \
    -DCONFIG_WEBM_IO=0 \
    -DBUILD_SHARED_LIBS=ON ..
    make -j${JOBS}
    make install
    rm ${SDK}/lib/libaom.a
fi # aom

echo "FFMPEG DONE"
