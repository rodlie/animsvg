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
LOG=${CWD}/CHANGES
AFTER="2023-01-01"
FORMAT="### %s%n* **Commit:** %H%d%n* **Author:** %an%n* **Date:** %ad%n%n%b"

FROM_TAG=${FROM_TAG:-""}
TO_TAG=${TO_TAG:-""}

if [ "${FROM_TAG}" != "" ] && [ "${TO_TAG}" != "" ]; then
    echo "# ChangeLog for ${TO_TAG}" > ${LOG}-${TO_TAG}.md
    echo "" >> ${LOG}-${TO_TAG}.md
    git log --pretty=format:"${FORMAT}" --no-merges ${FROM_TAG}...${TO_TAG} >> ${LOG}-${TO_TAG}.md
elif [ "${TO_TAG}" != "" ]; then
    echo "# ChangeLog for ${TO_TAG}" > ${LOG}-${TO_TAG}.md
    echo "" >> ${LOG}-${TO_TAG}.md
    git log --pretty=format:"${FORMAT}" --after="${AFTER}" ${TO_TAG} --no-merges >> ${LOG}-${TO_TAG}.md
else
    echo "# ChangeLog" > ${LOG}
    echo "" >> ${LOG}
    git log --pretty=format:"${FORMAT}" --after="${AFTER}" --no-merges >> ${LOG}.md
fi
