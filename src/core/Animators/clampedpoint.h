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

#ifndef CLAMPEDPOINT_H
#define CLAMPEDPOINT_H
#include "clampedvalue.h"

class CORE_EXPORT ClampedPoint {
    ClampedValue mX;
    ClampedValue mY;
public:
    ClampedPoint(const qreal x, const qreal y);

    void read(QIODevice * const target);

    void saveXValue();
    void restoreSavedXValue();

    void saveYValue();
    void restoreSavedYValue();

    void saveValue();
    void restoreSavedValue();

    QVector3D getRawSavedValue() const;
    qreal getRawSavedXValue() const;
    qreal getRawSavedYValue() const;

    QVector3D getRawValue() const;
    qreal getRawXValue() const;
    qreal getRawYValue() const;

    QVector3D getClampedSavedValue(const QPointF& relTo) const;
    QVector3D getClampedValue(const QPointF& relTo) const;

    void setXValue(const qreal value);
    void setYValue(const qreal value);
    void setValue(const QPointF& value);

    void clampValue();

    void setXRange(const qreal min, const qreal max);
    void setYRange(const qreal min, const qreal max);

    void setXMax(const qreal max);
    void setXMin(const qreal min);

    void setYMax(const qreal max);
    void setYMin(const qreal min);
};

#endif // CLAMPEDPOINT_H
