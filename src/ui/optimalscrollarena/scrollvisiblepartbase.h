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

#ifndef SCROLLVISIBLEPARTBASE_H
#define SCROLLVISIBLEPARTBASE_H

#include "ui_global.h"

class MinimalScrollWidget;
class SingleWidget;

#include <QWidget>

class UI_EXPORT ScrollVisiblePartBase : public QWidget
{
public:
    ScrollVisiblePartBase(MinimalScrollWidget * const parent);

    virtual QWidget *createNewSingleWidget() = 0;
    virtual void updateVisibleWidgetsContent() = 0;

    void updateParentHeightAndContent();

    void setVisibleTop(const int top);
    void setVisibleHeight(const int height);

    void planScheduleUpdateVisibleWidgetsContent();
    void updateVisibleWidgetsContentIfNeeded();

    void planScheduleUpdateParentHeight();
    void updateParentHeightIfNeeded();
    void updateVisibleWidgets();
    void updateParentHeight();
    void updateWidgetsWidth();

    void callUpdaters();

    void scheduleContentUpdate();

    MinimalScrollWidget* parentWidget() const
    { return mParentWidget; }

    int visibleTop() const { return mVisibleTop; }
    int visibleHeight() const { return mVisibleHeight; }

    const QList<QWidget*>& widgets() const
    { return mSingleWidgets; }

protected:
    bool event(QEvent* event);

private:
    void postUpdateEvent();

    bool mEventSent = false;
    bool mContentUpdateScheduled = false;
    bool mParentHeightUpdateScheduled = false;

    int mVisibleTop = 0;
    int mVisibleHeight = 0;

    MinimalScrollWidget *mParentWidget;
    QList<QWidget*> mSingleWidgets;
};

#endif // SCROLLVISIBLEPARTBASE_H
