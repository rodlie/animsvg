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

#ifndef SOUNDTMPFILEHANDLERS_H
#define SOUNDTMPFILEHANDLERS_H
#include "tmpdeleter.h"
#include "soundcachecontainer.h"
#include "Tasks/updatable.h"
#include <QTemporaryFile>
#include "skia/skiaincludes.h"
#include "tmpsaver.h"
#include "tmploader.h"

class SoundCacheContainer;
class CORE_EXPORT SoundContainerTmpFileDataLoader : public TmpLoader {
    e_OBJECT
public:
    SoundContainerTmpFileDataLoader(const qsptr<QTemporaryFile> &file,
                                    SoundCacheContainer *target);
    void read(eReadStream& src);
    void afterProcessing();
protected:
    const stdptr<SoundCacheContainer> mTarget;
    stdsptr<Samples> mSamples;
};

class CORE_EXPORT SoundContainerTmpFileDataSaver : public TmpSaver {
    e_OBJECT
public:
    SoundContainerTmpFileDataSaver(const stdsptr<Samples> &samples,
                                   SoundCacheContainer * const target);
    void write(eWriteStream& dst);
private:
    const stdsptr<Samples> mSamples;
};

#endif // SOUNDTMPFILEHANDLERS_H
