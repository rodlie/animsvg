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

#ifndef CURRENTGRADIENTWIDGET_H
#define CURRENTGRADIENTWIDGET_H

#include "ui_global.h"

#include "widgets/glwidget.h"
#include "conncontextptr.h"

class GradientWidget;
class ColorAnimator;
class Gradient;

class UI_EXPORT CurrentGradientWidget : public GLWidget
{
    Q_OBJECT

public:
    explicit CurrentGradientWidget(QWidget *parent = nullptr);

    void setCurrentGradient(Gradient * const gradient);
    void colorRightPress(const int x, const QPoint &point);
    void colorLeftPress(const int x);
    void setCurrentColorId(const int id);
    void colorAdd();
    void colorRemove();

    ColorAnimator *getColorAnimator();

protected:
    void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);

signals:
    void selectedColorChanged(ColorAnimator*);

private:
    void updateCurrentColor();
    int getColorIdAtX(const int x);

    bool mReordering = false;
    bool mFirstMove = true;
    int mHoveredX = 0;

    ConnContextQPtr<Gradient> mGradient;
    QPointer<ColorAnimator> mColor;
    int mColorId = 0;
};

#endif // CURRENTGRADIENTWIDGET_H
