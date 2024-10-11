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

#ifndef TWOCOLUMNLAYOUT_H
#define TWOCOLUMNLAYOUT_H

#include "ui_global.h"

#include <QHBoxLayout>

class UI_EXPORT TwoColumnLayout : public QHBoxLayout
{
public:
    TwoColumnLayout();

    void addWidgetToFirstColumn(QWidget *wid);
    void addWidgetToSecondColumn(QWidget *wid);
    void addPair(QWidget *column1, QWidget *column2);
    void addPair(QWidget *column1, QLayout *column2);
    void addSpacing(const int size);

protected:
    QVBoxLayout *mLayout1 = nullptr;
    QVBoxLayout *mLayout2 = nullptr;
};

#endif // TWOCOLUMNLAYOUT_H
