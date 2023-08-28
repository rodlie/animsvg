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

#include "settingswidget.h"

#include "Private/esettings.h"

#include <QVBoxLayout>
#include <QFrame>

#include "appsupport.h"

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
    , mSett(*eSettings::sInstance)
{
    mMainLauout = new QVBoxLayout;
    mMainLauout->setAlignment(Qt::AlignTop);
    setLayout(mMainLauout);
}

void SettingsWidget::add2HWidgets(QWidget * const widget1,
                                  QWidget * const widget2)
{
    const auto layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(widget1);
    layout->addWidget(widget2);
    addLayout(layout);
}

void SettingsWidget::addWidget(QWidget * const widget)
{
    mMainLauout->addWidget(widget);
}

void SettingsWidget::addLayout(QLayout * const layout)
{
    mMainLauout->addLayout(layout);
}

void SettingsWidget::addSeparator()
{
    const auto line0 = new QFrame(this);
    line0->setMinimumHeight(10);
    layout()->addWidget(line0);
}
