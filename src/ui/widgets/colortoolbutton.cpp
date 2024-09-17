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
#include "themesupport.h"

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
        adjustSize(); // force resize since it's a popup
        QWidget::showEvent(e);
    }
};

class AScrollArea : public QScrollArea
{
public:
    AScrollArea(QWidget *parent = nullptr) : QScrollArea(parent)
    {
        setObjectName("NoMarginVerticalScrollBar");
        setPalette(ThemeSupport::getDarkerPalette());
        setAutoFillBackground(true);
        setWidgetResizable(true);
        setContentsMargins(0, 0, 0, 0);
        setBackgroundRole(QPalette::Window);
        setFrameShadow(QFrame::Plain);
        setFrameShape(QFrame::NoFrame);
    }

protected:
    void showEvent(QShowEvent *e) override
    {
        widget()->update();
        widget()->adjustSize();
        setWidget(takeWidget());
        QWidget::showEvent(e);
    }
};

ColorToolButton::ColorToolButton(Document& document,
                                 QWidget *parent,
                                 const bool fillOnly,
                                 const bool strokeOnly,
                                 const bool flatOnly)
    : ToolButton(parent)
    , mIsFillOnly(fillOnly)
    , mIsStrokeOnly(strokeOnly)
    , mIsFlatOnly(flatOnly)
    , mColor(Qt::transparent)
    , mColorLabel(nullptr)
    , mBackgroundWidget(nullptr)
    , mFillStrokeWidget(nullptr)
    , mColorAct(nullptr)
    , mDocument(document)
{
    setAutoPopup(false); // TODO: add to settings (auto/click popup)
    setPopupMode(ToolButtonPopupMode::InstantPopup);

    const auto pop = new AWidget(this);
    const auto popLay = new QVBoxLayout(pop);

    pop->setContentsMargins(10, 10, 10, 10);
    pop->setMinimumSize({300, mIsFlatOnly ? 200 : 260});
    popLay->setMargin(0);

    const auto area = new AScrollArea(this);
    popLay->addWidget(area);

    if (!mIsFlatOnly) {
        mFillStrokeWidget = new FillStrokeSettingsWidget(mDocument,
                                                         this,
                                                         true,
                                                         mIsFillOnly,
                                                         mIsStrokeOnly);
        mFillStrokeWidget->setContentsMargins(0, 0, 0, 0);
        area->setWidget(mFillStrokeWidget);
    } else {
        mBackgroundWidget = new ColorSettingsWidget(this);
        mBackgroundWidget->setColorModeVisible(false);
        area->setWidget(mBackgroundWidget);
    }

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

void ColorToolButton::setCurrentBox(BoundingBox *target)
{
    mFillStrokeWidget->setCurrentBox(target);
}

void ColorToolButton::setColorFillTarget(FillSettingsAnimator * const target)
{
    if (mIsFlatOnly) { return; }
    mColorFillTarget.assign(target);
    setColorTarget(target ? target->getColorAnimator() : nullptr);
}

void ColorToolButton::setColorStrokeTarget(OutlineSettingsAnimator * const target)
{
    if (mIsFlatOnly) { return; }
    mColorStrokeTarget.assign(target);
    setColorTarget(target ? target->getColorAnimator() : nullptr);
}

void ColorToolButton::setColorTarget(ColorAnimator * const target)
{
    if (mBackgroundWidget) { mBackgroundWidget->setTarget(target); }
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
}

QColor ColorToolButton::color() const
{
    if (mColorTarget) { return mColorTarget->getColor(); }
    return mColor;
}
