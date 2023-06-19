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

#include "eshadow.h"

void eCreateNewestVersion(qsptr<CustomRasterEffect> &result) {
    // Use default, most up to date, version
    result = enve::make_shared<eShadow>();
}

void eCreate(const CustomIdentifier &identifier,
             qsptr<CustomRasterEffect>& result) {
    Q_UNUSED(identifier)
    // Choose version based on identifier
    // if(identifier.fVersion == CustomIdentifier::Version{0, 0, 0})
    result = enve::make_shared<eShadow>();
}

// Returned value must be unique, lets enve distinguish effects
QString effectId() {
    return "iz4784jsvg6fw7bc3clb";
}

#define eSName QStringLiteral("eShadow")

// Name of your effect used in UI
void eName(QString& result) {
    result = eSName;
}

// here specify your effect's most up to date version
CustomIdentifier::Version effectVersion() {
    return { 0, 0, 0 };
}

void eIdentifier(CustomIdentifier &result) {
    result = { effectId(), eSName, effectVersion() };
}

bool eSupports(const CustomIdentifier &identifier) {
    if(identifier.fEffectId != effectId()) return false;
    if(identifier.fEffectName != eSName) return false;
    return identifier.fVersion == effectVersion();
}

#include "enveCore/Animators/qrealanimator.h"
eShadow::eShadow() :
    CustomRasterEffect(eSName.toLower(), HardwareSupport::gpuPreffered, false) {
    mBlurRadius = enve::make_shared<QrealAnimator>("blur radius");
    mOpacity = enve::make_shared<QrealAnimator>("opacity");
    mColor = enve::make_shared<ColorAnimator>();
    mTranslation = enve::make_shared<QPointFAnimator>("translation");

    mBlurRadius->setValueRange(0, 300);
    mBlurRadius->setCurrentBaseValue(10);
    ca_addChild(mBlurRadius);

    mTranslation->setValuesRange(-1000, 1000);
    mTranslation->setBaseValue(QPointF(0, 0));
    ca_addChild(mTranslation);

    mColor->setColor(Qt::black);
    ca_addChild(mColor);

    mOpacity->setValueRange(0, 9);
    mOpacity->setCurrentBaseValue(1);
    ca_addChild(mOpacity);

    ca_setGUIProperty(mColor.data());
}

stdsptr<RasterEffectCaller>
eShadow::getEffectCaller(const qreal relFrame, const qreal resolution,
                         const qreal influence, BoxRenderData * const data) const {
    Q_UNUSED(data)
    const qreal blur = mBlurRadius->getEffectiveValue(relFrame)*resolution;
    const QColor color = mColor->getColor(relFrame);
    const QPointF trans = mTranslation->getEffectiveValue(relFrame)*resolution;
    const qreal opacity = mOpacity->getEffectiveValue(relFrame)*influence;

    const int iL = qMax(0, qCeil(blur - trans.x()));
    const int iT = qMax(0, qCeil(blur - trans.y()));
    const int iR = qMax(0, qCeil(blur + trans.x()));
    const int iB = qMax(0, qCeil(blur + trans.y()));

    return enve::make_shared<eShadowCaller>(instanceHwSupport(), blur,
                                            color, trans, opacity,
                                            QMargins(iL, iT, iR, iB));
}

CustomIdentifier eShadow::getIdentifier() const {
    return { effectId(), eSName, { 0, 0, 0 } };
}

void eShadowCaller::setupPaint(SkPaint &paint) const {
    const float sigma = mRadius*0.3333333f;
    const auto filter = SkDropShadowImageFilter::Make(
                mTranslation.x(), mTranslation.y(),
                sigma, sigma, toSkColor(mColor),
                SkDropShadowImageFilter::kDrawShadowAndForeground_ShadowMode,
                nullptr);
    paint.setImageFilter(filter);
    const float opacityM[20] = {
        1, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, mOpacity, 0};
    paint.setColorFilter(SkColorFilters::Matrix(opacityM));
}

void eShadowCaller::processGpu(QGL33 * const gl,
                               GpuRenderTools &renderTools) {
    Q_UNUSED(gl)

    renderTools.switchToSkia();
    const auto canvas = renderTools.requestTargetCanvas();
    canvas->clear(SK_ColorTRANSPARENT);

    const auto srcTex = renderTools.requestSrcTextureImageWrapper();
    canvas->drawImage(srcTex, 0, 0);

    SkPaint paint;
    setupPaint(paint);
    canvas->drawImage(srcTex, 0, 0, &paint);
    canvas->flush();

    renderTools.swapTextures();
}

void eShadowCaller::processCpu(CpuRenderTools &renderTools,
                               const CpuRenderData &data) {
    Q_UNUSED(data)

    SkBitmap tile;
    renderTools.fDstBtmp.extractSubset(&tile, data.fTexTile);
    SkCanvas canvas(tile);
    canvas.clear(SK_ColorTRANSPARENT);

    const int radCeil = static_cast<int>(ceil(mRadius));
    const auto& srcBtmp = renderTools.fSrcBtmp;
    const auto& texTile = data.fTexTile;
    auto srcRect = texTile.makeOutset(radCeil, radCeil);
    if(srcRect.intersect(srcRect, srcBtmp.bounds())) {
        SkBitmap tileSrc;
        srcBtmp.extractSubset(&tileSrc, srcRect);
        const int drawX = srcRect.left() - texTile.left();
        const int drawY = srcRect.top() - texTile.top();
        canvas.drawBitmap(tileSrc, drawX, drawY);

        SkPaint paint;
        setupPaint(paint);
        canvas.drawBitmap(tileSrc, drawX, drawY, &paint);
    }
}
