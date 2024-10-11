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

#ifndef COLORVALUERECT_H
#define COLORVALUERECT_H

#include "ui_global.h"

#include "widgets/colorwidget.h"
#include "widgets/colorwidgetshaders.h"

class UI_EXPORT ColorValueRect : public ColorWidget
{
    Q_OBJECT

public:
    ColorValueRect(const ColorProgram &program,
                   QWidget * const parent = nullptr);

    void setDisplayedValue(const qreal val_t);
    qreal value() const { return mVal; }

private:
    void paintGL();
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    //void wheelEvent(QWheelEvent *e);

    void mouseInteraction(const int x_t);
    void setColorParameterFromVal();
    void setValueAndEmitValueChanged(const qreal valT);

signals:
    void valueChanged(qreal);
    void editingFinished(qreal);
    void editingStarted(qreal);

private:
    const ColorProgram& mGLProgram;
    qreal mVal = 0;
};

#endif // COLORVALUERECT_H
