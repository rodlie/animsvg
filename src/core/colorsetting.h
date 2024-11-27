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

#ifndef COLORSETTING_H
#define COLORSETTING_H
class ColorAnimator;
#include <QColor>
#include "colorhelpers.h"
#include "smartPointers/ememory.h"
enum class ColorSettingType : short {
    start, change, finish, apply
};

enum class ColorParameter : short;

class CORE_EXPORT ColorSetting {
public:
    ColorSetting(const ColorMode settingModeT,
                 const ColorParameter changedValueT,
                 const qreal val1T,
                 const qreal val2T,
                 const qreal val3T,
                 const qreal alphaT,
                 const ColorSettingType typeT);
    void apply(ColorAnimator * const target) const;

    QColor getColor() const {
        QColor color;
        if(fSettingMode == ColorMode::rgb) {
            color.setRgbF(fVal1, fVal2, fVal3, fAlpha);
        } else if(fSettingMode == ColorMode::hsv) {
            color.setHsvF(fVal1, fVal2, fVal3, fAlpha);
        } else if(fSettingMode == ColorMode::hsl) {
            color.setHslF(fVal1, fVal2, fVal3, fAlpha);
        }
        return color;
    }

    ColorSettingType fType;
    ColorMode fSettingMode;
    ColorParameter fChangedValue;

    qreal fVal1;
    qreal fVal2;
    qreal fVal3;
    qreal fAlpha;
private:
    void startColorTransform(ColorAnimator* const target) const;
    void finishColorTransform(ColorAnimator* const target) const;
    void changeColor(ColorAnimator* const target) const;
};

#endif // COLORSETTING_H
