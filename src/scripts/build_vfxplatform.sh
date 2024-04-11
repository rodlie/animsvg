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

SDK=${SDK:-"/opt/friction"}
DISTFILES=${DISTFILES:-"/mnt"}
BUILD=${BUILD:-"${HOME}"}

REL=${REL:-1}
BRANCH=${BRANCH:-""}
COMMIT=${COMMIT:-""}
TAG=${TAG:-""}
MKJOBS=${MKJOBS:-4}
SDK_VERSION=${SDK_VERSION:-""}
ONLY_SDK=${ONLY_SDK:-0}
SDK_TAR="${DISTFILES}/friction-vfxplatform-sdk-${SDK_VERSION}.tar"
DOWNLOAD_SDK=${DOWNLOAD_SDK:-0}
TAR_VERSION=${TAR_VERSION:-""}

# Download SDK
if [ "${DOWNLOAD_SDK}" = 1 ] && [ ! -f "${SDK_TAR}.bz2" ]; then
    (cd ${DISTFILES} ;
        wget https://download.friction.graphics/distfiles/vfxplatform/friction-vfxplatform-sdk-${SDK_VERSION}.tar.bz2
    )
fi

# Build SDK
if [ ! -d "${SDK}" ]; then
    mkdir -p "${SDK}/lib"
    mkdir -p "${SDK}/bin"
    (cd "${SDK}"; ln -sf lib lib64)
fi
if [ -f "${SDK_TAR}.bz2" ]; then
(cd ${SDK}/.. ; tar xf ${SDK_TAR}.bz2 )
else
SDK=${SDK} DISTFILES=${DISTFILES} MKJOBS=${MKJOBS} ${BUILD}/build_vfxplatform_sdk01.sh
SDK=${SDK} DISTFILES=${DISTFILES} MKJOBS=${MKJOBS} ${BUILD}/build_vfxplatform_sdk02.sh
SDK=${SDK} DISTFILES=${DISTFILES} MKJOBS=${MKJOBS} ${BUILD}/build_vfxplatform_sdk03.sh
(cd ${SDK}/.. ;
    rm -rf friction/src
    tar cvvf ${SDK_TAR} friction
    bzip2 -9 ${SDK_TAR}
)
fi

if [ "${ONLY_SDK}" = 1 ]; then
    exit 0
fi

# Build Friction
SDK=${SDK} \
BUILD=${BUILD} \
MKJOBS=${MKJOBS} \
REL=${REL} \
BRANCH=${BRANCH} \
COMMIT=${COMMIT} \
TAG=${TAG} \
TAR_VERSION=${TAR_VERSION} \
${BUILD}/build_vfxplatform_friction.sh

# Get Friction version
VERSION=`cat ${BUILD}/friction/build-vfxplatform/version.txt`
if [ "${REL}" != 1 ]; then
    GIT_COMMIT=`(cd ${BUILD}/friction ; git rev-parse --short=8 HEAD)`
    VERSION="${VERSION}-${GIT_COMMIT}"
fi
if [ "${TAR_VERSION}" != "" ]; then
    VERSION=${TAR_VERSION}
fi

# Package Friction
SDK=${SDK} \
DISTFILES=${DISTFILES} \
BUILD=${BUILD} \
VERSION=${VERSION} \
${BUILD}/build_vfxplatform_package.sh
