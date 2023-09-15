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

#include "generalsettingswidget.h"
#include "appsupport.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

#include "../mainwindow.h"

GeneralSettingsWidget::GeneralSettingsWidget(QWidget *parent)
    : SettingsWidget(parent)
    , mAutoBackup(nullptr)
    , mAutoSave(nullptr)
    , mAutoSaveTimer(nullptr)
{
    const auto mGeneralWidget = new QWidget(this);
    mGeneralWidget->setContentsMargins(0, 0, 0, 0);
    const auto mGeneralLayout = new QVBoxLayout(mGeneralWidget);
    mGeneralLayout->setContentsMargins(0, 0, 0, 0);

    const auto mAutoBackupWidget = new QGroupBox(this);
    mAutoBackupWidget->setTitle(tr("Backup"));
    mAutoBackupWidget->setContentsMargins(0, 0, 0, 0);
    const auto mAutoBackupLayout = new QHBoxLayout(mAutoBackupWidget);

    const auto mAutoBackupLabel = new QLabel(tr("Enable Backup on Save"), this);
    mAutoBackupLabel->setToolTip(tr("Creates a backup file after each successful save.\n\n"
                                    "Backup files are stored in a folder called PROJECT.friction_backup."));

    mAutoBackup = new QCheckBox(this);
    mAutoBackup->setCheckable(true);

    mAutoBackupLayout->addWidget(mAutoBackupLabel);
    mAutoBackupLayout->addStretch();
    mAutoBackupLayout->addWidget(mAutoBackup);

    mGeneralLayout->addWidget(mAutoBackupWidget);

    const auto mAutoSaveWidget = new QGroupBox(this);
    mAutoSaveWidget->setTitle(tr("Auto Save"));
    mAutoSaveWidget->setContentsMargins(0, 0, 0, 0);
    const auto mAutoSaveLayout = new QHBoxLayout(mAutoSaveWidget);

    const auto mAutoSaveLabel = new QLabel(tr("Enable Auto Save"), this);
    mAutoSaveLabel->setToolTip(tr("Will auto save each X min if project is unsaved.\n\n"
                                  "Enable Backup on Save for incremental saves (and as a failsafe)."));

    mAutoSave = new QCheckBox(this);
    mAutoSave->setCheckable(true);

    mAutoSaveTimer = new QSpinBox(this);
    mAutoSaveTimer->setRange(1, 60);
    mAutoSaveTimer->setSuffix(tr(" min"));

    mAutoSaveLayout->addWidget(mAutoSaveLabel);
    mAutoSaveLayout->addStretch();
    mAutoSaveLayout->addWidget(mAutoSave);
    mAutoSaveLayout->addWidget(mAutoSaveTimer);

    mGeneralLayout->addWidget(mAutoSaveWidget);

    mGeneralLayout->addStretch();
    addWidget(mGeneralWidget);
}

void GeneralSettingsWidget::applySettings()
{
    AppSupport::setSettings("files",
                            "BackupOnSave",
                            mAutoBackup->isChecked());
    AppSupport::setSettings("files",
                            "AutoSave",
                            mAutoSave->isChecked());
    AppSupport::setSettings("files",
                            "AutoSaveTimeout",
                            (mAutoSaveTimer->value() * 60) * 1000);
    MainWindow::sGetInstance()->updateAutoSaveBackupState();
}

void GeneralSettingsWidget::updateSettings(bool restore)
{
    Q_UNUSED(restore)

    mAutoBackup->setChecked(AppSupport::getSettings("files",
                                                    "BackupOnSave",
                                                    false).toBool());
    mAutoSave->setChecked(AppSupport::getSettings("files",
                                                  "AutoSave",
                                                  false).toBool());
    int ms = AppSupport::getSettings("files",
                                     "AutoSaveTimeout",
                                     300000).toInt();
    mAutoSaveTimer->setValue((ms / 1000) / 60);
}
