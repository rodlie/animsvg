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

#include "widgets/colortoolbutton.h"
#include "Animators/coloranimator.h"
#include "Animators/outlinesettingsanimator.h"
#include "Animators/paintsettingsanimator.h"
#include "Private/document.h"
#include "colorsetting.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

using namespace Friction::Ui;

class AWidget : public QWidget
{
public:
    AWidget(QWidget *parent = nullptr) : QWidget(parent) {}

protected:
    void showEvent(QShowEvent *e) override
    {
        adjustSize(); // force resize since we live in a popup
        QWidget::showEvent(e);
    }
};

class AScrollArea : public QScrollArea
{
public:
    AScrollArea(QWidget *parent = nullptr) : QScrollArea(parent) {}

protected:
    void showEvent(QShowEvent *e) override
    {
        setWidget(takeWidget()); // yeah, I know ... only way to force adjustSize(?)
        QWidget::showEvent(e);
    }
};

ColorToolButton::ColorToolButton(QWidget * const parent,
                                 const bool flatOnly)
    : ToolButton(parent)
    , mIsFlatOnly(flatOnly)
    , mColor(Qt::transparent)
    , mColorLabel(nullptr)
    , mColorNoneButton(nullptr)
    , mColorFlatButton(nullptr)
    , mColorGradientButton(nullptr)
    , mColorWidget(nullptr)
    , mGradientWidget(nullptr)
    , mColorAct(nullptr)
{
    setAutoPopup(false); // TODO: add to settings (auto/click popup)
    setPopupMode(ToolButtonPopupMode::InstantPopup);

    const auto pop = new AWidget(this);
    const auto popLay = new QVBoxLayout(pop);

    pop->setContentsMargins(10, 10, 10, 10);
    pop->setMinimumWidth(250);
    popLay->setMargin(0);

    const auto popInner = new QWidget(this);
    const auto popInnerLay = new QVBoxLayout(popInner);

    popInner->setContentsMargins(0, 0, 0, 0);
    popInnerLay->setMargin(0);

    const auto area = new AScrollArea(this);
    area->setWidgetResizable(true);
    area->setContentsMargins(0, 0, 0, 0);
    area->setFrameShape(QFrame::NoFrame);
    area->setWidget(popInner);

    mColorWidget = new ColorSettingsWidget(this);
    mColorWidget->setColorModeVisible(false);

    if (!mIsFlatOnly) {
        mColorNoneButton = new QPushButton(QIcon::fromTheme("fill_none_2"),
                                           tr("None"), this);
        mColorFlatButton = new QPushButton(QIcon::fromTheme("fill_flat_2"),
                                           tr("Flat"), this);
        mColorGradientButton = new QPushButton(QIcon::fromTheme("fill_gradient_2"),
                                               tr("Gradient"), this);

        connect(mColorNoneButton, &QPushButton::released,
                this, [this]() { setColorType(PaintType::NOPAINT); });
        connect(mColorFlatButton, &QPushButton::released,
                this, [this]() { setColorType(PaintType::FLATPAINT); });
        connect(mColorGradientButton, &QPushButton::released,
                this, [this]() { setColorType(PaintType::GRADIENTPAINT); });

        mColorNoneButton->setCheckable(true);
        mColorFlatButton->setCheckable(true);
        mColorGradientButton->setCheckable(true);

        mGradientWidget = new GradientWidget(this);

        const auto buttWid = new QWidget(this);
        const auto buttLay = new QHBoxLayout(buttWid);

        buttWid->setContentsMargins(0, 0, 0, 0);
        buttLay->setMargin(0);

        buttLay->addWidget(mColorNoneButton);
        buttLay->addWidget(mColorFlatButton);
        buttLay->addWidget(mColorGradientButton);

        popLay->addWidget(buttWid);
        popInnerLay->addWidget(mGradientWidget);
    }
    popInnerLay->addWidget(mColorWidget);
    popLay->addWidget(area);

    updateColorTypeWidgets(mIsFlatOnly ? PaintType::FLATPAINT : PaintType::NOPAINT);

    mColorAct = new QWidgetAction(this);
    mColorAct->setDefaultWidget(pop);

    mColorLabel = new ColorLabel(this, false);

    const auto mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(0, 0, 0, 0);
    mainLay->setMargin(0);
    mainLay->addWidget(mColorLabel);

    addAction(mColorAct);
    updateColor();
}

ColorToolButton::ColorToolButton(ColorAnimator * const colorTarget,
                                 QWidget * const parent)
    : ColorToolButton(parent)
{
    setColorTarget(colorTarget);
}

void ColorToolButton::setColorFillTarget(FillSettingsAnimator * const target)
{
    if (mIsFlatOnly) { return; }
    mColorFillTarget.assign(target);
    mGradientWidget->setCurrentGradient(target ? target->getGradient() : nullptr);
    setColorTarget(target ?
                       (target->getPaintType() == PaintType::GRADIENTPAINT ?
                            mGradientWidget->getColorAnimator() : target->getColorAnimator())
                          : nullptr);
}

void ColorToolButton::setColorStrokeTarget(OutlineSettingsAnimator * const target)
{
    if (mIsFlatOnly) { return; }
    mColorStrokeTarget.assign(target);
    mGradientWidget->setCurrentGradient(target ? target->getGradient() : nullptr);
    setColorTarget(target ?
                       (target->getPaintType() == PaintType::GRADIENTPAINT ?
                                 mGradientWidget->getColorAnimator() : target->getColorAnimator())
                          : nullptr);
}

void ColorToolButton::setColorTarget(ColorAnimator * const target)
{
    mColorWidget->setTarget(target);
    mColorTarget.assign(target);
    if (target) {
        mColorTarget << connect(target->getVal1Animator(),
                                &QrealAnimator::effectiveValueChanged,
                                this, qOverload<>(&ColorToolButton::updateColor));
        mColorTarget << connect(target->getVal2Animator(),
                                &QrealAnimator::effectiveValueChanged,
                                this, qOverload<>(&ColorToolButton::updateColor));
        mColorTarget << connect(target->getVal3Animator(),
                                &QrealAnimator::effectiveValueChanged,
                                this, qOverload<>(&ColorToolButton::updateColor));
    }
    updateColor();
}

void ColorToolButton::setColorType(const PaintType &type)
{
    if (mIsFlatOnly) { return; }
    if (mColorFillTarget) {
        mColorFillTarget->setPaintType(type);
        Document::sInstance->actionFinished();
        setColorTarget(type == PaintType::FLATPAINT ?
                           mColorFillTarget->getColorAnimator() :
                           type == PaintType::GRADIENTPAINT ?
                               mGradientWidget->getColorAnimator() : nullptr);
    } else if (mColorStrokeTarget) {
        mColorStrokeTarget->setPaintType(type);
        Document::sInstance->actionFinished();
        setColorTarget(type == PaintType::FLATPAINT ?
                           mColorStrokeTarget->getColorAnimator() :
                           type == PaintType::GRADIENTPAINT ?
                               mGradientWidget->getColorAnimator() : nullptr);
    }
    updateColorTypeWidgets(type);
}

void ColorToolButton::updateColorTypeWidgets(const PaintType &type)
{
    if (mIsFlatOnly) { return; }
    mColorNoneButton->setChecked(type == PaintType::NOPAINT);
    mColorFlatButton->setChecked(type == PaintType::FLATPAINT);
    mColorGradientButton->setChecked(type == PaintType::GRADIENTPAINT);
    mGradientWidget->setEnabled(type == PaintType::GRADIENTPAINT);
    mGradientWidget->setVisible(type == PaintType::GRADIENTPAINT);
    mColorWidget->setEnabled(type == PaintType::FLATPAINT ||
                             type == PaintType::GRADIENTPAINT);

    if (type != PaintType::GRADIENTPAINT) { return ;}
    if (mColorFillTarget) {
        mGradientWidget->setCurrentGradient(mColorFillTarget->getGradient());
    } else if (mColorStrokeTarget) {
        mGradientWidget->setCurrentGradient(mColorStrokeTarget->getGradient());
    }
}

void ColorToolButton::updateColor()
{
    PaintType type = mIsFlatOnly ? PaintType::FLATPAINT : PaintType::NOPAINT;
    if (mColorFillTarget || mColorStrokeTarget) {
        if (mColorFillTarget) {
            type = mColorFillTarget->getPaintType();
            mColorLabel->setGradient(type == PaintType::GRADIENTPAINT ?
                                         mColorFillTarget->getGradient() : nullptr);
        } else if (mColorStrokeTarget) {
            type = mColorStrokeTarget->getPaintType();
            mColorLabel->setGradient(type == PaintType::GRADIENTPAINT ?
                                         mColorStrokeTarget->getGradient() : nullptr);
        }
    } else {
        if (!mIsFlatOnly) { mColorLabel->setGradient(nullptr); }
    }

    const QColor color = mColorTarget ? mColorTarget->getColor() : mColor;

    mColor = type == PaintType::NOPAINT ? Qt::transparent : color;
    mColorLabel->setColor(mColor);
    mColorLabel->setAlpha(mColor.alphaF());
    updateColorTypeWidgets(type);
}

QColor ColorToolButton::color() const
{
    if (mColorTarget) { return mColorTarget->getColor(); }
    return mColor;
}
