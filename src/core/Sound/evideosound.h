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

#ifndef EVIDEOSOUND_H
#define EVIDEOSOUND_H

#include "esoundobjectbase.h"

class VideoBox;

class CORE_EXPORT eVideoSound : public eSoundObjectBase {
    e_OBJECT
protected:
    eVideoSound(const qsptr<FixedLenAnimationRect>& durRect);
public:
    QMimeData *SWT_createMimeData() { return nullptr; }

    void prp_writeProperty_impl(eWriteStream& dst) const;
    void prp_readProperty_impl(eReadStream& src);

    int prp_getRelFrameShift() const { return 0; }
protected:
    void updateDurationRectLength() {}
};

#endif // EVIDEOSOUND_H
