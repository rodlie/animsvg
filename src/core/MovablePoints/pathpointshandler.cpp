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

#include "pathpointshandler.h"
#include "Animators/SmartPath/smartpathanimator.h"
#include "Animators/SmartPath/smartpathcollection.h"

PathPointsHandler::PathPointsHandler(
        SmartPathAnimator * const targetAnimator) :
    scheduleNodesRemoval([this]() { flushNodesRemoval(); }),
    mTargetAnimator(targetAnimator) {}

NormalSegment PathPointsHandler::getNormalSegment(const QPointF &absPos, const qreal invScale) const {
    qreal minDist = 5*invScale;
    NormalSegment bestSeg;
    for(int i = 0; i < count(); i++) {
        const auto point = getPointWithId<SmartNodePoint>(i);
        const auto nSeg = point->getNextNormalSegment();
        if(!nSeg.isValid()) continue;
        auto absSeg = nSeg.getAsAbsSegment();
        const qreal dist = absSeg.minDistanceTo(absPos);
        if(dist < minDist) {
            minDist = dist;
            bestSeg = nSeg;
        }
    }
    return bestSeg;
}

void PathPointsHandler::scheduleRemoveNode(const int nodeId) {
    mRemoveNodes << nodeId;
    scheduleNodesRemoval();
}

SmartNodePoint *PathPointsHandler::createNewNodePoint(const int nodeId) {
    const auto newPt = enve::make_shared<SmartNodePoint>(this, mTargetAnimator);
    insertPt(nodeId, newPt);
    return newPt.get();
}

SmartNodePoint *PathPointsHandler::createAndAssignNewNodePoint(const int nodeId) {
    const auto newPt = createNewNodePoint(nodeId);
    updatePoint(newPt, nodeId);
    return newPt;
}

SmartPath *PathPointsHandler::targetPath() const {
    return mTargetAnimator->getCurrentlyEdited();
}

void PathPointsHandler::updatePoint(const int nodeId) {
    const auto pt = getPointWithId<SmartNodePoint>(nodeId);
    updatePoint(pt, nodeId);
}

void PathPointsHandler::updatePoint(SmartNodePoint * const pt, const int nodeId) {
    pt->setNode(targetPath()->getNodePtr(nodeId));
}

void PathPointsHandler::updateAllPoints() {
    const int newCount = targetPath()->getNodeCount();
    while(newCount < count()) removeLast();
    for(int i = 0; i < count(); i++) getPointWithId<SmartNodePoint>(i)->clear();
    while(count() < newCount) createNewNodePoint(count());
    for(int i = 0; i < count(); i++) updatePoint(i);
}

void PathPointsHandler::updateAllPointsRadius() {
    for(int i = 0; i < count(); i++) {
        const auto node = getPointWithId<SmartNodePoint>(i);
        node->updateRadius();
        node->updateCtrlsRadius();
    }
}

void PathPointsHandler::flushNodesRemoval() {
    auto nodes = mRemoveNodes;
    mRemoveNodes.clear();
    std::sort(nodes.begin(), nodes.end());
    for(auto it = nodes.rbegin(); it != nodes.rend(); it++) {
        removeNode(*it, false);
    }
}

void PathPointsHandler::updatePoints(int min, int max) {
    const int lastId = count() - 1;
    min = clamp(min, 0, lastId);
    max = clamp(max, 0, lastId);
    for(int i = min; i <= max; i++) updatePoint(i);
}

void PathPointsHandler::setCtrlsMode(const int nodeId,
                                     const CtrlsMode mode) {
    mTargetAnimator->actionSetNormalNodeCtrlsMode(nodeId, mode);
    updatePoint(nodeId);
}

void PathPointsHandler::removeNode(const int nodeId, const bool approx) {
    mTargetAnimator->actionRemoveNode(nodeId, approx);
}

SmartNodePoint* PathPointsHandler::addNewAtStart(const QPointF &relPos) {
    const int id = mTargetAnimator->actionAddNewAtStart(relPos);
    return getPointWithId<SmartNodePoint>(id);
}

SmartNodePoint* PathPointsHandler::addNewAtEnd(const QPointF &relPos) {
    const int id = mTargetAnimator->actionAddNewAtEnd(relPos);
    return getPointWithId<SmartNodePoint>(id);
}

void PathPointsHandler::promoteToNormal(const int nodeId) {
    mTargetAnimator->actionPromoteToNormal(nodeId);

    const int prevNormalId = targetPath()->prevNormalId(nodeId);
    const int nextNormalId = targetPath()->nextNormalId(nodeId);
    updatePoints(qMin(prevNormalId, nextNormalId),
                 qMax(prevNormalId, nextNormalId));
}

void PathPointsHandler::demoteToDissolved(const int nodeId,
                                          const bool approx) {
    mTargetAnimator->actionDemoteToDissolved(nodeId, approx);

    const int prevNormalId = targetPath()->prevNormalId(nodeId);
    const int nextNormalId = targetPath()->nextNormalId(nodeId);
    updatePoints(qMin(prevNormalId, nextNormalId),
                 qMax(prevNormalId, nextNormalId));
}

int PathPointsHandler::moveToClosestSegment(const int nodeId,
                                            const QPointF &relPos) {
    NormalSegment::SubSegment minSubSeg{nullptr, nullptr, nullptr};
    qreal minDist = TEN_MIL;
    for(int i = 0; i < count(); i++) {
        const auto pt = getPointWithId<SmartNodePoint>(i);
        const auto seg = pt->getNextNormalSegment();
        if(!seg.isValid()) continue;
        qreal dist;
        const auto subSeg = seg.getClosestSubSegmentForDummy(relPos, dist);
        if(dist < minDist) {
            minDist = dist;
            minSubSeg = subSeg;
        }
    }
    if(!minSubSeg.isValid()) return nodeId;
    const auto prevPt = minSubSeg.fFirstPt;
    const int prevNodeId = prevPt->getNodeId();
    if(prevNodeId == nodeId) return nodeId;
    const auto nextPt = minSubSeg.fLastPt;
    const int nextNodeId = nextPt->getNodeId();
    if(nextNodeId == nodeId) return nodeId;

    mTargetAnimator->actionMoveNodeBetween(nodeId, prevNodeId, nextNodeId);
    return (nodeId < prevNodeId ? prevNodeId : nextNodeId);
}

void PathPointsHandler::mergeNodes(const int nodeId1, const int nodeId2) {
    mTargetAnimator->actionMergeNodes(nodeId1, nodeId2);
}

SmartNodePoint* PathPointsHandler::divideSegment(const int node1Id,
                                                 const int node2Id,
                                                 const qreal t) {
    const int id = mTargetAnimator->actionInsertNodeBetween(node1Id, node2Id, t);
    return getPointWithId<SmartNodePoint>(id);
}

void PathPointsHandler::createSegment(const int node1Id, const int node2Id) {
    mTargetAnimator->actionConnectNodes(node1Id, node2Id);
}

void PathPointsHandler::removeSegment(const NormalSegment &segment) {
    if(!segment.isValid()) return;
    const auto node1 = segment.getNodeAt(0);
    const auto node2 = segment.getNodeAt(1);
    if(!node1 || !node2) return;
    const int node1Id = node1->getNodeId();
    const int node2Id = node2->getNodeId();
    mTargetAnimator->actionDisconnectNodes(node1Id, node2Id);
    if(mTargetAnimator->hasDetached()) {
        const auto spColl = mTargetAnimator->getParent<SmartPathCollection>();
        if(!spColl) return;
        const auto newAnim = mTargetAnimator->createFromDetached();
        spColl->addChild(newAnim);
    }
}

int PathPointsHandler::getPrevNodeId(const int startId) const
{ return targetPath()->prevNodeId(startId); }

int PathPointsHandler::getNextNodeId(const int startId) const
{ return targetPath()->nextNodeId(startId); }

SmartNodePoint *PathPointsHandler::getPrevNode(const int startId) const
{ return getPointWithId<SmartNodePoint>(getPrevNodeId(startId)); }

SmartNodePoint *PathPointsHandler::getNextNode(const int startId) const
{ return getPointWithId<SmartNodePoint>(getNextNodeId(startId)); }

SmartNodePoint *PathPointsHandler::getPrevNormalNode(const int startId) const {
    const int normalId = targetPath()->prevNormalId(startId);
    return getPointWithId<SmartNodePoint>(normalId);
}

SmartNodePoint *PathPointsHandler::getNextNormalNode(const int startId) const {
    const int normalId = targetPath()->nextNormalId(startId);
    return getPointWithId<SmartNodePoint>(normalId);
}

SmartNodePoint *PathPointsHandler::getClosestNode(
        const QPointF &absPos, const qreal &maxDist) const {
    SmartNodePoint* result = nullptr;
    qreal minDist = maxDist;
    for(int i = 0; i < count(); i++) {
        const auto sNode = getPointWithId<SmartNodePoint>(i);
        const qreal dist = pointToLen(sNode->getAbsolutePos() - absPos);
        if(dist > minDist) continue;
        minDist = dist;
        result = sNode;
    }
    return result;
}
