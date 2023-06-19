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

#include "hddcachablerangecont.h"
#include "hddcachablecachehandler.h"

void HddCachableRangeCont::noDataLeft_k() {
    if(!mParentCacheHandler_k) return;
    const auto thisRef = ref<HddCachableRangeCont>();
    mParentCacheHandler_k->remove(thisRef);
}

int HddCachableRangeCont::getRangeMin() const {
    return mRange.fMin;
}

int HddCachableRangeCont::getRangeMax() const {
    return mRange.fMax;
}

bool HddCachableRangeCont::inRange(const int unary) const {
    return mRange.inRange(unary);
}

void HddCachableRangeCont::setUnaryRange(const int unary) {
    mRange.fMin = unary;
    mRange.fMax = unary;
}

void HddCachableRangeCont::setRangeMax(const int max) {
    mRange.fMax = max;
}

void HddCachableRangeCont::setRangeMin(const int min) {
    mRange.fMin = min;
}

void HddCachableRangeCont::setRange(const FrameRange &range) {
    mRange = range;
}
