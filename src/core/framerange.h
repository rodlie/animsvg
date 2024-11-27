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

#ifndef FRAMERANGE_H
#define FRAMERANGE_H

#include "core_global.h"
#include <QtCore>

#define TEN_MIL 10000000

struct CORE_EXPORT FrameMarker {
    QString title;
    bool enabled;
    int frame;
};

//! @brief Inclusive range
struct CORE_EXPORT iValueRange {
    static int EMIN;
    static int EMAX;
    static iValueRange EMINMAX;
    static iValueRange INVALID;

    int fMin; //! @brief Inclusive range min
    int fMax; //! @brief Inclusive range max

    bool isUnary() const {
        return fMin == fMax;
    }

    bool isValid() const {
        return fMax >= fMin;
    }

    bool inRange(const qreal val) const {
        return val >= fMin && val <= fMax;
    }

    bool inRange(const int val) const {
        return val >= fMin && val <= fMax;
    }

    bool inRange(const iValueRange& range) const {
        return inRange(range.fMin) && inRange(range.fMax);
    }

    int span() const {
        return qMax(0, fMax - fMin + 1);
    }

    iValueRange adjusted(const int dMin, const int dMax) const {
        return {fMin == EMIN ? EMIN : fMin + dMin,
                fMax == EMAX ? EMAX : fMax + dMax};
    }

    iValueRange shifted(const int by) const {
        return adjusted(by, by);
    }

    bool overlaps(const iValueRange& b) const {
        return this->operator*(b).isValid();
    }

    bool neighbours(const iValueRange& b) const {
        if(b.fMax == fMin - 1) return true;
        if(fMax == b.fMin - 1) return true;
        return false;
    }

    void fixOrder() {
        if(fMin <= fMax) return;
        std::swap(fMin, fMax);
    }

    static std::vector<iValueRange> sSum(const iValueRange& a,
                                         const iValueRange& b) {
        if(!a.isValid()) return { b };
        if(!b.isValid()) return { a };
        if(a.overlaps(b)) return { a + b };
        return { a, b };
    }

    static std::vector<iValueRange> sDiff(const iValueRange& a,
                                          const iValueRange& b) {
        if(!a.isValid() || !b.isValid()) return { a };
        if(a.inRange(b)) return { {a.fMin, b.fMin - 1}, {b.fMax + 1, a.fMax} };
        const auto overlap = a*b;
        if(overlap.isValid()) return sDiff(a, overlap);
        else return { a };
    }

    bool operator<(const iValueRange& b) const {
        if(overlaps(b)) return false;
        return fMin < b.fMin;
    }

    bool operator>(const iValueRange& b) const {
        if(overlaps(b)) return false;
        return fMin > b.fMin;
    }

    bool operator==(const iValueRange& b) const {
        if(!isValid() && !b.isValid()) return true;
        return fMin == b.fMin && fMax == b.fMax;
    }

    bool operator!=(const iValueRange& b) const {
        return !(*this == b);
    }

    iValueRange operator*(const iValueRange& b) const {
        if(!isValid() || !b.isValid()) return INVALID;
        return {qMax(this->fMin, b.fMin), qMin(this->fMax, b.fMax)};
    }

    iValueRange& operator*=(const iValueRange& b) {
        *this = *this * b;
        return *this;
    }

    iValueRange operator+(const iValueRange& b) const {
        if(!isValid()) return b;
        if(!b.isValid()) return *this;
        return {qMin(this->fMin, b.fMin), qMax(this->fMax, b.fMax)};
    }

    iValueRange& operator+=(const iValueRange& b) {
        *this = *this + b;
        return *this;
    }
};

struct CORE_EXPORT qValueRange {
    qreal fMin;
    qreal fMax;

    bool isValid() const {
        return fMax > fMin;
    }

    qreal clamp(const qreal value) const
    { return qBound(fMin, value, fMax); }

    bool inRange(const qreal val) const {
        return val >= fMin && val <= fMax;
    }

    qreal span() const {
        return qMax(0., fMax - fMin);
    }

    qValueRange shifted(const qreal by) const {
        return {fMin + by, fMax + by};
    }

    bool overlaps(const qValueRange& b) const {
        return !this->operator*(b).isValid();
    }

    void fixOrder() {
        if(fMin <= fMax) return;
        auto maxT = fMax;
        fMax = fMin;
        fMin = maxT;
    }

    qValueRange operator/(const qreal b) const {
        return {this->fMin/b, this->fMax/b};
    }

    qValueRange operator*(const qreal b) const {
        return {this->fMin*b, this->fMax*b};
    }

    qValueRange operator*(const qValueRange& b) const {
        return {qMax(this->fMin, b.fMin), qMin(this->fMax, b.fMax)};
    }

    qValueRange& operator*=(const qValueRange& b) {
        this->fMin = qMax(this->fMin, b.fMin);
        this->fMax = qMin(this->fMax, b.fMax);
        return *this;
    }

    qValueRange operator+(const qValueRange& b) const {
        return {qMin(this->fMin, b.fMin), qMax(this->fMax, b.fMax)};
    }

    qValueRange& operator+=(const qValueRange& b) {
        this->fMin = qMin(this->fMin, b.fMin);
        this->fMax = qMax(this->fMax, b.fMax);
        return *this;
    }
};

//! @brief Inclusive frame range
typedef iValueRange FrameRange;
typedef iValueRange AbsFrameRange;
typedef iValueRange RelFrameRange;

typedef iValueRange IdRange;

//! @brief Inclusive sample range
typedef iValueRange SampleRange;
#endif // FRAMERANGE_H
