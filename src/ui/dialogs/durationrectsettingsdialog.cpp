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

#include "durationrectsettingsdialog.h"
#include "widgets/twocolumnlayout.h"

#include <QDialogButtonBox>

DurationRectSettingsDialog::DurationRectSettingsDialog(
        DurationRectangle* const target, QWidget *parent) :
    QDialog(parent), mTarget(target) {
    const auto values = mTarget->getValues();
    const auto mainLayout = new QVBoxLayout(this);
    const auto twoColumnLayout = new TwoColumnLayout();

    const auto shiftLabel = new QLabel("Frame shift:");
    mShiftSpinBox = new QSpinBox(this);
    mShiftSpinBox->setRange(-99999, 99999);
    mShiftSpinBox->setValue(values.fShift);
    twoColumnLayout->addPair(shiftLabel, mShiftSpinBox);

    const auto minFrameLabel = new QLabel("Min frame:");
    mMinFrameSpinBox = new QSpinBox(this);
    mMinFrameSpinBox->setRange(-99999, values.fMax);
    mMinFrameSpinBox->setValue(values.fMin);
    twoColumnLayout->addPair(minFrameLabel, mMinFrameSpinBox);

    const auto maxFrameLabel = new QLabel("Max frame:");
    mMaxFrameSpinBox = new QSpinBox(this);
    mMaxFrameSpinBox->setRange(values.fMin, 99999);
    mMaxFrameSpinBox->setValue(values.fMax);
    twoColumnLayout->addPair(maxFrameLabel, mMaxFrameSpinBox);

    connect(mMinFrameSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            mMaxFrameSpinBox, &QSpinBox::setMinimum);
    connect(mMaxFrameSpinBox, qOverload<int>(&QSpinBox::valueChanged),
            mMinFrameSpinBox, &QSpinBox::setMaximum);


    const auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok |
                                              QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        mTarget->setValues({mShiftSpinBox->value(),
                            mMinFrameSpinBox->value(),
                            mMaxFrameSpinBox->value()});
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addLayout(twoColumnLayout);
    mainLayout->addWidget(buttons);
    setLayout(mainLayout);

    connect(target, &QObject::destroyed, this, &QDialog::reject);
}

int DurationRectSettingsDialog::getMinFrame() const {
    return mMinFrameSpinBox->value();
}

int DurationRectSettingsDialog::getMaxFrame() const {
    return mMaxFrameSpinBox->value();
}

int DurationRectSettingsDialog::getShift() const {
    return mShiftSpinBox->value();
}
