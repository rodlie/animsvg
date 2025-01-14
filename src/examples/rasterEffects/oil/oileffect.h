#ifndef OILEFFECT_H
#define OILEFFECT_H

#include "rastereffect.h"

#include "Animators/qvector3danimator.h"

class CORE_EXPORT OilEffect : public RasterEffect {
    e_OBJECT
private:
    OilEffect();
public:
    void prp_readProperty_impl(eReadStream &src);

    QMargins getMargin() const;

    stdsptr<RasterEffectCaller> getEffectCaller(
            const qreal relFrame, const qreal resolution,
            const qreal influence, BoxRenderData* const data) const;
private:
    qsptr<QPointFAnimator> mBrushSize;
    qsptr<QrealAnimator> mAccuracy;
    qsptr<QrealAnimator> mStrokeLength;
    qsptr<QrealAnimator> mMaxStrokes;
    qsptr<QrealAnimator> mBristleThickness;
    qsptr<QrealAnimator> mBristleDensity;
};

#endif // OILEFFECT_H
