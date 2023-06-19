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

#include "displacepatheffect.h"
#include "Animators/intanimator.h"
#include "Properties/boolpropertycontainer.h"
#include "pointhelpers.h"

DisplacePathEffect::DisplacePathEffect() :
    PathEffect("displace effect", PathEffectType::DISPLACE) {
    mSegLength = enve::make_shared<QrealAnimator>("segment length");
    mMaxDev = enve::make_shared<QrealAnimator>("max deviation");
    mSmoothness = QrealAnimator::sCreate0to1Animator("smoothness");
    mLengthBased = enve::make_shared<BoolPropertyContainer>("length inc");
    mSeed = enve::make_shared<QrealAnimator>(qrand() % 9999, 0, 9999, 1, "smooth seed");

    mSegLength->setValueRange(1, 1000);
    mSegLength->setCurrentBaseValue(20);

    mMaxDev->setValueRange(0, 1000);
    mMaxDev->setCurrentBaseValue(20);

    ca_addChild(mSeed);
    ca_addChild(mMaxDev);
    ca_addChild(mLengthBased);

    mLengthBased->ca_addChild(mSegLength);
    mLengthBased->ca_addChild(mSmoothness);
    mLengthBased->setValue(false);
}

class DisplaceEffectCaller : public PathEffectCaller {
public:
    DisplaceEffectCaller(const qreal baseSeed, const qreal maxDev,
                         const qreal segLen, const qreal smooth,
                         const bool lengthBased) :
        mBaseSeed(baseSeed), mMaxDev(toSkScalar(maxDev)),
        mSegLen(toSkScalar(segLen)), mSmooth(toSkScalar(smooth)),
        mLengthBased(lengthBased) {}

    void apply(SkPath& path);
private:
    const qreal mBaseSeed;
    const float mMaxDev;
    const float mSegLen;
    const float mSmooth;
    const bool mLengthBased;
};

void DisplaceEffectCaller::apply(SkPath &path) {
    SkPath src;
    path.swap(src);
    if(mLengthBased) {
        gAtomicDisplaceFilterPath(mBaseSeed, &path, src, mMaxDev,
                                  mSegLen, mSmooth);
    } else {
        gAtomicDisplaceFilterPath(mBaseSeed, &path, src, mMaxDev);
    }
}

stdsptr<PathEffectCaller> DisplacePathEffect::getEffectCaller(
        const qreal relFrame, const qreal influence) const {
    const qreal baseSeed = mSeed->getEffectiveValue(relFrame);
    const qreal maxDev = mMaxDev->getEffectiveValue(relFrame)*influence;
    const qreal segLen = mSegLength->getEffectiveValue(relFrame);
    const qreal smooth = mSmoothness->getEffectiveValue(relFrame);
    const bool lengthBased = mLengthBased->getValue();

    return enve::make_shared<DisplaceEffectCaller>(baseSeed, maxDev, segLen,
                                                   smooth, lengthBased);
}

bool DisplacePathEffect::skipZeroInfluence(const qreal relFrame) const {
    Q_UNUSED(relFrame)
    return !mLengthBased->getValue();
}
