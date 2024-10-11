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

#include "animationtopaintdialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

AnimationToPaintDialog::AnimationToPaintDialog(
        const QString &objName, const FrameRange &range,
        QWidget * const parent) :
    QDialog(parent) {
    setWindowTitle("Create Paint Object " + objName);

    const auto mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    const auto layout = new QHBoxLayout();
    mainLayout->addLayout(layout);
    const auto labelLayout = new QVBoxLayout();
    const auto spinLayout = new QVBoxLayout();
    layout->addLayout(labelLayout);
    layout->addLayout(spinLayout);

    mFirstFrame = new QSpinBox(this);
    mFirstFrame->setRange(-9999, 9999);
    mFirstFrame->setValue(range.fMin);
    mLastFrame = new QSpinBox(this);
    mLastFrame->setRange(-9999, 9999);
    mLastFrame->setValue(range.fMax);
    mIncrement = new QSpinBox(this);
    mIncrement->setRange(1, 100);
    mIncrement->setValue(1);

    spinLayout->addWidget(mFirstFrame);
    spinLayout->addWidget(mLastFrame);
    spinLayout->addWidget(mIncrement);

    connect(mFirstFrame, qOverload<int>(&QSpinBox::valueChanged),
            mLastFrame, &QSpinBox::setMinimum);
    connect(mLastFrame, qOverload<int>(&QSpinBox::valueChanged),
            mFirstFrame, &QSpinBox::setMaximum);

    labelLayout->addWidget(new QLabel("First Frame:"));
    labelLayout->addWidget(new QLabel("Last Frame:"));
    labelLayout->addWidget(new QLabel("Increment:"));

    const auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok |
                                              QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

int AnimationToPaintDialog::firstFrame() const
{ return mFirstFrame->value(); }

int AnimationToPaintDialog::lastFrame() const
{ return mLastFrame->value(); }

int AnimationToPaintDialog::increment() const
{ return mIncrement->value(); }

bool AnimationToPaintDialog::sExec(
        const AnimationBox * const src,
        int &firstAbsFrame, int &lastAbsFrame,
        int &increment, QWidget * const parent) {
    const QString& name = src->prp_getName();
    const FrameRange range = src->prp_absInfluenceRange();
    const auto dialog = new AnimationToPaintDialog(name, range, parent);
    const bool accepted = dialog->exec() == QDialog::Accepted;
    firstAbsFrame = dialog->firstFrame();
    lastAbsFrame = dialog->lastFrame();
    increment = dialog->increment();
    delete dialog;
    return accepted;
}
