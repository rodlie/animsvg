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

#ifndef IMAGEDATAHANDLER_H
#define IMAGEDATAHANDLER_H

#include "skia/skiaincludes.h"
#include "../core_global.h"

#include <QList>

class CORE_EXPORT ImageDataHandler {
protected:
    ImageDataHandler();
    ImageDataHandler(const sk_sp<SkImage>& img);

    int clearImageMemory();
    void replaceImage(const sk_sp<SkImage> &img);
public:
    int getImageByteCount() const;

    void drawImage(SkCanvas * const canvas,
                   const SkFilterQuality filter) const;

    bool hasImage() const { return mImage.get(); }
    const sk_sp<SkImage>& getImage() const;
    sk_sp<SkImage> requestImageCopy();
    void addImageCopy(const sk_sp<SkImage> &img);
private:
    sk_sp<SkImage> mImage;
    QList<sk_sp<SkImage>> mImageCopies;
};

#endif // IMAGEDATAHANDLER_H
