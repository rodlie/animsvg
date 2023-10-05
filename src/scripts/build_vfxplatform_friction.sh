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

source /opt/rh/devtoolset-7/enable
gcc -v

CWD=`pwd`
SDK=${SDK:-"/opt/friction"}
SRC=${SDK}/src

export PATH="${SDK}/bin:${PATH}"
export PKG_CONFIG_PATH="${SDK}/lib/pkgconfig"
export LD_LIBRARY_PATH="${SDK}/lib:${LD_LIBRARY_PATH}"

if [ ! -d "${SDK}" ]; then
    echo "MISSING SDK"
    exit 1
fi

if [ ! -d "${SRC}" ]; then
    mkdir -p ${SRC}
fi

if [ ! -d "${SRC}/friction" ]; then
    (cd ${SRC} ; git clone https://github.com/friction2d/friction)
fi

cd ${SRC}/friction
git fetch --all
git checkout main
git pull
git submodule update -i docs

if [ ! -d "src/skia/out" ]; then
    mv src/skia src/skia.orig
    ln -sf ${SDK}/skia src/skia
fi

if [ ! -d "src/gperftools/.libs" ]; then
    mv src/gperftools src/gperftools.orig
    ln -sf ${SDK}/gperftools src/gperftools
fi

rm -rf build-vfxplatform || true
mkdir build-vfxplatform && cd build-vfxplatform
cmake -GNinja \
-DCMAKE_INSTALL_PREFIX=${SDK} \
-DCMAKE_PREFIX_PATH=${SDK} \
-DCMAKE_BUILD_TYPE=Release \
-DUSE_ROBOTO=ON \
-DQSCINTILLA_INCLUDE_DIRS=${SDK}/include \
-DQSCINTILLA_LIBRARIES_DIRS=${SDK}/lib \
-DQSCINTILLA_LIBRARIES=qscintilla2_friction_qt5 ..
# -DCMAKE_CXX_COMPILER=clang++ \
# -DCMAKE_C_COMPILER=clang ..
cmake --build .

ls -lah src/app/friction
strip -s src/app/friction
ls -lah src/app/friction
ldd src/app/friction
objdump -p src/app/friction | grep NEEDED
