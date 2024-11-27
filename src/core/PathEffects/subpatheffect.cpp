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

#include "subpatheffect.h"

#include "pointhelpers.h"
#include "Animators/qrealanimator.h"
#include "Properties/boolproperty.h"

SubPathEffect::SubPathEffect() :
    PathEffect("sub-path effect", PathEffectType::SUB) {
    mPathWise = enve::make_shared<BoolProperty>("path-wise");

    mMin = enve::make_shared<QrealAnimator>("min length");
    mMin->setValueRange(-999, 999);
    mMin->setCurrentBaseValue(0);

    mMax = enve::make_shared<QrealAnimator>("max length");
    mMax->setValueRange(-999, 999);
    mMax->setCurrentBaseValue(100);

    mOffset = enve::make_shared<QrealAnimator>("offset");
    mOffset->setValueRange(-999, 999);
    mOffset->setCurrentBaseValue(0);

    ca_addChild(mPathWise);
    ca_addChild(mMin);
    ca_addChild(mMax);
    ca_addChild(mOffset);
}

class SubPathEffectCaller : public PathEffectCaller {
public:
    SubPathEffectCaller(const bool pathWise,
                        const qreal minFrac, const qreal maxFrac) :
        mPathWise(pathWise), mMinFrac(minFrac), mMaxFrac(maxFrac) {}

    void apply(SkPath& path);
private:
    const bool mPathWise;
    const qreal mMinFrac;
    const qreal mMaxFrac;
};

void SubPathEffectCaller::apply(SkPath &path) {
    if(isZero6Dec(mMaxFrac - 1) && isZero6Dec(mMinFrac)) return;

    if(isZero6Dec(mMaxFrac - mMinFrac)) {
        path.reset();
        return;
    }

    auto paths = CubicList::sMakeFromSkPath(path);
    const auto srcFillType = path.getFillType();
    path.reset();
    path.setFillType(srcFillType);

    if(mPathWise) {
        for(auto& iPath : paths) {
            path.addPath(iPath.getFragmentUnbound(mMinFrac, mMaxFrac).toSkPath());
        }
        return;
    } // else

    qreal totalLength = 0;
    for(auto& iPath : paths) {
        totalLength += iPath.getTotalLength();
    }
    const qreal minLength = mMinFrac*totalLength;
    const qreal maxLength = mMaxFrac*totalLength;

    qreal currLen = qFloor(minLength/totalLength)*totalLength;
    bool first = true;
    while(currLen < maxLength) {
        for(auto& iPath : paths) {
            const qreal pathLen = iPath.getTotalLength();

            const qreal minRemLen = minLength - currLen;
            const qreal maxRemLen = maxLength - currLen;
            currLen += pathLen;

            if(first) {
                if(currLen > minLength) {
                    first = false;
                    qreal maxFrag;
                    const bool last = currLen > maxLength;
                    if(last) maxFrag = maxRemLen/pathLen;
                    else maxFrag = 1;
                    path.addPath(iPath.getFragment(minRemLen/pathLen, maxFrag).toSkPath());
                    if(last) break;
                }
            } else {
                if(currLen > maxLength) {
                    path.addPath(iPath.getFragment(0, maxRemLen/pathLen).toSkPath());
                    break;
                } else {
                    path.addPath(iPath.toSkPath());
                }
            }
        }
    }
}

stdsptr<PathEffectCaller> SubPathEffect::getEffectCaller(
        const qreal relFrame, const qreal influence) const {
    const bool pathWise = mPathWise->getValue();
    const qreal offset = mOffset->getEffectiveValue(relFrame);
    const qreal minFrac = (mMin->getEffectiveValue(relFrame) + offset) * 0.01 * influence;
    const qreal maxFrac = (mMax->getEffectiveValue(relFrame) + offset) * 0.01 * influence + 1 - influence;

    return enve::make_shared<SubPathEffectCaller>(pathWise, minFrac, maxFrac);
}
