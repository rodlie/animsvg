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

#ifndef QUADSEGMENT_H
#define QUADSEGMENT_H

#include <QPointF>
#include "../core_global.h"

struct SkPoint;
template <typename T>
struct CubicSegment;

template <typename T>
struct CORE_EXPORT QuadSegment {
    T p0;
    T p1;
    T p2;

    T closestPointTo(const T& point);

    template <typename T2> explicit operator QuadSegment<T2> () const {
        T2 nP0; switchSkQ(p0, nP0);
        T2 nP1; switchSkQ(p1, nP1);
        T2 nP2; switchSkQ(p2, nP2);
        return { nP0, nP1, nP2 };
    }

    CubicSegment<T> toCubic() const {
        return {p0, p0 + 2.*(p1 - p0)/3., p2 + 2.*(p1 - p2)/3., p2};
    }
};

typedef QuadSegment<qreal> qQuadSegment1D;
typedef QuadSegment<float> SkQuadSegment1D;
typedef QuadSegment<QPointF> qQuadSegment2D;
typedef QuadSegment<SkPoint> SkQuadSegment2D;

#endif // QUADSEGMENT_H
