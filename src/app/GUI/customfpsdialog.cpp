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

#include "customfpsdialog.h"
#include <QPushButton>

CustomFpsDialog::CustomFpsDialog() {
    mMainLayout = new QVBoxLayout(this);
    setLayout(mMainLayout);

    mCheckBox = new QCheckBox("Enabled", this);

    mFpsLayout = new QHBoxLayout();
    mFpsLabel = new QLabel("Fps", this);
    mSpinBox = new QDoubleSpinBox(this);
    mSpinBox->setRange(1., 100.);

    mFpsLayout->addWidget(mFpsLabel);
    mFpsLayout->addWidget(mSpinBox);

    mButtonsLayout = new QHBoxLayout();
    mMainLayout->addLayout(mButtonsLayout);
    mAcceptButton = new QPushButton("Ok", this);
    mCancelButton = new QPushButton("Cancel", this);
    mButtonsLayout->addWidget(mAcceptButton);
    mButtonsLayout->addWidget(mCancelButton);
    connect(mAcceptButton, SIGNAL(released()),
            this, SLOT(accept()));
    connect(mCancelButton, SIGNAL(released()),
            this, SLOT(reject()));

    mMainLayout->addWidget(mCheckBox);
    mMainLayout->addLayout(mFpsLayout);
    mMainLayout->addLayout(mButtonsLayout);
}
