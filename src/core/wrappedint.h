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

#ifndef WRAPPEDINT_H
#define WRAPPEDINT_H

#include "core_global.h"

class CORE_EXPORT WrappedInt {
public:
    WrappedInt(const int value, const int wrapValue,
               const bool reverse);

    static int sCount(int from, int to,
                      const int wrapValue,
                      const bool reverse);

    int count(const int from, const int to) const;

    void setValue(const int value);
    void setWrapValue(const int wrapVal);

    //! @brief Count [from, to], e.g., count(0, 0) returns 1.
    int toInt() const { return mVal; }
    bool isReverse() const { return mSign == -1; }

    WrappedInt& operator+=(const int val) {
        setValue(mVal + mSign*val);
        return *this;
    }

    WrappedInt& operator-=(const int val) {
        setValue(mVal - mSign*val);
        return *this;
    }

    WrappedInt& operator++() {
        setValue(mVal + mSign);
        return *this;
    }

    WrappedInt& operator--() {
        setValue(mVal - mSign);
        return *this;
    }

    WrappedInt operator++(int) {
        WrappedInt result = *this;
        setValue(mVal + mSign);
        return result;
    }

    WrappedInt operator--(int) {
        WrappedInt result = *this;
        setValue(mVal - mSign);
        return result;
    }

    WrappedInt operator-(const int val) {
        return WrappedInt(mVal - val, mWrapVal, mSign == -1);
    }

    WrappedInt operator+(const int val) {
        return WrappedInt(mVal + val, mWrapVal, mSign == -1);
    }

    bool operator==(const int val) const {
        return mVal == val;
    }

    bool operator!=(const int val) const {
        return mVal != val;
    }
private:
    const int mSign;
    int mVal;
    int mWrapVal;
};

#endif // WRAPPEDINT_H
