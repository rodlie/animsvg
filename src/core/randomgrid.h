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

#ifndef RANDOMGRID_H
#define RANDOMGRID_H
#include "Animators/staticcomplexanimator.h"
#include "Animators/intanimator.h"

class CORE_EXPORT RandomGrid : public StaticComplexAnimator {
    e_OBJECT
protected:
    RandomGrid();
public:
    qreal getBaseSeed(const qreal relFrame) const;
    qreal getGridSize(const qreal relFrame) const;

    qreal getRandomValue(const qreal relFrame, const QVector3D pos) const;

    static qreal sGetRandomValue(const qreal baseSeed, const qreal gridSize,
                                 const QVector3D pos);
    static qreal sGetRandomValue(const qreal min, const qreal max,
                                 const qreal baseSeed, const qreal gridSize,
                                 const QVector3D pos);
private:
    static qreal sGetRandomValue(const qreal baseSeed, const QVector3D& gridId);

    qsptr<QrealAnimator> mGridSize;
    qsptr<QrealAnimator> mSeed;
};

#endif // RANDOMGRID_H
