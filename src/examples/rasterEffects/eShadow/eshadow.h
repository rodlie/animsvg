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

#ifndef ESHADOW_H
#define ESHADOW_H

#include "eshadow_global.h"
#include "enveCore/skia/skqtconversions.h"
#include "enveCore/Animators/coloranimator.h"
#include "enveCore/Animators/qvector3danimator.h"
#include "enveCore/gpurendertools.h"

class eShadowCaller : public RasterEffectCaller {
public:
    eShadowCaller(const HardwareSupport hwSupport,
                  const qreal radius,
                  const QColor& color,
                  const QPointF& translation,
                  const qreal opacity,
                  const QMargins& margin) :
        RasterEffectCaller(hwSupport, true, margin),
        mRadius(static_cast<float>(radius)),
        mColor(toSkColor(color)),
        mTranslation(toSkPoint(translation)),
        mOpacity(static_cast<float>(opacity)) {}

    void processGpu(QGL33 * const gl,
                    GpuRenderTools& renderTools);
    void processCpu(CpuRenderTools& renderTools,
                    const CpuRenderData &data);
private:
    void setupPaint(SkPaint& paint) const;

    const float mRadius;
    const SkColor mColor;
    const SkPoint mTranslation;
    const SkScalar mOpacity;
};

class eShadow : public CustomRasterEffect {
public:
    eShadow();

    stdsptr<RasterEffectCaller> getEffectCaller(
            const qreal relFrame, const qreal resolution,
            const qreal influence, BoxRenderData* const data) const;
    bool forceMargin() const { return true; }

    CustomIdentifier getIdentifier() const;
private:
    qsptr<QrealAnimator> mBlurRadius;
    qsptr<QrealAnimator> mOpacity;
    qsptr<ColorAnimator> mColor;
    qsptr<QPointFAnimator> mTranslation;
};

#endif // ESHADOW_H
