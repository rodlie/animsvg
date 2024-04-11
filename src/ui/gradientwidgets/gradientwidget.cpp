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

#include "gradientwidget.h"
#include <QMouseEvent>
#include <QMenu>
#include <QAction>

#include "gradientwidgets/displayedgradientswidget.h"
#include "Animators/gradient.h"

GradientWidget::GradientWidget(QWidget * const parent)
    : QWidget(parent)
    , mGradientsListWidget(nullptr)
    , mCurrentGradientWidget(nullptr)
    , mReordering(false)
    , mFirstMove(false)
    , mNumberVisibleGradients(6)
    , mHalfHeight(64)
    , mQuorterHeight(32)
    , mCurrentGradient(nullptr)
    , mCurrentColor(nullptr)
    , mCurrentColorId(0)
    , mCenterGradientId(1)
{
    setContentsMargins(0, 0, 0, 0);

    const auto mMainLayout = new QVBoxLayout(this);
    mMainLayout->setMargin(0);

    mGradientsListWidget = new GradientsListWidget(this);
    mCurrentGradientWidget = new CurrentGradientWidget(this);

    mMainLayout->addWidget(mGradientsListWidget);
    mMainLayout->addWidget(mCurrentGradientWidget);

    /*eSizesUI::widget.add(this, [this](const int size) {
        const qreal mult = (3 + mNumberVisibleGradients + 0.5);
        setFixedHeight(qRound(mult*size));
    });*/

    const auto list = mGradientsListWidget->getList();
    connect(list, &DisplayedGradientsWidget::triggered,
            this, &GradientWidget::setCurrentGradient);
    connect(this, &GradientWidget::selectionChanged,
            list, &DisplayedGradientsWidget::setSelectedGradient);
    connect(list, &DisplayedGradientsWidget::triggered,
            this, &GradientWidget::triggered);

    connect(this, &GradientWidget::selectionChanged,
            mCurrentGradientWidget, &CurrentGradientWidget::setCurrentGradient);
    connect(mCurrentGradientWidget, &CurrentGradientWidget::selectedColorChanged,
            this, &GradientWidget::selectedColorChanged);
}

void GradientWidget::clearAll()
{
    mCurrentGradient = nullptr;
    mCenterGradientId = 1;
    mCurrentColorId = 0;
}

void GradientWidget::setCurrentGradient(Gradient *gradient)
{
    if (mCurrentGradient == gradient) { return; }
    mCurrentGradient = gradient;
    emit selectionChanged(gradient);
}

Gradient *GradientWidget::getCurrentGradient()
{
    return mCurrentGradient;
}

ColorAnimator *GradientWidget::getColorAnimator()
{
    return mCurrentGradientWidget->getColorAnimator();
}

void GradientWidget::showEvent(QShowEvent *e)
{
    Q_UNUSED(e)
    mCurrentGradientWidget->update();
    return QWidget::showEvent(e);
}
