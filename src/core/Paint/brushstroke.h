// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef BRUSHSTROKE_H
#define BRUSHSTROKE_H

#include <QRect>

#include "Segments/qcubicsegment1d.h"
#include "Segments/qcubicsegment2d.h"
//#include "libmypaintincludes.h"
#include "pointhelpers.h"
#include "pathoperations.h"

#define DEFAULT_TILT_CURVE qCubicSegment1D{0, 0, 0, 0}

struct CORE_EXPORT BrushStroke {
    friend struct BrushStrokeSet;
    qCubicSegment2D fStrokePath;
    qCubicSegment1D fPressure;
    qCubicSegment1D fXTilt;
    qCubicSegment1D fYTilt;
    qCubicSegment1D fTimeCurve;
    qCubicSegment1D fWidthCurve;
    qCubicSegment1D fSpacingCurve;
    bool fUseColor = false;
    QColor fColor = Qt::black;
private:
    QRect execute(MyPaintBrush * const brush,
                  MyPaintSurface * const surface,
                  const bool press,
                  double dLen) const;

    QRect executeMove(MyPaintBrush * const brush,
                      MyPaintSurface * const surface,
                      const double t,
                      const double lenFrag) const;

    QRect executePress(MyPaintBrush * const brush,
                       MyPaintSurface * const surface) const;
};

#endif // BRUSHSTROKE_H
