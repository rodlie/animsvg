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

#include "viewertoolboxcanvas.h"
#include "GUI/global.h"
#include "Private/document.h"
#include "widgets/editablecombobox.h"

#include <QLabel>
#include <QLineEdit>
#include <QDebug>

using namespace Friction::Ui;

ViewerToolBoxCanvas::ViewerToolBoxCanvas(QWidget *parent)
    : QToolBar(parent)
    , mSpinWidth(nullptr)
    , mSpinHeight(nullptr)
    , mComboResolution(nullptr)
    , mColorFill(nullptr)
    , mColorStroke(nullptr)
    , mColorBackground(nullptr)
    , mSpinWidthAct(nullptr)
    , mSpinHeightAct(nullptr)
    , mComboResolutionAct(nullptr)
    , mColorFillAct(nullptr)
    , mColorStrokeAct(nullptr)
    , mColorBackgroundAct(nullptr)
{
    setup();
}

void ViewerToolBoxCanvas::setCurrentCanvas(Canvas * const target)
{
    mColorBackground->setColorTarget(target ? target->getBgColorAnimator() : nullptr);

    mConCanvas.assign(target);
    if (target) {
        mConCanvas << connect(mComboResolution, &QComboBox::currentTextChanged,
                              this, [this, target](const QString &text) {
            setResolution(text, target);
        });
        mConCanvas << connect(mSpinWidth,
                              QOverload<int>::of(&QSpinBox::valueChanged),
                              this, [this, target]() {
            setDimension({mSpinWidth->value(), mSpinHeight->value()},
                          target);
        });
        mConCanvas << connect(mSpinHeight,
                              QOverload<int>::of(&QSpinBox::valueChanged),
                              this, [this, target]() {
            setDimension({mSpinWidth->value(), mSpinHeight->value()},
                         target);
        });
        mConCanvas << connect(target, &Canvas::dimensionsChanged,
                              this, [this](int width, int height) {
            updateDimension({width, height});
        });
        mConCanvas << connect(target, &Canvas::requestUpdate,
                              this, [this]() {
            mColorFill->updateColor();
            mColorStroke->updateColor();
            mColorBackground->updateColor();
        });
    }
    updateWidgets(target);
}

void ViewerToolBoxCanvas::setCurrentBox(BoundingBox *target)
{
    if (!target) {
        mColorFill->setColorTarget(nullptr);
        mColorStroke->setColorTarget(nullptr);
    } else {
        const auto fillSettings = target->getFillSettings();
        mColorFill->setColorTarget(fillSettings ? fillSettings->getColorAnimator() : nullptr);
        const auto strokeSettings = target->getStrokeSettings();
        mColorStroke->setColorTarget(strokeSettings ? strokeSettings->getColorAnimator() : nullptr);
    }
}

QComboBox *ViewerToolBoxCanvas::getResolutionComboBox()
{
    return mComboResolution;
}

void ViewerToolBoxCanvas::setup()
{
    eSizesUI::widget.add(this, [this](const int size) {
        this->setIconSize({size, size});
    });

    setEnabled(false);
    setObjectName("DarkToolBar");

    setupColors();
    addSpacer();
    setupDimensions();
    setupResolution();
}

void ViewerToolBoxCanvas::setupColors()
{
    mColorFill = new ColorToolButton(Qt::black, this);
    mColorStroke = new ColorToolButton(Qt::black, this);
    mColorBackground = new ColorToolButton(Qt::black, this);

    mColorFill->setMinimumWidth(75);
    mColorStroke->setMinimumWidth(75);
    mColorBackground->setMinimumWidth(75);

    eSizesUI::widget.add(mColorFill, [this](const int size) {
        mColorFill->setFixedHeight(size / 2);
        mColorStroke->setFixedHeight(size / 2);
        mColorBackground->setFixedHeight(size / 2);
    });

    addWidget(new QLabel(tr("Fill"), this));
    addSeparator();

    mColorFillAct = addWidget(mColorFill);

    addSeparator();
    addWidget(new QLabel(tr("Stroke"), this));
    addSeparator();

    mColorStrokeAct = addWidget(mColorStroke);

    addSeparator();
    addWidget(new QLabel(tr("Background"), this));
    addSeparator();

    mColorBackgroundAct = addWidget(mColorBackground);
}

void ViewerToolBoxCanvas::setupDimensions()
{
    mSpinWidth = new QSpinBox(this);
    mSpinWidth->setFocusPolicy(Qt::ClickFocus);
    mSpinWidth->setObjectName("ComboSpinBox");
    mSpinWidth->setMinimum(1);
    mSpinWidth->setMaximum(99999);
    mSpinWidth->setKeyboardTracking(false);

    mSpinHeight = new QSpinBox(this);
    mSpinWidth->setFocusPolicy(Qt::ClickFocus);
    mSpinHeight->setObjectName("ComboSpinBox");
    mSpinHeight->setMinimum(1);
    mSpinHeight->setMaximum(99999);
    mSpinHeight->setKeyboardTracking(false);

    addWidget(new QLabel(tr("Width"), this));
    addSeparator();

    mSpinWidthAct = addWidget(mSpinWidth);

    addSeparator();
    addWidget(new QLabel(tr("Height"), this));
    addSeparator();

    mSpinHeightAct = addWidget(mSpinHeight);
}

void ViewerToolBoxCanvas::setupResolution()
{
    mComboResolution = new EditableComboBox(this);
    mComboResolution->setMinimumWidth(100);
    mComboResolution->setFocusPolicy(Qt::ClickFocus);
    mComboResolution->addItem("100 %");
    mComboResolution->addItem("75 %");
    mComboResolution->addItem("50 %");
    mComboResolution->addItem("25 %");
    mComboResolution->lineEdit()->setInputMask("D00 %");
    mComboResolution->setCurrentText("100 %");
    mComboResolution->setInsertPolicy(QComboBox::NoInsert);
    mComboResolution->setSizePolicy(QSizePolicy::Maximum,
                                    QSizePolicy::Preferred);

    addSeparator();
    addWidget(new QLabel(tr("Resolution"), this));
    addSeparator();

    addWidget(mComboResolution);
}

void ViewerToolBoxCanvas::addSpacer()
{
    const auto space = new QWidget(this);
    space->setSizePolicy(QSizePolicy::Expanding,
                         QSizePolicy::Minimum);
    addWidget(space);
}

void ViewerToolBoxCanvas::updateWidgets(Canvas * const target)
{
    if (!target) {
        setEnabled(false);
        return;
    }
    setEnabled(true);

    updateDimension(target->getCanvasSize());

    mComboResolution->blockSignals(true);
    mComboResolution->setCurrentText(QString("%1 %").arg(target->getResolution() * 100));
    mComboResolution->blockSignals(false);
}

void ViewerToolBoxCanvas::updateDimension(const QSize dim)
{
    mSpinWidth->blockSignals(true);
    mSpinWidth->setValue(dim.width());
    mSpinWidth->blockSignals(false);

    mSpinHeight->blockSignals(true);
    mSpinHeight->setValue(dim.height());
    mSpinHeight->blockSignals(false);
}

void ViewerToolBoxCanvas::setResolution(QString text,
                                        Canvas * const target)
{
    if (!target) { return; }

    const qreal percent = clamp(text.remove(" %").toDouble(), 1, 200) / 100;

    target->setResolution(percent);
    Document::sInstance->actionFinished();
}

void ViewerToolBoxCanvas::setDimension(const QSize dim,
                                       Canvas * const target)
{
    if (!target) { return; }

    target->setCanvasSize(dim.width(),
                          dim.height());
    Document::sInstance->actionFinished();
}
