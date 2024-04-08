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

#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include "widgets/glwidget.h"
#include "colorhelpers.h"

class ColorWidget : public GLWidget {
    Q_OBJECT
public:
    ColorWidget(QWidget *parent = nullptr);

    void setColor(const QColor& color);
    void setColor(const qreal h, const qreal s, const qreal v);
protected:
    bool mValueBlocked = false;

    GLfloat mHslSaturatonTmp = -1; // HSL saturation saved when changing lightness

    GLfloat mHue = 0;
    GLfloat mSaturation = 0;
    GLfloat mValue = 0;
};

#endif // COLORWIDGET_H
