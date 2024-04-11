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

#ifndef INTANIMATORCREATOR_H
#define INTANIMATORCREATOR_H

#include "Animators/intanimator.h"
#include "shaderpropertycreator.h"

struct CORE_EXPORT IntAnimatorCreator : public ShaderPropertyCreator {
    e_OBJECT
    IntAnimatorCreator(const int iniVal,
                       const int minVal,
                       const int maxVal,
                       const int step,
                       const bool glValue,
                       const QString& name,
                       const QString& nameUI) :
        ShaderPropertyCreator(glValue, name, nameUI),
        fIniVal(iniVal), fMinVal(minVal),
        fMaxVal(maxVal), fStep(step) {}

    const int fIniVal;
    const int fMinVal;
    const int fMaxVal;
    const int fStep;

    qsptr<Property> create() const {
        return enve::make_shared<IntAnimator>(
                    fIniVal, fMinVal, fMaxVal, fStep, fNameUI);
    }
};
#endif // INTANIMATORCREATOR_H
