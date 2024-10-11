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

#include "colorsetting.h"
#include "Animators/coloranimator.h"

ColorSetting::ColorSetting(const ColorMode settingModeT,
                           const ColorParameter changedValueT,
                           const qreal val1T,
                           const qreal val2T,
                           const qreal val3T,
                           const qreal alphaT,
                           const ColorSettingType typeT) {
    fType = typeT;
    fSettingMode = settingModeT;
    fChangedValue = changedValueT;
    fVal1 = val1T;
    fVal2 = val2T;
    fVal3 = val3T;
    fAlpha = alphaT;
}

void ColorSetting::apply(ColorAnimator * const target) const {
    if(fChangedValue == ColorParameter::colorMode) {
        target->setColorMode(fSettingMode);
        return;
    }
    if(fType == ColorSettingType::start ||
       fType == ColorSettingType::apply) {
        startColorTransform(target);
    }
    if(fType == ColorSettingType::change ||
       fType == ColorSettingType::apply) {
        changeColor(target);
    }
    if(fType == ColorSettingType::finish ||
       fType == ColorSettingType::apply) {
        finishColorTransform(target);
    }
}

void ColorSetting::finishColorTransform(ColorAnimator* const target) const {
    changeColor(target);
    target->prp_finishTransform();
}

void ColorSetting::changeColor(ColorAnimator* const target) const {
    const ColorMode targetMode = target->getColorMode();
    if(targetMode == fSettingMode) {
        target->setCurrentVal1Value(fVal1);
        target->setCurrentVal2Value(fVal2);
        target->setCurrentVal3Value(fVal3);
    } else {
        qreal val1, val2, val3;
        const auto col = getColor();
        if(targetMode == ColorMode::rgb) {
            val1 = col.redF();
            val2 = col.greenF();
            val3 = col.blueF();
        } else if(targetMode == ColorMode::hsv) {
            val1 = col.hueF();
            val2 = col.hsvSaturationF();
            val3 = col.valueF();
        } else { //if(targetMode == ColorMode::hsl) {
            val1 = col.hueF();
            val2 = col.hslSaturationF();
            val3 = col.lightnessF();
        }
        target->setCurrentVal1Value(val1);
        target->setCurrentVal2Value(val2);
        target->setCurrentVal3Value(val3);
    }
    target->setCurrentAlphaValue(fAlpha);
}

void ColorSetting::startColorTransform(ColorAnimator* const target) const {
    const ColorMode targetMode = target->getColorMode();
    if(targetMode == fSettingMode && fChangedValue != ColorParameter::all) {
        const qreal targetVal1 = target->getVal1Animator()->getCurrentBaseValue();
        const qreal targetVal2 = target->getVal2Animator()->getCurrentBaseValue();
        const qreal targetVal3 = target->getVal3Animator()->getCurrentBaseValue();

        if(fChangedValue == ColorParameter::red || fChangedValue == ColorParameter::hue) {
            target->startVal1Transform();

            if(!isZero4Dec(targetVal2 - fVal2)) target->startVal2Transform();
            if(!isZero4Dec(targetVal3 - fVal3)) target->startVal3Transform();
        } else if(fChangedValue == ColorParameter::green ||
                  fChangedValue == ColorParameter::hsvSaturation ||
                  fChangedValue == ColorParameter::hslSaturation) {
            target->startVal2Transform();

            if(!isZero4Dec(targetVal1 - fVal1)) target->startVal1Transform();
            if(!isZero4Dec(targetVal3 - fVal3)) target->startVal3Transform();
        } else if(fChangedValue == ColorParameter::blue ||
                  fChangedValue == ColorParameter::value ||
                  fChangedValue == ColorParameter::lightness) {
            target->startVal3Transform();

            if(!isZero4Dec(targetVal1 - fVal1)) target->startVal1Transform();
            if(!isZero4Dec(targetVal2 - fVal2)) target->startVal2Transform();
        }
    } else {
        target->startVal1Transform();
        target->startVal2Transform();
        target->startVal3Transform();
    }
    qreal targetAlpha = target->getAlphaAnimator()->getCurrentBaseValue();
    if(!isZero4Dec(targetAlpha - fAlpha) ||
       fChangedValue == ColorParameter::all || fChangedValue == ColorParameter::alpha) {
        target->startAlphaTransform();
    }
    changeColor(target);
}
