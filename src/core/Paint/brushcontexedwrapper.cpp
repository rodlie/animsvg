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

#include "brushcontexedwrapper.h"
#include "brushescontext.h"
#include "Private/document.h"

BrushContexedWrapper::BrushContexedWrapper(BrushesContext *context,
                                           const BrushData &brushData) :
    mBrushData(brushData), mContext(context) {}

void BrushContexedWrapper::setSelected(const bool selected) {
    if(selected == mSelected) return;
    mSelected = selected;
    emit selectionChanged(mSelected);
}

bool BrushContexedWrapper::selected() const {
    return mSelected;
}

bool BrushContexedWrapper::bookmarked() const {
    return mBookmarked;
}

void BrushContexedWrapper::setBookmarked(const bool bookmarked) {
    if(bookmarked == mBookmarked) return;
    mBookmarked = bookmarked;
    emit bookmarkedChanged(mBookmarked);
}

const BrushData &BrushContexedWrapper::getBrushData() const {
    return mBrushData;
}

SimpleBrushWrapper *BrushContexedWrapper::getSimpleBrush() {
    return mBrushData.fWrapper.get();
}

BrushesContext *BrushContexedWrapper::getContext() const {
    return mContext;
}

void BrushContexedWrapper::bookmark() {
    if(mBookmarked) return;
    Document::sInstance->addBookmarkBrush(getSimpleBrush());
    setBookmarked(true);
}

void BrushContexedWrapper::unbookmark() {
    if(!mBookmarked) return;
    Document::sInstance->removeBookmarkBrush(getSimpleBrush());
    setBookmarked(false);
}
