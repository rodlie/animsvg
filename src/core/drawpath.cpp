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

#include "drawpath.h"

#include "Segments/fitcurves.h"
#include "pointhelpers.h"
#include "Segments/smoothcurves.h"

DrawPath::DrawPath() {}

void DrawPath::lineTo(const QPointF& pos) {
    if(!mPts.isEmpty()) {
        const QPointF lastPos = mPts.last();
        const QPointF dPos = pos - lastPos;
        const qreal dist = pointToLen(dPos);
        const int n = qFloor(dist/3);
        mPts.reserve(mPts.size() + n);
        const qreal div = 1./(1 + n);
        for(int i = 1; i <= n; i++) {
            mPts.append(lastPos + i*dPos*div);
        }
    }
    mPts.append(pos);
}

QPointF operator*(const QPointF& p1, const QPointF& p2);

QPointF operator/(const QPointF& p1, const QPointF& p2) {
    return {p1.x()/p2.x(), p1.y()/p2.y()};
}

void DrawPath::smooth(const int window) {
    mSmoothPts.clear();
    SmoothCurves::movingAverage(mPts, mSmoothPts, true, true, window);
}

void DrawPath::fit(const qreal maxError, const bool split) {
    mFitted.clear();
    if(mSmoothPts.count() < 2) return;

    const auto adder = [this](const int n, const BezierCurve curve) {
        Q_UNUSED(n)
        const auto qptData = reinterpret_cast<QPointF*>(curve);
        const QPointF& p0 = qptData[0];
        const QPointF& c1 = qptData[1];
        const QPointF& c2 = qptData[2];
        const QPointF& p3 = qptData[3];

        mFitted.append(qCubicSegment2D{p0, c1, c2, p3});
    };
    std::sort(mForceSplits.begin(), mForceSplits.end());
    int min = 0;
    for(int i = 0; i < mForceSplits.count() + 1; i++) {
        const bool last = i == mForceSplits.count();
        int max = last ? mSmoothPts.count() - 1 :
                         mForceSplits.at(i);
        FitCurves::FitCurve(mSmoothPts, maxError, adder, min, max, false, split);
        min = max;
    }
}

void DrawPath::clear() {
    mForceSplits.clear();
    mSmoothPts.clear();
    mPts.clear();
    mFitted.clear();
}

void DrawPath::addForceSplit(const int id) {
    removeForceSplit(id);
    mForceSplits << id;
}

void DrawPath::removeForceSplit(const int id) {
    mForceSplits.removeOne(id);
}

int DrawPath::nearestSmoothPt(const QPointF& pos, qreal* const dist) const {
    int nearestSmoothId = -1;
    qreal minDist = DBL_MAX;
    for(int i = 0; i < mSmoothPts.count(); i++) {
        const auto& pt = mSmoothPts.at(i);
        const qreal dist = pointToLen(pos - pt);
        if(dist < minDist) {
            minDist = dist;
            nearestSmoothId = i;
        }
    }
    if(dist) *dist = minDist;
    return nearestSmoothId;
}

int DrawPath::nearestForceSplit(const QPointF& pos, qreal* const dist) const {
    int nearestSplitId = -1;
    qreal minDist = DBL_MAX;
    for(const int split : mForceSplits) {
        const QPointF& splitPos = mSmoothPts.at(split);
        const qreal dist = pointToLen(pos - splitPos);
        if(dist < minDist) {
            minDist = dist;
            nearestSplitId = split;
        }
    }
    if(dist) *dist = minDist;
    return nearestSplitId;
}
