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

#ifndef BOOKMARKEDWIDGET_H
#define BOOKMARKEDWIDGET_H

#include "ui_global.h"

#include <QWidget>
#include <QPushButton>

class UI_EXPORT BookmarkedWidget : public QWidget
{
public:
    explicit BookmarkedWidget(const bool vertical,
                              const int dimension,
                              QWidget *parent = nullptr);

    void addWidget(QWidget* const wid);
    void removeWidget(QWidget* const wid);
    QWidget* getWidget(const int id) const;
    int count() const
    { return mWidgets.count(); }
    void updateSize();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void updateLayout();

    const bool mVertical;
    const int mDimension;
    QPushButton* mUpArrow;
    QPushButton* mDownArrow;
    QList<QWidget*> mWidgets;
    int mFirstViewed = 0;
    int mLastViewed = 0;
};

#endif // BOOKMARKEDWIDGET_H
