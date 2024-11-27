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

#ifndef IMAGECACHEHANDLER_H
#define IMAGECACHEHANDLER_H
#include "skia/skiahelpers.h"
#include "filecachehandler.h"
#include "Tasks/updatable.h"
#include "CacheHandlers/usepointer.h"
#include "CacheHandlers/imagecachecontainer.h"
class ImageFileDataHandler;

class CORE_EXPORT ImageLoader : public eHddTask
{
    e_OBJECT

protected:
    ImageLoader(const QString &filePath,
                ImageFileDataHandler * const handler);

public:
    void process();
    void afterProcessing();
    void afterCanceled();

protected:
    const qptr<ImageFileDataHandler> mTargetHandler;
    const QString mFilePath;
    sk_sp<SkImage> mImage;
};

/*class CORE_EXPORT OraLoader : public ImageLoader
{
    e_OBJECT

protected:
    using ImageLoader::ImageLoader;

public:
    void process();
};*/

class CORE_EXPORT ImageFileDataHandler : public FileDataCacheHandler
{
    e_OBJECT
    friend class ImageLoader;

    enum class Type {
        image, ora, none
    };

    class ImageCacheContainerX : public ImageCacheContainer
    {
        e_OBJECT
    protected:
        ImageCacheContainerX(const sk_sp<SkImage> &img,
                             ImageFileDataHandler* const handler)
        : ImageCacheContainer(img, FrameRange::EMINMAX, nullptr)
        , mHandler(handler) {}

        void noDataLeft_k()
        {
            ImageCacheContainer::noDataLeft_k();
            if (!mHandler) { return; }
            mHandler->mImage.reset();
        }

    private:
        const qptr<ImageFileDataHandler> mHandler;
    };

protected:
    ImageFileDataHandler();

public:
    void afterSourceChanged();
    void clearCache();

    eTask *scheduleLoad();

    bool hasImage() const;
    sk_sp<SkImage> getImage() const;
    ImageCacheContainer* getImageContainer() { return mImage.get(); }

private:
    void replaceImage(const sk_sp<SkImage> &img);

    stdsptr<ImageCacheContainerX> mImage;
    Type mType = Type::none;
    stdsptr<ImageLoader> mImageLoader;
};

class CORE_EXPORT ImageFileHandler : public FileCacheHandler
{
    e_OBJECT

protected:
    ImageFileHandler() {}

    void reload()
    {
        if (fileMissing()) { return mDataHandler.reset(); }
        mDataHandler = ImageFileDataHandler::sGetCreateDataHandler<ImageFileDataHandler>(path());
        mDataHandler->clearCache();
    }

public:
    void replace();

    eTask * scheduleLoad()
    {
        if (!mDataHandler) { return nullptr; }
        return mDataHandler->scheduleLoad();
    }

    bool hasImage() const
    {
        if (!mDataHandler) { return false; }
        return mDataHandler->hasImage();
    }

    sk_sp<SkImage> getImage() const
    {
        if (!mDataHandler) { return nullptr; }
        return mDataHandler->getImage();
    }

    ImageCacheContainer* getImageContainer() const
    {
        if (!mDataHandler) { return nullptr; }
        return mDataHandler->getImageContainer();
    }

private:
    qsptr<ImageFileDataHandler> mDataHandler;
};

#endif // IMAGECACHEHANDLER_H
