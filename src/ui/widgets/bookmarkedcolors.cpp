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

#include "bookmarkedcolors.h"
#include "widgets/savedcolorbutton.h"
#include "Private/document.h"

BookmarkedColors::BookmarkedColors(const bool vertical,
                                   const int dimension,
                                   QWidget * const parent) :
    BookmarkedWidget(vertical, dimension, parent) {
    connect(Document::sInstance, &Document::bookmarkColorAdded,
            this, &BookmarkedColors::addColor);
    connect(Document::sInstance, &Document::bookmarkColorRemoved,
            this, &BookmarkedColors::removeColor);
    connect(Document::sInstance, &Document::brushColorChanged,
            this, &BookmarkedColors::setCurrentColor);
}

void BookmarkedColors::setCurrentColor(const QColor &color) {
    const int iCount = count();
    for(int i = 0 ; i < iCount; i++) {
        const auto widget = static_cast<SavedColorButton*>(getWidget(i));
        widget->setSelected(widget->getColor().rgba() == color.rgba());
    }
}

void BookmarkedColors::addColor(const QColor &color) {
    const auto button = new SavedColorButton(color);
    connect(button, &SavedColorButton::selected, this, [color]() {
        Document::sInstance->setBrushColor(color);
    });
    addWidget(button);
}

void BookmarkedColors::removeColor(const QColor &color) {
    const auto rgba = color.rgba();
    const int iCount = count();
    for(int i = 0 ; i < iCount; i++) {
        const auto button = static_cast<SavedColorButton*>(getWidget(i));
        if(button->getColor().rgba() == rgba) {
            removeWidget(button);
            break;
        }
    }
}
