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

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "colorconversions.h"
#include "autotiledsurface.h"

AutoTiledSurfaceBase::AutoTiledSurfaceBase(const TileCreator tileCreator,
                                           const Request requestStart,
                                           const Request requestEnd) :
    mMyPaintSurface(&mParent.parent),
    mAutoTilesData(tileCreator),
    mRequestStart(requestStart),
    mRequestEnd(requestEnd) {
    mypaint_tiled_surface_init(&mParent, requestStart, requestEnd);
    mParent.parent.destroy = sFree;
//#ifdef _OPENMP
    mParent.threadsafe_tile_requests = true;
//#else
//    mParent.threadsafe_tile_requests = false;
//#endif
}

AutoTiledSurfaceBase::AutoTiledSurfaceBase(const AutoTiledSurfaceBase &other) :
    AutoTiledSurfaceBase(other.mTileCreator,
                         other.mRequestStart,
                         other.mRequestEnd) {
    mAutoTilesData = other.mAutoTilesData;
}

AutoTiledSurfaceBase::AutoTiledSurfaceBase(AutoTiledSurfaceBase &&other) :
    AutoTiledSurfaceBase(other.mTileCreator,
                         other.mRequestStart,
                         other.mRequestEnd) {
    mAutoTilesData = std::move(other.mAutoTilesData);
}

AutoTiledSurfaceBase &AutoTiledSurfaceBase::operator=(const AutoTiledSurfaceBase &other) {
    mAutoTilesData = other.mAutoTilesData;
    return *this;
}

AutoTiledSurfaceBase &AutoTiledSurfaceBase::operator=(AutoTiledSurfaceBase &&other) {
    mAutoTilesData = std::move(other.mAutoTilesData);
    return *this;
}

AutoTiledSurfaceBase::~AutoTiledSurfaceBase() {
    free();
}

void AutoTiledSurfaceBase::setPixelClamp(const QRect &pixRect) {
    mAutoTilesData.setPixelClamp(pixRect);
}

void AutoTiledSurfaceBase::loadPixmap(const SkPixmap& src) {
    mAutoTilesData.loadPixmap(src);
}

void AutoTiledSurfaceBase::loadPixmap(const QImage &src) {
    mAutoTilesData.loadPixmap(src);
}

void AutoTiledSurfaceBase::replaceTile(const int tx, const int ty,
                                       const stdsptr<Tile> &tile) {
    mAutoTilesData.replaceTile(tx, ty, tile);
}

void AutoTiledSurfaceBase::crop(const QRect& crop) {
    mAutoTilesData.crop(crop);
}

void AutoTiledSurfaceBase::move(const int dx, const int dy) {
    mAutoTilesData.move(dx, dy);
}

void AutoTiledSurfaceBase::discardTransparentTiles() {
    mAutoTilesData.discardTransparentTiles();
}

void AutoTiledSurfaceBase::autoCrop() {
    mAutoTilesData.autoCrop();
}

stdsptr<Tile> AutoTiledSurfaceBase::requestTile(const int tx, const int ty) {
    return mAutoTilesData.requestTile(tx, ty);
}

void AutoTiledSurfaceBase::sFree(MyPaintSurface *surface) {
    const auto self = reinterpret_cast<AutoTiledSurfaceBase*>(surface);
    self->free();
}


void AutoTiledSurfaceBase::free() {
    mypaint_tiled_surface_destroy(&mParent);
}

#ifdef _OPENMP
#include <omp.h>
#endif

void AutoTiledSurface::sRequestStart(MyPaintTiledSurface *surface,
                                     MyPaintTileRequest *request) {
    const auto self = reinterpret_cast<AutoTiledSurface*>(surface);
    #pragma omp critical
    {
        // make copy for undo/redo if not yet done,
        // keep references to tiles,
        // flush undo/redo later
        const auto tile = self->requestTile(request->tx, request->ty);
        if(tile) request->buffer = tile->requestZeroedData();
        else request->buffer = nullptr;
    }
}

void AutoTiledSurface::sRequestEnd(MyPaintTiledSurface *,
                                   MyPaintTileRequest *) {}

AutoTiledSurface::AutoTiledSurface() :
    AutoTiledSurfaceBase([](const size_t& size) {
                            return std::make_shared<Tile>(size);
                         }, sRequestStart, sRequestEnd) {}

void UndoableAutoTiledSurface::sRequestStart(MyPaintTiledSurface *surface,
                                             MyPaintTileRequest *request) {
    const auto self = reinterpret_cast<UndoableAutoTiledSurface*>(surface);
    #pragma omp critical
    {
        // make copy for undo/redo if not yet done,
        // keep references to tiles,
        // flush undo/redo later
        const auto tile = self->requestTile(request->tx, request->ty);
        const auto undoableTile = std::static_pointer_cast<UndoableTile>(tile);
        if(!undoableTile->fUndo) {
            self->addToUndoList(UndoTile(request->tx, request->ty, undoableTile));
        }
        request->buffer = tile->requestZeroedData();
    }
}

void UndoableAutoTiledSurface::sRequestEnd(MyPaintTiledSurface *,
                                           MyPaintTileRequest *) {}
UndoableAutoTiledSurface::UndoableAutoTiledSurface() :
    AutoTiledSurfaceBase([](const size_t& size) {
    return std::make_shared<UndoableTile>(size);
}, sRequestStart, sRequestEnd) {}

void UndoableAutoTiledSurface::triggerAllChange() {
    const QRect tiles = tileBoundingRect();
    for(int tx = tiles.left(); tx <= tiles.right(); tx++) {
        for(int ty = tiles.top(); ty <= tiles.bottom(); ty++) {
            const auto tile = requestTile(tx, ty);
            const auto undoableTile = std::static_pointer_cast<UndoableTile>(tile);
            if(!undoableTile->fUndo) {
                addToUndoList(UndoTile(tx, ty, undoableTile));
            }
        }
    }
}
