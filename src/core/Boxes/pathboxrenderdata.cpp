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

#include "pathboxrenderdata.h"
#include "pathbox.h"

PathBoxRenderData::PathBoxRenderData(BoundingBox * const parentBox)
    : BoxRenderData(parentBox)
    , mDirectDraw(false)
{

}

void PathBoxRenderData::setupRenderData()
{
    mDirectDraw = (!fForceRasterize && !hasEffects() && fBlendMode == SkBlendMode::kSrcOver);
    if (mDirectDraw) { setupDirectDraw(); }
}

void PathBoxRenderData::setupDirectDraw()
{
    fBaseMargin = QMargins();
    dataSet();
    updateGlobalRect();
    fAntiAlias = true;
    finishedProcessing();
}

void PathBoxRenderData::copyFrom(BoxRenderData *src)
{
    BoxRenderData::copyFrom(src);
    if (const auto pathSrc = enve_cast<PathBoxRenderData*>(src)) {
        mDirectDraw = pathSrc->mDirectDraw;
        if (!mDirectDraw) { return; }
        fPaintSettings = pathSrc->fPaintSettings;
        fFillPath = pathSrc->fFillPath;
        fStrokeSettings = pathSrc->fStrokeSettings;
        fOutlinePath = pathSrc->fOutlinePath;
    }
}

void PathBoxRenderData::drawOnParentLayer(SkCanvas * const canvas,
                                          SkPaint& paint)
{
    if (!mDirectDraw) { return BoxRenderData::drawOnParentLayer(canvas, paint); }
    if (isZero4Dec(fOpacity)) { return; }
    canvas->concat(toSkMatrix(fScaledTransform));
    paint.setBlendMode(fBlendMode);
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);

    if (!fFillPath.isEmpty()) {
        fPaintSettings.applyPainterSettingsSk(paint, fOpacity*0.01f);
        canvas->drawPath(fFillPath, paint);
    }
    if (!fOutlinePath.isEmpty()) {
        paint.setShader(nullptr);
        fStrokeSettings.applyPainterSettingsSk(paint, fOpacity*0.01f);
        canvas->drawPath(fOutlinePath, paint);
    }
}

void PathBoxRenderData::updateRelBoundingRect()
{
    SkPath totalPath;
    totalPath.addPath(fFillPath);
    totalPath.addPath(fOutlinePath);
    fRelBoundingRect = toQRectF(totalPath.computeTightBounds());
}

QVector3D PathBoxRenderData::getCenterPosition()
{
    return toQRectF(fEditPath.getBounds()).center();
}

void PathBoxRenderData::drawSk(SkCanvas * const canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);

    if (!fFillPath.isEmpty()) {
        fPaintSettings.applyPainterSettingsSk(paint);
        canvas->drawPath(fFillPath, paint);
    }
    if (!fOutlinePath.isEmpty()) {
        paint.setShader(nullptr);
        fStrokeSettings.applyPainterSettingsSk(paint);
        canvas->drawPath(fOutlinePath, paint);
    }
}
