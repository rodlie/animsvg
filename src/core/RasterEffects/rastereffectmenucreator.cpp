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

#include "rastereffectmenucreator.h"
#include "RasterEffects/rastereffectsinclude.h"
#include "RasterEffects/customrastereffectcreator.h"
#include "ShaderEffects/shadereffectcreator.h"
#include "ShaderEffects/shadereffect.h"

void RasterEffectMenuCreator::forEveryEffect(const EffectAdder& add) {
    add("Blur", "", []() { return enve::make_shared<BlurEffect>(); });
    add("Shadow", "", []() { return enve::make_shared<ShadowEffect>(); });
    add("Motion Blur", "", []() { return enve::make_shared<MotionBlurEffect>(); });
    add("Brightness-Contrast", "Color", []() { return enve::make_shared<BrightnessContrastEffect>(); });
    add("Colorize", "Color", []() { return enve::make_shared<ColorizeEffect>(); });
    add("Wipe", "Transitions", []() { return enve::make_shared<WipeEffect>(); });
    add("Noise Fade", "Transitions", []() { return enve::make_shared<NoiseFadeEffect>(); });

    CustomRasterEffectCreator::sForEveryEffect(add);
    ShaderEffectCreator::sForEveryEffect(add);
}
