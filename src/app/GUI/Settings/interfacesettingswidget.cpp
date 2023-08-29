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

#include "interfacesettingswidget.h"

#include "Private/esettings.h"
#include "GUI/global.h"
#include "labeledslider.h"

#include <QLabel>

InterfaceSettingsWidget::InterfaceSettingsWidget(QWidget *parent) :
    SettingsWidget(parent) {

    const auto uiScaleLayout = new LabeledSlider("%");
    mInterfaceScaling = uiScaleLayout->slider();
    mInterfaceScaling->setRange(50, 150);
    emit mInterfaceScaling->valueChanged(100);

    uiScaleLayout->insertWidget(0, new QLabel("Interface scaling"));

    addLayout(uiScaleLayout);
}

void InterfaceSettingsWidget::applySettings() {
    mSett.fInterfaceScaling = mInterfaceScaling->value()*0.01;
    eSizesUI::font.updateSize();
    eSizesUI::widget.updateSize();
}

void InterfaceSettingsWidget::updateSettings(bool restore)
{
    Q_UNUSED(restore)
    mInterfaceScaling->setValue(100*mSett.fInterfaceScaling);
}
