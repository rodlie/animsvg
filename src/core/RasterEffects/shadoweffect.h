#ifndef SHADOWEFFECT_H
#define SHADOWEFFECT_H

#include "rastereffect.h"

#include "skia/skqtconversions.h"
#include "Animators/coloranimator.h"
#include "Animators/qpointfanimator.h"
#include "gpurendertools.h"

class CORE_EXPORT ShadowEffect : public RasterEffect {
public:
    ShadowEffect();

    stdsptr<RasterEffectCaller> getEffectCaller(
            const qreal relFrame, const qreal resolution,
            const qreal influence, BoxRenderData* const data) const;
    bool forceMargin() const { return true; }

    QDomElement saveShadowSVG(SvgExporter& exp,
                              const FrameRange& visRange,
                              const QDomElement& child) const;
private:
    qsptr<QrealAnimator> mBlurRadius;
    qsptr<QrealAnimator> mOpacity;
    qsptr<ColorAnimator> mColor;
    qsptr<QPointFAnimator> mTranslation;
};

#endif // SHADOWEFFECT_H
