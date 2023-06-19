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

#ifndef PAINTCOLORWIDGET_H
#define PAINTCOLORWIDGET_H

#include <QWidget>
#include <QColor>

class ColorValueRect;
class ColorLabel;

class PaintColorWidget : public QWidget {
    Q_OBJECT
public:
    PaintColorWidget(QWidget* const parent = nullptr);

    void setDisplayedColor(const QColor& color);
signals:
    void colorChanged(const QColor& color);
private:
    void startColorPicking();
    void setColor(const QColor& color);

    void updateFromRGB();
    void updateFromHSV();
    void updateFromHSL();

    ColorValueRect* rRect;
    ColorValueRect* gRect;
    ColorValueRect* bRect;
    ColorValueRect* hRect;
    ColorValueRect* hsvSatRect;
    ColorValueRect* vRect;
    ColorValueRect* hslSatRect;
    ColorValueRect* lRect;
    ColorValueRect* aRect;
    ColorLabel* mColorLabel;
};

#endif // PAINTCOLORWIDGET_H
