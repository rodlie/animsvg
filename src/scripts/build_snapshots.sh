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

JAMMY=${JAMMY:-1}
LUNAR=${LUNAR:-1}
EL=${EL:-1}
FC=${FC:-1}

CWD=`pwd`
DOCKER="docker run -t --mount type=bind,source=${CWD}/snapshots,target=/snapshots"
FRICTION_DIST="${CWD}/snapshots/distfiles"
SF_NET_SRC="https://sourceforge.net/projects/friction/files/source"

QUAZIP_V="1.4"
QSCINTILLA_V="2.13.4"
FFMPEG_V="4.4.4"
UNWIND_V="1.6.2"
GPERF_V="e590eba"
SKIA_V="4fcb5c225a"

if [ ! -d "${FRICTION_DIST}" ]; then
    mkdir -p ${FRICTION_DIST}
fi

if [ "${EL}" = 1 ] || [ "${FC}" = 1 ]; then
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
if [ "${EL}" = 1 ]; then
    $DOCKER friction-el9
fi
if [ "${FC}" = 1 ]; then
    $DOCKER friction-fc38
    $DOCKER friction-fc37
fi

cd ${CWD}

echo "# Friction Snapshots" > snapshots/README.md
echo "" >> snapshots/README.md
echo "Snapshots of [Friction](https://github.com/friction2d/friction) for supported platforms." >> snapshots/README.md
echo "" >> snapshots/README.md

for md in snapshots/friction-latest-*.md; do
    cat ${md} >> snapshots/README.md
done

echo "" >> snapshots/README.md
echo "***Friction is beta quality, expect missing and/or broken features.***" >> snapshots/README.md
echo "" >> snapshots/README.md
