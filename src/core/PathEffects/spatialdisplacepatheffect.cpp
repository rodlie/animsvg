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

#include "spatialdisplacepatheffect.h"
#include "Animators/intanimator.h"
#include "randomgrid.h"
#include "Properties/boolpropertycontainer.h"
#include "pointhelpers.h"

SpatialDisplacePathEffect::SpatialDisplacePathEffect() :
    PathEffect("spatial displace effect",
               PathEffectType::SPATIAL_DISPLACE) {
    mSegLength = enve::make_shared<QrealAnimator>("segment length");
    mMaxDev = enve::make_shared<QrealAnimator>("max deviation");
    mSmoothness = QrealAnimator::sCreate0to1Animator("smoothness");
    mLengthBased = enve::make_shared<BoolPropertyContainer>("length inc");
    mSeed = enve::make_shared<RandomGrid>();

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

class SpatialDisplaceEffectCaller : public PathEffectCaller {
public:
    SpatialDisplaceEffectCaller(const qreal baseSeed, const qreal gridSize,
                                const qreal maxDev, const qreal segLen,
                                const qreal smooth, const bool lengthBased) :
        mBaseSeed(baseSeed), mGridSize(gridSize),
        mMaxDev(toSkScalar(maxDev)), mSegLen(toSkScalar(segLen)),
        mSmooth(toSkScalar(smooth)), mLengthBased(lengthBased) {}

    void apply(SkPath& path);
private:
    const qreal mBaseSeed;
    const qreal mGridSize;
    const float mMaxDev;
    const float mSegLen;
    const float mSmooth;
    const bool mLengthBased;
};

void SpatialDisplaceEffectCaller::apply(SkPath &path) {
    SkPath src;
    path.swap(src);
    if(mLengthBased) {
        gSpatialDisplaceFilterPath(mBaseSeed, mGridSize,
                                   &path, src, mMaxDev, mSegLen, mSmooth);
    } else {
        gSpatialDisplaceFilterPath(mBaseSeed, mGridSize,
                                   &path, src, mMaxDev);
    }
}

stdsptr<PathEffectCaller> SpatialDisplacePathEffect::getEffectCaller(
        const qreal relFrame, const qreal influence) const {
    const qreal baseSeed = mSeed->getBaseSeed(relFrame);
    const qreal gridSize = mSeed->getGridSize(relFrame);
    const qreal maxDev = mMaxDev->getEffectiveValue(relFrame)*influence;
    const qreal segLen = mSegLength->getEffectiveValue(relFrame);
    const qreal smooth = mSmoothness->getEffectiveValue(relFrame);
    const bool lengthBased = mLengthBased->getValue();

    return enve::make_shared<SpatialDisplaceEffectCaller>(
                baseSeed, gridSize, maxDev, segLen, smooth, lengthBased);
}

bool SpatialDisplacePathEffect::skipZeroInfluence(const qreal relFrame) const {
    Q_UNUSED(relFrame)
    return !mLengthBased->getValue();
}
