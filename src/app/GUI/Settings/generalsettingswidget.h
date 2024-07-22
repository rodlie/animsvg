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

#ifndef GENERALSETTINGSWIDGET_H
#define GENERALSETTINGSWIDGET_H

#include "widgets/settingswidget.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QSlider>
#include <QComboBox>

class GeneralSettingsWidget : public SettingsWidget
{
public:
    explicit GeneralSettingsWidget(QWidget *parent = nullptr);
    void applySettings();
    void updateSettings(bool restore = false);

private:
    QCheckBox *mAutoBackup;
    QCheckBox *mAutoSave;
    QSpinBox *mAutoSaveTimer;
    QCheckBox *mDefaultInterfaceScaling;
    QSlider *mInterfaceScaling;
    QComboBox *mImportFileDir;
};

#endif // GENERALSETTINGSWIDGET_H
