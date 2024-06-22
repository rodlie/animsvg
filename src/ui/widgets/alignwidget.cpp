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

#include "alignwidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>

#include "GUI/global.h"
#include "Private/esettings.h"

AlignWidget::AlignWidget(QWidget* const parent)
    : QWidget(parent)
    , mAlignPivot(nullptr)
    , mRelativeTo(nullptr)
{
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, AppSupport::getThemeBaseColor());
    setAutoFillBackground(true);
    setPalette(pal);

    const auto mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    const auto combosLay = new QHBoxLayout;
    mainLayout->addLayout(combosLay);

    combosLay->addWidget(new QLabel(tr("Align")));
    mAlignPivot = new QComboBox(this);
    mAlignPivot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mAlignPivot->setFocusPolicy(Qt::NoFocus);
    mAlignPivot->addItem(tr("Geometry"));
    mAlignPivot->addItem(tr("Pivot"));
    combosLay->addWidget(mAlignPivot);

    combosLay->addWidget(new QLabel(tr("Relative to")));
    mRelativeTo = new QComboBox(this);
    mRelativeTo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mRelativeTo->setFocusPolicy(Qt::NoFocus);
    mRelativeTo->addItem(tr("Scene"));
    mRelativeTo->addItem(tr("Last Selected"));
    combosLay->addWidget(mRelativeTo);

    const auto buttonsLay = new QHBoxLayout;
    mainLayout->addLayout(buttonsLay);
    mainLayout->addStretch();

    const auto leftButton = new QPushButton(this);
    leftButton->setFocusPolicy(Qt::NoFocus);
    leftButton->setIcon(QIcon::fromTheme("pivot-align-left"));
    leftButton->setToolTip(tr("Align Left"));
    connect(leftButton, &QPushButton::pressed, this, [this]() {
        triggerAlign(Qt::AlignLeft);
    });
    buttonsLay->addWidget(leftButton);

    const auto hCenterButton = new QPushButton(this);
    hCenterButton->setFocusPolicy(Qt::NoFocus);
    hCenterButton->setIcon(QIcon::fromTheme("pivot-align-hcenter"));
    hCenterButton->setToolTip(tr("Align Horizontal Center"));
    connect(hCenterButton, &QPushButton::pressed, this, [this]() {
        triggerAlign(Qt::AlignHCenter);
    });
    buttonsLay->addWidget(hCenterButton);

    const auto rightButton = new QPushButton(this);
    rightButton->setFocusPolicy(Qt::NoFocus);
    rightButton->setIcon(QIcon::fromTheme("pivot-align-right"));
    rightButton->setToolTip(tr("Align Right"));
    connect(rightButton, &QPushButton::pressed, this, [this]() {
        triggerAlign(Qt::AlignRight);
    });
    buttonsLay->addWidget(rightButton);

    const auto topButton = new QPushButton(this);
    topButton->setFocusPolicy(Qt::NoFocus);
    topButton->setIcon(QIcon::fromTheme("pivot-align-top"));
    topButton->setToolTip(tr("Align Top"));
    connect(topButton, &QPushButton::pressed, this, [this]() {
        triggerAlign(Qt::AlignTop);
    });
    buttonsLay->addWidget(topButton);

    const auto vCenterButton = new QPushButton(this);
    vCenterButton->setFocusPolicy(Qt::NoFocus);
    vCenterButton->setIcon(QIcon::fromTheme("pivot-align-vcenter"));
    vCenterButton->setToolTip(tr("Align Vertical Center"));
    connect(vCenterButton, &QPushButton::pressed, this, [this]() {
        triggerAlign(Qt::AlignVCenter);
    });
    buttonsLay->addWidget(vCenterButton);

    const auto bottomButton = new QPushButton(this);
    bottomButton->setFocusPolicy(Qt::NoFocus);
    bottomButton->setIcon(QIcon::fromTheme("pivot-align-bottom"));
    bottomButton->setToolTip(tr("Align Bottom"));
    connect(bottomButton, &QPushButton::pressed, this, [this]() {
        triggerAlign(Qt::AlignBottom);
    });
    buttonsLay->addWidget(bottomButton);

    //if (eSettings::instance().fCurrentInterfaceDPI != 1.) {
        int buttonSize = eSizesUI::widget;
        leftButton->setIconSize(QSize(buttonSize, buttonSize));
        hCenterButton->setIconSize(QSize(buttonSize, buttonSize));
        rightButton->setIconSize(QSize(buttonSize, buttonSize));
        topButton->setIconSize(QSize(buttonSize, buttonSize));
        vCenterButton->setIconSize(QSize(buttonSize, buttonSize));
        bottomButton->setIconSize(QSize(buttonSize, buttonSize));
    //}
}

void AlignWidget::triggerAlign(const Qt::Alignment align)
{
    const auto alignPivot = static_cast<AlignPivot>(mAlignPivot->currentIndex());
    const auto relativeTo = static_cast<AlignRelativeTo>(mRelativeTo->currentIndex());
    emit alignTriggered(align, alignPivot, relativeTo);
}
