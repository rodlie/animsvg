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

#include "simplebrushwrapper.h"

SimpleBrushWrapper::SimpleBrushWrapper(const QString &collName,
                                       const QString &brushName,
                                       /*MyPaintBrush * const brush,*/
                                       const QByteArray& wholeFile) :
    /*mBrush(brush),*/ mWholeFile(wholeFile),
    mCollectionName(collName), mBrushName(brushName) {
}

SimpleBrushWrapper::~SimpleBrushWrapper() {
    //mypaint_brush_unref(mBrush);
}

stdsptr<SimpleBrushWrapper> SimpleBrushWrapper::createDuplicate() {
    /*auto brush = mypaint_brush_new_with_buckets(256);
    const char *data = mWholeFile.constData();

    if(!mypaint_brush_from_string(brush, data)) {
        mypaint_brush_unref(brush);
        return nullptr;
    }
    return enve::make_shared<SimpleBrushWrapper>(
                mCollectionName, mBrushName,
                brush, mWholeFile);*/
    return nullptr;
}
