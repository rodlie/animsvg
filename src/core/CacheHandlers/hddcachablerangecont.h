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

#ifndef HddCACHABLERANGECONT_H
#define HddCACHABLERANGECONT_H
#include "hddcachablecont.h"
#include "tmpdeleter.h"
#include "framerange.h"
class eTask;
class HddCachableCacheHandler;

class CORE_EXPORT HddCachableRangeCont : public HddCachableCont {
    friend class SoundContainerTmpFileDataSaver;
protected:
    HddCachableRangeCont(const FrameRange &range,
                         HddCachableCacheHandler * const parent) :
        mRange(range), mParentCacheHandler_k(parent) {}
    virtual int clearMemory() = 0;
public:
    void noDataLeft_k();

    int getRangeMin() const;
    const FrameRange& getRange() const { return mRange; }
    int getRangeMax() const;
    int rangeSpan() const { return mRange.span(); }

    void setUnaryRange(const int unary);
    void setRangeMax(const int max);
    void setRangeMin(const int min);
    void setRange(const FrameRange &range);
    bool inRange(const int unary) const;
private:
    FrameRange mRange;
    HddCachableCacheHandler * const mParentCacheHandler_k;
};

#endif // HddCACHABLERANGECONT_H
