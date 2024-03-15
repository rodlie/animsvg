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

#include "conicsegment.h"
#include "quadsegment.h"
#include "qcubicsegment2d.h"
#include "skia/skiaincludes.h"
#include <QList>

//template<typename T, typename Tw>
//QList<QuadSegment<T> > ConicSegment<T, Tw>::toQuads() const {
//    SkAutoConicToQuads helper;
//    const SkPoint * const pts =
//            helper.computeQuads(SkConic(fP0, fP1, fP2, fW), 0.1f);
//    int nQuads = helper.countQuads();

//    QList<QuadSegment<T>> segs;
//    int ptId = 0;
//    for(int i = 0; i < nQuads; i++) {
//        segs.append({pts[ptId++], pts[ptId++], pts[ptId]});
//    }
//    return segs;
//}
