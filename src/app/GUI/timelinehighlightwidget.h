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

#ifndef TIMELINEHIGHLIGHTWIDGET_H
#define TIMELINEHIGHLIGHTWIDGET_H

#include <QWidget>

class eSettings;

class TimelineHighlightWidget : public QWidget {
public:
    TimelineHighlightWidget(const bool track, QWidget* const parent);

    void setOther(TimelineHighlightWidget* const other);
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *) override;
    void paintEvent(QPaintEvent *) override;
private:
    void setHoverRow(const int row);

    int mHoverRow = -1;
    const eSettings& mSettings;
    TimelineHighlightWidget* mOther = nullptr;
};

#endif // TIMELINEHIGHLIGHTWIDGET_H
