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

#include "colortoolbar.h"
#include "GUI/global.h"

using namespace Friction::Ui;

ColorToolBar::ColorToolBar(QWidget *parent)
    : QToolBar(parent)
    , mColorFill(nullptr)
    , mColorStroke(nullptr)
    , mColorBackground(nullptr)
    , mColorFillAct(nullptr)
    , mColorStrokeAct(nullptr)
    , mColorBackgroundAct(nullptr)
{
    setWindowTitle(tr("Color Toolbar"));
    setObjectName("ColorToolBar");
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setEnabled(false);
    setupWidgets();

    eSizesUI::widget.add(this, [this](const int size) {
        this->setIconSize({size, size});
    });

    connect(this, &QToolBar::orientationChanged,
            this, &ColorToolBar::adjustWidgets);
}

void ColorToolBar::setCurrentCanvas(Canvas * const target)
{
    const bool enabled = target ? true : false;
    setEnabled(enabled);
    mColorBackground->setEnabled(enabled);
    mColorBackgroundAct->setEnabled(enabled);

    mColorBackground->setColorTarget(target ? target->getBgColorAnimator() : nullptr);

    mCanvas.assign(target);
    if (target) {
        mCanvas << connect(target, &Canvas::requestUpdate,
                           this, [this]() {
            mColorFill->updateColor();
            mColorStroke->updateColor();
            mColorBackground->updateColor();
        });
    }
}

void ColorToolBar::setCurrentBox(BoundingBox *target)
{
    const bool enabled = target ? true : false;
    mColorFillAct->setEnabled(enabled);
    mColorStrokeAct->setEnabled(enabled);
    mColorFill->setEnabled(enabled);
    mColorStroke->setEnabled(enabled);

    if (!target) {
        mColorFill->setColorTarget(nullptr);
        mColorStroke->setColorTarget(nullptr);
    } else {
        const auto fillSettings = target->getFillSettings();
        mColorFill->setColorTarget(fillSettings ?
                                       fillSettings->getColorAnimator() :
                                       nullptr);
        const auto strokeSettings = target->getStrokeSettings();
        mColorStroke->setColorTarget(strokeSettings ?
                                         strokeSettings->getColorAnimator() :
                                         nullptr);
    }
}

void ColorToolBar::setupWidgets()
{
    mColorFill = new ColorToolButton(Qt::black, this);
    mColorStroke = new ColorToolButton(Qt::black, this);
    mColorBackground = new ColorToolButton(Qt::black, this);

    mColorFillAct = new QAction(QIcon::fromTheme("color"),
                                tr("Fill"), this);
    mColorStrokeAct = new QAction(QIcon::fromTheme("color"),
                                  tr("Stroke"), this);
    mColorBackgroundAct = new QAction(QIcon::fromTheme("color"),
                                      tr("Background"), this);

    mColorFillAct->setToolTip(tr("Fill Color"));
    mColorStrokeAct->setToolTip(tr("Stroke Color"));
    mColorBackgroundAct->setToolTip(tr("Background Color"));

    eSizesUI::widget.add(mColorFill, [this](const int size) {
        const int wid = orientation() == Qt::Horizontal ? size / 2 : size * 3;
        mColorFill->setFixedHeight(wid);
        mColorStroke->setFixedHeight(wid);
        mColorBackground->setFixedHeight(wid);
    });

    addAction(mColorFillAct);
    addWidget(mColorFill);
    addSeparator();
    addAction(mColorStrokeAct);
    addWidget(mColorStroke);
    addSeparator();
    addAction(mColorBackgroundAct);
    addWidget(mColorBackground);

    adjustWidgets();
}

void ColorToolBar::adjustWidgets()
{
    const bool horiz = orientation() == Qt::Horizontal;
    const int min = horiz ? eSizesUI::widget * 3 : eSizesUI::widget / 2;
    const int wid = horiz ? eSizesUI::widget / 2 : eSizesUI::widget * 3;

    setToolButtonStyle(horiz ?
                           Qt::ToolButtonTextBesideIcon :
                           Qt::ToolButtonTextUnderIcon);

    mColorFill->setMinimumWidth(min);
    mColorStroke->setMinimumWidth(min);
    mColorBackground->setMinimumWidth(min);

    mColorFill->setFixedHeight(wid);
    mColorStroke->setFixedHeight(wid);
    mColorBackground->setFixedHeight(wid);

    mColorFillAct->setText(horiz ? tr("Fill") : tr("F"));
    mColorStrokeAct->setText(horiz ? tr("Stroke") : tr("S"));
    mColorBackgroundAct->setText(horiz ? tr("Background") : tr("B"));
}
