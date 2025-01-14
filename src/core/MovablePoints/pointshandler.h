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

#ifndef POINTSHANDLER_H
#define POINTSHANDLER_H
#include "movablepoint.h"
#include "../smartPointers/stdselfref.h"
class BoxTransformAnimator;
enum class CanvasMode : short;

class CORE_EXPORT PointsHandler : public StdSelfRef {
    e_OBJECT
protected:
    PointsHandler();
public:
    void insertPt(const int id, const stdsptr<MovablePoint>& pt);
    void appendPt(const stdsptr<MovablePoint>& pt);
    void remove(const stdsptr<MovablePoint>& pt);
    void removeLast();
    void clear();

    void addToSelection(MovablePoint* const pt);
    void removeFromSelection(MovablePoint* const pt);
    void clearSelection();

    MovablePoint *getPointAtAbsPos(const QVector3D &absPos,
                                   const CanvasMode mode,
                                   const qreal invScale);

    void addAllPointsToSelection(const MovablePoint::PtOp &adder,
                                 const CanvasMode mode) const;

    void addInRectForSelection(const QRectF &absRect,
                               const MovablePoint::PtOp &adder,
                               const CanvasMode mode) const;

    void drawPoints(SkCanvas * const canvas,
                    const float invScale,
                    const bool keyOnCurrentFrame,
                    const CanvasMode mode,
                    const bool ctrlPressed) const;

    template <class T>
    T* getPointWithId(const int id) const {
        if(id < 0) return nullptr;
        if(id >= mPts.count()) return nullptr;
        return static_cast<T*>(mPts.at(id).get());
    }

    bool isEmpty() const;
    int count() const;

    void move(const int from, const int to);

    BasicTransformAnimator * transform() const;

    void setTransform(BasicTransformAnimator * const trans);
private:
    BasicTransformAnimator * mTrans = nullptr;
    QList<MovablePoint*> mSelectedPts;
    QList<stdsptr<MovablePoint>> mPts;
};

#endif // POINTSHANDLER_H
