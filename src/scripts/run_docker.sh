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

REL=${REL:-0}
BRANCH=${BRANCH:-""}
COMMIT=${COMMIT:-""}
TAG=${TAG:-""}
MKJOBS=${MKJOBS:-4}

JAMMY=${JAMMY:-1}
MANTIC=${MANTIC:-1}
NOBLE=${NOBLE:-1}

MOUNT_DIR="distfiles"
DOCKER_MOUNT="-t --mount type=bind,source=${CWD}/${MOUNT_DIR},target=/${MOUNT_DIR}"
DOCKER="docker run -e REL=${REL} -e MKJOBS=${MKJOBS}"

if [ "${BRANCH}" != "" ]; then
    DOCKER="${DOCKER} -e FRICTION_BRANCH=${BRANCH}"
fi
if [ "${COMMIT}" != "" ]; then
    DOCKER="${DOCKER} -e FRICTION_COMMIT=${COMMIT}"
fi
if [ "${TAG}" != "" ]; then
    DOCKER="${DOCKER} -e FRICTION_TAG=${TAG}"
fi
DOCKER="${DOCKER} ${DOCKER_MOUNT}"

if [ "${JAMMY}" = 1 ]; then
    (cd ${CWD}/src/scripts; docker build -t friction-jammy -f Dockerfile.jammy .)
    $DOCKER friction-jammy
fi
if [ "${MANTIC}" = 1 ]; then
    (cd ${CWD}/src/scripts; docker build -t friction-mantic -f Dockerfile.mantic .)
    $DOCKER friction-mantic
fi
if [ "${NOBLE}" = 1 ]; then
    (cd ${CWD}/src/scripts; docker build -t friction-noble -f Dockerfile.noble .)
    $DOCKER friction-noble
fi
