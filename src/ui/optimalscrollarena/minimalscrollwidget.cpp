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

#include "minimalscrollwidget.h"
#include "scrollvisiblepartbase.h"
#include "singlewidget.h"
#include "swt_abstraction.h"
#include "singlewidgettarget.h"
#include "scrollarea.h"
#include "GUI/global.h"

MinimalScrollWidget::MinimalScrollWidget(ScrollVisiblePartBase * const visiblePart,
                                         ScrollArea * const parent) :
    QWidget(parent),
    mVisiblePartWidget(visiblePart),
    mParentScrollArea(parent) {
    visiblePart->setParent(this);
}

void MinimalScrollWidget::scrollParentAreaBy(const int by) {
    mParentScrollArea->scrollBy(0, by);
}

void MinimalScrollWidget::setWidth(const int width) {
    setFixedWidth(width);
    mVisiblePartWidget->setFixedWidth(width);
    mVisiblePartWidget->updateWidgetsWidth();
}

int MinimalScrollWidget::minHeight() const
{ return mParentScrollArea->height(); }

void MinimalScrollWidget::changeVisibleTop(const int top) {
    int newTop = top - top % eSizesUI::widget;
    mVisiblePartWidget->move(0, newTop);
    mVisiblePartWidget->setVisibleTop(newTop);
}

void MinimalScrollWidget::changeVisibleHeight(const int height) {
    int newHeight = qCeil(height/(qreal)eSizesUI::widget)*
                    eSizesUI::widget;
    mVisiblePartWidget->setVisibleHeight(newHeight);
}
