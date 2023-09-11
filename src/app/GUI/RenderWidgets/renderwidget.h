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

#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QPushButton>
#include "smartPointers/ememory.h"

class ScrollArea;
class Canvas;
class RenderInstanceWidget;
class RenderInstanceSettings;
class eWriteStream;
class eReadStream;

class RenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RenderWidget(QWidget *parent = nullptr);
    void createNewRenderInstanceWidgetForCanvas(Canvas *canvas);
    void setRenderedFrame(const int frame);
    void clearRenderQueue();
    void write(eWriteStream& dst) const;
    void read(eReadStream& src);

signals:
    void progress(int frame, int total);

private:
    void render(RenderInstanceSettings& settings);
    void addRenderInstanceWidget(RenderInstanceWidget *wid);
    QVBoxLayout *mMainLayout;
    QProgressBar *mRenderProgressBar;
    QPushButton *mStartRenderButton;
    QPushButton *mStopRenderButton;
    QWidget *mContWidget;
    QVBoxLayout *mContLayout;
    ScrollArea *mScrollArea;
    QList<RenderInstanceWidget*> mRenderInstanceWidgets;
    RenderInstanceSettings *mCurrentRenderedSettings;
    QList<RenderInstanceWidget*> mAwaitingSettings;

public:
    void leaveOnlyInterruptionButtonsEnabled();
    void leaveOnlyStartRenderButtonEnabled();
    void disableButtons();
    void enableButtons();
    void render();
    void stopRendering();
    void clearAwaitingRender();
    void sendNextForRender();
};

#endif // RENDERWIDGET_H
