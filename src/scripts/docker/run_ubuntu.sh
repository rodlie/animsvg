#!/bin/bash
set -e -x

g++ --version
cmake --version
python --version
qmake-qt5 --version || qmake --version
env

uname -a
cat /etc/os-release

mkdir -p /friction2d
cd /friction2d

#git clone --recurse-submodules https://github.com/friction2d/friction
git clone https://github.com/friction2d/friction
cd friction

MKJOBS=4 SNAP=1 DOCKER=1 GPERF_TAR=1 SKIA_TAR=1 ./src/scripts/build_ubuntu.sh
