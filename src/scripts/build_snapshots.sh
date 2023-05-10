#!/bin/bash

set -e -x

CWD=`pwd`
$DOCKER="docker run -t --mount type=bind,source=${CWD}/snapshots,target=/snapshots"

if [ ! -d "${CWD}/snapshots" ]; then
    mkdir -p ${CWD}/snapshots
fi

$DOCKER friction-jammy
$DOCKER friction-lunar
