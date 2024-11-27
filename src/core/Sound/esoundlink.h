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

#ifndef ESOUNDLINK_H
#define ESOUNDLINK_H
#include "esound.h"

class CORE_EXPORT eSoundLink : public eSound {
public:
    eSoundLink(eSound* const target);

    bool isLink() const final { return true; }

    qsptr<eSound> createLink() {
        return enve::make_shared<eSoundLink>(mTarget);
    }

    FrameRange prp_relInfluenceRange() const;
    int prp_getRelFrameShift() const;

    qreal durationSeconds() const;
    QrealSnapshot getVolumeSnap() const;
    stdsptr<Samples> getSamplesForSecond(const int relSecondId);
    SoundReaderForMerger * getSecondReader(const int relSecondId);
    qreal getStretch() const;
private:
    eSound* const mTarget;
};

#endif // ESOUNDLINK_H
