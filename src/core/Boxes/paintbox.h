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

#ifndef PAINTBOX_H
#define PAINTBOX_H
#include "boundingbox.h"
#include "Paint/animatedsurface.h"
#include "imagebox.h"
class QPointFAnimator;
class AnimatedPoint;
class SimpleBrushWrapper;

class CORE_EXPORT PaintBox : public BoundingBox
{
    e_OBJECT
    e_DECLARE_TYPE(PaintBox)

protected:
    PaintBox();

public:
    void setupRenderData(const qreal relFrame,
                         const QMatrix& parentM,
                         BoxRenderData * const data,
                         Canvas * const scene);
    stdsptr<BoxRenderData> createRenderData();

    void setupCanvasMenu(PropertyMenu * const menu);

    void saveSVG(SvgExporter& exp, DomEleTask* const task) const;

    AnimatedSurface * getSurface() const
    { return mSurface.get(); }

private:
    qsptr<AnimatedSurface> mSurface;
};

#endif // PAINTBOX_H
