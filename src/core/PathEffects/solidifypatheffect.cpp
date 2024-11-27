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

#include "solidifypatheffect.h"
#include "pathoperations.h"
#include "Boxes/pathbox.h"

SolidifyPathEffect::SolidifyPathEffect() :
    PathEffect("solidify effect", PathEffectType::SOLIDIFY) {
    mDisplacement = enve::make_shared<QrealAnimator>("displacement");
    mDisplacement->setValueRange(-999.999, 999.999);
    mDisplacement->setCurrentBaseValue(5);

    ca_addChild(mDisplacement);

    ca_setGUIProperty(mDisplacement.get());
}

class SolidifyEffectCaller : public PathEffectCaller {
public:
    SolidifyEffectCaller(const qreal displ) :
        mDisplacement(displ) {}

    void apply(SkPath& path);
private:
    const qreal mDisplacement;
};

void SolidifyEffectCaller::apply(SkPath &path) {
    SkPath src;
    path.swap(src);
    gSolidify(mDisplacement, src, &path);
}

stdsptr<PathEffectCaller> SolidifyPathEffect::getEffectCaller(
        const qreal relFrame, const qreal influence) const {
    const qreal displ = mDisplacement->getEffectiveValue(relFrame)*influence;
    return enve::make_shared<SolidifyEffectCaller>(displ);
}
