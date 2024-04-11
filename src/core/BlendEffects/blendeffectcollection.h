#ifndef BLENDEFFECTCOLLECTION_H
#define BLENDEFFECTCOLLECTION_H

#include "Animators/dynamiccomplexanimator.h"

#include "blendeffect.h"

CORE_EXPORT
qsptr<BlendEffect> readIdCreateBlendEffect(eReadStream& src);
CORE_EXPORT
void writeBlendEffectType(BlendEffect* const obj, eWriteStream& dst);

CORE_EXPORT
qsptr<BlendEffect> readIdCreateBlendEffectXEV(const QDomElement& ele);
CORE_EXPORT
void writeBlendEffectTypeXEV(BlendEffect* const obj, QDomElement& ele);

typedef DynamicComplexAnimator<
    BlendEffect,
    writeBlendEffectType,
    readIdCreateBlendEffect,
    writeBlendEffectTypeXEV,
    readIdCreateBlendEffectXEV> BlendEffectCollectionBase;

class CORE_EXPORT BlendEffectCollection : public BlendEffectCollectionBase {
    Q_OBJECT
    e_OBJECT
protected:
    BlendEffectCollection();
public:
    void prp_setupTreeViewMenu(PropertyMenu * const menu);

    void prp_writeProperty_impl(eWriteStream &dst) const;
    void prp_readProperty_impl(eReadStream &src);

    void blendSetup(ChildRenderData &data,
                    const int index, const qreal relFrame,
                    QList<ChildRenderData> &delayed) const;
    void detachedBlendUISetup(int& drawId,
                        QList<BlendEffect::UIDelayed> &delayed) const;
    void detachedBlendSetup(const BoundingBox * const boxToDraw,
                        SkCanvas * const canvas,
                        const SkFilterQuality filter, int& drawId,
                        QList<BlendEffect::Delayed> &delayed) const;
    void drawBlendSetup(SkCanvas * const canvas);
};

#endif // BLENDEFFECTCOLLECTION_H
