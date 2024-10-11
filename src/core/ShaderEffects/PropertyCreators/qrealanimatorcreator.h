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

#ifndef QREALANIMATORCREATOR_H
#define QREALANIMATORCREATOR_H

#include "Animators/qrealanimator.h"
#include "shaderpropertycreator.h"

struct CORE_EXPORT QrealAnimatorCreator : public ShaderPropertyCreator {
    e_OBJECT
    QrealAnimatorCreator(const qreal iniVal,
                         const qreal minVal,
                         const qreal maxVal,
                         const qreal step,
                         const bool glValue,
                         const QString& name,
                         const QString& nameUI) :
        ShaderPropertyCreator(glValue, name, nameUI),
        fIniVal(iniVal), fMinVal(minVal),
        fMaxVal(maxVal), fStep(step) {}

    const qreal fIniVal;
    const qreal fMinVal;
    const qreal fMaxVal;
    const qreal fStep;

    qsptr<Property> create() const {
        return enve::make_shared<QrealAnimator>(
                    fIniVal, fMinVal, fMaxVal, fStep, fNameUI);
    }
};

#endif // QREALANIMATORCREATOR_H
