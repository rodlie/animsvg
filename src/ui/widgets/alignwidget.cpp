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

#include "alignwidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStandardItemModel>

#include "GUI/global.h"
#include "Private/esettings.h"
#include "themesupport.h"

#define INDEX_ALIGN_GEOMETRY 0
#define INDEX_ALIGN_GEOMETRY_PIVOT 1
#define INDEX_ALIGN_PIVOT 2

#define INDEX_REL_SCENE 0
#define INDEX_REL_LAST_SELECTED 1
#define INDEX_REL_LAST_SELECTED_PIVOT 2
#define INDEX_REL_BOUNDINGBOX 3

AlignWidget::AlignWidget(QWidget* const parent)
    : QWidget(parent)
    , mAlignPivot(nullptr)
    , mRelativeTo(nullptr)
{
    const auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    setLayout(mainLayout);

    const auto combosLay = new QHBoxLayout;
    mainLayout->addLayout(combosLay);

    combosLay->addWidget(new QLabel(tr("Align"), this));
    mAlignPivot = new QComboBox(this);
    mAlignPivot->setMinimumWidth(20);
    mAlignPivot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mAlignPivot->setFocusPolicy(Qt::NoFocus);
    mAlignPivot->addItem(tr("Geometry")); // INDEX_ALIGN_GEOMETRY
    mAlignPivot->addItem(tr("Geometry by Pivot")); // INDEX_ALIGN_GEOMETRY_PIVOT
    mAlignPivot->addItem(tr("Pivot")); // INDEX_ALIGN_PIVOT
    combosLay->addWidget(mAlignPivot);

    combosLay->addWidget(new QLabel(tr("To"), this));
    mRelativeTo = new QComboBox(this);
    mRelativeTo->setMinimumWidth(20);
    mRelativeTo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mRelativeTo->setFocusPolicy(Qt::NoFocus);
    mRelativeTo->addItem(tr("Scene")); // INDEX_REL_SCENE
    mRelativeTo->addItem(tr("Last Selected")); // INDEX_REL_LAST_SELECTED
    mRelativeTo->addItem(tr("Last Selected Pivot")); // INDEX_REL_LAST_SELECTED_PIVOT
    mRelativeTo->addItem(tr("Bounding Box")); // INDEX_REL_BOUNDINGBOX
    combosLay->addWidget(mRelativeTo);

    setComboBoxItemState(mRelativeTo, INDEX_REL_LAST_SELECTED_PIVOT, false);
    setComboBoxItemState(mRelativeTo, INDEX_REL_BOUNDINGBOX, false);

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

    connect(mAlignPivot,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        setComboBoxItemState(mRelativeTo,
                             INDEX_REL_LAST_SELECTED_PIVOT,
                             index == INDEX_ALIGN_PIVOT);
        setComboBoxItemState(mRelativeTo,
                             INDEX_REL_BOUNDINGBOX,
                             index == INDEX_ALIGN_PIVOT);
        if (index == INDEX_ALIGN_PIVOT) { mRelativeTo->setCurrentIndex(INDEX_REL_BOUNDINGBOX); }
        else { mRelativeTo->setCurrentIndex(INDEX_REL_SCENE); }
    });

    connect(mRelativeTo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [leftButton,
                   rightButton,
                   topButton,
                   bottomButton](int index) {
        leftButton->setEnabled(index != INDEX_REL_LAST_SELECTED_PIVOT);
        rightButton->setEnabled(index != INDEX_REL_LAST_SELECTED_PIVOT);
        topButton->setEnabled(index != INDEX_REL_LAST_SELECTED_PIVOT);
        bottomButton->setEnabled(index != INDEX_REL_LAST_SELECTED_PIVOT);
    });

    eSizesUI::widget.add(leftButton, [leftButton,
                                      hCenterButton,
                                      rightButton,
                                      topButton,
                                      vCenterButton,
                                      bottomButton](const int size) {
        Q_UNUSED(size)
        leftButton->setFixedHeight(eSizesUI::button);
        hCenterButton->setFixedHeight(eSizesUI::button);
        rightButton->setFixedHeight(eSizesUI::button);
        topButton->setFixedHeight(eSizesUI::button);
        vCenterButton->setFixedHeight(eSizesUI::button);
        bottomButton->setFixedHeight(eSizesUI::button);
    });
}

void AlignWidget::triggerAlign(const Qt::Alignment align)
{
    const auto alignPivot = static_cast<AlignPivot>(mAlignPivot->currentIndex());
    const auto relativeTo = static_cast<AlignRelativeTo>(mRelativeTo->currentIndex());
    emit alignTriggered(align, alignPivot, relativeTo);
}

void AlignWidget::setComboBoxItemState(QComboBox *box,
                                       int index,
                                       bool enabled)
{
    auto model = qobject_cast<QStandardItemModel*>(box->model());
    if (!model) { return; }

    if (index >= box->count() || index < 0) { return; }

    auto item = model->item(index);
    if (!item) { return; }

    item->setEnabled(enabled);
}
