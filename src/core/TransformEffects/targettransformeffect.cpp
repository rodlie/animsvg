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

#include "targettransformeffect.h"

#include "Boxes/boundingbox.h"
#include "Animators/transformanimator.h"
#include "Animators/qrealanimator.h"
#include "Animators/qvector3danimator.h"

TargetTransformEffect::TargetTransformEffect(
        const QString& name,
        const TransformEffectType type) :
    TransformEffect(name, type) {
    mTarget = enve::make_shared<BoxTargetProperty>("target");

    connect(mTarget.get(), &BoxTargetProperty::targetSet,
            this, [this](BoundingBox* const newTarget) {
        auto& conn = mTargetConn.assign(newTarget);
        if(newTarget) {
            const auto parent = getFirstAncestor<BoundingBox>();
            const auto parentTransform = parent->getTransformAnimator();
            const auto targetTransform = newTarget->getTransformAnimator();
            conn << connect(targetTransform, &Property::prp_absFrameRangeChanged,
                            this, [parentTransform](const FrameRange& range,
                            const bool clip) {
                parentTransform->prp_afterChangedAbsRange(range, clip);
            });
            conn << connect(targetTransform,
                            &AdvancedTransformAnimator::totalTransformChanged,
                            this, [parentTransform, targetTransform](const UpdateReason reason) {
                const int absFrame = targetTransform->anim_getCurrentAbsFrame();
                parentTransform->anim_setAbsFrame(absFrame);
                parentTransform->prp_afterChangedCurrent(reason);
            });
            conn << connect(parentTransform,
                            &AdvancedTransformAnimator::inheritedTransformChanged,
                            this, [parentTransform](const UpdateReason reason) {
                parentTransform->prp_afterChangedCurrent(reason);
            });
        }
    });

    connect(mTarget.get(), &BoxTargetProperty::setActionStarted,
            this, [this]() {
        const auto parent = getFirstAncestor<BoundingBox>();
        if(!parent) return;
        mPosBeforeTargetChange = parent->getPivotAbsPos();
    });

    connect(mTarget.get(), &BoxTargetProperty::setActionFinished,
            this, [this](BoundingBox* const oldTarget,
                         BoundingBox* const newTarget) {
        const auto parent = getFirstAncestor<BoundingBox>();
        if(!parent) return;
        const auto posAfter = parent->getPivotAbsPos();

        parent->startPosTransform();
        parent->moveByAbs(mPosBeforeTargetChange - posAfter);

        setRotScaleAfterTargetChange(oldTarget, newTarget);

        parent->finishTransform();
    });

    ca_addChild(mTarget);
}

FrameRange TargetTransformEffect::prp_getIdenticalRelRange(
        const int relFrame) const {
    const auto thisIdent = ComplexAnimator::prp_getIdenticalRelRange(relFrame);
    const auto target = mTarget->getTarget();
    if(!target) return thisIdent;
    const auto targetTransform = target->getTransformAnimator();
    const int absFrame = prp_relFrameToAbsFrame(relFrame);
    const int tRelFrame = targetTransform->prp_absFrameToRelFrame(absFrame);
    const auto parentIdent = targetTransform->prp_getIdenticalRelRange(tRelFrame);
    const auto absParentIdent = targetTransform->prp_relRangeToAbsRange(parentIdent);
    return thisIdent*prp_absRangeToRelRange(absParentIdent);
}

void TargetTransformEffect::setRotScaleAfterTargetChange(
        BoundingBox* const oldTarget, BoundingBox* const newTarget) {
    Q_UNUSED(oldTarget)
    Q_UNUSED(newTarget)
}

BoxTargetProperty* TargetTransformEffect::targetProperty() const {
    return mTarget.get();
}
