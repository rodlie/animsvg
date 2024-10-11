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

#ifndef MOTIONBLUREFFECT_H
#define MOTIONBLUREFFECT_H

#include "rastereffect.h"

class BoundingBox;

class CORE_EXPORT MotionBlurEffect : public RasterEffect {
    friend class SelfRef;
    MotionBlurEffect();
public:
    FrameRange prp_getIdenticalRelRange(const int relFrame) const;

    stdsptr<RasterEffectCaller> getEffectCaller(
            const qreal relFrame, const qreal resolution,
            const qreal influence, BoxRenderData* const data) const;
private:
    FrameRange getMotionBlurPropsIdenticalRange(const int relFrame) const;

    mutable bool mBlocked = false;
    qptr<BoundingBox> mParentBox;
    qsptr<QrealAnimator> mOpacity;
    qsptr<QrealAnimator> mNumberSamples;
    qsptr<QrealAnimator> mFrameStep;
};

#endif // MOTIONBLUREFFECT_H
