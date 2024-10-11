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

#ifndef INTERPOLATIONKEY_H
#define INTERPOLATIONKEY_H
#include "graphkey.h"

class CORE_EXPORT InterpolationKey : public GraphKey {
public:
    InterpolationKey(const int frame,
                     Animator * const parentAnimator);
    InterpolationKey(Animator * const parentAnimator);

    qreal getValueForGraph() const {
        return mRelFrame;
    }

    void setValueForGraph(const qreal value) {
        Q_UNUSED(value)
    }

    void setRelFrame(const int frame) {
        if(frame == this->mRelFrame) return;
        const int dFrame = frame - mRelFrame;
        GraphKey::setRelFrame(frame);
        setC0ValueVar(c0Clamped().getRawYValue() + dFrame);
        setC1ValueVar(c1Clamped().getRawYValue() + dFrame);
    }
};

#endif // INTERPOLATIONKEY_H
