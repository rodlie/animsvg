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

#ifndef LINESPATHEFFECT_H
#define LINESPATHEFFECT_H
#include "PathEffects/patheffect.h"

class CORE_EXPORT LinesPathEffect : public PathEffect {
    e_OBJECT
protected:
    LinesPathEffect();
public:
    stdsptr<PathEffectCaller> getEffectCaller(
            const qreal relFrame, const qreal influence) const;
    bool skipZeroInfluence(const qreal relFrame) const {
        Q_UNUSED(relFrame)
        return false;
    }
private:
    qsptr<QrealAnimator> mAngle;
    qsptr<QrealAnimator> mDistance;
};

#endif // LINESPATHEFFECT_H
