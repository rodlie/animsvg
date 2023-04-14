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

#include "onionskin.h"


void OnionSkin::draw(SkCanvas * const canvas) {
    fPrev.draw(canvas);
    fNext.draw(canvas);
}

void OnionSkin::clear() {
    fPrev.clear();
    fNext.clear();
}

SkIRect OnionSkin::Skin::boundingRect() const {
    return toSkIRect(fSurface->pixelBoundingRect());
}

SkIRect OnionSkin::SkinsSide::boundingRect() const {
    SkIRect result = SkIRect::MakeXYWH(0, 0, 0, 0);
    bool first = true;
    for(const auto& skin : fSkins) {
        if(first) {
            result = skin.boundingRect();
            first = false;
        } else result.join(skin.boundingRect());
    }
    return result;
}

void OnionSkin::SkinsSide::draw(SkCanvas * const canvas) {
    if(fSkins.isEmpty()) return;
    if(!fImage) setupImage(canvas->recordingContext()->asDirectContext());
    SkPaint paint;
    paint.setAlphaf(0.5f);
    canvas->drawImage(fImage, fImageXY.x(), fImageXY.y(), &paint);
}

void OnionSkin::SkinsSide::clear() {
    fSkins.clear();
    fImage.reset();
}

void OnionSkin::SkinsSide::setupImage(GrDirectContext * const grContext) {
    const auto bRect = boundingRect();
    if(bRect.width() <= 0 || bRect.height() <= 0) return;
    fImageXY = bRect.topLeft();
    const auto grTex = grContext->createBackendTexture(
                bRect.width(), bRect.height(),
                kRGBA_8888_SkColorType, GrMipMapped::kNo,
                GrRenderable::kYes);

    sk_sp<SkSurface> gpuSurface = SkSurface::MakeFromBackendTexture(
                grContext, grTex,
                kTopLeft_GrSurfaceOrigin, 0,
                kRGBA_8888_SkColorType,
                nullptr, nullptr);
    const auto texCanvas = gpuSurface->getCanvas();
    texCanvas->clear(SK_ColorTRANSPARENT);
    texCanvas->translate(-fImageXY.x(), -fImageXY.y());
    for(const auto& skin : fSkins) {
        SkPaint paint;
        const float rgbMax = qMax(fColor.fR, qMax(fColor.fG, fColor.fB));
        const float colM[20] = {
            1 - rgbMax, 0, 0, fColor.fR, 0,
            0, 1 - rgbMax, 0, fColor.fG, 0,
            0, 0, 1 - rgbMax, fColor.fB, 0,
            0, 0, 0, fColor.fA*skin.fWeight, 0};
        const auto colF = SkColorFilters::Matrix(colM);
        paint.setColorFilter(colF);

        skin.fSurface->drawOnCanvas(texCanvas, {0, 0}, &paint);
    }
    texCanvas->flush();
    fImage = SkImage::MakeFromAdoptedTexture(grContext, grTex,
                                             kTopLeft_GrSurfaceOrigin,
                                             kRGBA_8888_SkColorType);
}
