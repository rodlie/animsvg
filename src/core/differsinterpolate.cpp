/*
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
# See 'README.md' for more information.
#
*/

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#include "differsinterpolate.h"
#include "Segments/qcubicsegment1d.h"
#include "Animators/SmartPath/smartpath.h"

bool gDiffers(const QString &val1, const QString &val2) {
    return val1 != val2;
}

void gInterpolate(const qreal val1, const qreal val2,
                  const qreal t, qreal &val) {
    val = val1*(1 - t) + val2*t;
}

bool gDiffers(const bool val1, const bool val2) {
    return val1 != val2;
}

bool gDiffers(const qCubicSegment1D &val1,
              const qCubicSegment1D &val2) {
    return val1 != val2;
}

void gInterpolate(const qCubicSegment1D &val1,
                  const qCubicSegment1D &val2,
                  const qreal t, qCubicSegment1D &val) {
    val = val1*(1 - t) + val2*t;
}

bool gDiffers(const SmartPath &path1, const SmartPath &path2) {
    return &path1 != &path2;
}

void gInterpolate(const SmartPath &path1, const SmartPath &path2,
                  const qreal path2Weight, SmartPath &target) {
    SmartPath::sInterpolate(path1, path2, path2Weight, target);
}
