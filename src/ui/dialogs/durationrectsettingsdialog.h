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

#ifndef DURATIONRECTSETTINGSDIALOG_H
#define DURATIONRECTSETTINGSDIALOG_H

#include "ui_global.h"

#include <QDialog>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include "Timeline/durationrectangle.h"
#include "dialog.h"

class QHBoxLayout;
class QVBoxLayout;
class TwoColumnLayout;

class UI_EXPORT DurationRectSettingsDialog : public Friction::Ui::Dialog
{
public:
    DurationRectSettingsDialog(DurationRectangle* const target,
                               QWidget *parent = nullptr);

    int getMinFrame() const;
    int getMaxFrame() const;
    int getShift() const;

private:
    DurationRectangle* const mTarget;

    QSpinBox *mShiftSpinBox;
    QSpinBox *mMinFrameSpinBox;
    QSpinBox *mMaxFrameSpinBox;
};

#endif // DURATIONRECTSETTINGSDIALOG_H
