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

#ifndef SKIAHELPERS_H
#define SKIAHELPERS_H

#include "ReadWrite/ereadstream.h"
#include "ReadWrite/ewritestream.h"

#include "skiaincludes.h"
#include "../exceptions.h"

#include <QIODevice>

namespace SkiaHelpers {
    CORE_EXPORT
    sk_sp<SkImage> makeCopy(const sk_sp<SkImage>& img);
    CORE_EXPORT
    SkBitmap makeCopy(const SkBitmap& btmp);

    CORE_EXPORT
    SkImageInfo getPremulRGBAInfo(const int width,
                                  const int height);
    CORE_EXPORT
    void saveImage(const QString& fileName,
                   const sk_sp<SkImage>& img,
                   const SkEncodedImageFormat& format = SkEncodedImageFormat::kPNG,
                   const int quality = 100);

    CORE_EXPORT
    sk_sp<SkImage> transferDataToSkImage(SkBitmap& bitmap);

    CORE_EXPORT
    void writeImg(const sk_sp<SkImage>& img, eWriteStream &dst);
    CORE_EXPORT
    sk_sp<SkImage> readImg(eReadStream& src);

    CORE_EXPORT
    SkBitmap readBitmap(eReadStream &src);
    CORE_EXPORT
    void writeBitmap(const SkBitmap& bitmap, eWriteStream &dst);

    CORE_EXPORT
    void writePixmap(const SkPixmap& pix, eWriteStream &dst);

    CORE_EXPORT
    void drawOutlineOverlay(SkCanvas * const canvas,
                            const SkPath &path,
                            const float invScale,
                            const SkColor &color = SK_ColorWHITE);

    CORE_EXPORT
    void drawOutlineOverlay(SkCanvas * const canvas,
                            const SkPath &path,
                            const float invScale,
                            const SkMatrix& transform,
                            const SkColor &color = SK_ColorWHITE);

    CORE_EXPORT
    void drawOutlineOverlay(SkCanvas * const canvas,
                            const SkPath &path,
                            const float invScale,
                            const bool dashes,
                            const float intervalSize,
                            const SkColor &color = SK_ColorWHITE);

    CORE_EXPORT
    void drawOutlineOverlay(SkCanvas * const canvas,
                            const SkPath &path,
                            const float invScale,
                            const SkMatrix& transform,
                            const bool dashes,
                            const float intervalSize,
                            const SkColor &color = SK_ColorWHITE);

    CORE_EXPORT
    void forceLink();

    CORE_EXPORT
    void textToPath(const SkFont& font,
                    const SkScalar x, const SkScalar y,
                    const QString& text, SkPath& path);
}

#endif // SKIAHELPERS_H
