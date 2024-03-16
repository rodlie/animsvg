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

#include "wrappedint.h"

WrappedInt::WrappedInt(const int value, const int wrapValue,
                       const bool reverse) :
    mSign(reverse ? -1 : 1), mWrapVal(wrapValue) {
    setValue(value);
}

void WrappedInt::setValue(const int value) {
    if(value < 0) {
        mVal = mWrapVal + value % mWrapVal;
    } else {
        mVal = value % mWrapVal;
    }
}

void WrappedInt::setWrapValue(const int wrapVal) {
    mWrapVal = wrapVal;
    setValue(mVal);
}

int WrappedInt::sCount(int from, int to,
                       const int wrapValue,
                       const bool reverse) {
    from = WrappedInt(from, wrapValue, reverse).toInt();
    to = WrappedInt(to, wrapValue, reverse).toInt();
    int count = 0;
    for(WrappedInt i(from, wrapValue, reverse);; i++) {
        count++;
        if(i.toInt() == to) break;
    }
    return count;
}

int WrappedInt::count(const int from, const int to) const {
    return sCount(from, to, mWrapVal, isReverse());
}
