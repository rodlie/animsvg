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

#ifndef SOUNDCACHECONTAINER_H
#define SOUNDCACHECONTAINER_H
#include "hddcachablerangecont.h"
#include "CacheHandlers/samples.h"

class CORE_EXPORT SoundCacheContainer : public HddCachableRangeCont {
    e_OBJECT
    typedef stdsptr<SoundCacheContainer> stdptrSCC;
protected:
    SoundCacheContainer(const iValueRange &second,
                        HddCachableCacheHandler * const parent);
    SoundCacheContainer(const stdsptr<Samples>& samples,
                        const iValueRange &second,
                        HddCachableCacheHandler * const parent);
public:
    int getByteCount() {
        if(!mSamples) return 0;
        return mSamples->fSampleRange.span()*static_cast<int>(sizeof(float));
    }

    stdsptr<Samples> getSamples() { return mSamples; }

    void setDataLoadedFromTmpFile(const stdsptr<Samples> &samples) {
        replaceSamples(samples);
        afterDataLoadedFromTmpFile();
    }

    void replaceSamples(const stdsptr<Samples> &samples) {
        mSamples = samples;
        afterDataReplaced();
    }
protected:
    stdsptr<eHddTask> createTmpFileDataSaver();
    stdsptr<eHddTask> createTmpFileDataLoader();
    int clearMemory();

    stdsptr<Samples> mSamples;
};

#endif // SOUNDCACHECONTAINER_H
