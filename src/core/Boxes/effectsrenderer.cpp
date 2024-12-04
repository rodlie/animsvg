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

#include "effectsrenderer.h"
#include "boxrenderdata.h"
#include "RasterEffects/rastereffectcaller.h"
#include "gpurendertools.h"

void EffectsRenderer::processGpu(QGL33 * const gl,
                                 SwitchableContext &context,
                                 BoxRenderData * const boxData) {
    Q_ASSERT(!isEmpty());

    auto& srcImage = boxData->fRenderedImage;
    const int srcWidth = srcImage->width();
    const int srcHeight = srcImage->height();

    gl->glViewport(0, 0, srcWidth, srcHeight);

    GpuRenderTools renderTools(gl, context, srcImage, boxData->fGlobalRect);
    while(mCurrentId < mEffects.count()) {
        const auto& effect = mEffects.at(mCurrentId);
        if(effect->hardwareSupport() == HardwareSupport::cpuOnly) break;
        effect->processGpu(gl, renderTools);
        mCurrentId++;
    }

    boxData->fRenderedImage = renderTools.getSrcTexture().imageSnapshot(gl);
}

#include "effectsubtaskspawner.h"
void EffectsRenderer::processCpu(BoxRenderData * const boxData) {
    const auto& effect = mEffects.at(mCurrentId++);

    Q_ASSERT(effect->hardwareSupport() != HardwareSupport::gpuOnly);
    EffectSubTaskSpawner::sSpawn(effect, boxData->ref<BoxRenderData>());
}

void EffectsRenderer::setBaseGlobalRect(SkIRect &currRect,
                                        const SkIRect &skMaxBounds) const {
    for(const auto& effect : mEffects) {
        effect->setSrcRect(currRect, skMaxBounds);
        const auto dstRect = effect->getDstRect();
        currRect = SkIRect::MakeLTRB(qMin(dstRect.left(), currRect.left()),
                                     qMin(dstRect.top(), currRect.top()),
                                     qMax(dstRect.right(), currRect.right()),
                                     qMax(dstRect.bottom(), currRect.bottom()));
    }
}

HardwareSupport EffectsRenderer::nextHardwareSupport() const
{
    //Q_ASSERT(!isEmpty());
    if (isEmpty()) { return HardwareSupport::hardwareDefault; }
    return mEffects.at(mCurrentId)->hardwareSupport();
}
