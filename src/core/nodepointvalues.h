/*
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
# See 'README.md' for more information.
#
*/

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#ifndef NODEPOINTVALUES_H
#define NODEPOINTVALUES_H
#include <QPointF>
#include "core_global.h"

struct CORE_EXPORT NodePointValues {
    QVector3D fC0;
    QVector3D fP1;
    QVector3D fC2;

    NodePointValues &operator/=(const qreal val) {
        qreal inv = 1/val;
        fC0 *= inv;
        fP1 *= inv;
        fC2 *= inv;
        return *this;
    }

    NodePointValues &operator*=(const qreal val) {
        fC0 *= val;
        fP1 *= val;
        fC2 *= val;
        return *this;
    }

    NodePointValues &operator+=(const NodePointValues &ppv) {
        fC0 += ppv.fC0;
        fP1 += ppv.fP1;
        fC2 += ppv.fC2;
        return *this;
    }

    NodePointValues &operator-=(const NodePointValues &ppv) {
        fC0 -= ppv.fC0;
        fP1 -= ppv.fP1;
        fC2 -= ppv.fC2;
        return *this;
    }
};

NodePointValues operator+(const NodePointValues &ppv1, const NodePointValues &ppv2);
NodePointValues operator-(const NodePointValues &ppv1, const NodePointValues &ppv2);
NodePointValues operator/(const NodePointValues &ppv, const qreal val);
NodePointValues operator*(const NodePointValues &ppv, const qreal val);
NodePointValues operator*(const qreal val, const NodePointValues &ppv);

#endif // NODEPOINTVALUES_H
