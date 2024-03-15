#ifndef BLENDEFFECTBOXSHADOW_H
#define BLENDEFFECTBOXSHADOW_H
#include "Animators/eboxorsound.h"

class BlendEffect;

class CORE_EXPORT BlendEffectBoxShadow : public eBoxOrSound {
    e_OBJECT
    e_DECLARE_TYPE(BlendEffectBoxShadow)
protected:
    BlendEffectBoxShadow(BoundingBox * const box,
                         BlendEffect* const effect);
public:
    bool SWT_shouldBeVisible(
            const SWT_RulesCollection &rules,
            const bool parentSatisfies,
            const bool parentMainTarget) const;


    void prp_setupTreeViewMenu(PropertyMenu * const menu)
    { Q_UNUSED(menu); }

    void prp_drawTimelineControls(
            QPainter * const p, const qreal pixelsPerFrame,
            const FrameRange &absFrameRange, const int rowHeight);

    QMimeData *SWT_createMimeData() { return nullptr; }

    void prp_writeProperty_impl(eWriteStream& dst) const
    { Q_UNUSED(dst) Q_ASSERT(false); }
    void prp_readProperty_impl(eReadStream& src)
    { Q_UNUSED(src) Q_ASSERT(false); }

    qsptr<BlendEffectBoxShadow> createLink() const;
private:
    BoundingBox* const mBox;
    BlendEffect* const mEffect;
};

#endif // BLENDEFFECTBOXSHADOW_H
