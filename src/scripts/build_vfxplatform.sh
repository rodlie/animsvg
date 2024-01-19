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

# Build SDK
if [ ! -d "${SDK}" ]; then
    mkdir -p ${SDK}
fi
if [ -f "${SDK_TAR}.xz" ]; then
(cd ${SDK}/.. ; tar xf ${SDK_TAR}.xz )
else
SDK=${SDK} DISTFILES=${DISTFILES} MKJOBS=${MKJOBS} ${BUILD}/build_vfxplatform_sdk01.sh
SDK=${SDK} DISTFILES=${DISTFILES} MKJOBS=${MKJOBS} ${BUILD}/build_vfxplatform_sdk02.sh
SDK=${SDK} DISTFILES=${DISTFILES} MKJOBS=${MKJOBS} ${BUILD}/build_vfxplatform_sdk03.sh
(cd ${SDK}/.. ;
    rm -rf friction/src
    tar cvvf ${SDK_TAR} friction
    xz -9 ${SDK_TAR}
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
${BUILD}/build_vfxplatform_friction.sh

# Get Friction version
VERSION=`cat ${BUILD}/friction/build-vfxplatform/version.txt`
if [ "${REL}" != 1 ]; then
    GIT=`(cd ${BUILD}/friction ; git rev-parse --short HEAD)`
    VERSION="${VERSION}-dev-${GIT}"
fi

# Package Friction
SDK=${SDK} \
DISTFILES=${DISTFILES} \
BUILD=${BUILD} \
VERSION=${VERSION} \
${BUILD}/build_vfxplatform_package.sh
