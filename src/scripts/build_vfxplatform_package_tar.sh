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
SDK=${SDK:-"/opt/friction"}
FRICTION_PKG=${FRICTION_PKG:-friction-0.9.5-Linux-x86_64-X11}

if [ ! -d "${CWD}/${FRICTION_PKG}" ]; then
    echo "Missing Friction build"
    exit 1
fi

export PATH="${SDK}/bin:${PATH}"
export PKG_CONFIG_PATH="${SDK}/lib/pkgconfig"
export LD_LIBRARY_PATH="${SDK}/lib:${LD_LIBRARY_PATH}"

BIN_DIR=${CWD}/${FRICTION_PKG}/opt/friction/bin
LIB_DIR=${CWD}/${FRICTION_PKG}/opt/friction/lib
PLUG_DIR=${CWD}/${FRICTION_PKG}/opt/friction/plugins

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

mkdir -p ${CWD}/${FRICTION_PKG}/usr/bin
mkdir -p ${CWD}/${FRICTION_PKG}/usr/share/mime/packages

(cd ${CWD}/${FRICTION_PKG}/usr/bin; ln -sf ../../opt/friction/bin/friction .)
(cd ${CWD}/${FRICTION_PKG}/usr/share/mime/packages ; ln -sf ../../../../opt/friction/share/mime/packages/friction.xml .)

for icon in ${HICOLOR}; do
    ICON_SUFFIX=png
    if [ "${icon}" = "scalable" ]; then
        ICON_SUFFIX=svg
    fi
    mkdir -p ${CWD}/${FRICTION_PKG}/usr/share/icons/hicolor/${icon}/{apps,mimetypes}
    ICON_APP_DIR=${CWD}/${FRICTION_PKG}/usr/share/icons/hicolor/${icon}/apps
    ICON_MIME_DIR=${CWD}/${FRICTION_PKG}/usr/share/icons/hicolor/${icon}/mimetypes
    (cd ${ICON_APP_DIR} ; ln -sf ../../../../../../opt/friction/share/icons/hicolor/${icon}/apps/friction.${ICON_SUFFIX} .)
    (cd ${ICON_MIME_DIR} ; ln -sf ../../../../../../opt/friction/share/icons/hicolor/${icon}/mimetypes/application-x-friction.${ICON_SUFFIX} .)
done

cd ${CWD}
tar cvvf ${FRICTION_PKG}.tar ${FRICTION_PKG}

echo "PKG TAR DONE"
