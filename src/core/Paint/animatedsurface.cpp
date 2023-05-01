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

#include "animatedsurface.h"

#include "Tasks/domeletask.h"

AnimatedSurface::AnimatedSurface() : Animator("canvas"),
    mBaseValue(enve::make_shared<DrawableAutoTiledSurface>()),
    mCurrent_d(mBaseValue.get()) {
    connect(this, &Animator::anim_addedKey,
            this, [this](Key* const key) {
        updateCurrent();
        if(!mUseRange.inRange(key->getRelFrame())) return;
        const auto asKey = static_cast<ASKey*>(key);
        const auto dSurf = asKey->dSurface();
        if(!dSurf.storesDataInMemory())
            asKey->dSurface().scheduleLoadFromTmpFile();
        mUsed.append(&asKey->dSurface());
    });
    connect(this, &Animator::anim_removedKey,
            this, [this](Key* const key) {
        updateCurrent();
        if(!anim_hasKeys()) {
            *mBaseValue.get() = static_cast<ASKey*>(key)->dSurface();
        }
        if(!mUseRange.inRange(key->getRelFrame())) return;
        const auto asKey = static_cast<ASKey*>(key);
        mUsed.removeOne(&asKey->dSurface());
    });
}

void AnimatedSurface::setupOnionSkinFor(
        const int relFrame, const int sideRange, OnionSkin& skins,
        const std::function<void ()>& missingLoaded) {
    skins.clear();
    const auto minId = anim_getNextKeyId(relFrame - sideRange - 1);
    const auto maxId = anim_getPrevKeyId(relFrame + sideRange + 1);
    if(minId == -1 || maxId == -1) return;
    for(int i = minId; i <= maxId; i++) {
        const auto asKey = anim_getKeyAtIndex<ASKey>(i);
        if(asKey->getRelFrame() < relFrame)
            setupOnionSkinFor(relFrame, sideRange, asKey,
                              skins.fPrev, missingLoaded);
        else if(asKey->getRelFrame() > relFrame) {
            setupOnionSkinFor(relFrame, sideRange, asKey,
                              skins.fNext, missingLoaded);
        }
    }
}

void AnimatedSurface::prp_readProperty_impl(eReadStream& src) {
    Animator::prp_readProperty_impl(src);
    anim_readKeys(src);
    mBaseValue->read(src);
}

void AnimatedSurface::prp_writeProperty_impl(eWriteStream& dst) const {
    Animator::prp_writeProperty_impl(dst);
    anim_writeKeys(dst);
    mBaseValue->write(dst);
}

void savePaintImageXEV(const QString& path, const XevExporter& exp,
                       const DrawableAutoTiledSurface& surf) {
    const auto image = surf.toImage(true);
    exp.processAsset(path, [&](QIODevice* const dst) {
        image.save(dst, "PNG");
    }, false);
}

QDomElement AnimatedSurface::prp_writePropertyXEV_impl(const XevExporter& exp) const {
    auto result = exp.createElement("PaintSurface");
    if(anim_hasKeys()) {
        QString frames;
        QString pivots;
        for(const auto& key : anim_getKeys()) {
            if(!frames.isEmpty()) frames += ' ';
            if(!pivots.isEmpty()) pivots += ',';
            const auto frameStr = QString::number(key->getRelFrame());
            frames += frameStr;

            const auto& asKey = static_cast<ASKey*>(key);
            auto& surf = asKey->dSurface();
            const auto pivot = surf.zeroTilePos();
            pivots += QString("%1 %2").arg(pivot.x()).
                                       arg(pivot.y());
            const auto loadTask = surf.scheduleLoadFromTmpFile();
            if(loadTask) {
                const stdptr<DrawableAutoTiledSurface> surfPtr = &surf;
                const auto expPtr = exp.ref<const XevExporter>();
                const auto saveImage = [surfPtr, expPtr, frameStr]() {
                    if(!surfPtr) return;
                    savePaintImageXEV(frameStr + ".png", *expPtr, *surfPtr);
                };
                loadTask->addDependent({saveImage, nullptr});
            } else {
                savePaintImageXEV(frameStr + ".png", exp, surf);
            }
        }
        result.setAttribute("frames", frames);
        result.setAttribute("pivots", pivots);
    } else {
        const auto pivot = mBaseValue->zeroTilePos();
        const auto pivotStr = QString("%1 %2").arg(pivot.x()).
                                               arg(pivot.y());
        result.setAttribute("pivot", pivotStr);
        savePaintImageXEV("value.png", exp, *mBaseValue);
    }
    return result;
}

void AnimatedSurface::prp_readPropertyXEV_impl(const QDomElement& ele, const XevImporter& imp) {
    if(ele.hasAttribute("frames")) {
        const auto framesStr = ele.attribute("frames");
        const auto pivotsStr = ele.attribute("pivots");

        const auto frameStrs = framesStr.splitRef(' ', QString::SkipEmptyParts);
        const auto pivotStrs = pivotsStr.splitRef(',', QString::SkipEmptyParts);

        if(frameStrs.count() != pivotStrs.count())
            RuntimeThrow("Frames and pivot count does not match");
        const int iMax = frameStrs.count();
        for(int i = 0; i < iMax; i++) {
            const auto& frameStr = frameStrs[i];

            const auto& pivotStr = pivotStrs[i];
            const auto pivotValStrs = pivotStr.split(' ');
            if(pivotValStrs.size() != 2)
                RuntimeThrow("Invalid pivot '" + pivotStr.toString() + "'");
            const int pivotX = XmlExportHelpers::stringToInt(pivotValStrs[0]);
            const int pivotY = XmlExportHelpers::stringToInt(pivotValStrs[1]);

            const int frame = XmlExportHelpers::stringToInt(frameStr);

            imp.processAsset(frameStr + ".png", [&](QIODevice* const src) {
                QImage image;
                const bool ret = image.load(src, "PNG");
                if(!ret) RuntimeThrow("Failed to load value.png");
                const auto key = enve::make_shared<ASKey>(frame, this);
                auto& surf = key->dSurface();
                surf.loadPixmap(image);
                surf.move(-pivotX, -pivotY);
                anim_appendKey(key);
            });
        }
    } else {
        const auto pivotStr = ele.attribute("pivot");
        const auto pivotValStrs = pivotStr.splitRef(' ', QString::SkipEmptyParts);
        if(pivotValStrs.size() != 2)
            RuntimeThrow("Invalid pivot '" + pivotStr + "'");
        const int pivotX = XmlExportHelpers::stringToInt(pivotValStrs[0]);
        const int pivotY = XmlExportHelpers::stringToInt(pivotValStrs[1]);

        imp.processAsset("value.png", [&](QIODevice* const src) {
            QImage image;
            const bool ret = image.load(src, "PNG");
            if(!ret) RuntimeThrow("Failed to load value.png");
            mBaseValue->loadPixmap(image);
            mBaseValue->move(-pivotX, -pivotY);
        });
    }
}

void AnimatedSurface::prp_afterChangedAbsRange(const FrameRange &range, const bool clip) {
    Animator::prp_afterChangedAbsRange(range, clip);
    const auto relRange = prp_absRangeToRelRange(range);
    mFrameImagesCache.remove(relRange);
}

void AnimatedSurface::anim_addKeyAtRelFrame(const int relFrame) {
    if(anim_getKeyAtRelFrame(relFrame)) return;
    const auto prevNextKey = anim_getPrevAndNextKey<ASKey>(relFrame);
    stdsptr<ASKey> newKey;
    if(prevNextKey.first) {
        const auto& value = prevNextKey.first->dSurface();
        newKey = enve::make_shared<ASKey>(value, relFrame, this);
    } else if(prevNextKey.second) {
        const auto& value = prevNextKey.second->dSurface();
        newKey = enve::make_shared<ASKey>(value, relFrame, this);
    } else {
        newKey = enve::make_shared<ASKey>(*mBaseValue.get(), relFrame, this);
    }
    anim_appendKeyAction(newKey);
}

DrawableAutoTiledSurface* AnimatedSurface::getSurface(const int relFrame) {
    const auto spk = anim_getKeyAtRelFrame<ASKey>(relFrame);
    if(spk) return &spk->dSurface();
    const auto prevNextKey = anim_getPrevAndNextKey<ASKey>(relFrame);
    if(prevNextKey.first) {
        return &prevNextKey.first->dSurface();
    } else if(prevNextKey.second) {
        return &prevNextKey.second->dSurface();
    } else {
        return mBaseValue.get();
    }
}

void AnimatedSurface::afterSurfaceChanged(DrawableAutoTiledSurface * const surface) {
    if(!anim_hasKeys() && mCurrent_d == surface)
        prp_afterWholeInfluenceRangeChanged();
    for(const auto& key : anim_getKeys()) {
        const auto& asKey = static_cast<ASKey*>(key);
        if(&asKey->dSurface() != surface) continue;
        anim_updateAfterChangedKey(key);
        break;
    }
}

void AnimatedSurface::newEmptyFrame(const int relFrame) {
    const auto currKey = anim_getKeyAtRelFrame<ASKey>(relFrame);
    if(currKey) anim_removeKey(currKey->ref<ASKey>());
    const auto newKey = enve::make_shared<ASKey>(relFrame, this);
    anim_appendKeyAction(newKey);
}

template <typename T>
void AnimatedSurface::loadPixmapT(const T &src) {
    const bool createNewFrame = anim_isRecording() &&
                                !anim_getKeyOnCurrentFrame();
    if(createNewFrame) newEmptyFrame();
    auto& target = mCurrent_d->surface();
    const bool undoRedo = getParentScene();
    if(undoRedo) {
        target.triggerAllChange();
        const auto roi = mCurrent_d->pixelBoundingRect();
        mCurrent_d->loadPixmap(src);
        addUndoRedo("Load Image", roi);
    } else mCurrent_d->loadPixmap(src);
    afterChangedCurrentContent();
}

void AnimatedSurface::loadPixmap(const QImage &src) {
    loadPixmapT(src);
}

void AnimatedSurface::loadPixmap(const sk_sp<SkImage> &src) {
    if(!src) return;
    SkPixmap pixmap;
    const auto raster = src->makeRasterImage();
    if(raster->peekPixels(&pixmap)) loadPixmap(pixmap);
}

void AnimatedSurface::loadPixmap(const SkPixmap &src) {
    loadPixmapT(src);
}

eTask* AnimatedSurface::getFrameImage(const int relFrame, sk_sp<SkImage>& img) {
    const auto cont = mFrameImagesCache.atFrame<ImageCacheContainer>(relFrame);
    if(cont) {
        if(cont->storesDataInMemory()) {
            img = cont->requestImageCopy();
            return nullptr;
        }
        return cont->scheduleLoadFromTmpFile();
    }
    const auto surf = getSurface(relFrame);
    if(surf) {
        if(!surf->storesDataInMemory())
            return surf->scheduleLoadFromTmpFile();
        auto bitmap = surf->surface().toBitmap();
        img = SkiaHelpers::transferDataToSkImage(bitmap);
        const auto imgCpy = SkiaHelpers::makeCopy(img);
        const auto range = prp_getIdenticalRelRange(relFrame);
        const auto newCont = enve::make_shared<ImageCacheContainer>(
                                 imgCpy, range, &mFrameImagesCache);
        mFrameImagesCache.add(newCont);
    }
    return nullptr;
}

void AnimatedSurface::afterChangedCurrentContent() {
    const int relFrame = anim_getCurrentRelFrame();
    const auto identicalRange = prp_getIdenticalRelRange(relFrame);
    prp_afterChangedRelRange(identicalRange);
}

void AnimatedSurface::addUndoRedo(const QString& name, const QRect& roi) {
    auto& target = mCurrent_d->surface();
    auto undoList = target.takeUndoList();
    if(undoList.isEmpty()) return;
    {
        prp_pushUndoRedoName(name);
        const stdptr<DrawableAutoTiledSurface> ptr = mCurrent_d;
        UndoRedo ur;

        const auto replaceTile = [this, undoList, ptr, roi](
                                 const stdsptr<Tile>& (UndoTile::*getter)() const) {
            if(!ptr) return;
            auto& surface = ptr->surface();
            for(const auto& undoTile : undoList) {
                surface.replaceTile(undoTile.tileX(),
                                    undoTile.tileY(),
                                    (undoTile.*getter)());
            }
            surface.autoCrop();
            ptr->updateTileDimensions();
            ptr->pixelRectChanged(roi);
            afterChangedCurrentContent();
        };

        ur.fUndo = [replaceTile]() {
            replaceTile(&UndoTile::oldValue);
        };
        ur.fRedo = [replaceTile]() {
            replaceTile(&UndoTile::newValue);
        };
        prp_addUndoRedo(ur);
    }
}

#include "svgexporthelpers.h"
#include "Private/Tasks/taskscheduler.h"

class ASurfaceSaverSVG : public ComplexTask {
public:
    ASurfaceSaverSVG(AnimatedSurface* const src,
                     SvgExporter& exp, QDomElement& use,
                     const qreal div, const FrameRange& relRange,
                     const FrameRange& visRelRange) :
        ComplexTask(relRange.fMax, "SVG Animation"),
        mSrc(src), mExp(exp), mUse(use),
        mRelRange(relRange), mVisRage(visRelRange),
        mDiv(div), mKeyRelFrame(relRange.fMin - 1) {}

    void nextStep() override {
        if(!mSrc) return cancel();
        if(setValue(mKeyRelFrame)) return finish();
        if(done()) return;

        bool first = mKeyId == 0;
        if(first) {
            const auto idRange = mSrc->prp_getIdenticalRelRange(mVisRage.fMin);
            const int span = mExp.fAbsRange.span();

            if(idRange.inRange(mVisRage) || span == 1) {
                addSurface(mVisRage.fMin, nullptr);
                mKeyRelFrame = mVisRage.fMax;
                return nextStep();
            }
        }

        const auto& keys = mSrc->anim_getKeys();
        if(mKeyId >= keys.count()) {
            mKeyRelFrame = mVisRage.fMax;
            return nextStep();
        }
        const auto key = static_cast<ASKey*>(keys.atId(mKeyId++));
        mKeyRelFrame = key->getRelFrame();
        if(mKeyRelFrame >= mVisRage.fMax) return nextStep();
        if(mKeyRelFrame >= mVisRage.fMin) {
            DrawableAutoTiledSurface* surf = nullptr;
            if(first) {
                first = false;
                const int prevId = mKeyId - 2;
                const auto null = static_cast<Key*>(nullptr);
                const auto prevKey = prevId >= 0 ? keys.atId(prevId) : null;
                const auto prevASKey = static_cast<ASKey*>(prevKey);
                const bool keyOnFirstFrame = mKeyRelFrame == mVisRage.fMin;
                const bool useKey = keyOnFirstFrame || !prevKey;
                const auto firstKey = useKey ? key : prevASKey;
                surf = &firstKey->dSurface();
            } else surf = &key->dSurface();
            const bool wait = addSurface(mKeyRelFrame, surf);
            if(!wait) addEmptyTask();
        } else nextStep();
    }

private:
    //! @brief Returns true if there is a task, does have to wait.
    bool addSurface(const int relFrame, DrawableAutoTiledSurface* surf) {
        if(!surf) surf = mSrc->getSurface(relFrame);
        const QString imageId = SvgExportHelpers::ptrToStr(surf);
        const QPoint pos = -surf->zeroTilePos();
        sk_sp<SkImage> image;
        const auto task = mSrc->getFrameImage(relFrame, image);
        if(task) {
            const auto imgTask = static_cast<ImgLoader*>(task);
            const QPointer<ASurfaceSaverSVG> ptr = this;
            task->addDependent({[ptr, imgTask, relFrame, imageId, pos]() {
                if(!ptr) return;
                const auto image = imgTask->image();
                ptr->saveSurfaceValues(relFrame, image, imageId, pos);
            }, nullptr});
            addTask(task->ref<eTask>());
            return true;
        } else {
            saveSurfaceValues(relFrame, image, imageId, pos);
            return false;
        }
    }

    void saveSurfaceValues(const int relFrame,
                           const sk_sp<SkImage> &image,
                           const QString &imageId,
                           const QPoint &pos)
    {
        if (!image) { return; }
        const qreal t = (relFrame - mRelRange.fMin)/mDiv;
        mKeyTimes << QString::number(t);

        SvgExportHelpers::defImage(mExp, image, imageId);
        mHrefValues << "#" + imageId;

        mXValues << QString::number(pos.x());
        mYValues << QString::number(pos.y());
    }

    void finish() {
        if(mHrefValues.isEmpty()) return;

        const QString href = mHrefValues.first();
        const QString x = mXValues.first();
        const QString y = mYValues.first();
        mUse.setAttribute("href", href);
        mUse.setAttribute("x", x);
        mUse.setAttribute("y", y);
        if(mHrefValues.count() > 1) {
            if(mKeyTimes.last() != "1") {
                mHrefValues << mHrefValues.last();
                mXValues << mXValues.last();
                mYValues << mYValues.last();
                mKeyTimes << "1";
            }
            if(mKeyTimes.first() != "0") {
                mHrefValues.prepend(mHrefValues.first());
                mXValues.prepend(mXValues.first());
                mYValues.prepend(mYValues.first());
                mKeyTimes.prepend("0");
            }

            const qreal dur = mDiv/mExp.fFps;
            const auto durStr = QString::number(dur)  + 's';
            const auto keyTimesStr = mKeyTimes.join(';');
            {
                auto anim = mExp.createElement("animate");
                anim.setAttribute("attributeName", "href");
                anim.setAttribute("dur", durStr);
                anim.setAttribute("values", mHrefValues.join(';'));
                anim.setAttribute("keyTimes", keyTimesStr);
                SvgExportHelpers::assignLoop(anim, mExp.fLoop);
                mUse.appendChild(anim);
            }
            {
                auto anim = mExp.createElement("animate");
                anim.setAttribute("attributeName", "x");
                anim.setAttribute("dur", durStr);
                anim.setAttribute("values", mXValues.join(';'));
                anim.setAttribute("keyTimes", keyTimesStr);
                SvgExportHelpers::assignLoop(anim, mExp.fLoop);
                mUse.appendChild(anim);
            }
            {
                auto anim = mExp.createElement("animate");
                anim.setAttribute("attributeName", "y");
                anim.setAttribute("dur", durStr);
                anim.setAttribute("values", mYValues.join(';'));
                anim.setAttribute("keyTimes", keyTimesStr);
                SvgExportHelpers::assignLoop(anim, mExp.fLoop);
                mUse.appendChild(anim);
            }
        }
    }

    const QPointer<AnimatedSurface> mSrc;
    SvgExporter& mExp;
    QDomElement& mUse;
    const FrameRange mRelRange;
    const FrameRange mVisRage;
    const qreal mDiv;

    int mKeyRelFrame = 0;
    int mKeyId = 0;

    QStringList mHrefValues;
    QStringList mXValues;
    QStringList mYValues;
    QStringList mKeyTimes;
};

eTaskBase* AnimatedSurface::savePaintSVG(SvgExporter& exp,
                                         QDomElement& use,
                                         const FrameRange& visRelRange) {
    const auto relRange = prp_absRangeToRelRange(exp.fAbsRange);
    const int span = exp.fAbsRange.span();
    const qreal div = span - 1;

    const auto task = new ASurfaceSaverSVG(this, exp, use, div,
                                           relRange, visRelRange);
    const auto taskSPtr = QSharedPointer<ASurfaceSaverSVG>(
                              task, &QObject::deleteLater);
    task->nextStep();

    if(task->done()) return nullptr;
    TaskScheduler::instance()->addComplexTask(taskSPtr);
    return task;
}

void AnimatedSurface::updateCurrent() {
    const auto spk = anim_getKeyOnCurrentFrame<ASKey>();
    if(spk) setCurrent(&spk->dSurface());
    else {
        const int relFrame = anim_getCurrentRelFrame();
        const auto prevNextKey = anim_getPrevAndNextKey<ASKey>(relFrame);
        if(prevNextKey.first) {
            setCurrent(&prevNextKey.first->dSurface());
        } else if(prevNextKey.second) {
            setCurrent(&prevNextKey.second->dSurface());
        } else {
            setCurrent(mBaseValue.get());
        }
    }
}

void AnimatedSurface::setCurrent(DrawableAutoTiledSurface* const surf) {
    if(mCurrent_d == surf) return;
    mCurrent_d = surf;
    emit currentSurfaceChanged(mCurrent_d);
}

ASKey::ASKey(AnimatedSurface * const parent) :
    Key(parent),
    mValue(enve::make_shared<DrawableAutoTiledSurface>()) {}

ASKey::ASKey(const int frame, AnimatedSurface * const parent) :
    Key(frame, parent),
    mValue(enve::make_shared<DrawableAutoTiledSurface>()) {}

ASKey::ASKey(const DrawableAutoTiledSurface &value,
             const int frame, AnimatedSurface * const parent) :
    ASKey(frame, parent) {
    *mValue.get() = value;
}
