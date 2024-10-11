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

#ifndef PAINTSETTINGS_H
#define PAINTSETTINGS_H

#include "Segments/qcubicsegment1d.h"
#include "smartPointers/ememory.h"
#include "skia/skiaincludes.h"

#include <QColor>

enum PaintType : short {
    NOPAINT,
    FLATPAINT,
    GRADIENTPAINT,
    BRUSHPAINT
};

enum class ColorMode : short;
struct CORE_EXPORT ColorSettings {
    QColor fColor;
    ColorMode fMode;
};

class SceneBoundGradient;
enum class GradientType : short;
struct CORE_EXPORT GradientSettings {
    GradientType fType;
    qptr<SceneBoundGradient> fGradient;
};

struct CORE_EXPORT FillSettings {
    PaintType fType;
    ColorSettings fColor;
    GradientSettings fGradient;
};

class SimpleBrushWrapper;
struct CORE_EXPORT BrushSettings {
    SimpleBrushWrapper* fBrush = nullptr;
    qCubicSegment1D fWidth;
    qCubicSegment1D fPressure;
    qCubicSegment1D fSpacing;
    qCubicSegment1D fTime;
};

struct CORE_EXPORT StrokeSettings : public FillSettings {
    qreal fWidth;
    SkPaint::Cap fCapStyle;
    SkPaint::Join fJoinStyle;
    BrushSettings fBrush;
};

#endif // PAINTSETTINGS_H
