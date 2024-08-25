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

CWD=`pwd`
MKJOBS=${MKJOBS:-2}
COMMIT=`git rev-parse --short=8 HEAD`
BRANCH=`git rev-parse --abbrev-ref HEAD`

ASDK=20240401
SDK=20240609
URL=https://github.com/friction2d/friction-sdk/releases/download/${SDK}
APPIMAGE_TAR=friction-appimage-tools-${ASDK}.tar.xz
SDK_TAR=friction-vfxplatform-CY2021-sdk-${SDK}.tar.bz2

mkdir -p distfiles/sdk

cd distfiles
if [ ! -d "linux" ]; then
    if [ ! -f "${APPIMAGE_TAR}" ]; then
        wget ${URL}/${APPIMAGE_TAR}
    fi
    tar xvf ${APPIMAGE_TAR}
fi

cd sdk
if [ ! -f "${SDK_TAR}" ]; then
    wget ${URL}/${SDK_TAR}
fi

cd ${CWD}

MKJOBS=${MKJOBS} REL=0 BRANCH=${BRANCH} COMMIT=${COMMIT} ./src/scripts/run_vfxplatform.sh
