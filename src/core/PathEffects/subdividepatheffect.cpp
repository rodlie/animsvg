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

#include "subdividepatheffect.h"
#include "Animators/intanimator.h"

SubdividePathEffect::SubdividePathEffect() :
    PathEffect("subdivide effect", PathEffectType::SUBDIVIDE) {
    mCount = enve::make_shared<IntAnimator>(1, 0, 9, 1, "count");
    ca_addChild(mCount);
}

class SubdivideEffectCaller : public PathEffectCaller {
public:
    SubdivideEffectCaller(const int count) : mCount(count) {}

    void apply(SkPath& path);
private:
    const int mCount;
};

void SubdivideEffectCaller::apply(SkPath &path) {
    SkPath src;
    path.swap(src);
    path.setFillType(src.getFillType());
    auto lists = CubicList::sMakeFromSkPath(src);
    for(auto & list : lists) {
        list.subdivide(mCount);
        path.addPath(list.toSkPath());
    }
}

stdsptr<PathEffectCaller> SubdividePathEffect::getEffectCaller(
        const qreal relFrame, const qreal influence) const {
    Q_UNUSED(influence)
    const int count = mCount->getEffectiveIntValue(relFrame);
    return enve::make_shared<SubdivideEffectCaller>(count);
}
