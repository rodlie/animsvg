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

#include "dashpatheffect.h"
#include "Animators/qrealanimator.h"

DashPathEffect::DashPathEffect() :
    PathEffect("dash effect", PathEffectType::DASH) {
    mSize = enve::make_shared<QrealAnimator>("size");
    mSize->setValueRange(0.1, 9999.999);
    mSize->setCurrentBaseValue(5);

    ca_addChild(mSize);

    ca_setGUIProperty(mSize.get());
}

class DashEffectCaller : public PathEffectCaller {
public:
    DashEffectCaller(const qreal width) :
        mWidth(toSkScalar(width)) {}

    void apply(SkPath& path);
private:
    const float mWidth;
};

void DashEffectCaller::apply(SkPath &path) {
    SkPath src;
    path.swap(src);
    path.setFillType(src.getFillType());
    const float intervals[] = { mWidth, mWidth };
    SkStrokeRec rec(SkStrokeRec::kHairline_InitStyle);
    SkRect cullRec = src.getBounds();
    SkDashPathEffect::Make(intervals, 2, 0.f)->filterPath(&path, src, &rec, &cullRec);
}

stdsptr<PathEffectCaller> DashPathEffect::getEffectCaller(
        const qreal relFrame, const qreal influence) const {
    const qreal width = mSize->getEffectiveValue(relFrame)*influence;
    return enve::make_shared<DashEffectCaller>(width);
}
