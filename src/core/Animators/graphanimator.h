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

#ifndef GRAPHANIMATOR_H
#define GRAPHANIMATOR_H
#include "animator.h"
#include <QPainterPath>
#define GetAsGK(key) static_cast<GraphKey*>(key)

class GraphKey;
class QrealPoint;
enum class CtrlsMode : short;
enum class QrealPointType : short;

class CORE_EXPORT GraphAnimator : public Animator {
    e_OBJECT
protected:
    GraphAnimator(const QString& name);
public:
    virtual void graph_getValueConstraints(
            GraphKey *key, const QrealPointType type,
            qreal &minMoveValue, qreal &maxMoveValue) const = 0;
    virtual qValueRange graph_getMinAndMaxValues() const;
    virtual qValueRange graph_getMinAndMaxValuesBetweenFrames(
            const int startFrame, const int endFrame) const;
    virtual qreal graph_clampGraphValue(const qreal value)
    { return value; }
    virtual QPainterPath graph_getPathForSegment(
            const GraphKey * const prevKey,
            const GraphKey * const nextKey) const;

    void prp_afterChangedAbsRange(const FrameRange& range,
                                  const bool clip = true);

    void graph_constrainCtrlsFrameValues();

    void graph_updateKeysPath(const FrameRange& relFrameRange);

    void gAddKeysInRectToList(const QRectF &frameValueRect,
                              QList<GraphKey*> &target);

    void graph_drawKeysPath(QPainter * const p,
                            const QColor &paintColor,
                            const FrameRange &absFrameRange);

    void graph_getFrameValueConstraints(GraphKey *key,
                                  const QrealPointType type,
                                  qreal &minMoveFrame,
                                  qreal &maxMoveFrame,
                                  qreal &minMoveValue,
                                  qreal &maxMoveValue) const;

    QrealPoint *graph_getPointAt(const qreal value,
                                 const qreal frame,
                                 const qreal pixelsPerFrame,
                                 const qreal pixelsPerValUnit);
    void graph_changeSelectedKeysFrameAndValueStart(const QPointF &frameVal);
    void graph_changeSelectedKeysFrameAndValue(const QPointF& frameVal);
    void graph_enableCtrlPtsForSelected();
    void graph_setCtrlsModeForSelectedKeys(const CtrlsMode mode);
    void graph_getSelectedSegments(QList<QList<GraphKey*>> &segments);

    void graph_startSelectedKeysTransform();
    void graph_finishSelectedKeysTransform();
    void graph_cancelSelectedKeysTransform();

    void graph_saveSVG(SvgExporter& exp,
                       QDomElement& parent,
                       const FrameRange& visRange,
                       const QString& attrName,
                       const ValueGetter& valueGetter,
                       const bool transform = false,
                       const QString& type = "",
                       const QString& beginEvent = "",
                       const QString& endEvent = "") const;
protected:
    qreal graph_prevKeyWeight(const GraphKey * const prevKey,
                              const GraphKey * const nextKey,
                              const qreal  frame) const;
    void graph_adjustCtrlsForKeyAdd(GraphKey* const key);
private:
    IdRange graph_relFrameRangeToGraphPathIdRange(
            const FrameRange &relFrameRange) const;

    void graph_getFrameConstraints(
            GraphKey *key, const QrealPointType type,
            qreal &minMoveFrame, qreal &maxMoveFrame) const;

    struct GraphPath {
        operator const QPainterPath&() const { return fPath; }

        void reset() { fPath = QPainterPath(); }
        bool isEmpty() const { return fPath.isEmpty(); }

        QPainterPath fPath;
        FrameRange fRange;
    };

    QList<GraphPath> graph_mKeyPaths;
};

#endif // GRAPHANIMATOR_H
