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

#ifndef PERFORMANCESETTINGSWIDGET_H
#define PERFORMANCESETTINGSWIDGET_H

#include "ui_global.h"

#include "widgets/settingswidget.h"
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QSlider>
#include <QComboBox>
#include <QList>

class UI_EXPORT PerformanceSettingsWidget : public SettingsWidget
{
public:
    explicit PerformanceSettingsWidget(QWidget *parent = nullptr);

    void applySettings();
    void updateSettings(bool restore = false);
    void updateAudioDevices();

private:
    void setupRasterEffectWidgets();
    void saveRasterEffectsSupport();
    void restoreDefaultRasterEffectsSupport();
    void updateAccPreferenceDesc();

    QCheckBox* mCpuThreadsCapCheck = nullptr;
    QLabel* mCpuThreadsCapLabel = nullptr;
    QSlider* mCpuThreadsCapSlider = nullptr;

    QCheckBox* mRamMBCapCheck = nullptr;
    QSpinBox* mRamMBCapSpin = nullptr;
    QSlider* mRamMBCapSlider = nullptr;

    QLabel* mAccPreferenceLabel = nullptr;
    QLabel* mAccPreferenceDescLabel = nullptr;
    QLabel* mAccPreferenceCpuLabel = nullptr;
    QSlider* mAccPreferenceSlider = nullptr;
    QLabel* mAccPreferenceGpuLabel = nullptr;

    QCheckBox* mPathGpuAccCheck = nullptr;
    QComboBox *mMsaa = nullptr;

    QComboBox *mAudioDevicesCombo;

    QList<QComboBox*> mRasterEffectsHardwareSupport;
};

#endif // PERFORMANCESETTINGSWIDGET_H
