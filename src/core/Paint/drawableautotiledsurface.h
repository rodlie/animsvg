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

#ifndef DRAWABLEAUTOTILEDSURFACE_H
#define DRAWABLEAUTOTILEDSURFACE_H
//#include "autotiledsurface.h"
#include "skia/skiahelpers.h"
#include "CacheHandlers/hddcachablecont.h"
#include "tilebitmaps.h"

class CORE_EXPORT DrawableAutoTiledSurface : public HddCachableCont {
    e_OBJECT
    typedef QList<QList<SkBitmap>> Tiles;
public:
    DrawableAutoTiledSurface();
    DrawableAutoTiledSurface(const DrawableAutoTiledSurface& other);
    DrawableAutoTiledSurface& operator=(const DrawableAutoTiledSurface& other);
protected:
    stdsptr<eHddTask> createTmpFileDataSaver();
    stdsptr<eHddTask> createTmpFileDataLoader();

    int getByteCount();
    int clearMemory();
    void noDataLeft_k() { Q_ASSERT(false); }
public:
    void drawOnCanvas(SkCanvas * const canvas,
                      const SkPoint &dst,
                      const QRect * const minPixSrc,
                      SkPaint * const paint) const;

    void drawOnCanvas(SkCanvas * const canvas,
                      const SkPoint &dst,
                      const QRect * const minPixSrc) const {
        drawOnCanvas(canvas, dst, minPixSrc, nullptr);
    }

    void drawOnCanvas(SkCanvas * const canvas,
                      const SkPoint &dst) const {
        drawOnCanvas(canvas, dst, nullptr, nullptr);
    }

    void drawOnCanvas(SkCanvas * const canvas,
                      const SkPoint &dst,
                      SkPaint * const paint) const {
        drawOnCanvas(canvas, dst, nullptr, paint);
    }

    /*UndoableAutoTiledSurface &surface()
    { return mSurface; }*/

    void pixelRectChanged(const QRect& pixRect);

    QRect pixelBoundingRect() const
    { return tileRectToPixRect(tileBoundingRect()); }

    int width() const
    { return pixelBoundingRect().width(); }

    int height() const
    { return pixelBoundingRect().height(); }

    void write(eWriteStream& dst);
    void read(eReadStream& src);

    void loadPixmap(const SkPixmap& src);
    void loadPixmap(const QImage &src);

    QImage toImage(const bool use16Bit,
                   const QMargins &margin = QMargins()) const;

    void updateTileBitmaps();

    void clearBitmaps();

    bool hasTileBitmaps()
    { return !mTileBitmaps.isEmpty(); }

    void drawingDoneForNow() { afterDataReplaced(); }

    void updateTileDimensions();

    void crop(const QRect& crop);
    void move(const int dx, const int dy);

    QPoint zeroTile() const
    { return QPoint(mZeroTileCol, mZeroTileRow); }

    QPoint zeroTilePos() const
    { return zeroTile()*/*TILE_SIZE*/64; }
private:
    void removeFirstColumn();
    void removeLastColumn();
    void removeFirstRow();
    void removeLastRow();

    void removeFirstColumns(const int count);
    void removeLastColumns(const int count);
    void removeFirstRows(const int count);
    void removeLastRows(const int count);

    void updateTileRecBitmaps(QRect tileRect);

    void setTileBitmaps(const TileBitmaps& tiles);
    void setTileBitmaps(TileBitmaps&& tiles);

    void stretchBitmapsToTile(const int tx, const int ty);

    QList<SkBitmap> newBitmapColumn();

    void prependBitmapRow();
    void appendBitmapRow();
    void prependBitmapColumn();
    void appendBitmapColumn();

    void prependBitmapRows(const int count);
    void appendBitmapRows(const int count);
    void prependBitmapColumns(const int count);
    void appendBitmapColumns(const int count);

    SkBitmap bitmapForTile(const int tx, const int ty) const;
    SkBitmap imageForTileId(const int colId, const int rowId) const;


    QRect tileBoundingRect() const;
    QRect tileRectToPixRect(const QRect& tileRect) const;
    QRect pixRectToTileRect(const QRect& pixRect) const;

    //UndoableAutoTiledSurface mSurface;
    TileBitmaps mTileBitmaps;
    int &mRowCount;
    int &mColumnCount;
    int &mZeroTileRow;
    int &mZeroTileCol;
    Tiles &mBitmaps;
};

#endif // DRAWABLEAUTOTILEDSURFACE_H
