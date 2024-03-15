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

#include "canvassettingswidget.h"

#include "Private/esettings.h"
#include "labeledslider.h"
#include "GUI/coloranimatorbutton.h"
#include "widgets/twocolumnlayout.h"

#include <QLabel>

#include "GUI/global.h"

CanvasSettingsWidget::CanvasSettingsWidget(QWidget* const parent) :
    SettingsWidget(parent) {

    const auto pathColorsLay = new TwoColumnLayout;

    const auto nodeLayout = new LabeledSlider("%");
    mPathNodeSize = nodeLayout->slider();
    mPathNodeSize->setRange(50, 150);
    emit mPathNodeSize->valueChanged(100);
    mPathNodeColor = new ColorAnimatorButton(mSett.fPathNodeColor, this);
    nodeLayout->addWidget(mPathNodeColor);
    mPathNodeSelectedColor = new ColorAnimatorButton(mSett.fPathNodeSelectedColor, this);
    nodeLayout->addWidget(mPathNodeSelectedColor);
    pathColorsLay->addPair(new QLabel("Node size"), nodeLayout);

    const auto dissolvedNodeLayout = new LabeledSlider("%");
    mPathDissolvedNodeSize = dissolvedNodeLayout->slider();
    mPathDissolvedNodeSize->setRange(50, 150);
    emit mPathDissolvedNodeSize->valueChanged(100);
    mPathDissolvedNodeColor = new ColorAnimatorButton(mSett.fPathDissolvedNodeColor, this);
    dissolvedNodeLayout->addWidget(mPathDissolvedNodeColor);
    mPathDissolvedNodeSelectedColor = new ColorAnimatorButton(mSett.fPathDissolvedNodeSelectedColor, this);
    dissolvedNodeLayout->addWidget(mPathDissolvedNodeSelectedColor);
    pathColorsLay->addPair(new QLabel("Dissolved node size"), dissolvedNodeLayout);

    const auto controlLayout = new LabeledSlider("%");
    mPathControlSize = controlLayout->slider();
    mPathControlSize->setRange(50, 150);
    emit mPathControlSize->valueChanged(100);
    mPathControlColor = new ColorAnimatorButton(mSett.fPathControlColor, this);
    controlLayout->addWidget(mPathControlColor);
    mPathControlSelectedColor = new ColorAnimatorButton(mSett.fPathControlSelectedColor, this);
    controlLayout->addWidget(mPathControlSelectedColor);
    pathColorsLay->addPair(new QLabel("Control size"), controlLayout);

    addLayout(pathColorsLay);

    addSeparator();

    mRtlSupport = new QCheckBox("RTL language support", this);
    addWidget(mRtlSupport);

    eSizesUI::widget.add(mRtlSupport, [this](const int size) {
        mRtlSupport->setFixedHeight(size);
        mRtlSupport->setStyleSheet(QString("QCheckBox::indicator { width: %1px; height: %1px;}").arg(size/1.5));
    });
}

void CanvasSettingsWidget::applySettings() {
    mSett.fPathNodeScaling = mPathNodeSize->value()*0.01;
    mSett.fPathNodeColor = mPathNodeColor->color();
    mSett.fPathNodeSelectedColor = mPathNodeSelectedColor->color();

    mSett.fPathDissolvedNodeScaling = mPathDissolvedNodeSize->value()*0.01;
    mSett.fPathDissolvedNodeColor = mPathDissolvedNodeColor->color();
    mSett.fPathDissolvedNodeSelectedColor = mPathDissolvedNodeSelectedColor->color();

    mSett.fPathControlScaling = mPathControlSize->value()*0.01;
    mSett.fPathControlColor = mPathControlColor->color();
    mSett.fPathControlSelectedColor = mPathControlSelectedColor->color();

    mSett.fCanvasRtlSupport = mRtlSupport->isChecked();
}

void CanvasSettingsWidget::updateSettings(bool restore)
{
    Q_UNUSED(restore)
    mPathNodeSize->setValue(100*mSett.fPathNodeScaling);
    mPathNodeColor->setColor(mSett.fPathNodeColor);
    mPathNodeSelectedColor->setColor(mSett.fPathNodeSelectedColor);

    mPathDissolvedNodeSize->setValue(100*mSett.fPathDissolvedNodeScaling);
    mPathDissolvedNodeColor->setColor(mSett.fPathDissolvedNodeColor);
    mPathDissolvedNodeSelectedColor->setColor(mSett.fPathDissolvedNodeSelectedColor);

    mPathControlSize->setValue(100*mSett.fPathControlScaling);
    mPathControlColor->setColor(mSett.fPathControlColor);
    mPathControlSelectedColor->setColor(mSett.fPathControlSelectedColor);

    mRtlSupport->setChecked(mSett.fCanvasRtlSupport);
}
