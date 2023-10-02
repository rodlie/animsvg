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
DIST=${DIST:-"/mnt"}
JOBS=${JOBS:-4}

XKBCOMMON_V=0.7.1
QT_V=5.12.12
QSCINTILLA_V=2.14.1
GPERF_V=4df0b85
PELF_V=0.17.0
CMAKE_V=3.26.3
UNWIND_V=1.4.0

NINJA_BIN=${SDK}/bin/ninja
CMAKE_BIN=${SDK}/bin/cmake
PELF_BIN=${SDK}/bin/patchelf
QMAKE_BIN=${SDK}/bin/qmake

GPERF_DIR=${SDK}/gperftools
GPERF_LIB=${GPERF_DIR}/.libs/libtcmalloc.a

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

# patchelf
if [ ! -f "${PELF_BIN}" ]; then
    cd ${SRC}
    PELF_SRC=patchelf-${PELF_V}
    rm -rf ${PELF_SRC} || true
    tar xf ${DIST}/${PELF_SRC}.tar.bz2
    cd ${PELF_SRC}
    ./configure ${COMMON_CONFIGURE}
    make -j${JOBS}
    make install
fi # patchelf

# cmake
if [ ! -f "${CMAKE_BIN}" ]; then
    cd ${SRC}
    CMAKE_SRC=cmake-${CMAKE_V}
    rm -rf ${CMAKE_SRC} || true
    tar xf ${DIST}/mxe/pkg/${CMAKE_SRC}.tar.gz
    cd ${CMAKE_SRC}
    ./configure ${COMMON_CONFIGURE} -- -DCMAKE_USE_OPENSSL=OFF
    make -j${JOBS}
    make install
fi # cmake

# libunwind
if [ ! -f "${SDK}/lib/pkgconfig/libunwind.pc" ]; then
    cd ${SRC}
    UNWIND_SRC=libunwind-${UNWIND_V}
    rm -rf ${UNWIND_SRC} || true
    tar xf ${DIST}/${UNWIND_SRC}.tar.gz
    cd ${UNWIND_SRC}
    ./configure ${DEFAULT_CONFIGURE} --disable-minidebuginfo --disable-tests
    make -j${JOBS}
    make install
fi # libunwind

# gperftools
if [ ! -f "${GPERF_LIB}" ]; then
    cd ${SRC}
    GPERF_SRC=gperftools-${GPERF_V}
    rm -rf ${GPERF_SRC} || true
    rm -rf ${GPERF_DIR} || true
    tar xf ${DIST}/${GPERF_SRC}.tar.xz
    mv ${GPERF_SRC} ${GPERF_DIR}
    cd ${GPERF_DIR}
    ./autogen.sh
    CFLAGS="${DEFAULT_CFLAGS}" \
    CXXFLAGS="${DEFAULT_CFLAGS}" \
    LDFLAGS="${DEFAULT_LDFLAGS} -lunwind" \
    ./configure ${STATIC_CONFIGURE} --enable-libunwind
    make -j${JOBS}
fi # gperftools

# libxkbcommon
if [ ! -f "${SDK}/lib/pkgconfig/xkbcommon.pc" ]; then
    cd ${SRC}
    XKB_SRC=libxkbcommon-${XKBCOMMON_V}
    rm -rf ${XKB_SRC} || true
    tar xf ${DIST}/${XKB_SRC}.tar.xz
    cd ${XKB_SRC}
    ./configure ${DEFAULT_CONFIGURE} --disable-docs
    make -j${JOBS}
    make install
fi # libxkbcommon

# qt
if [ ! -f "${QMAKE_BIN}" ]; then
    cd ${SRC}
    QT_SRC="qt-everywhere-src-${QT_V}"
    rm -rf ${QT_SRC} || true
    tar xf ${DIST}/${QT_SRC}.tar.xz
    cd ${QT_SRC}
    (cd qtbase ; patch -p1 < ${DIST}/hidpi.patch)
    ./configure \
    -prefix ${SDK} \
    -c++std c++11 \
    -qtlibinfix Friction \
    -opengl desktop \
    -release \
    -shared \
    -opensource \
    -confirm-license \
    -optimize-size \
    -strip \
    -pulseaudio \
    -fontconfig \
    -system-freetype \
    -qt-pcre \
    -qt-zlib \
    -qt-xcb \
    -qt-libpng \
    -no-mtdev \
    -no-syslog \
    -no-pch \
    -no-glib \
    -no-dbus \
    -no-avx2 \
    -no-avx512 \
    -no-gif \
    -no-ico \
    -no-tiff \
    -no-webp \
    -no-jasper \
    -no-libjpeg \
    -no-ssl \
    -no-cups \
    -no-mng \
    -no-gstreamer \
    -no-alsa \
    -no-sql-db2 \
    -no-sql-ibase \
    -no-sql-mysql \
    -no-sql-oci \
    -no-sql-odbc \
    -no-sql-psql \
    -no-sql-sqlite2 \
    -no-sql-sqlite \
    -no-sql-tds \
    -no-gtk \
    -no-eglfs \
    -no-kms \
    -no-linuxfb \
    -nomake examples \
    -nomake tests \
    -skip qt3d \
    -skip qtactiveqt \
    -skip qtcanvas3d \
    -skip qtcharts \
    -skip qtconnectivity \
    -skip qtdatavis3d \
    -skip qtdoc \
    -skip qtgamepad \
    -skip qtgraphicaleffects \
    -skip qtlocation \
    -skip qtnetworkauth \
    -skip qtpurchasing \
    -skip qtquickcontrols \
    -skip qtquickcontrols2 \
    -skip qtremoteobjects \
    -skip qtscript \
    -skip qtscxml \
    -skip qtsensors \
    -skip qtserialbus \
    -skip qtserialport \
    -skip qtspeech \
    -skip qttools \
    -skip qtvirtualkeyboard \
    -skip qtwebchannel \
    -skip qtwebengine \
    -skip qtwebglplugin \
    -skip qtwebsockets \
    -skip qtwebview \
    -skip qtx11extras \
    -skip qtxmlpatterns \
    -skip qttools
    make -j${JOBS}
    make install
fi # qt

# qscintilla
if [ ! -f "${SDK}/lib/libqscintilla2_friction_qt5.so" ]; then
    cd ${SRC}
    QSC_SRC="QScintilla_src-${QSCINTILLA_V}"
    rm -rf ${QSC_SRC}
    tar xf ${DIST}/${QSC_SRC}.tar.gz
    cd ${QSC_SRC}/src
    sed -i 's/qscintilla2_qt/qscintilla2_friction_qt/g' qscintilla.pro
    ${SDK}/bin/qmake CONFIG+=release
    make -j${JOBS}
    cp -a libqscintilla2_friction_qt5* ${SDK}/lib/
    cp -a Qsci ${SDK}/include/
fi # qscintilla

echo "SDK DONE"
