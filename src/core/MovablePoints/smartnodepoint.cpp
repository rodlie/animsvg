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

#include "smartnodepoint.h"
#include "undoredo.h"
#include <QPainter>
#include <QDebug>
#include "pointhelpers.h"
#include "Animators/SmartPath/smartpathanimator.h"
#include "pathpointshandler.h"
#include "Animators/complexanimator.h"
#include "smartctrlpoint.h"
#include "pointtypemenu.h"
#include "Private/esettings.h"

SmartNodePoint::SmartNodePoint(PathPointsHandler * const handler,
                               SmartPathAnimator * const parentAnimator) :
    NonAnimatedMovablePoint(TYPE_SMART_PATH_POINT),
    mHandler_k(handler), mParentAnimator(parentAnimator) {

    mC0Pt = enve::make_shared<SmartCtrlPoint>(this, SmartCtrlPoint::C0);
    mC2Pt = enve::make_shared<SmartCtrlPoint>(this, SmartCtrlPoint::C2);

    mC0Pt->setOtherCtrlPt(mC2Pt.get());
    mC2Pt->setOtherCtrlPt(mC0Pt.get());
}

void SmartNodePoint::updateRadius() {
    if(isNormal()) setRadius(6.5*eSettings::instance().fPathNodeScaling);
    else setRadius(5.5*eSettings::instance().fPathDissolvedNodeScaling);
}

void SmartNodePoint::updateCtrlsRadius() {
    mC0Pt->updateRadius();
    mC2Pt->updateRadius();
}

void SmartNodePoint::startTransform() {
    NonAnimatedMovablePoint::startTransform();
    if(!mC0Pt->isSelected()) mC0Pt->NonAnimatedMovablePoint::startTransform();
    if(!mC2Pt->isSelected()) mC2Pt->NonAnimatedMovablePoint::startTransform();
    mParentAnimator->prp_startTransform();
}

void SmartNodePoint::saveTransformPivotAbsPos(const QPointF &absPivot) {
    NonAnimatedMovablePoint::saveTransformPivotAbsPos(absPivot);
    if(!mC0Pt->isSelected()) mC0Pt->saveTransformPivotAbsPos(absPivot);
    if(!mC2Pt->isSelected()) mC2Pt->saveTransformPivotAbsPos(absPivot);
}

void SmartNodePoint::rotateRelativeToSavedPivot(const qreal rot) {
    NonAnimatedMovablePoint::rotateRelativeToSavedPivot(rot);
    if(!mC0Pt->isSelected()) mC0Pt->rotateRelativeToSavedPivot(rot);
    if(!mC2Pt->isSelected()) mC2Pt->rotateRelativeToSavedPivot(rot);
}

void SmartNodePoint::scaleRelativeToSavedPivot(const qreal sx,
                                          const qreal sy) {
    NonAnimatedMovablePoint::scaleRelativeToSavedPivot(sx, sy);
    if(!mC0Pt->isSelected()) mC0Pt->scale(sx, sy);
    if(!mC2Pt->isSelected()) mC2Pt->scale(sx, sy);
}

void SmartNodePoint::cancelTransform() {
    NonAnimatedMovablePoint::cancelTransform();
    if(!mC0Pt->isSelected()) mC0Pt->NonAnimatedMovablePoint::cancelTransform();
    if(!mC2Pt->isSelected()) mC2Pt->NonAnimatedMovablePoint::cancelTransform();
    mParentAnimator->prp_cancelTransform();
}

void SmartNodePoint::finishTransform() {
    mParentAnimator->prp_finishTransform();
}

int SmartNodePoint::moveToClosestSegment(const QPointF &absPos) {
    const QPointF relPos = mapAbsoluteToRelative(absPos);
    return mHandler_k->moveToClosestSegment(getNodeId(), relPos);
}

#include <QApplication>
void SmartNodePoint::setRelativePos(const QPointF &relPos) {
    if(getType() == NodeType::normal) {
        NonAnimatedMovablePoint::setRelativePos(relPos);
        currentPath()->actionSetNormalNodeP1(getNodeId(), getRelativePos());
        mNextNormalSegment.afterChanged();
        if(mPrevNormalPoint) mPrevNormalPoint->afterNextNodeC0P1Changed();

        const QPointF change = relPos - getSavedRelPos();
        mC0Pt->moveByRel(change);
        mC2Pt->moveByRel(change);
    } else if(getType() == NodeType::dissolved) {
        const auto parentSeg = mPrevNormalPoint->getNextNormalSegment();
        const auto tRange = currentPath()->dissolvedTRange(getNodeId());
        const auto parentRelSeg = parentSeg.getAsRelSegment();
        auto seg = parentRelSeg.tFragment(tRange.fMin, tRange.fMax);
        const auto closest = seg.closestPosAndT(relPos);
        const qreal mappedT = gMapTFromFragment(tRange.fMin, tRange.fMax,
                                                closest.fT);
        currentPath()->actionSetDissolvedNodeT(getNodeId(), mappedT);
        NonAnimatedMovablePoint::setRelativePos(closest.fPos);
    }
    mParentAnimator->changed();
}

void SmartNodePoint::remove() {
    mHandler_k->scheduleRemoveNode(getNodeId());
}

void SmartNodePoint::canvasContextMenu(PointTypeMenu * const menu) {
    if(menu->hasActionsForType<SmartNodePoint>()) return;
    menu->addedActionsForType<SmartNodePoint>();
    if(isNormal()) {
        if(!isEndPoint()) {
            PointTypeMenu::PlainSelectedOp<SmartNodePoint> op = [](SmartNodePoint * pt) {
                pt->actionDemoteToDissolved(false);
            };
            menu->addPlainAction("Demote to dissolved", op);
            PointTypeMenu::PlainSelectedOp<SmartNodePoint> opApprox = [](SmartNodePoint * pt) {
                pt->actionDemoteToDissolved(true);
            };
            menu->addPlainAction("Demote to dissolved approx.", opApprox);
            menu->addSeparator();
        }
        PointTypeMenu::AllOp<SmartNodePoint> op =
        [](const QList<SmartNodePoint*>& pts) {
            for(const auto pt : pts) {
                pt->remove();
                pt->deselect();
            }
        };
        menu->addPlainAction("Remove", op);
        PointTypeMenu::PlainSelectedOp<SmartNodePoint> opApprox = [](SmartNodePoint * pt) {
            pt->actionRemove(true);
        };
        menu->addPlainAction("Remove approx.", opApprox);
    } else { //if(isDissolved()) {
        PointTypeMenu::PlainSelectedOp<SmartNodePoint> op = [](SmartNodePoint * pt) {
            pt->actionPromoteToNormal();
        };
        menu->addPlainAction("Promote to normal", op);
        PointTypeMenu::PlainSelectedOp<SmartNodePoint> rOp = [](SmartNodePoint * pt) {
            pt->actionRemove(false);
        };
        menu->addPlainAction("Remove", rOp);
    }
}
#include "Private/document.h"
bool SmartNodePoint::isVisible(const CanvasMode mode) const {
    if(mode == CanvasMode::pointTransform) {
        const auto nodeVis = Document::sInstance->fNodeVisibility;
        if(isNormal() && nodeVis == NodeVisiblity::dissolvedOnly) return false;
        if(isDissolved() && nodeVis == NodeVisiblity::normalOnly) return false;
        return true;
    } else if(mode == CanvasMode::pathCreate) {
        return isEndPoint() || isSelected();
    } else if(mode == CanvasMode::drawPath) {
        return isNormal();
    }

    return false;
}

MovablePoint *SmartNodePoint::getPointAtAbsPos(const QPointF &absPos,
                                               const CanvasMode mode,
                                               const qreal invScale) {
    if(mode == CanvasMode::pointTransform) {
        if(mC0Pt->isPointAtAbsPos(absPos, mode, invScale)) {
            return mC0Pt.get();
        } else if(mC2Pt->isPointAtAbsPos(absPos, mode, invScale)) {
            return mC2Pt.get();
        }
    } else if(isEndPoint() && mode == CanvasMode::pathCreate) {
    } else if(isNormal() && mode == CanvasMode::drawPath) {
    } else return nullptr;
    return MovablePoint::getPointAtAbsPos(absPos, mode, invScale);
}

void SmartNodePoint::moveC0ToAbsPos(const QPointF &c0) {
    moveC0ToRelPos(mapAbsoluteToRelative(c0));
}

void SmartNodePoint::moveC2ToAbsPos(const QPointF &c2) {
    moveC2ToRelPos(mapAbsoluteToRelative(c2));
}

void SmartNodePoint::moveC2ToRelPos(const QPointF &c2) {
    if(!getC2Enabled()) setC2Enabled(true);
    mC2Pt->setRelativePos(c2);
}

void SmartNodePoint::moveC0ToRelPos(const QPointF &c0) {
    if(!getC0Enabled()) setC0Enabled(true);
    mC0Pt->setRelativePos(c0);
}

QPointF SmartNodePoint::getC0AbsPos() const {
    return mapRelativeToAbsolute(getC0Value());
}

QPointF SmartNodePoint::getC0Value() const {
    if(getC0Enabled()) return mC0Pt->getRelativePos();
    return getRelativePos();
}

SmartCtrlPoint *SmartNodePoint::getC0Pt() {
    return mC0Pt.get();
}

void SmartNodePoint::updateCtrlPtPos(SmartCtrlPoint * const pointToUpdate) {
    const auto otherPt = pointToUpdate == mC0Pt.get() ? mC2Pt.get() : mC0Pt.get();
    const QPointF relPos = otherPt->getRelativePos();
    QPointF newPos;
    if(getCtrlsMode() == CtrlsMode::symmetric) {
        newPos = symmetricToPos(relPos, getRelativePos());
    } else if(getCtrlsMode() == CtrlsMode::smooth) {
        const qreal distFromCenter =
                pointToLen(pointToUpdate->getRelativePos() -
                           getRelativePos());
        newPos = symmetricToPosNewLen(relPos, getRelativePos(),
                                      distFromCenter);
    } else newPos = relPos;
    pointToUpdate->::NonAnimatedMovablePoint::setRelativePos(newPos);
}

QPointF SmartNodePoint::getC2AbsPos() {
    return mapRelativeToAbsolute(getC2Value());
}

QPointF SmartNodePoint::getC2Value() const {
    if(mNode_d->getC2Enabled()) return mC2Pt->getRelativePos();
    return getRelativePos();
}

SmartCtrlPoint *SmartNodePoint::getC2Pt() {
    return mC2Pt.get();
}

void drawCtrlPtLine(SkCanvas * const canvas,
                    const QPointF& qCtrlAbsPos,
                    const QPointF& qAbsPos,
                    const SkPoint& skAbsPos,
                    const float invScale) {
    if(pointToLen(qCtrlAbsPos - qAbsPos) > 1) {
        const SkPoint skCtrlAbsPos = toSkPoint(qCtrlAbsPos);
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(SK_ColorBLACK);
        paint.setStrokeWidth(1.5f*invScale);
        paint.setStyle(SkPaint::kStroke_Style);
        canvas->drawLine(skAbsPos, skCtrlAbsPos, paint);

        paint.setColor(SK_ColorWHITE);
        paint.setStrokeWidth(0.75f*invScale);
        canvas->drawLine(skAbsPos, skCtrlAbsPos, paint);
    }
}
#include "GUI/global.h"

void SmartNodePoint::drawSk(SkCanvas * const canvas,
                            const CanvasMode mode,
                            const float invScale,
                            const bool keyOnCurrent,
                            const bool ctrlPressed) {
    const QPointF qAbsPos = getAbsolutePos();
    const SkPoint skAbsPos = toSkPoint(qAbsPos);

    const auto& sett = eSettings::instance();

    if(getType() == NodeType::normal) {
        const QColor settColor = isSelected() ? sett.fPathNodeSelectedColor :
                                                sett.fPathNodeColor;
        const SkColor fillCol = toSkColor(settColor);
        drawOnAbsPosSk(canvas, skAbsPos, invScale, fillCol, keyOnCurrent);

        if((mode == CanvasMode::pointTransform && isNextNormalSelected()) ||
           (mode == CanvasMode::pathCreate && isSelected())) {
            SkPaint paint;
            paint.setAntiAlias(true);
            if(mC2Pt->isVisible(mode)) {
                drawCtrlPtLine(canvas, mC2Pt->getAbsolutePos(),
                               qAbsPos, skAbsPos, invScale);
                mC2Pt->drawSk(canvas, mode, invScale, keyOnCurrent, ctrlPressed);
            } else if(mode == CanvasMode::pathCreate) {
                drawCtrlPtLine(canvas, mC2Pt->getAbsolutePos(),
                               qAbsPos, skAbsPos, invScale);
            }
        }
        if((mode == CanvasMode::pointTransform && isPrevNormalSelected()) ||
           (mode == CanvasMode::pathCreate && isSelected())) {
            SkPaint paint;
            paint.setAntiAlias(true);
            if(mC0Pt->isVisible(mode)) {
                drawCtrlPtLine(canvas, mC0Pt->getAbsolutePos(),
                               qAbsPos, skAbsPos, invScale);
                mC0Pt->drawSk(canvas, mode, invScale, keyOnCurrent, ctrlPressed);
            } else if(mode == CanvasMode::pathCreate) {
                drawCtrlPtLine(canvas, mC0Pt->getAbsolutePos(),
                               qAbsPos, skAbsPos, invScale);
            }
        }
    } else if(getType() == NodeType::dissolved) {
        const QColor settColor = isSelected() ? sett.fPathDissolvedNodeSelectedColor :
                                                sett.fPathDissolvedNodeColor;
        const SkColor fillCol = toSkColor(settColor);
        drawOnAbsPosSk(canvas, skAbsPos, invScale, fillCol, keyOnCurrent);
    }

    if(ctrlPressed) {
        SkPaint paint;
        paint.setAntiAlias(true);

        SkFont font;
        font.setSize(eSizesUI::font*invScale);
        const auto fontStyle = SkFontStyle(SkFontStyle::kNormal_Weight,
                                           SkFontStyle::kNormal_Width,
                                           SkFontStyle::kUpright_Slant);
        font.setTypeface(SkTypeface::MakeFromName(nullptr, fontStyle));
        const auto nodeIdStr = QString::number(getNodeId());
        const ulong sizeT = static_cast<ulong>(nodeIdStr.size());
        const auto stdStr = nodeIdStr.toStdString();
        const auto cStr = stdStr.c_str();
        SkRect bounds;
        font.measureText(cStr, sizeT*sizeof(char), SkTextEncoding::kUTF8, &bounds);

        const float x = skAbsPos.x() - bounds.width()*0.5f - 1;
        const float y = skAbsPos.y() + bounds.height()*0.5f;

        paint.setStyle(SkPaint::kFill_Style);
        paint.setColor(SK_ColorWHITE);
        auto drawRect = bounds.makeOffset(x, y);
        drawRect.outset(eSizesUI::font/4, eSizesUI::font/4);
        canvas->drawRect(drawRect, paint);

        paint.setColor(SK_ColorBLACK);
        canvas->drawString(cStr, x, y, font, paint);
    }
}

void SmartNodePoint::setTransform(BasicTransformAnimator * const trans) {
    MovablePoint::setTransform(trans);
    mC0Pt->setTransform(trans);
    mC2Pt->setTransform(trans);
}

SmartNodePoint* SmartNodePoint::getNextPoint() {
    return mNextPoint;
}

SmartNodePoint *SmartNodePoint::getPreviousPoint() {
    return mPrevPoint;
}

SmartNodePoint *SmartNodePoint::getConnectedSeparateNodePoint() {
    if(isSeparateNodePoint() || !mPrevNormalPoint) return this;
    return mPrevNormalPoint->getConnectedSeparateNodePoint();
}

void SmartNodePoint::setC2Enabled(const bool enabled) {
    if(enabled == getC2Enabled()) return;
    if(getC2Enabled()) setCtrlsMode(CtrlsMode::corner);
    currentPath()->actionSetNormalNodeC2Enabled(getNodeId(), enabled);
    mParentAnimator->changed();
}

void SmartNodePoint::setC0Enabled(const bool enabled) {
    if(enabled == getC0Enabled()) return;
    if(mNode_d->getC0Enabled()) setCtrlsMode(CtrlsMode::corner);
    currentPath()->actionSetNormalNodeC0Enabled(getNodeId(), enabled);
    mParentAnimator->changed();
}

void SmartNodePoint::resetC2() {
    mC2Pt->setRelativePos(getRelativePos());
}

void SmartNodePoint::resetC0() {
    mC0Pt->setRelativePos(getRelativePos());
}

int SmartNodePoint::getNodeId() const {
    return mNode_d->getNodeId();
}

NodePointValues SmartNodePoint::getPointValues() const {
    return {getC0Value(), getRelativePos(), getC2Value()};
}

bool SmartNodePoint::isPrevNormalSelected() const {
    const bool prevSelected = mPrevNormalPoint ?
                mPrevNormalPoint->isSelected() : false;
    return isSelected() || prevSelected;
}

bool SmartNodePoint::isNextNormalSelected() const {
    const bool nextSelected = mNextNormalPoint ?
                mNextNormalPoint->isSelected() : false;
    return isSelected() || nextSelected;
}

bool SmartNodePoint::isNeighbourNormalSelected() const {
    const bool nextSelected = mNextNormalPoint ?
                mNextNormalPoint->isSelected() : false;
    const bool prevSelected = mPrevNormalPoint ?
                mPrevNormalPoint->isSelected() : false;
    return isSelected() || nextSelected || prevSelected;
}

SmartPathAnimator *SmartNodePoint::getTargetAnimator() const {
    return mParentAnimator;
}

void SmartNodePoint::setSeparateNodePoint(const bool separateNodePoint) {
    mSeparateNodePoint = separateNodePoint;
}

bool SmartNodePoint::isSeparateNodePoint() {
    return mSeparateNodePoint;
}

void SmartNodePoint::setCtrlsMode(const CtrlsMode mode) {
    if(getCtrlsMode() == mode) return;
    currentPath()->actionSetNormalNodeCtrlsMode(getNodeId(), mode);
    updateFromNodeData();
    mParentAnimator->changed();
    if(mode == CtrlsMode::corner) return;
    mNextNormalSegment.afterChanged();
    if(mPrevNormalPoint) mPrevNormalPoint->afterNextNodeC0P1Changed();
}

bool SmartNodePoint::hasNextNormalPoint() const {
    return mNextNormalPoint;
}

bool SmartNodePoint::hasPrevNormalPoint() const {
    return mPrevNormalPoint;
}

PathPointsHandler *SmartNodePoint::getHandler() {
    return mHandler_k.data();
}

void SmartNodePoint::setPrevNormalPoint(SmartNodePoint * const prevPoint) {
    if(mPrevNormalPoint == this) RuntimeThrow("Node cannot point to itself");
    mPrevNormalPoint = prevPoint;
}

void SmartNodePoint::setNextNormalPoint(SmartNodePoint * const nextPoint) {
    if(mNextNormalPoint == this) RuntimeThrow("Node cannot point to itself");
    mNextNormalPoint = nextPoint;
    if(nextPoint && isNormal()) {
        mNextNormalSegment = NormalSegment(this, nextPoint);
    } else mNextNormalSegment.reset();
}

SmartPath *SmartNodePoint::currentPath() const {
    return mParentAnimator->getCurrentlyEdited();
}

bool SmartNodePoint::hasNextPoint() const {
    return mNextPoint;
}

bool SmartNodePoint::hasPrevPoint() const {
    return mPrevPoint;
}

void SmartNodePoint::setPrevPoint(SmartNodePoint * const prevPoint) {
    if(prevPoint == this) RuntimeThrow("Node cannot point to itself");
    mPrevPoint = prevPoint;
}

void SmartNodePoint::setNextPoint(SmartNodePoint * const nextPoint) {
    if(nextPoint == this) RuntimeThrow("Node cannot point to itself");
    mNextPoint = nextPoint;
}

bool SmartNodePoint::actionConnectToNormalPoint(
        SmartNodePoint * const other) {
    const bool thisHasPrev = hasPrevNormalPoint();
    const bool thisHasNext = hasNextNormalPoint();
    if(thisHasNext && thisHasPrev) return false;
    const bool otherHasPrev = other->hasPrevNormalPoint();
    const bool otherHasNext = other->hasNextNormalPoint();
    if(otherHasPrev && otherHasNext) return false;

    const auto thisAnim = getTargetAnimator();
    const auto otherAnim = other->getTargetAnimator();
    if(!thisAnim || !otherAnim) return false;
    const bool sameAnim = thisAnim == otherAnim;
    if(sameAnim) {
        if(!hasNextNormalPoint()) {
            mHandler_k->createSegment(getNodeId(), other->getNodeId());
        } else if(!hasPrevNormalPoint()) {
            mHandler_k->createSegment(other->getNodeId(), getNodeId());
        } else return false;
    } else {
        const auto thisParentAnim = thisAnim->getParent();
        const auto otherParentAnim = otherAnim->getParent();
        if(thisParentAnim != otherParentAnim) return false;
        if(!thisHasNext && !otherHasPrev) {
            otherAnim->actionPrependMoveAllFrom(thisAnim);
        } else if(!thisHasPrev && !otherHasNext) {
            otherAnim->actionAppendMoveAllFrom(thisAnim);
        } else { // one path has to be reversed to connect
            thisAnim->actionReverseAll();
            if(!thisHasPrev) otherAnim->actionPrependMoveAllFrom(thisAnim);
            else otherAnim->actionAppendMoveAllFrom(thisAnim);
        }
    }
    return true;
}

void SmartNodePoint::actionDisconnectFromNormalPoint(
        SmartNodePoint * const other) {
    if(other == mNextNormalPoint) {
        mHandler_k->removeSegment(mNextNormalSegment);
    } else if(other == mPrevNormalPoint) {
        mPrevNormalPoint->actionDisconnectFromNormalPoint(this);
    }
}

void SmartNodePoint::actionMergeWithNormalPoint(SmartNodePoint * const other) {
    if(other == mNextNormalPoint || other == mPrevNormalPoint) {
        mHandler_k->mergeNodes(getNodeId(), other->getNodeId());
    }
}

void SmartNodePoint::actionPromoteToNormal() {
    return mHandler_k->promoteToNormal(getNodeId());
}

void SmartNodePoint::actionDemoteToDissolved(const bool approx) {
    return mHandler_k->demoteToDissolved(getNodeId(), approx);
}

void SmartNodePoint::actionRemove(const bool approx) {
    return mHandler_k->removeNode(getNodeId(), approx);
}

SmartNodePoint* SmartNodePoint::actionAddPointRelPos(const QPointF &relPos) {
    if(hasNextPoint()) return mHandler_k->addNewAtStart(relPos);
    else return mHandler_k->addNewAtEnd(relPos);
}

SmartNodePoint* SmartNodePoint::actionAddPointAbsPos(const QPointF &absPos) {
    return actionAddPointRelPos(mapAbsoluteToRelative(absPos));
}

void SmartNodePoint::c0Moved(const QPointF &c0) {
    currentPath()->actionSetNormalNodeC0(getNodeId(), c0);
    if(getCtrlsMode() != CtrlsMode::corner) {
        updateCtrlPtPos(mC2Pt.get());
        currentPath()->actionSetNormalNodeC2(getNodeId(),
                                             mC2Pt->getRelativePos());
    }

    if(mPrevNormalPoint) mPrevNormalPoint->afterNextNodeC0P1Changed();
    mParentAnimator->changed();
}

void SmartNodePoint::c2Moved(const QPointF &c2) {
    currentPath()->actionSetNormalNodeC2(getNodeId(), c2);
    if(getCtrlsMode() != CtrlsMode::corner) {
        updateCtrlPtPos(mC0Pt.get());
        currentPath()->actionSetNormalNodeC0(getNodeId(),
                                             mC0Pt->getRelativePos());
    }

    mNextNormalSegment.afterChanged();
    mParentAnimator->changed();
}

void SmartNodePoint::updateFromNodeDataPosOnly() {
    if(!mNode_d) return;
    if(mNode_d->isNormal()) {
        mC0Pt->NonAnimatedMovablePoint::setRelativePos(mNode_d->c0());
        NonAnimatedMovablePoint::setRelativePos(mNode_d->p1());
        mC2Pt->NonAnimatedMovablePoint::setRelativePos(mNode_d->c2());
    } else if(mNode_d->isDissolved()) {
        currentPath()->updateDissolvedNodePosition(getNodeId());
        NonAnimatedMovablePoint::setRelativePos(mNode_d->p1());
    }
}

void SmartNodePoint::updateFromNodeData() {
    if(!mNode_d) {
        clear();
        return;
    }

    updateFromNodeDataPosOnly();

    const int prevNodeId = currentPath()->prevNodeId(mNode_d->getNodeId());
    const auto prevNode = mHandler_k->getPointWithId<SmartNodePoint>(prevNodeId);
    setPrevPoint(prevNode);

    const int nextNodeId = currentPath()->nextNodeId(mNode_d->getNodeId());
    const auto nextNode = mHandler_k->getPointWithId<SmartNodePoint>(nextNodeId);
    setNextPoint(nextNode);

    const auto prevNormalNode = mHandler_k->getPrevNormalNode(getNodeId());
    setPrevNormalPoint(prevNormalNode);

    const auto nextNormalNode = mHandler_k->getNextNormalNode(getNodeId());
    setNextNormalPoint(nextNormalNode);

    const auto type = getType();
    updateRadius();
    setSelectionEnabled(type == NodeType::normal);
}

bool SmartNodePoint::isEndPoint() const {
    return !mPrevNormalPoint || !mNextNormalPoint;
}
