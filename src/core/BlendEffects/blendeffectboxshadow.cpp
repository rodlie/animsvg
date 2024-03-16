#include "blendeffectboxshadow.h"

#include "Boxes/boundingbox.h"

BlendEffectBoxShadow::BlendEffectBoxShadow(BoundingBox* const box,
                                           BlendEffect * const effect) :
    eBoxOrSound(box->prp_getName()), mBox(box), mEffect(effect) {
    connect(box, &Property::prp_nameChanged,
            this, &Property::prp_setName);
    setVisible(effect->isVisible());
    connect(effect, &eEffect::effectVisibilityChanged,
            this, &eBoxOrSound::setVisible);
    connect(this, &eBoxOrSound::visibilityChanged,
            effect, &eEffect::setVisible);
}

bool BlendEffectBoxShadow::SWT_shouldBeVisible(
        const SWT_RulesCollection &rules,
        const bool parentSatisfies,
        const bool parentMainTarget) const {
    const SWT_BoxRule rule = rules.fRule;
    if(rule == SWT_BoxRule::selected) return false;
    return mBox->SWT_shouldBeVisible(
                rules, parentSatisfies, parentMainTarget);
}

void BlendEffectBoxShadow::prp_drawTimelineControls(
        QPainter * const p,
        const qreal pixelsPerFrame,
        const FrameRange &absFrameRange,
        const int rowHeight) {
    return mBox->drawDurationRectangle(p, pixelsPerFrame,
                                       absFrameRange, rowHeight);
}

qsptr<BlendEffectBoxShadow> BlendEffectBoxShadow::createLink() const {
    return enve::make_shared<BlendEffectBoxShadow>(mBox, mEffect);
}
