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

#ifndef SEGMENT_H
#define SEGMENT_H

class SmartNodePoint;
class SmartCtrlPoint;
class PathPointsHandler;

#include "smartPointers/stdpointer.h"
#include "Segments/qcubicsegment2d.h"
#include "pointhelpers.h"

class CORE_EXPORT Segment {
public:

};

class CORE_EXPORT NormalSegment {
public:
    bool operator==(const NormalSegment& other) const;

    bool operator!=(const NormalSegment& other) const {
        return !this->operator==(other);
    }

    struct PointOnSegment {
        qreal fT;
        QVector3D fPos;
    };
    struct SubSegment {
        SmartNodePoint* fFirstPt;
        SmartNodePoint* fLastPt;
        const NormalSegment* fParentSeg;

        bool isValid() const {
            return fFirstPt && fLastPt && fParentSeg;
        }

        qreal getMinT() const;
        qreal getMaxT() const;

        qreal getParentTAtThisT(const qreal thisT) const;
        QVector3D getRelPosAtT(const qreal thisT) const;
    };
    NormalSegment();
    NormalSegment(SmartNodePoint * const firstNode,
                  SmartNodePoint * const lastNode);

    SmartNodePoint* divideAtAbsPos(const QVector3D absPos);
    SmartNodePoint *divideAtT(const qreal &t);

    QVector3D getRelPosAtT(const qreal t) const;
    QVector3D getAbsPosAtT(const qreal t) const;

    void makePassThroughAbs(const QVector3D &absPos, const qreal t);
    void makePassThroughRel(const QVector3D &relPos, const qreal t);

    void finishPassThroughTransform();
    void startPassThroughTransform();
    void cancelPassThroughTransform();

    void generateSkPath();

    void drawHoveredSk(SkCanvas * const canvas,
                       const float invScale);

    SmartNodePoint *getFirstNode() const;
    SmartNodePoint *getLastNode() const;

    QVector3D getSlopeVector(const qreal t);

    bool isValid() const;
    bool isNormal() const;

    void reset() { clear(); }

    void clear();

    void disconnect() const;

    int nodesCount() const;

    int innerNodesCount() const {
        return mInnerDissolved.count();
    }

    SmartNodePoint* getNodeAt(const int id) const;

    SubSegment getClosestSubSegment(const QVector3D relPos,
                                    qreal& minDist) const {
        auto relSeg = getAsRelSegment();
        qreal closestT;
        minDist = relSeg.minDistanceTo(relPos, &closestT);
        return subSegmentAtT(closestT);
    }

    SubSegment getClosestSubSegmentForDummy(const QVector3D relPos,
                                            qreal& minDist) const;

    void afterChanged() const {
        updateDissolvedPos();
    }

    qCubicSegment2D getAsAbsSegment() const;
    qCubicSegment2D getAsRelSegment() const;

    void updateDissolved();

    qreal closestRelT(const QVector3D relPos) const {
        return getAsRelSegment().tValueForPointClosestTo(relPos);
    }

    qreal closestAbsT(const QVector3D absPos) const {
        return getAsAbsSegment().tValueForPointClosestTo(absPos);
    }
private:
    void updateDissolvedPos() const;
    SubSegment subSegmentAtT(const qreal t) const;

    SkPath mSkPath;
    stdptr<PathPointsHandler> mHandler_k;
    stdptr<SmartNodePoint> mFirstNode;
    stdptr<SmartCtrlPoint> mFirstNodeC2;
    QList<stdptr<SmartNodePoint>> mInnerDissolved;
    stdptr<SmartCtrlPoint> mLastNodeC0;
    stdptr<SmartNodePoint> mLastNode;
};

#endif // SEGMENT_H
