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

#ifndef OUTPUTSETTINGSPROFILESDIALOG_H
#define OUTPUTSETTINGSPROFILESDIALOG_H
#include <QDialog>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include "renderinstancesettings.h"
class OutputSettingsDisplayWidget;
class OutputProfilesDialog : public QDialog {
public:
    OutputProfilesDialog(const OutputSettings &currentSettings,
                                 QWidget *parent = nullptr);

    OutputSettingsProfile *getCurrentProfile();
protected:
    void updateButtonsEnabled();
    void currentProfileChanged();

    void setCurrentProfileName(const QString &name);
    void deleteCurrentProfile();
    void duplicateCurrentProfile();
    void createAndEditNewProfile();
    void editCurrentProfile();
    void saveCurrentProfile();

    QVBoxLayout *mInnerLayout;

    QHBoxLayout *mProfileLayout;
    QLabel *mProfileLabel;
    QComboBox *mProfilesComboBox;

    QHBoxLayout *mProfileButtonsLayout;
    QPushButton *mNewProfileButton;
    QPushButton *mDuplicateProfileButton;
    QPushButton *mEditProfileButton;
    QPushButton *mSaveProfileButton;
    QPushButton *mDeleteProfileButton;

    OutputSettingsDisplayWidget *mOutputSettingsDisplayWidget = nullptr;

    QHBoxLayout *mButtonsLayout;
    QPushButton *mOkButton = nullptr;
    QPushButton *mCancelButton = nullptr;

    QStatusBar* mStatusBar;

    OutputSettings mCurrentSettings;
};

#endif // OUTPUTSETTINGSPROFILESDIALOG_H
