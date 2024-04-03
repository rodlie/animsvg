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

#ifndef QREALPOINTVALUEDIALOG_H
#define QREALPOINTVALUEDIALOG_H

#include "ui_global.h"

#include "Animators/qrealanimator.h"
#include <QDialog>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QPushButton>

class UI_EXPORT QrealPointValueDialog : public QDialog {
    Q_OBJECT
public:
    QrealPointValueDialog(QrealPoint *point, QWidget *parent);

private:
    void setValue(double value);
    void setFrame(double frame);
    void cancel();
    void ok();
signals:
    void repaintSignal();
private:
    QPushButton *mOkButton;
    QPushButton *mCancelButton;

    QVBoxLayout *mMainLayout;
    QHBoxLayout *mSpinLayout;
    QHBoxLayout *mButtonsLayout;

    void resetValue();
    QrealPoint *mPoint;

    qreal mSavedValue;
    qreal mSavedFrame;

    QDoubleSpinBox *mValueSpinBox;
    QDoubleSpinBox *mFrameSpinBox;
};

#endif // QREALPOINTVALUEDIALOG_H
