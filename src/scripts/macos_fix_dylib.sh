#!/bin/bash
#
# Friction - https://friction.graphics
#
# Copyright (c) Ole-André Rodlie and contributors
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

BASEPATH=`pwd`

for lib in *.dylib; do
    DY=`otool -L ${lib} | grep ${BASEPATH}`
    for dylib in $DY; do
        if [ -f "${dylib}" ]; then
            BASE=`basename ${dylib}`
            if [ "${BASE}" = "${lib}" ]; then
                install_name_tool -id @rpath/${BASE} ${lib}
            else
                install_name_tool -change ${dylib} @rpath/${BASE} ${lib}
            fi
        fi
    done
done
