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

#ifndef PRESETSETTINGSWIDGET_H
#define PRESETSETTINGSWIDGET_H

#include "widgets/settingswidget.h"

#include <QTreeWidget>
#include <QCheckBox>

class PresetSettingsWidget : public SettingsWidget
{
public:
    explicit PresetSettingsWidget(QWidget *parent = nullptr);
    void applySettings();
    void updateSettings(bool restore = false);

private:
    QTreeWidget *mTreeResolutions;
    QTreeWidget *mTreeFps;
    QCheckBox *mCheckResolutions;
    QCheckBox *mCheckResolutionsAuto;
    QCheckBox *mCheckFps;
    QCheckBox *mCheckFpsAuto;

    void setupResolutionPresetWidget();
    void populateResolutionsPresets();
    void saveResolutionsPresets();

    void setupFpsPresetWidget();
    void populateFpsPresets();
    void saveFpsPresets();
};

#endif // PRESETSETTINGSWIDGET_H
