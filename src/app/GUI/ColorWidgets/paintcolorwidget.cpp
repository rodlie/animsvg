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

#include "paintcolorwidget.h"

#include "colorvaluerect.h"
#include "colorlabel.h"
#include "colorpickingwidget.h"
#include "GUI/mainwindow.h"
#include "widgets/actionbutton.h"
#include <QWindow>

#include <QVBoxLayout>

PaintColorWidget::PaintColorWidget(QWidget* const parent) :
    QWidget(parent) {
    rRect = new ColorValueRect(RED_PROGRAM, this);
    gRect = new ColorValueRect(GREEN_PROGRAM, this);
    bRect = new ColorValueRect(BLUE_PROGRAM, this);

    hRect = new ColorValueRect(HUE_PROGRAM, this);
    hsvSatRect = new ColorValueRect(HSV_SATURATION_PROGRAM, this);
    vRect = new ColorValueRect(VALUE_PROGRAM, this);

    hslSatRect = new ColorValueRect(HSL_SATURATION_PROGRAM, this);
    lRect = new ColorValueRect(LIGHTNESS_PROGRAM, this);

    aRect = new ColorValueRect(ALPHA_PROGRAM, this);

    mColorLabel = new ColorLabel(this);

    connect(rRect, &ColorValueRect::valueChanged,
            this, &PaintColorWidget::updateFromRGB);
    connect(gRect, &ColorValueRect::valueChanged,
            this, &PaintColorWidget::updateFromRGB);
    connect(bRect, &ColorValueRect::valueChanged,
            this, &PaintColorWidget::updateFromRGB);

    connect(hRect, &ColorValueRect::valueChanged,
            this, &PaintColorWidget::updateFromHSV);
    connect(hsvSatRect, &ColorValueRect::valueChanged,
            this, &PaintColorWidget::updateFromHSV);
    connect(vRect, &ColorValueRect::valueChanged,
            this, &PaintColorWidget::updateFromHSV);

    connect(hslSatRect, &ColorValueRect::valueChanged,
            this, &PaintColorWidget::updateFromHSL);
    connect(lRect, &ColorValueRect::valueChanged,
            this, &PaintColorWidget::updateFromHSL);

    connect(aRect, &ColorValueRect::valueChanged,
            this, &PaintColorWidget::updateFromHSV);

    const auto layout = new QVBoxLayout(this);

    layout->addWidget(rRect);
    layout->addWidget(gRect);
    layout->addWidget(bRect);

    layout->addWidget(hRect);
    layout->addWidget(hsvSatRect);
    layout->addWidget(vRect);

    layout->addWidget(hslSatRect);
    layout->addWidget(lRect);

    layout->addWidget(aRect);

    const QString pickIcon = "toolbarButtons/pickUnchecked.png";
    const auto pickingButton = new ActionButton(pickIcon, "", this);
    connect(pickingButton, &ActionButton::released,
            this, &PaintColorWidget::startColorPicking);

    const auto colorPickLay = new QHBoxLayout();
    colorPickLay->addWidget(mColorLabel);
    colorPickLay->addWidget(pickingButton);
    layout->addLayout(colorPickLay);

    setLayout(layout);
}

void PaintColorWidget::setDisplayedColor(const QColor& color) {
    rRect->setDisplayedValue(color.redF());
    gRect->setDisplayedValue(color.greenF());
    bRect->setDisplayedValue(color.blueF());

    hRect->setDisplayedValue(qBound(0., color.hueF(), 1.));
    hsvSatRect->setDisplayedValue(color.hsvSaturationF());
    vRect->setDisplayedValue(color.valueF());

    hslSatRect->setDisplayedValue(color.hslSaturationF());
    lRect->setDisplayedValue(color.lightnessF());

    aRect->setDisplayedValue(color.alphaF());

    rRect->setColor(color);
    gRect->setColor(color);
    bRect->setColor(color);

    hRect->setColor(color);
    hsvSatRect->setColor(color);
    vRect->setColor(color);

    hslSatRect->setColor(color);
    lRect->setColor(color);

    aRect->setColor(color);

    mColorLabel->setColor(color);
    mColorLabel->setAlpha(color.alphaF());
}

void PaintColorWidget::startColorPicking() {
    const auto parent = MainWindow::sGetInstance();
    const auto screen = parent->windowHandle()->screen();
    const auto wid = new ColorPickingWidget(screen, parent);
    connect(wid, &ColorPickingWidget::colorSelected,
            [this](const QColor & color) {
        setColor(color);
    });
}

void PaintColorWidget::setColor(const QColor& color) {
    setDisplayedColor(color);
    emit colorChanged(color);
}

void PaintColorWidget::updateFromRGB() {
    const qreal red = rRect->value();
    const qreal green = gRect->value();
    const qreal blue = bRect->value();
    const qreal alpha = aRect->value();

    setColor(QColor::fromRgbF(red, green, blue, alpha));
}

void PaintColorWidget::updateFromHSV() {
    const qreal hue = hRect->value();
    const qreal saturation = hsvSatRect->value();
    const qreal value = vRect->value();
    const qreal alpha = aRect->value();

    setColor(QColor::fromHsvF(hue, saturation, value, alpha));
}

void PaintColorWidget::updateFromHSL() {
    const qreal hue = hRect->value();
    const qreal saturation = hslSatRect->value();
    const qreal lightness = lRect->value();
    const qreal alpha = aRect->value();

    setColor(QColor::fromHslF(hue, saturation, lightness, alpha));
}
