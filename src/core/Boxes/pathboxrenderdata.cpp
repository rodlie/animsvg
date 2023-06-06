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

#include "pathboxrenderdata.h"
#include "pathbox.h"

PathBoxRenderData::PathBoxRenderData(BoundingBox * const parentBox) :
    BoxRenderData(parentBox) {}

void PathBoxRenderData::setupRenderData() {
    const bool isBrush = fStrokeSettings.fPaintType == PaintType::BRUSHPAINT;
    mDirectDraw = !fForceRasterize && !isBrush && !hasEffects() &&
                  fBlendMode == SkBlendMode::kSrcOver;
    if(mDirectDraw) setupDirectDraw();
}

void PathBoxRenderData::setupDirectDraw() {
    fBaseMargin = QMargins();
    dataSet();
    updateGlobalRect();
    fAntiAlias = true;
    finishedProcessing();
}

void PathBoxRenderData::copyFrom(BoxRenderData *src) {
    BoxRenderData::copyFrom(src);
    if(const auto pathSrc = enve_cast<PathBoxRenderData*>(src)) {
        mDirectDraw = pathSrc->mDirectDraw;
        if(!mDirectDraw) return;
        fPaintSettings = pathSrc->fPaintSettings;
        fFillPath = pathSrc->fFillPath;
        fStrokeSettings = pathSrc->fStrokeSettings;
        fOutlinePath = pathSrc->fOutlinePath;
    }
}

void PathBoxRenderData::drawOnParentLayer(SkCanvas * const canvas,
                                          SkPaint& paint) {
    if(!mDirectDraw) return BoxRenderData::drawOnParentLayer(canvas, paint);
    if(isZero4Dec(fOpacity)) return;
    canvas->concat(toSkMatrix(fScaledTransform));
    paint.setBlendMode(fBlendMode);
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);

    if(!fFillPath.isEmpty()) {
        fPaintSettings.applyPainterSettingsSk(paint, fOpacity*0.01f);
        canvas->drawPath(fFillPath, paint);
    }
    if(!fOutlinePath.isEmpty()) {
        paint.setShader(nullptr);
        fStrokeSettings.applyPainterSettingsSk(paint, fOpacity*0.01f);
        canvas->drawPath(fOutlinePath, paint);
    }
}

void PathBoxRenderData::updateRelBoundingRect() {
    SkPath totalPath;
    totalPath.addPath(fFillPath);
    totalPath.addPath(fOutlinePath);
    fRelBoundingRect = toQRectF(totalPath.computeTightBounds());
}

QPointF PathBoxRenderData::getCenterPosition() {
    return toQRectF(fEditPath.getBounds()).center();
}

void PathBoxRenderData::drawSk(SkCanvas * const canvas) {
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);

    if(!fFillPath.isEmpty()) {
        fPaintSettings.applyPainterSettingsSk(paint);
        canvas->drawPath(fFillPath, paint);
    }
    if(!fOutlinePath.isEmpty()) {
        paint.setShader(nullptr);
        /*if(fStrokeSettings.fPaintType == PaintType::BRUSHPAINT) {
            if(!fStrokeSettings.fStrokeBrush) return;
            AutoTiledSurface surf;
            surf.setPixelClamp(fMaxBoundsRect.translated(-fGlobalRect.topLeft()));
            surf.loadPixmap(mBitmap.pixmap());

            SkPath pathT;
            QMatrix trans;
            trans.translate(-fGlobalRect.x(), -fGlobalRect.y());
            trans = fScaledTransform*trans;
            fOutlineBasePath.transform(toSkMatrix(trans), &pathT);

        //                const auto fillBrush = fStrokeSettings.fStrokeBrush->getBrush();
        //                auto fillWidthCurve = fStrokeSettings.fWidthCurve*fResolution;

        //                auto fillBrushSet = BrushStrokeSet::lineFillStrokesForSkPath(
        //                            pathT, fStrokeSettings.fTimeCurve,
        //                            fStrokeSettings.fPressureCurve,
        //                            fillWidthCurve, fStrokeSettings.fSpacingCurve,
        //                            0, 40*fResolution);
        //                for(auto& set : fillBrushSet)
        //                    surf.execute(fillBrush, set);

            //const auto strokeBrush = fStrokeSettings.fStrokeBrush->getBrush();
            //auto strokeWidthCurve = fStrokeSettings.fWidthCurve*fResolution;
            const auto strokeBrushSet = BrushStrokeSet::sOutlineStrokesForSkPath(
                        pathT,
                        fStrokeSettings.fTimeCurve,
                        fStrokeSettings.fPressureCurve,
                        strokeWidthCurve, fStrokeSettings.fSpacingCurve, 5, 5);
            fStrokeSettings.fStrokeBrush->setColor(
                        toSkScalar(fStrokeSettings.fPaintColor.hueF()),
                        toSkScalar(fStrokeSettings.fPaintColor.saturationF()),
                        toSkScalar(fStrokeSettings.fPaintColor.valueF()));
            //const auto brush = fStrokeSettings.fStrokeBrush->getBrush();
            for(const auto& set : strokeBrushSet)
                surf.execute(strokeBrush, set);

            mBitmap.reset();

            QRect baseRect = fGlobalRect;
            baseRect.translate(-surf.zeroTilePos());
            const auto pixRect = surf.pixelBoundingRect();
            baseRect.setSize(QSize(pixRect.width(), pixRect.height()));
            setBaseGlobalRect(baseRect);

            const QMargins iMargins(baseRect.left() - fGlobalRect.left(),
                                    baseRect.top() - fGlobalRect.top(),
                                    fGlobalRect.right() - baseRect.right(),
                                    fGlobalRect.bottom() - baseRect.bottom());
            mBitmap = surf.toBitmap(iMargins);
        } else {*/
            fStrokeSettings.applyPainterSettingsSk(paint);
            canvas->drawPath(fOutlinePath, paint);
        //}
    }
}
