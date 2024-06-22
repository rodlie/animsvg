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

#include "interpolationanimator.h"
#include "graphkey.h"
#include "qrealpoint.h"

InterpolationAnimator::InterpolationAnimator(const QString &name) :
    GraphAnimator(name) {}

qreal InterpolationAnimator::getInterpolatedFrameAtRelFrame(
        const qreal frame) const {
    if(!anim_hasKeys()) return frame;
    const auto pn = anim_getPrevAndNextKeyIdF(frame);
    const int prevId = pn.first;
    const int nextId = pn.second;

    const bool adjKeys = nextId - prevId == 1;
    const auto keyAtRelFrame = adjKeys ? nullptr :
                               anim_getKeyAtIndex(prevId + 1);
    if(keyAtRelFrame) return frame;
    const auto prevKey = anim_getKeyAtIndex<GraphKey>(prevId);
    const auto nextKey = anim_getKeyAtIndex<GraphKey>(nextId);
    if(!prevKey || !nextKey) {
        return frame;
    } else { // if(prevKey && nextKey) {
        const qCubicSegment1D seg{qreal(prevKey->getRelFrame()),
                                  prevKey->getC1Frame(),
                                  nextKey->getC0Frame(),
                                  qreal(nextKey->getRelFrame())};
        const qreal t = gTFromX(seg, frame);
        const qreal p0y = prevKey->getValueForGraph();
        const qreal p1y = prevKey->getC1Value();
        const qreal p2y = nextKey->getC0Value();
        const qreal p3y = nextKey->getValueForGraph();
        return gCubicValueAtT({p0y, p1y, p2y, p3y}, t);
    }
}

void InterpolationAnimator::graph_getValueConstraints(
        GraphKey *key, const QrealPointType type,
        qreal &minValue, qreal &maxValue) const {
    if(type == QrealPointType::keyPt) {
        minValue = key->getRelFrame();
        maxValue = minValue;
        //getFrameConstraints(key, type, minValue, maxValue);
    } else {
        minValue = -DBL_MAX;
        maxValue = DBL_MAX;
    }
}
