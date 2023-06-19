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

#ifndef CUSTOMFPSDIALOG_H
#define CUSTOMFPSDIALOG_H
#include <QDialog>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>

class CustomFpsDialog : public QDialog
{
public:
    CustomFpsDialog();

    void setFps(const qreal fps) {
        mSpinBox->setValue(fps);
    }
    void setEnabled(const bool bT) {
        mCheckBox->setChecked(bT);
    }

    qreal getFps() {
        return mSpinBox->value();
    }

    bool getFpsEnabled() {
        return mCheckBox->isChecked();
    }
protected:
    QVBoxLayout *mMainLayout = nullptr;

    QHBoxLayout *mFpsLayout = nullptr;

    QLabel *mFpsLabel = nullptr;

    QCheckBox *mCheckBox = nullptr;
    QDoubleSpinBox *mSpinBox = nullptr;

    QHBoxLayout *mButtonsLayout = nullptr;
    QPushButton *mAcceptButton = nullptr;
    QPushButton *mCancelButton = nullptr;
};

#endif // CUSTOMFPSDIALOG_H
