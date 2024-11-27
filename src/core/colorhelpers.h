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

#ifndef HELPERS_H
#define HELPERS_H
#include "simplemath.h"

enum class ColorMode : short {
    rgb, hsv, hsl
};

enum class ColorParameter : short {
    none,
    red, green, blue,
    hue, hsvSaturation, value,
    hslSaturation, lightness,
    alpha,
    colorMode,
    all
};

CORE_EXPORT
extern bool shouldValPointerBeLightHSV(const float h,
                                       const float s,
                                       const float v);
CORE_EXPORT
extern bool shouldValPointerBeLightHSL(float h, float s, float l);
CORE_EXPORT
extern bool shouldValPointerBeLightRGB(float r, float g, float b);

CORE_EXPORT
extern void rgb_to_hsv_float (float &r_ /*h*/, float &g_ /*s*/, float &b_ /*v*/);
CORE_EXPORT
extern void hsv_to_rgb_float (float &h_, float &s_, float &v_);
CORE_EXPORT
extern void rgb_to_hsl_float (float &r_, float &g_, float &b_);
CORE_EXPORT
extern void hsl_to_rgb_float (float &h_, float &s_, float &l_);
CORE_EXPORT
extern void hsv_to_hsl(float &h, float &s, float &v);
CORE_EXPORT
extern void hsl_to_hsv(float &h, float &s, float &l);

CORE_EXPORT
extern void qrgb_to_hsv(qreal &r_ /*h*/, qreal &g_ /*s*/, qreal &b_ /*v*/);
CORE_EXPORT
extern void qhsv_to_rgb(qreal &h_, qreal &s_, qreal &v_);
CORE_EXPORT
extern void qrgb_to_hsl(qreal &r_, qreal &g_, qreal &b_);
CORE_EXPORT
extern void qhsl_to_rgb(qreal &h_, qreal &s_, qreal &l_);
CORE_EXPORT
extern void qhsv_to_hsl(qreal &h, qreal &s, qreal &v);
CORE_EXPORT
extern void qhsl_to_hsv(qreal &h, qreal &s, qreal &l);

#endif // HELPERS_H
