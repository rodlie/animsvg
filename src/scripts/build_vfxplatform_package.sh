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

SDK=${SDK:-"/opt/friction"}
DISTFILES=${DISTFILES:-"/mnt"}
BUILD=${BUILD:-"${HOME}"}
VERSION=${VERSION:-""}
APPID="graphics.friction.Friction"
FRICTION_PKG=friction-${VERSION}

APPIMAGETOOL=bfe6e0c
APPIMAGERUNTIME=1bb1157

if [ "${VERSION}" = "" ]; then
    echo "Missing version"
    exit 1
fi
PKG_VERSION=`echo ${VERSION} | sed 's/-/./g'`

if [ ! -d "${BUILD}/${FRICTION_PKG}" ]; then
    echo "Missing Friction build"
    exit 1
fi

if [ ! -d "${DISTFILES}/builds/${VERSION}" ]; then
    mkdir -p ${DISTFILES}/builds/${VERSION}
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
mkdir -p ${BUILD}/${FRICTION_PKG}/usr/share/{applications,metainfo}

(cd ${BUILD}/${FRICTION_PKG}/usr/bin; ln -sf ../../opt/friction/bin/friction .)
(cd ${BUILD}/${FRICTION_PKG}/usr/share/mime/packages ; ln -sf ../../../../opt/friction/share/mime/packages/${APPID}.xml .)
(cd ${BUILD}/${FRICTION_PKG}/usr/share/applications; ln -sf ../../../opt/friction/share/applications/${APPID}.desktop .)
(cd ${BUILD}/${FRICTION_PKG}/usr/share/metainfo; ln -sf ../../../opt/friction/share/metainfo/${APPID}.appdata.xml .)

for icon in ${HICOLOR}; do
    ICON_SUFFIX=png
    if [ "${icon}" = "scalable" ]; then
        ICON_SUFFIX=svg
    fi
    mkdir -p ${BUILD}/${FRICTION_PKG}/usr/share/icons/hicolor/${icon}/{apps,mimetypes}
    ICON_APP_DIR=${BUILD}/${FRICTION_PKG}/usr/share/icons/hicolor/${icon}/apps
    ICON_MIME_DIR=${BUILD}/${FRICTION_PKG}/usr/share/icons/hicolor/${icon}/mimetypes
    (cd ${ICON_APP_DIR} ; ln -sf ../../../../../../opt/friction/share/icons/hicolor/${icon}/apps/${APPID}.${ICON_SUFFIX} .)
    (cd ${ICON_MIME_DIR} ; ln -sf ../../../../../../opt/friction/share/icons/hicolor/${icon}/mimetypes/application-x-${APPID}.${ICON_SUFFIX} .)
done

strip -s ${BUILD}/${FRICTION_PKG}/opt/friction/bin/friction
strip -s ${BUILD}/${FRICTION_PKG}/opt/friction/lib/*so*
strip -s ${BUILD}/${FRICTION_PKG}/opt/friction/plugins/*/*.so

echo "[Paths]" > ${BUILD}/${FRICTION_PKG}/opt/friction/bin/qt.conf
echo "Prefix = .." >> ${BUILD}/${FRICTION_PKG}/opt/friction/bin/qt.conf
echo "Plugins = plugins" >> ${BUILD}/${FRICTION_PKG}/opt/friction/bin/qt.conf

(cd ${BUILD}/${FRICTION_PKG}/opt/friction/bin ; patchelf --set-rpath '$ORIGIN/../lib' friction)
(cd ${BUILD}/${FRICTION_PKG}/opt/friction/lib ;
for so in *.so*; do
    patchelf --set-rpath '$ORIGIN' ${so}
done
)

PLUGS="
audio
generic
platforminputcontexts
platforms
xcbglintegrations
"
for pdir in ${PLUGS}; do
    for so in ${BUILD}/${FRICTION_PKG}/opt/friction/plugins/${pdir}/*.so; do
        patchelf --set-rpath '$ORIGIN/../../lib' ${so}
    done
done

# RPM
cd ${BUILD}
tar cvf ${FRICTION_PKG}.tar ${FRICTION_PKG}
if [ ! -d "${HOME}/rpmbuild/SOURCES" ]; then
    mkdir -p ${HOME}/rpmbuild/SOURCES
fi
mv ${FRICTION_PKG}.tar ${HOME}/rpmbuild/SOURCES/
cat ${BUILD}/friction/src/scripts/vfxplatform.spec | sed 's/__FRICTION_PKG_VERSION__/'${PKG_VERSION}'/g;s/__FRICTION_VERSION__/'${VERSION}'/g;s/__APPID__/'${APPID}'/g' > rpm.spec
rpmbuild -bb rpm.spec
cp -a ${HOME}/rpmbuild/RPMS/*/*.rpm ${DISTFILES}/builds/${VERSION}/

# Portable
FRICTION_PORTABLE=${FRICTION_PKG}-portable-linux-x86_64
FRICTION_PORTABLE_DIR=${BUILD}/${FRICTION_PORTABLE}
cd ${BUILD}
rm -f ${FRICTION_PORTABLE_DIR} || true
mv ${BUILD}/${FRICTION_PKG} ${FRICTION_PORTABLE_DIR}
(cd ${FRICTION_PORTABLE_DIR} ;
cp ${BUILD}/friction/src/scripts/desktop_integration.sh .
chmod +x desktop_integration.sh
rm -rf usr
mv opt/friction/* .
rm -rf opt share/doc
ln -sf bin/friction .
)
cd ${BUILD}
tar cvf ${FRICTION_PORTABLE}.tar ${FRICTION_PORTABLE}
bzip2 -9 ${FRICTION_PORTABLE}.tar
cp -a ${FRICTION_PORTABLE}.tar.bz2 ${DISTFILES}/builds/${VERSION}/

# AppImage
(cd ${FRICTION_PORTABLE_DIR} ;
rm -f desktop_integration.sh
rm -f friction
mkdir usr
mv lib bin plugins share usr/
ln -sf usr/bin/friction AppRun
ln -sf usr/share/applications/${APPID}.desktop .
ln -sf usr/share/icons/hicolor/256x256/apps/${APPID}.png .
ln -sf usr/share/icons/hicolor/256x256/apps/${APPID}.png .DirIcon
)
tar xf ${DISTFILES}/linux/appimagetool-${APPIMAGETOOL}.tar.bz2
ARCH=x86_64 ./appimagetool/AppRun --verbose --runtime-file=${DISTFILES}/linux/runtime-x86_64-${APPIMAGERUNTIME}.bin ${FRICTION_PORTABLE}

# TODO
# FRICTION_ISO=${FRICTION_PKG}-x86_64.squashfs
# FRICTION_APP=${FRICTION_PKG}-test-x86_64.AppImage
# mksquashfs ${FRICTION_PORTABLE} ${FRICTION_ISO} -comp zstd -root-owned -noappend -b 1M -mkfs-time 0
# cat ${DISTFILES}/linux/runtime-x86_64-${APPIMAGERUNTIME}.bin > ${FRICTION_APP}
# cat ${FRICTION_ISO} >> ${FRICTION_APP}
# chmod a+x ${FRICTION_ISO}

cp -a *.AppImage ${DISTFILES}/builds/${VERSION}/

echo "FRICTION PACKAGE DONE"
