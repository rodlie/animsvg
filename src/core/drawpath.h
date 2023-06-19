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

#ifndef DRAWPATH_H
#define DRAWPATH_H

#include <QtCore>

#include "Segments/cubiclist.h"

enum class ManualDrawPathState {
    none, drawn, fitted
};

class CORE_EXPORT DrawPath {
public:
    DrawPath();

    void lineTo(const QPointF& pos);
    void smooth(const int window);
    void fit(const qreal maxError, const bool split);
    QList<qCubicSegment2D>& getFitted()
    { return mFitted; }
    void clear();

    void addForceSplit(const int id);
    void removeForceSplit(const int id);

    int nearestSmoothPt(const QPointF& pos, qreal* const dist) const;
    int nearestForceSplit(const QPointF& pos, qreal* const dist) const;

    const QVector<QPointF>& smoothPts() const
    { return mSmoothPts; }
    const QList<int>& forceSplits() const
    { return mForceSplits; }
private:
    QList<int> mForceSplits;
    QList<qCubicSegment2D> mFitted;
    QVector<QPointF> mSmoothPts;
    QVector<QPointF> mPts;
};

#endif // DRAWPATH_H
