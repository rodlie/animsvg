#ifndef BLUREFFECT_H
#define BLUREFFECT_H

#include "rastereffect.h"

class CORE_EXPORT BlurEffect : public RasterEffect {
    e_OBJECT
protected:
    BlurEffect();
public:
    stdsptr<RasterEffectCaller> getEffectCaller(
            const qreal relFrame, const qreal resolution,
            const qreal influence, BoxRenderData* const data) const;
    QMargins getMargin() const;
    bool forceMargin() const { return true; }

    QDomElement saveBlurSVG(SvgExporter& exp,
                            const FrameRange& visRange,
                            const QDomElement& child) const;
private:
    qsptr<QrealAnimator> mRadius;
};

#endif // BLUREFFECT_H
