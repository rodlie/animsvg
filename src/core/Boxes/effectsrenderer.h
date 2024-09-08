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

#ifndef EFFECTSRENDERER_H
#define EFFECTSRENDERER_H
#include "smartPointers/ememory.h"
#include "glhelpers.h"
#include "Tasks/updatable.h"

class RasterEffectCaller;
struct BoxRenderData;

class CORE_EXPORT EffectsRenderer {
public:
    void add(const stdsptr<RasterEffectCaller>& effect) {
        mEffects.append(effect);
    }

    void processGpu(QGL33 * const gl,
                    SwitchableContext &context,
                    BoxRenderData * const boxData);
    void processCpu(BoxRenderData * const boxData);

    bool isEmpty() const { return mCurrentId >= mEffects.count(); }

    void setBaseGlobalRect(SkIRect& currRect,
                           const SkIRect& skMaxBounds) const;

    Friction::Core::HardwareSupport nextHardwareSupport() const;
private:
    int mCurrentId = 0;
    QList<stdsptr<RasterEffectCaller>> mEffects;
};
#endif // EFFECTSRENDERER_H
