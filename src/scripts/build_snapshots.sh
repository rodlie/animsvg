#!/bin/bash
set -e -x

JAMMY=${JAMMY:-1}
LUNAR=${LUNAR:-1}
EL9=${EL9:-1}

CWD=`pwd`
DOCKER="docker run -t --mount type=bind,source=${CWD}/snapshots,target=/snapshots"
FRICTION_DIST=${CWD}/snapshots/distfiles
SF_NET_SRC="https://sourceforge.net/projects/friction/files/source"

if [ ! -d "${FRICTION_DIST}" ]; then
    mkdir -p ${FRICTION_DIST}
fi

if [ "${EL9}" = 1 ]; then
    if [ ! -f "${FRICTION_DIST}/quazip.tar.gz" ]; then
        curl -L -k "${SF_NET_SRC}/quazip-${QUAZIP_V}.tar.gz" --output ${FRICTION_DIST}/quazip.tar.gz
    fi
    if [ ! -f "${FRICTION_DIST}/qscintilla.tar.gz" ]; then
        curl -L -k "${SF_NET_SRC}/QScintilla_src-${QSCINTILLA_V}.tar.gz/download" --output ${FRICTION_DIST}/qscintilla.tar.gz
    fi
    if [ ! -f "${FRICTION_DIST}/ffmpeg.tar.xz" ]; then
        curl -L -k "${SF_NET_SRC}/ffmpeg-${FFMPEG_V}.tar.xz/download" --output ${FRICTION_DIST}/ffmpeg.tar.xz
    fi
    if [ ! -f "${FRICTION_DIST}/libunwind.tar.gz" ]; then
        curl -L -k "${SF_NET_SRC}/libunwind-${UNWIND_V}.tar.gz/download" --output ${FRICTION_DIST}/libunwind.tar.gz
    fi
fi
if [ ! -f "${FRICTION_DIST}/gperftools.tar.xz" ]; then
    curl -k -L "${SF_NET_SRC}/gperftools-${GPERF_V}.tar.xz/download" --output ${FRICTION_DIST}/gperftools.tar.xz
fi
if [ ! -f "${FRICTION_DIST}/skia.tar.xz" ]; then
    curl -k -L "${SF_NET_SRC}/skia-${SKIA_V}-minimal.tar.xz/download" --output ${FRICTION_DIST}/skia.tar.xz
fi

if [ "${JAMMY}" = 1 ]; then
    $DOCKER friction-jammy
fi
if [ "${LUNAR}" = 1 ]; then
    $DOCKER friction-lunar
fi
if [ "${EL9}" = 1 ]; then
    $DOCKER friction-el9
fi
