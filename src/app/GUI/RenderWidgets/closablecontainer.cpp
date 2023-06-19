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

#include "closablecontainer.h"
#include "GUI/global.h"
#include "Private/esettings.h"
#include <QCheckBox>

ClosableContainer::ClosableContainer(QWidget *parent) : QWidget(parent) {
    mMainLayout->setAlignment(Qt::AlignTop);
    setLayout(mMainLayout);
    mContentArrow = new QPushButton("", this);
    mContentArrow->setObjectName("iconButton");
    mContentArrow->setCheckable(true);
    mContentArrow->setFixedSize(eSizesUI::widget, eSizesUI::widget);
    mContentArrow->setIconSize(QSize(eSizesUI::widget, eSizesUI::widget));
    connect(mContentArrow, &QPushButton::toggled,
            this, &ClosableContainer::setContentVisible);

    mMainLayout->addWidget(mContentArrow, 0, Qt::AlignTop);
    mMainLayout->addLayout(mVLayout);
    mVLayout->setAlignment(Qt::AlignTop);
    mVLayout->addWidget(mContWidget);
    mContWidget->setContentsMargins(0, 0, 0, 0);
    mContWidget->setLayout(mContLayout);
    mContLayout->setMargin(0);
    mContLayout->setAlignment(Qt::AlignTop);
    mVLayout->setSpacing(0);
    mVLayout->setMargin(0);
    setContentVisible(false);
}

void ClosableContainer::setLabelWidget(QWidget *widget) {
    if(mLabelWidget) delete mLabelWidget;
    mLabelWidget = widget;
    if(widget) mVLayout->insertWidget(0, widget);
}

void ClosableContainer::addContentWidget(QWidget *widget) {
    mContWidgets << widget;
    mContLayout->addWidget(widget);
}

void ClosableContainer::setCheckable(const bool check) {
    if(check == bool(mCheckBox)) return;
    if(check) {
        mCheckBox = new QCheckBox(this);
        mCheckBox->setFixedSize(eSizesUI::widget, eSizesUI::widget);
        mMainLayout->insertWidget(0, mCheckBox, 0, Qt::AlignTop);
        mCheckBox->setChecked(true);
    } else {
        delete mCheckBox;
        mCheckBox = nullptr;
    }
}

void ClosableContainer::setChecked(const bool check) {
    if(!mCheckBox) return;
    if(mCheckBox->isChecked() == check) return;
    mCheckBox->setChecked(true);
}

bool ClosableContainer::isChecked() const {
    if(!mCheckBox) return true;
    return mCheckBox->isChecked();
}

void ClosableContainer::setContentVisible(const bool visible) {
    QString iconPath = eSettings::sIconsDir();
    if(visible) iconPath += "/down-arrow.png";
    else iconPath += "/right-arrow.png";
    mContentArrow->setIcon(QIcon(iconPath));
    mContWidget->setVisible(visible);
}
