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

# Build SDK
# CWD=${CWD} SDK=${SDK} DISTFILES=${DISTFILES} ${CWD}/build_vfxplatform_skia.sh
# CWD=${CWD} SDK=${SDK} DISTFILES=${DISTFILES} ${CWD}/build_vfxplatform_sdk.sh
# CWD=${CWD} SDK=${SDK} DISTFILES=${DISTFILES} ${CWD}/build_vfxplatform_ffmpeg.sh

# Build friction
CWD=${CWD} SDK=${SDK} BUILD=${BUILD} ${CWD}/build_vfxplatform_friction.sh

# Get Friction version
VERSION=`cat ${BUILD}/friction/build-vfxplatform/version.txt`

# Package Friction (TAR)
CWD=${CWD} SDK=${SDK} BUILD=${BUILD} VERSION=${VERSION} ${CWD}/build_vfxplatform_package_tar.sh
