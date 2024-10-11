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

#include "esoundlink.h"

eSoundLink::eSoundLink(eSound * const target) :
    mTarget(target) {
    connect(mTarget, &eBoxOrSound::visibilityChanged,
            this, &eBoxOrSound::setVisible);
    connect(mTarget, &eSound::prp_absFrameRangeChanged,
            this, [this](const FrameRange& range) {
        const auto relRange = mTarget->prp_absRangeToRelRange(range);
        prp_afterChangedRelRange(relRange);
    });
}

FrameRange eSoundLink::prp_relInfluenceRange() const {
    return mTarget->prp_relInfluenceRange();
}

int eSoundLink::prp_getRelFrameShift() const {
    return mTarget->prp_getRelFrameShift();
}

qreal eSoundLink::durationSeconds() const {
    return mTarget->durationSeconds();
}

QrealSnapshot eSoundLink::getVolumeSnap() const {
    return mTarget->getVolumeSnap();
}

stdsptr<Samples> eSoundLink::getSamplesForSecond(const int relSecondId) {
    return mTarget->getSamplesForSecond(relSecondId);
}

SoundReaderForMerger * eSoundLink::getSecondReader(const int relSecondId) {
    return mTarget->getSecondReader(relSecondId);
}

qreal eSoundLink::getStretch() const {
    return mTarget->getStretch();
}
