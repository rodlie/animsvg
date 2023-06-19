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

#include "animationrect.h"
#include "Properties/property.h"
#include "Private/esettings.h"

int AnimationRect::getMaxAnimAbsFrame() const {
    return mParentProperty.prp_relFrameToAbsFrame(getMaxAnimRelFrame());
}

int AnimationRect::getMinAnimAbsFrame() const {
    return mParentProperty.prp_relFrameToAbsFrame(getMinAnimRelFrame());
}

void AnimationRect::setAnimationFrameDuration(const int frameDuration) {
    setMaxAnimRelFrame(getMinAnimRelFrame() + frameDuration - 1);
}

int AnimationRect::getAnimationFrameDuration() {
    return getMaxAnimRelFrame() - getMinAnimRelFrame() + 1;
}

void AnimationRect::draw(QPainter * const p,
                         const QRect& drawRect,
                         const qreal fps,
                         const qreal pixelsPerFrame,
                         const FrameRange &absFrameRange) {
    const int clampedMin = qMax(absFrameRange.fMin, getMinAnimAbsFrame());
    const int firstRelDrawFrame = clampedMin - absFrameRange.fMin;
    const int clampedMax = qMin(absFrameRange.fMax, getMaxAnimAbsFrame());
    const int lastRelDrawFrame = clampedMax - absFrameRange.fMin;
    const int drawFrameSpan = lastRelDrawFrame - firstRelDrawFrame + 1;
    if(drawFrameSpan > 0) {
        QRect animDurRect(qFloor(firstRelDrawFrame*pixelsPerFrame), drawRect.y(),
                          qCeil(drawFrameSpan*pixelsPerFrame), drawRect.height());

        const auto& sett = eSettings::instance();
        p->fillRect(animDurRect.adjusted(0, 1, 0, -1), sett.fAnimationRangeColor);
    }
    DurationRectangle::draw(p, drawRect, fps, pixelsPerFrame, absFrameRange);
}
