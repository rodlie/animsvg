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

#include "savedcolorswidget.h"
#include "widgets/savedcolorbutton.h"
#include <QAction>
#include <QMenu>
#include "colorhelpers.h"
#include "Private/document.h"

SavedColorsWidget::SavedColorsWidget(QWidget *parent)
    : QWidget(parent) {
    mMainLayout = new FlowLayout(this);
    setLayout(mMainLayout);
    for(const auto& color : Document::sInstance->fColors) {
        addColor(color);
    }
    connect(Document::sInstance, &Document::bookmarkColorAdded,
            this, &SavedColorsWidget::addColor);
    connect(Document::sInstance, &Document::bookmarkColorRemoved,
            this, &SavedColorsWidget::removeColor);
    setVisible(!mButtons.isEmpty());
}

void SavedColorsWidget::addColor(const QColor& color) {
    const auto button = new SavedColorButton(color, this);
    connect(button, &SavedColorButton::selected,
            this, &SavedColorsWidget::colorSet);
    mMainLayout->addWidget(button);
    mButtons << button;
    setVisible(!mButtons.isEmpty());
}

void SavedColorsWidget::removeColor(const QColor &color) {
    const auto rgba = color.rgba();
    for(const auto wid : mButtons) {
        if(wid->getColor().rgba() == rgba) {
            mButtons.removeOne(wid);
            wid->deleteLater();
            break;
        }
    }
    setVisible(!mButtons.isEmpty());
}

void SavedColorsWidget::setColor(const QColor &color) {
    for(const auto wid : mButtons) {
        wid->setSelected(wid->getColor().rgba() == color.rgba());
    }
}
