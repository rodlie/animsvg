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

#include "twocolumnlayout.h"

TwoColumnLayout::TwoColumnLayout() {
    mLayout1 = new QVBoxLayout();
    mLayout2 = new QVBoxLayout();
    addLayout(mLayout1);
    addLayout(mLayout2);
}

void TwoColumnLayout::addWidgetToFirstColumn(QWidget *wid) {
    mLayout1->addWidget(wid);
}

void TwoColumnLayout::addWidgetToSecondColumn(QWidget *wid) {
    mLayout2->addWidget(wid);
}

void TwoColumnLayout::addPair(QWidget *column1, QWidget *column2) {
    addWidgetToFirstColumn(column1);
    addWidgetToSecondColumn(column2);
}

void TwoColumnLayout::addPair(QWidget* column1, QLayout* column2) {
    addWidgetToFirstColumn(column1);
    mLayout2->addLayout(column2);
}

void TwoColumnLayout::addSpacing(const int size) {
    mLayout1->addSpacing(size);
    mLayout2->addSpacing(size);
}
