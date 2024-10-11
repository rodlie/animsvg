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

#ifndef IMAGESEQUENCECACHEHANDLER_H
#define IMAGESEQUENCECACHEHANDLER_H
#include "imagecachehandler.h"
#include "animationcachehandler.h"

class CORE_EXPORT ImageSequenceFileHandler : public FileCacheHandler {
protected:
    void reload();
public:
    void replace();

    ImageCacheContainer* getFrameAtFrame(const int relFrame);
    ImageCacheContainer* getFrameAtOrBeforeFrame(const int relFrame);
    eTask* scheduleFrameLoad(const int frame);
    int getFrameCount() const { return mFrameImageHandlers.count(); }
private:
    QList<qsptr<ImageFileDataHandler>> mFrameImageHandlers;
};

class CORE_EXPORT ImageSequenceCacheHandler : public AnimationFrameHandler {
    e_OBJECT
protected:
    ImageSequenceCacheHandler(ImageSequenceFileHandler* fileHandler);
public:
    ImageCacheContainer* getFrameAtFrame(const int relFrame) {
        if(!mFileHandler) return nullptr;
        return mFileHandler->getFrameAtFrame(relFrame);
    }

    ImageCacheContainer* getFrameAtOrBeforeFrame(const int relFrame) {
        if(!mFileHandler) return nullptr;
        return mFileHandler->getFrameAtOrBeforeFrame(relFrame);
    }
    eTask* scheduleFrameLoad(const int frame) {
        if(!mFileHandler) return nullptr;
        return mFileHandler->scheduleFrameLoad(frame);
    }
    void reload() {
        if(mFileHandler) mFileHandler->reloadAction();
    }
    int getFrameCount() const {
        if(!mFileHandler) return 0;
        return mFileHandler->getFrameCount();
    }
private:
    const qptr<ImageSequenceFileHandler> mFileHandler;

};
#endif // IMAGESEQUENCECACHEHANDLER_H
