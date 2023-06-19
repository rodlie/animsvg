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

#ifndef CONICSEGMENT_H
#define CONICSEGMENT_H

#include "../core_global.h"

#include <QPointF>

template <typename T>
struct CubicSegment;

template <typename T>
struct QuadSegment;

struct SkPoint;

template <typename T, typename Tw>
struct CORE_EXPORT ConicSegment {
    T fP0;
    T fP1;
    T fP2;
    Tw fW;

    template <typename T2, typename T2w>
    explicit operator ConicSegment<T2, T2w> () const {
        T2 nP0; switchSkQ(fP0, nP0);
        T2 nP1; switchSkQ(fP1, nP1);
        T2 nP2; switchSkQ(fP2, nP2);
        T2w nW; switchSkQ(fW, nW);
        return { nP0, nP1, nP2, nW };
    }

//    QList<QuadSegment<T>> toQuads() const;

    CubicSegment<T> toCubic() const {
        float u = 4.f*fW/(3.f*(1.f + fW));
        return {fP0, fP0*(1.f - u) + fP1*u, fP2*(1.f - u) + fP1*u, fP2};
    }

    QuadSegment<T> toQuad() const {
        float u = 2.f*fW/(1.f + fW);
        return {fP0, (fP0 + fP2)*((1.f - u)/2.f) + fP1*u, fP2};
    }
};

typedef ConicSegment<qreal, qreal> qConicSegment1D;
typedef ConicSegment<float, float> SkConicSegment1D;
typedef ConicSegment<QPointF, qreal> qConicSegment2D;
typedef ConicSegment<SkPoint, float> SkConicSegment2D;

#endif // CONICSEGMENT_H
