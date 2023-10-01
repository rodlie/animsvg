#!/bin/bash
set -e -x

CWD=`pwd`
SDK=${SDK:-"/opt/friction"}

rm -rf build-test || true
mkdir build-test && cd build-test
cmake -GNinja \
-DCMAKE_INSTALL_PREFIX=${SDK} \
-DCMAKE_PREFIX_PATH=${SDK} \
-DCMAKE_BUILD_TYPE=Release \
-DUSE_ROBOTO=ON \
-DQSCINTILLA_INCLUDE_DIRS=${SDK}/include \
-DQSCINTILLA_LIBRARIES_DIRS=${SDK}/lib \
-DQSCINTILLA_LIBRARIES=qscintilla2_friction_qt5 \
-DCMAKE_CXX_COMPILER=clang++ \
-DCMAKE_C_COMPILER=clang ..
cmake --build .

ls -lah src/app/friction
strip -s src/app/friction
ls -lah src/app/friction
ldd src/app/friction
objdump -p src/app/friction | grep NEEDED
