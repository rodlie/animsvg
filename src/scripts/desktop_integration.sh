#!/bin/sh
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

# Desktop integration for Linux portable

set -e -x

CWD=`pwd`
SRC=${SRC:-${CWD}}
DEST=${DEST:-"${HOME}/.local/share"}

DESKTOP_FILE=${SRC}/share/applications/graphics.friction.Friction.desktop
MIME_FILE=${SRC}/share/mime/packages/graphics.friction.Friction.xml
DESKTOP_ICON_SVG=${SRC}/share/icons/hicolor/scalable/apps/graphics.friction.Friction.svg
DESKTOP_ICON_PNG=${SRC}/share/icons/hicolor/256x256/apps/graphics.friction.Friction.png
MIME_ICON_SVG=${SRC}/share/icons/hicolor/scalable/mimetypes/application-x-graphics.friction.Friction.svg
MIME_ICON_PNG=${SRC}/share/icons/hicolor/256x256/mimetypes/application-x-graphics.friction.Friction.png

if [ ! -d "${DEST}" ]; then
    mkdir -p ${DEST}
fi
if [ ! -d "${DEST}/applications" ]; then
    mkdir -p ${DEST}/applications
fi
if [ ! -d "${DEST}/mime/packages" ]; then
    mkdir -p ${DEST}/mime/packages
fi
if [ ! -d "${DEST}/icons/hicolor/scalable/apps" ]; then
    mkdir -p ${DEST}/icons/hicolor/scalable/apps
fi
if [ ! -d "${DEST}/icons/hicolor/scalable/mimetypes" ]; then
    mkdir -p ${DEST}/icons/hicolor/scalable/mimetypes
fi
if [ ! -d "${DEST}/icons/hicolor/256x256/apps" ]; then
    mkdir -p ${DEST}/icons/hicolor/256x256/apps
fi
if [ ! -d "${DEST}/icons/hicolor/256x256/mimetypes" ]; then
    mkdir -p ${DEST}/icons/hicolor/256x256/mimetypes
fi

cat ${DESKTOP_FILE} | sed 's#Exec=friction#Exec='${SRC}'/bin/friction#' > ${DEST}/applications/graphics.friction.Friction.desktop
cp -a ${MIME_FILE} ${DEST}/mime/packages/graphics.friction.Friction.xml
cp -a ${DESKTOP_ICON_SVG} ${DEST}/icons/hicolor/scalable/apps/graphics.friction.Friction.svg
cp -a ${DESKTOP_ICON_PNG} ${DEST}/icons/hicolor/256x256/apps/graphics.friction.Friction.png
cp -a ${MIME_ICON_SVG} ${DEST}/icons/hicolor/scalable/mimetypes/application-x-graphics.friction.Friction.svg
cp -a ${MIME_ICON_PNG} ${DEST}/icons/hicolor/256x256/mimetypes/application-x-graphics.friction.Friction.png
