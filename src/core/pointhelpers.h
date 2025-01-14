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

#ifndef POINTHELPERS_H
#define POINTHELPERS_H
#include <QPointF>
#include <QString>
#include <QRectF>
#include <QPainter>
#include <type_traits>
#include "simplemath.h"
#include "Segments/conicsegment.h"
#include "Segments/qcubicsegment1d.h"
#include "Segments/qcubicsegment2d.h"
#include "Segments/quadsegment.h"
#include "Segments/cubiclist.h"

typedef std::pair<qreal, qreal> qrealPair;

enum class CtrlsMode : short {
    smooth,
    symmetric,
    corner
};

//template <typename T>
//struct Solution {
//    Solution(const bool valid, const T& value = T()) :
//        fValid(valid), fValue(value) {}

//    operator bool() const {
//        return fValid;
//    }

//    operator const T&() const {
//        return fValue;
//    }

//    const bool fValid;
//    const T fValue;
//};

extern qreal gSolveForP1(const qreal p0, const qreal p2,
                         const qreal p3, const qreal t,
                         const qreal value);

extern qreal gSolveForP2(const qreal p0, const qreal p1,
                         const qreal p3, const qreal t,
                         const qreal value);

extern CtrlsMode gGuessCtrlsMode(const QPointF& c0,
                                 const QPointF& p1,
                                 const QPointF& c2,
                                 const bool c0Enabled,
                                 const bool c2Enabled);

extern bool gIsSymmetric(const QPointF& startPos,
                         const QPointF& centerPos,
                         const QPointF& endPos,
                         const qreal threshold = 0.01);

extern bool gIsSmooth(const QPointF& startPos,
                      const QPointF& centerPos,
                      const QPointF& endPos,
                      const qreal threshold = 0.01);

extern void gGetCtrlsSymmetricPos(const QPointF& startPos,
                                  const QPointF& centerPos,
                                  const QPointF& endPos,
                                  QVector3D &newStartPos,
                                  QVector3D &newEndPos);

extern void gGetCtrlsSmoothPos(const QPointF& startPos,
                               const QPointF& centerPos,
                               const QPointF& endPos,
                               QVector3D &newStartPos,
                               QVector3D &newEndPos);

extern qreal gCubicValueAtT(const qCubicSegment1D &seg,
                            const qreal t);
extern QVector3D gCubicValueAtT(const qCubicSegment2D &seg,
                              const qreal t);

//! @brief Only for beziers that do not have multiple points of the same x value,
//! e.g., for GraphAnimators.
extern qCubicSegment1D::Pair gDividedAtX(const qCubicSegment1D &seg,
                                         const qreal x, qreal* t = nullptr);
//! @brief Only for beziers that do not have multiple points of the same x value,
//! e.g., for GraphAnimators.
extern qreal gTFromX(const qCubicSegment1D &seg,
                     const qreal x);


extern QVector3D gGetClosestPointOnLineSegment(const QVector3D &a,
                                             const QVector3D &b,
                                             const QVector3D &p);
extern QVector3D gClosestPointOnRect(const QRectF &rect,
                                   const QVector3D &point,
                                   qreal *dist = nullptr);

extern qCubicSegment2D gBezierLeastSquareV1V2(
        const qCubicSegment2D &seg, const QList<QPointF> &vs,
        const int minVs, const int maxVs);


extern void gDrawCosmeticEllipse(QPainter *p,
                                 const QVector3D &absPos,
                                 const qreal rX, const qreal rY);


extern qreal gCubicGetTFurthestInDirection(const qCubicSegment2D& seg,
                                           const qreal deg);

extern void gSmoothyAbsCtrlsForPtBetween(
        const QVector3D &lastP, const QVector3D &currP,
        const QVector3D &nextP, QVector3D &c1, QVector3D &c2,
        qreal smoothness);
extern void gSmoothyAbsCtrlsForPtBetween(
        const SkPoint3 &lastP, const SkPoint3 &currP,
        const SkPoint3 &nextP, SkPoint3 &c1, SkPoint3 &c2,
        float smoothness);

extern SkPath gPathToPolyline(const SkPath& path);
extern void gForEverySegmentInPath(
        const SkPath& path,
        const std::function<void(const qCubicSegment2D&)>& func);

extern void gForEverySegmentInPath(
        const SkPath& path,
        const std::function<void(const SkPath&)>& func);

extern void gGetSmoothAbsCtrlsForPtBetween(const SkPoint3 &prevP,
                                           const SkPoint3 &currP,
                                           const SkPoint3 &nextP,
                                           SkPoint3 &c1, SkPoint3 &c2,
                                           const float smoothLen);

extern void gAtomicDisplaceFilterPath(const qreal baseSeed,
                                      SkPath* const dst,
                                      const SkPath& src,
                                      const float maxDev);

extern bool gAtomicDisplaceFilterPath(const qreal baseSeed,
                                      SkPath * const dst,
                                      const SkPath& src,
                                      const float maxDev,
                                      const float segLen,
                                      const float smoothness);

extern void gSpatialDisplaceFilterPath(const qreal baseSeed,
                                       const qreal gridSize,
                                       SkPath* const dst,
                                       const SkPath& src,
                                       const float maxDev);

extern bool gSpatialDisplaceFilterPath(const qreal baseSeed,
                                       const qreal gridSize,
                                       SkPath* const dst,
                                       const SkPath& src,
                                       const float maxDev,
                                       const float segLen,
                                       const float smoothness);

qreal gMapTToFragment(const qreal minAbsT,
                      const qreal maxAbsT,
                      const qreal absT);

qreal gMapTFromFragment(const qreal minAbsT,
                        const qreal maxAbsT,
                        const qreal relT);

void gGetValuesForNodeInsertion(
        const QPointF& prevP1, QPointF& prevC2,
        QPointF& newC0, QPointF& newP1, QPointF& newC2,
        QVector3D &nextC0, const QPointF& nextP1,
        qreal t);

void gGetValuesForNodeRemoval(
        const QVector3D &prevP1, QVector3D &prevC2,
        const QVector3D &pC0, const QVector3D &pP1, const QVector3D &pC2,
        QVector3D &nextC0, const QVector3D &nextP1,
        qreal t);

QList<SkPath> gBreakApart(const SkPath& src);

bool gIsClockwise(const QList<qCubicSegment2D>& segs);

#endif // POINTHELPERS_H
