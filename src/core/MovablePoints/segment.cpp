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

#include "segment.h"
#include "MovablePoints/smartnodepoint.h"
#include "MovablePoints/smartctrlpoint.h"
#include "pointhelpers.h"
#include "pathpointshandler.h"

bool NormalSegment::operator==(const NormalSegment &other) const {
    return mFirstNode == other.getFirstNode() &&
           mLastNode == other.getLastNode();
}

NormalSegment::NormalSegment() {}

NormalSegment::NormalSegment(SmartNodePoint * const firstNode,
                             SmartNodePoint * const lastNode) :
    mHandler_k(firstNode->getHandler()) {
    mFirstNode = firstNode;
    if(mFirstNode) mFirstNodeC2 = mFirstNode->getC2Pt();
    mLastNode = lastNode;
    if(mLastNode) mLastNodeC0 = mLastNode->getC0Pt();
    updateDissolved();
}

SmartNodePoint *NormalSegment::divideAtAbsPos(const QPointF &absPos) {
    if(!isValid()) return nullptr;
    const qreal t = closestAbsT(absPos);
    return divideAtT(t);
}

SmartNodePoint *NormalSegment::divideAtT(const qreal &t) {
    if(!isValid()) return nullptr;
    const int firstId = mFirstNode->getNodeId();
    const int lastId = mLastNode->getNodeId();
    const auto result = mHandler_k->divideSegment(firstId, lastId, t);
    reset();
    return result;
}

void NormalSegment::disconnect() const {
    mHandler_k->removeSegment(*this);
}

int NormalSegment::nodesCount() const {
    return mInnerDissolved.count() + (mFirstNode ? 1 : 0) + (mLastNode ? 1 : 0);
}

SmartNodePoint *NormalSegment::getNodeAt(const int id) const {
    if(id < 0 || id >= nodesCount()) return nullptr;
    if(id == 0) return mFirstNode;
    const int innerId = id - 1;
    if(innerId < mInnerDissolved.count()) mInnerDissolved.at(innerId);
    return mLastNode;
}

NormalSegment::SubSegment NormalSegment::getClosestSubSegmentForDummy(
        const QPointF &relPos, qreal &minDist) const {
    auto prevNode = mFirstNode;
    minDist = TEN_MIL;
    SubSegment bestSeg{nullptr, nullptr, nullptr};
    for(const auto &nextNode : mInnerDissolved) {
        const auto subSeg = SubSegment{prevNode, nextNode, this};
        const QPointF halfPos = subSeg.getRelPosAtT(0.5);
        const qreal dist = pointToLen(halfPos - relPos);
        if(dist < minDist) {
            minDist = dist;
            bestSeg = subSeg;
        }
        prevNode = nextNode;
    }
    const auto subSeg = SubSegment{prevNode, mLastNode, this};
    const QPointF halfPos = subSeg.getRelPosAtT(0.5);
    const qreal dist = pointToLen(halfPos - relPos);
    if(dist < minDist) {
        minDist = dist;
        bestSeg = subSeg;
    }
    return bestSeg;
}

void NormalSegment::updateDissolvedPos() const {
    for(const auto& inner : mInnerDissolved)
        inner->updateFromNodeDataPosOnly();
}

QPointF NormalSegment::getRelPosAtT(const qreal t) const {
    return getAsRelSegment().posAtT(t);
}

QPointF NormalSegment::getAbsPosAtT(const qreal t) const {
    return getAsAbsSegment().posAtT(t);
}

void NormalSegment::makePassThroughAbs(const QPointF &absPos, const qreal t) {
    if(!mLastNode->getC0Enabled()) mLastNode->setC0Enabled(true);
    if(!mFirstNode->getC2Enabled()) mFirstNode->setC2Enabled(true);

    auto absSeg = getAsAbsSegment();
    absSeg.makePassThroughRel(absPos, t);

    mFirstNodeC2->moveToAbs(absSeg.c1());
    mLastNodeC0->moveToAbs(absSeg.c2());
}

void NormalSegment::makePassThroughRel(const QPointF &relPos, const qreal t) {
    if(!mLastNode->getC0Enabled()) mLastNode->setC0Enabled(true);
    if(!mFirstNode->getC2Enabled()) mFirstNode->setC2Enabled(true);

    auto relSeg = getAsRelSegment();
    relSeg.makePassThroughRel(relPos, t);

    mFirstNodeC2->moveToRel(relSeg.c1());
    mLastNodeC0->moveToRel(relSeg.c2());
}

void NormalSegment::finishPassThroughTransform() {
    mFirstNodeC2->finishTransform();
    mLastNodeC0->finishTransform();
}

void NormalSegment::startPassThroughTransform() {
    mFirstNodeC2->startTransform();
    mLastNodeC0->startTransform();
}

void NormalSegment::cancelPassThroughTransform() {
    mFirstNodeC2->cancelTransform();
    mLastNodeC0->cancelTransform();
}

void NormalSegment::generateSkPath() {
    mSkPath.reset();
    mSkPath.moveTo(toSkPoint(mFirstNode->getAbsolutePos()));
    mSkPath.cubicTo(toSkPoint(mFirstNode->getC2AbsPos()),
                    toSkPoint(mLastNode->getC0AbsPos()),
                    toSkPoint(mLastNode->getAbsolutePos()));
}

void NormalSegment::drawHoveredSk(SkCanvas * const canvas,
                            const float invScale) {
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SK_ColorBLACK);
    paint.setStrokeWidth(2.5f*invScale);
    paint.setStyle(SkPaint::kStroke_Style);
    canvas->drawPath(mSkPath, paint);

    paint.setColor(SK_ColorRED);
    paint.setStrokeWidth(1.25f*invScale);
    canvas->drawPath(mSkPath, paint);
}

SmartNodePoint *NormalSegment::getFirstNode() const {
    return mFirstNode;
}

SmartNodePoint *NormalSegment::getLastNode() const {
    return mLastNode;
}

qCubicSegment2D NormalSegment::getAsAbsSegment() const {
    Q_ASSERT(mFirstNode && mLastNode);
    return {mFirstNode->getAbsolutePos(),
            mFirstNode->getC2AbsPos(),
            mLastNode->getC0AbsPos(),
            mLastNode->getAbsolutePos()};
}

qCubicSegment2D NormalSegment::getAsRelSegment() const {
    Q_ASSERT(mFirstNode && mLastNode);
    return {mFirstNode->getRelativePos(),
            mFirstNode->getC2Value(),
            mLastNode->getC0Value(),
            mLastNode->getRelativePos()};
}

void NormalSegment::updateDissolved() {
    mInnerDissolved.clear();
    if(!mFirstNode || !mLastNode) return;
    int currId = mHandler_k->getNextNodeId(mFirstNode->getNodeId());
    auto currNode = mHandler_k->getPointWithId<SmartNodePoint>(currId);
    while(currNode && currNode != mLastNode && currNode != mFirstNode) {
        mInnerDissolved << currNode;
        currId = mHandler_k->getNextNodeId(currId);
        currNode = mHandler_k->getPointWithId<SmartNodePoint>(currId);
    }
    updateDissolvedPos();
}

NormalSegment::SubSegment NormalSegment::subSegmentAtT(const qreal t) const {
    SmartNodePoint* firstNode = mFirstNode;
    SmartNodePoint* lastNode = mLastNode;
    qreal firstNodeT = 0;
    qreal lastNodeT = 1;
    for(const auto& inner : mInnerDissolved) {
        const qreal innerT = inner->getT();
        if(innerT < t && innerT > firstNodeT) {
            firstNodeT = innerT;
            firstNode = inner;
        }
        if(innerT > t && innerT < lastNodeT) {
            lastNodeT = innerT;
            lastNode = inner;
        }
    }
    return {firstNode, lastNode, this};
}

QPointF NormalSegment::getSlopeVector(const qreal t) {
    const QPointF posAtT = getRelPosAtT(t);
    const QPointF posAtTPlus = getRelPosAtT(t + 0.01);
    return scalePointToNewLen(posAtTPlus - posAtT, 1);
}

bool NormalSegment::isValid() const {
    return mFirstNode && mFirstNodeC2 &&
           mLastNodeC0 && mLastNode && mHandler_k;
}

bool NormalSegment::isNormal() const {
    if(!isValid()) return false;
    return mFirstNode->getType() == NodeType::normal &&
            mLastNode->getType() == NodeType::normal;
}

void NormalSegment::clear() {
    mFirstNode = nullptr;
    mFirstNodeC2 = nullptr;
    mLastNodeC0 = nullptr;
    mLastNode = nullptr;
    mInnerDissolved.clear();
}

qreal NormalSegment::SubSegment::getMinT() const {
    if(fFirstPt == fParentSeg->getFirstNode()) return 0;
    return fFirstPt->getT();
}

qreal NormalSegment::SubSegment::getMaxT() const {
    if(fLastPt == fParentSeg->getLastNode()) return 1;
    return fLastPt->getT();
}

qreal NormalSegment::SubSegment::getParentTAtThisT(const qreal thisT) const {
    return gMapTFromFragment(getMinT(), getMaxT(), thisT);
}

QPointF NormalSegment::SubSegment::getRelPosAtT(const qreal thisT) const {
    const qreal parentT = getParentTAtThisT(thisT);
    return fParentSeg->getRelPosAtT(parentT);
}
