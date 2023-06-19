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

#ifndef SOUNDMERGER_H
#define SOUNDMERGER_H
#include "Tasks/updatable.h"
#include "CacheHandlers/samples.h"
#include "soundcomposition.h"
#include "Animators/qrealanimator.h"
#include "esoundsettings.h"
extern "C" {
    #include <libavutil/opt.h>
    #include <libswresample/swresample.h>
}
struct CORE_EXPORT SingleSoundData {
    int fSampleShift;
    SampleRange fSSAbsRange;
    QrealSnapshot fVolume;
    qreal fStretch;
    stdsptr<Samples> fSamples;
};

class CORE_EXPORT SoundMerger : public eCpuTask {
    e_OBJECT
protected:
    SoundMerger(const int secondId, const SampleRange& sampleRange,
                SoundComposition* const composition) :
        mSecondId(secondId), mSampleRange(sampleRange),
        mComposition(composition), mSettings(eSoundSettings::sData()) {

    }

    void afterProcessing() {
        if(mComposition)
            mComposition->secondFinished(mSecondId, mSamples);
    }
public:
    void process();

    void addSoundToMerge(const SingleSoundData& data) {
        mSounds << data;
    }
private:
    const int mSecondId;
    const SampleRange mSampleRange;
    const qptr<SoundComposition> mComposition;
    const eSoundSettingsData mSettings;
    stdsptr<Samples> mSamples;
    QList<SingleSoundData> mSounds;
};

#endif // SOUNDMERGER_H
