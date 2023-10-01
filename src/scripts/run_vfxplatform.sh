#!/bin/bash
CWD=`pwd`
docker run -it --mount type=bind,source=${CWD}/snapshots/distfiles,target=/mnt friction-vfxplatform

