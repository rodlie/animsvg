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
VERSION=${VERSION:-""}
FRICTION_PKG=friction-${VERSION}

if [ "${VERSION}" = "" ]; then
    echo "Missing version"
    exit 1
fi

if [ ! -d "${BUILD}/${FRICTION_PKG}" ]; then
    echo "Missing Friction build"
    exit 1
fi

export PATH="${SDK}/bin:${PATH}"
export PKG_CONFIG_PATH="${SDK}/lib/pkgconfig"
export LD_LIBRARY_PATH="${SDK}/lib:${LD_LIBRARY_PATH}"

BIN_DIR=${BUILD}/${FRICTION_PKG}/opt/friction/bin
LIB_DIR=${BUILD}/${FRICTION_PKG}/opt/friction/lib
PLUG_DIR=${BUILD}/${FRICTION_PKG}/opt/friction/plugins

APP_DEPENDS=`(cd ${BIN_DIR} ; ldd friction | awk '{print $3}' | grep ${SDK})`

for so in ${APP_DEPENDS}; do
    cp ${so} ${LIB_DIR}/
done

for so in ${LIB_DIR}/*.so*; do
    DEPENDS=`ldd ${so} | awk '{print $3}'`
    for lib in ${DEPENDS}; do
        if [[ ${lib} == ${SDK}* ]]; then
            cp ${lib} ${LIB_DIR}/
        fi
    done
done

cp ${SDK}/plugins/audio/libqtmedia_pulse.so ${PLUG_DIR}/audio/
cp ${SDK}/plugins/generic/libqevdevkeyboardplugin.so ${PLUG_DIR}/generic/
cp ${SDK}/plugins/generic/libqevdevmouseplugin.so ${PLUG_DIR}/generic/
cp ${SDK}/plugins/generic/libqevdevtabletplugin.so ${PLUG_DIR}/generic/
cp ${SDK}/plugins/generic/libqevdevtouchplugin.so ${PLUG_DIR}/generic/
cp ${SDK}/plugins/generic/libqtuiotouchplugin.so ${PLUG_DIR}/generic/
cp ${SDK}/plugins/imageformats/libqsvg.so ${PLUG_DIR}/imageformats/
cp ${SDK}/plugins/platforminputcontexts/libcomposeplatforminputcontextplugin.so ${PLUG_DIR}/platforminputcontexts/
cp ${SDK}/plugins/platforms/libqoffscreen.so ${PLUG_DIR}/platforms/
cp ${SDK}/plugins/platforms/libqxcb.so ${PLUG_DIR}/platforms/
cp ${SDK}/plugins/xcbglintegrations/libqxcb-glx-integration.so ${PLUG_DIR}/xcbglintegrations/

for so in ${PLUG_DIR}/*/*.so; do
    DEPENDS=`ldd ${so} | awk '{print $3}'`
    for lib in ${DEPENDS}; do
        if [[ ${lib} == ${SDK}* ]]; then
            cp ${lib} ${LIB_DIR}/
        fi
    done
done

for so in ${LIB_DIR}/*.so*; do
    DEPENDS=`ldd ${so} | awk '{print $3}'`
    for lib in ${DEPENDS}; do
        if [[ ${lib} == ${SDK}* ]]; then
            cp ${lib} ${LIB_DIR}/
        fi
    done
done

HICOLOR="
128x128
16x16
192x192
22x22
256x256
32x32
48x48
64x64
96x96
scalable
"

mkdir -p ${BUILD}/${FRICTION_PKG}/usr/bin
mkdir -p ${BUILD}/${FRICTION_PKG}/usr/share/mime/packages
mkdir -p ${BUILD}/${FRICTION_PKG}/usr/share/applications

(cd ${BUILD}/${FRICTION_PKG}/usr/bin; ln -sf ../../opt/friction/bin/friction .)
(cd ${BUILD}/${FRICTION_PKG}/usr/share/mime/packages ; ln -sf ../../../../opt/friction/share/mime/packages/friction.xml .)
(cd ${BUILD}/${FRICTION_PKG}/usr/share/applications; ln -sf ../../../opt/friction/share/applications/friction.desktop .)

for icon in ${HICOLOR}; do
    ICON_SUFFIX=png
    if [ "${icon}" = "scalable" ]; then
        ICON_SUFFIX=svg
    fi
    mkdir -p ${BUILD}/${FRICTION_PKG}/usr/share/icons/hicolor/${icon}/{apps,mimetypes}
    ICON_APP_DIR=${BUILD}/${FRICTION_PKG}/usr/share/icons/hicolor/${icon}/apps
    ICON_MIME_DIR=${BUILD}/${FRICTION_PKG}/usr/share/icons/hicolor/${icon}/mimetypes
    (cd ${ICON_APP_DIR} ; ln -sf ../../../../../../opt/friction/share/icons/hicolor/${icon}/apps/friction.${ICON_SUFFIX} .)
    (cd ${ICON_MIME_DIR} ; ln -sf ../../../../../../opt/friction/share/icons/hicolor/${icon}/mimetypes/application-x-friction.${ICON_SUFFIX} .)
done

strip -s ${BUILD}/${FRICTION_PKG}/opt/friction/bin/friction
strip -s ${BUILD}/${FRICTION_PKG}/opt/friction/lib/*so*
strip -s ${BUILD}/${FRICTION_PKG}/opt/friction/plugins/*/*.so

cd ${BUILD}
tar cvf ${FRICTION_PKG}.tar ${FRICTION_PKG}

if [ ! -d "${HOME}/rpmbuild/SOURCES" ]; then
    mkdir -p ${HOME}/rpmbuild/SOURCES
fi

mv ${FRICTION_PKG}.tar ${HOME}/rpmbuild/SOURCES/
cat ${CWD}/vfxplatform.spec | sed 's/__FRICTION_VERSION__/'${VERSION}'/g' > rpm.spec
RPM_PKG=friction-${VERSION}-1.x86_64.rpm
DEB_PKG=friction_${VERSION}-1_amd64.deb

# RPM
rpmbuild -bb rpm.spec

# DEB
rm -rf deb || true
mkdir deb
(cd deb; fakeroot alien -k --to-deb --scripts ${HOME}/rpmbuild/RPMS/x86_64/${RPM_PKG})

# Portable
FRICTION_PORTABLE=${FRICTION_PKG}-portable-x86_64
FRICTION_PORTABLE_DIR=${BUILD}/${FRICTION_PORTABLE}
cd ${BUILD}
mv ${BUILD}/${FRICTION_PKG} ${FRICTION_PORTABLE_DIR}
(cd ${FRICTION_PORTABLE_DIR} ;
rm -rf usr
mv opt/friction/* .
rm -rf opt share/doc
ln -sf bin/friction .
echo "[Paths]" > bin/qt.conf
echo "Prefix = .." >> bin/qt.conf
echo "Plugins = plugins" >> bin/qt.conf
)
(cd ${FRICTION_PORTABLE_DIR}/bin ; patchelf --set-rpath '$ORIGIN/../lib' friction)
(cd ${FRICTION_PORTABLE_DIR}/lib ;
for so in *.so*; do
    patchelf --set-rpath '$ORIGIN' ${so}
done
)

PLUGS="
audio
generic
imageformats
platforminputcontexts
platforms
xcbglintegrations
"
for pdir in ${PLUGS}; do
    for so in ${FRICTION_PORTABLE_DIR}/plugins/${pdir}/*.so; do
        patchelf --set-rpath '$ORIGIN/../../lib' ${so}
    done
done

cd ${BUILD}
tar cvf ${FRICTION_PORTABLE}.tar ${FRICTION_PORTABLE}
xz -9 ${FRICTION_PORTABLE}.tar

# AppImage
(cd ${FRICTION_PORTABLE_DIR} ;
ln -sf bin/friction AppRun
ln -sf share/applications/friction.desktop .
ln -sf share/icons/hicolor/256x256/apps/friction.png .
ln -sf share/icons/hicolor/256x256/apps/friction.png .DirIcon
)
tar xf ${DISTFILES}/appimagetool.tar.xz
ARCH=x86_64 ./appimagetool/AppRun ${FRICTION_PORTABLE}

echo "PKGS DONE"
