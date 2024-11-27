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

#ifndef ANIMATIONRECT_H
#define ANIMATIONRECT_H
#include "durationrectangle.h"

class CORE_EXPORT AnimationRect : public DurationRectangle {
    e_OBJECT
protected:
    AnimationRect(Property& parentProp) :
        DurationRectangle(parentProp) {}
    virtual void setMinAnimRelFrame(const int min) = 0;
    virtual void setMaxAnimRelFrame(const int max) = 0;
public:
    virtual int getMinAnimRelFrame() const = 0;
    virtual int getMaxAnimRelFrame() const = 0;

    bool hasAnimationFrameRange() { return true; }

    void draw(QPainter * const p,
              const QRect &drawRect,
              const qreal fps,
              const qreal pixelsPerFrame,
              const FrameRange &absFrameRange);

    FrameRange getAnimRelRange() const {
        return { getMinAnimRelFrame(), getMaxAnimRelFrame()};
    }

    int getMinAnimAbsFrame() const;
    int getMaxAnimAbsFrame() const;

    void setAnimationFrameDuration(const int frameDuration);

    int getAnimationFrameDuration();
};

#endif // ANIMATIONRECT_H
