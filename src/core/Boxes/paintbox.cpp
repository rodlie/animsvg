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

#include "paintbox.h"
#include "canvas.h"
#include "MovablePoints/animatedpoint.h"
#include "Animators/transformanimator.h"
#include "Private/esettings.h"
#include "Private/document.h"
//#include "Ora/oracreator.h"
//#include "Ora/oraparser.h"
#include "svgexporter.h"

#include "GUI/edialogs.h"
#include "typemenu.h"
#include "filesourcescache.h"

PaintBox::PaintBox() : BoundingBox("Paint Box", eBoxType::paint)
{
    //mSurface = enve::make_shared<AnimatedSurface>();
    //ca_addChild(mSurface);
}

struct PaintBoxRenderData : public ImageRenderData
{
    e_OBJECT

public:
    PaintBoxRenderData(BoundingBox * const parentBoxT) :
        ImageRenderData(parentBoxT) {}

    void loadImageFromHandler()
    {
        //if (fImage) { return; }
        //if (fASurface) { fASurface->getFrameImage(qFloor(fRelFrame), fImage); }
    }

    void updateRelBoundingRect() final
    {
        //Q_ASSERT(fSurface);
        //fRelBoundingRect = fSurface->surface().pixelBoundingRect();
    }

    qptr<AnimatedSurface> fASurface;
    stdsptr<DrawableAutoTiledSurface> fSurface;
};

void PaintBox::setupRenderData(const qreal relFrame, const QMatrix& parentM,
                               BoxRenderData * const data,
                               Canvas* const scene)
{
    /*BoundingBox::setupRenderData(relFrame, parentM, data, scene);
    const auto paintData = static_cast<PaintBoxRenderData*>(data);
    const int imgFrame = qFloor(relFrame);
    const auto imgTask = mSurface->getFrameImage(imgFrame, paintData->fImage);
    if (imgTask) { imgTask->addDependent(data); }

    paintData->fASurface = mSurface.get();
    paintData->fSurface = enve::shared(mSurface->getSurface(imgFrame));*/
}

stdsptr<BoxRenderData> PaintBox::createRenderData()
{
    //return enve::make_shared<PaintBoxRenderData>(this);
    return nullptr;
}

void PaintBox::setupCanvasMenu(PropertyMenu * const menu)
{
    /*if (menu->hasActionsForType<PaintBox>()) { return; }
    menu->addedActionsForType<PaintBox>();

    BoundingBox::setupCanvasMenu(menu);*/
}

void PaintBox::saveSVG(SvgExporter& exp, DomEleTask* const task) const
{
    /*auto& ele = task->initialize("use");
    const auto visRelRange = prp_absRangeToRelRange(task->visRange());
    const auto pTask = mSurface->savePaintSVG(exp, ele, visRelRange);
    if (pTask) { pTask->addDependent(task); }*/
}
