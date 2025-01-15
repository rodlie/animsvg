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

#include "randomgrid.h"

RandomGrid::RandomGrid() : StaticComplexAnimator("randomness grid") {
    mSeed = enve::make_shared<QrealAnimator>(QRandomGenerator::global()->generate() % 9999, 0, 9999, 1, "smooth seed");
    mGridSize = enve::make_shared<QrealAnimator>(100, 1, 9999, 1, "grid size");

    ca_addChild(mSeed);
    ca_addChild(mGridSize);
}

qreal RandomGrid::getBaseSeed(const qreal relFrame) const {
    return mSeed->getEffectiveValue(relFrame);
}

qreal RandomGrid::getGridSize(const qreal relFrame) const {
    return mGridSize->getEffectiveValue(relFrame);
}

qreal RandomGrid::getRandomValue(const qreal relFrame, const QVector3D &pos) const {
    return sGetRandomValue(getBaseSeed(relFrame), getGridSize(relFrame), pos);
}

qreal RandomGrid::sGetRandomValue(const qreal baseSeed, const qreal gridSize,
                                  const QVector3D &pos) {
    const QVector3D gridIdF = pos/gridSize;
    if(isInteger4Dec(gridIdF.x()) && isInteger4Dec(gridIdF.y())) {
        const QVector3D gridId = QVector3D{qRound(gridIdF.x()), qRound(gridIdF.y()), qRound(gridIdF.z())};
        return sGetRandomValue(baseSeed, gridId);
    }

    const QVector3D gridId1{qCeil(gridIdF.x()), qCeil(gridIdF.y())};
    const QVector3D gridId2{qFloor(gridIdF.x()), qCeil(gridIdF.y())};
    const QVector3D gridId3{qCeil(gridIdF.x()), qFloor(gridIdF.y())};
    const QVector3D gridId4{qFloor(gridIdF.x()), qFloor(gridIdF.y()), qFloor(gridIdF.z())};

    const qreal gridId1Dist = pointToLen(gridId1 * gridSize - pos);
    const qreal gridId2Dist = pointToLen(gridId2 * gridSize - pos);
    const qreal gridId3Dist = pointToLen(gridId3 * gridSize - pos);
    const qreal gridId4Dist = pointToLen(gridId4 * gridSize - pos);

    const qreal grid1W = 1 - clamp(gridId1Dist/gridSize, 0, 1);
    const qreal grid2W = 1 - clamp(gridId2Dist/gridSize, 0, 1);
    const qreal grid3W = 1 - clamp(gridId3Dist/gridSize, 0, 1);
    const qreal grid4W = 1 - clamp(gridId4Dist/gridSize, 0, 1);

    const qreal wSum = grid1W + grid2W + grid3W + grid4W;

    const qreal grid1V = sGetRandomValue(baseSeed, gridId1);
    const qreal grid2V = sGetRandomValue(baseSeed, gridId2);
    const qreal grid3V = sGetRandomValue(baseSeed, gridId3);
    const qreal grid4V = sGetRandomValue(baseSeed, gridId4);

    return (grid1V*grid1W + grid2V*grid2W + grid3V*grid3W + grid4V*grid4W)/wSum;
}

qreal RandomGrid::sGetRandomValue(const qreal min, const qreal max,
                                  const qreal baseSeed, const qreal gridSize,
                                  const QVector3D &pos) {
    return sGetRandomValue(baseSeed, gridSize, pos)*(max - min) + min;
}

qreal RandomGrid::sGetRandomValue(const qreal baseSeed, const QVector3D &gridId) {
    const int seedInc = gridId.x()*100 + gridId.y()*1000;
    if(isInteger4Dec(baseSeed)) {
        QRandomGenerator rand(static_cast<quint32>(qRound(baseSeed) + seedInc));
        return rand.generateDouble();
    } else {
        QRandomGenerator fRand(static_cast<quint32>(qFloor(baseSeed) + seedInc));
        QRandomGenerator cRand(static_cast<quint32>(qCeil(baseSeed) + seedInc));
        const qreal cRandWeight = baseSeed - qFloor(baseSeed);
        return fRand.generateDouble()*(1 - cRandWeight) +
                cRand.generateDouble()*cRandWeight;
    }
}
