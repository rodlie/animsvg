#!/bin/bash

CWD=`pwd`

REL=${REL:-0}
BRANCH=${BRANCH:-""}
COMMIT=${COMMIT:-""}
TAG=${TAG:-""}
MKJOBS=${JOBS:-4}}
SDK_VERSION="20240119"
ONLY_SDK=${ONLY_SDK:-0}

DOCKER="docker run"
DOCKER="${DOCKER} -e REL=${REL} -e MKJOBS=${JOBS} -e SDK_VERSION=${SDK_VERSION} -e ONLY_SDK=${ONLY_SDK} -e BRANCH=${BRANCH} -e COMMIT=${COMMIT} -e TAG=${TAG}"
DOCKER="${DOCKER} -t --mount type=bind,source=${CWD}/distfiles,target=/mnt"

${DOCKER} friction-vfxplatform
