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

CWD=`pwd`
SRC=${CWD}/src
PDIR=${CWD}/presets
TAG="/*_FRICTION_EXPRESSION_PRESET_*/"
PRESETS="
easeInSine
easeOutSine
easeInOutSine
easeInQuad
easeOutQuad
easeInOutQuad
easeInCubic
easeOutCubic
easeInOutCubic
easeInQuart
easeOutQuart
easeInOutQuart
easeInQuint
easeOutQuint
easeInOutQuint
easeInExpo
easeOutExpo
easeInOutExpo
easeInCirc
easeOutCirc
easeInOutCirc
easeInBack
easeOutBack
easeInOutBack
easeInElastic
easeOutElastic
easeInOutElastic
easeInBounce
easeOutBounce
easeInOutBounce
"

for preset in ${PRESETS}; do
    PFILE=${PDIR}/${preset}.js
    echo ${TAG} > ${PFILE}
    cat ${SRC}/${preset}.js >> ${PFILE}
    echo ${TAG} >> ${PFILE}
    cat ${SRC}/bindings.js >> ${PFILE}
    echo ${TAG} >> ${PFILE}
    cat ${SRC}/common.js >> ${PFILE}
    echo "return Math.${preset}(frame, startVal, endVal, duration);" >> ${PFILE}
done
