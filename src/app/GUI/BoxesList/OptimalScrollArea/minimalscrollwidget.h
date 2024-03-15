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

#ifndef MINIMALSCROLLWIDGET_H
#define MINIMALSCROLLWIDGET_H


#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QtMath>
#include <functional>
class SingleWidget;
class ScrollVisiblePartBase;
class ScrollArea;

class MinimalScrollWidget : public QWidget {
    Q_OBJECT
public:
    explicit MinimalScrollWidget(ScrollVisiblePartBase * const visiblePart,
                                 ScrollArea * const parent);

    virtual void updateHeight() = 0;

    void scrollParentAreaBy(const int by);

    void changeVisibleTop(const int top);
    void changeVisibleHeight(const int height);
    void setWidth(const int width);

    int minHeight() const;
protected:
    ScrollVisiblePartBase* visiblePartWidget() const
    { return mVisiblePartWidget; }
private:
    ScrollVisiblePartBase * const mVisiblePartWidget;
    ScrollArea * const mParentScrollArea;
};


#endif // MINIMALSCROLLWIDGET_H
