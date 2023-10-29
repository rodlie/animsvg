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

CWD=${CWD:-`pwd`}
SDK=${SDK:-"/opt/friction"}
DISTFILES=${DISTFILES:-"/mnt"}
BUILD=${BUILD:-"${HOME}"}

REL=${REL:-1}
BRANCH=${BRANCH:-""}
COMMIT=${COMMIT:-""}
TAG=${TAG:-""}
MKJOBS=${MKJOBS:-4}

# Build SDK
# CWD=${CWD} SDK=${SDK} DISTFILES=${DISTFILES} MKJOBS=${MKJOBS} ${CWD}/build_vfxplatform_skia.sh
# CWD=${CWD} SDK=${SDK} DISTFILES=${DISTFILES} MKJOBS=${MKJOBS} ${CWD}/build_vfxplatform_sdk.sh
# CWD=${CWD} SDK=${SDK} DISTFILES=${DISTFILES} MKJOBS=${MKJOBS} ${CWD}/build_vfxplatform_ffmpeg.sh

# Build Friction
CWD=${CWD} \
SDK=${SDK} \
BUILD=${BUILD} \
MKJOBS=${MKJOBS} \
REL=${REL} \
BRANCH=${BRANCH} \
COMMIT=${COMMIT} \
TAG=${TAG} \
${CWD}/build_vfxplatform_friction.sh

# Get Friction version
VERSION=`cat ${BUILD}/friction/build-vfxplatform/version.txt`
if [ "${REL}" != 1 ]; then
    GIT=`(cd ${BUILD}/friction ; git rev-parse --short HEAD)`
    VERSION="${VERSION}-dev-${GIT}"
fi

# Package Friction
CWD=${CWD} \
SDK=${SDK} \
DISTFILES=${DISTFILES} \
BUILD=${BUILD} \
VERSION=${VERSION} \
${CWD}/build_vfxplatform_package_tar.sh
