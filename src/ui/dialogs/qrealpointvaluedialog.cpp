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

#include "qrealpointvaluedialog.h"
#include "Animators/qrealpoint.h"

QrealPointValueDialog::QrealPointValueDialog(QrealPoint *point,
                                             QWidget *parent)
    : Friction::Ui::Dialog(parent)
{
    setWindowTitle(tr("Edit key"));
    mMainLayout = new QVBoxLayout(this);
    setLayout(mMainLayout);

    mFrameSpinBox = new QDoubleSpinBox(this);
    mValueSpinBox = new QDoubleSpinBox(this);
    mSpinLayout = new QHBoxLayout();
    mSpinLayout->addWidget(mFrameSpinBox);
    mSpinLayout->addWidget(mValueSpinBox);
    mFrameSpinBox->setRange(0, 10000);
    if(point->isKeyPt()) {
        mFrameSpinBox->setDecimals(0);
    }
    mValueSpinBox->setRange(-10000, 10000);

    mOkButton = new QPushButton("Ok", this);
    mCancelButton = new QPushButton("Cancel", this);
    mButtonsLayout = new QHBoxLayout();
    mButtonsLayout->addWidget(mOkButton);
    mButtonsLayout->addWidget(mCancelButton);

    mMainLayout->addLayout(mSpinLayout);
    mMainLayout->addLayout(mButtonsLayout);

    mPoint = point;
    mSavedValue = mPoint->getValue();
    mSavedFrame = mPoint->getRelFrame();
    resetValue();

    connect(mOkButton, &QPushButton::pressed,
            this, &QrealPointValueDialog::ok);
    connect(mCancelButton, &QPushButton::pressed,
            this, &QrealPointValueDialog::cancel);
    connect(mFrameSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged),
            this, &QrealPointValueDialog::setFrame);
    connect(mValueSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged),
            this, &QrealPointValueDialog::setValue);
}

void QrealPointValueDialog::setValue(double value) {
    mPoint->setValue(value);
    emit repaintSignal();
}

void QrealPointValueDialog::setFrame(double frame) {
    mPoint->setRelFrame(frame);
    emit repaintSignal();
}

void QrealPointValueDialog::cancel() {
    resetValue();
    close();
}

void QrealPointValueDialog::ok() {
    close();
}

void QrealPointValueDialog::resetValue() {
    mValueSpinBox->setValue(mSavedValue);
    mFrameSpinBox->setValue(mSavedFrame);
    setValue(mSavedValue);
    setFrame(mSavedFrame);
    emit repaintSignal();
}
