#!/bin/bash
set -e -x

CWD=`pwd`
REL=${REL:-0}
BRANCH=${BRANCH:-""}
COMMIT=${COMMIT:-""}
TAG=${TAG:-""}
MKJOBS=${JOBS:-4}
ONLY_SDK=${ONLY_SDK:-0}

SDK_VERSION="20240123"

DOCKER="docker run"
DOCKER="${DOCKER} -e REL=${REL} -e MKJOBS=${JOBS} -e SDK_VERSION=${SDK_VERSION} -e ONLY_SDK=${ONLY_SDK} -e BRANCH=${BRANCH} -e COMMIT=${COMMIT} -e TAG=${TAG}"
DOCKER="${DOCKER} -t --mount type=bind,source=${CWD}/distfiles,target=/mnt"

if [ ! -d "${CWD}/distfiles" ]; then
    echo "No distfiles!"
    exit 1
fi

(cd src/scripts; docker build -t friction-vfxplatform -f Dockerfile.vfxplatform .)

${DOCKER} friction-vfxplatform
