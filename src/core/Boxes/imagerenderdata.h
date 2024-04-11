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

#ifndef IMAGERENDERDATA_H
#define IMAGERENDERDATA_H
#include "Boxes/boxrenderdata.h"
#include "CacheHandlers/imagecachecontainer.h"

struct CORE_EXPORT ImageRenderData : public BoxRenderData {
    ImageRenderData(BoundingBox * const parentBoxT);

    virtual void loadImageFromHandler() = 0;

    void updateRelBoundingRect();
    void setupRenderData() final;

    sk_sp<SkImage> fImage;
private:
    void setupDirectDraw();

    void drawSk(SkCanvas * const canvas);
};

struct CORE_EXPORT ImageContainerRenderData : public ImageRenderData {
public:
    using ImageRenderData::ImageRenderData;

    void setContainer(ImageCacheContainer* container);

    void afterProcessing();
private:
    using ImageRenderData::fImage;
    stdptr<ImageCacheContainer> mSrcContainer;
};

#endif // IMAGERENDERDATA_H
