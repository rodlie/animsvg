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

#include "pluginssettingswidget.h"
#include "appsupport.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QFile>

PluginsSettingsWidget::PluginsSettingsWidget(QWidget *parent)
    : SettingsWidget(parent)
    , mShaderPath(nullptr)
{
    const auto mShaderWidget = new QWidget(this);
    const auto mShaderLayout = new QHBoxLayout(mShaderWidget);

    const auto mShaderLabel = new QLabel(tr("Shaders Path"), this);
    mShaderPath = new QLineEdit(this);
    mShaderPath->setText(AppSupport::getAppShaderEffectsPath());
    const auto mShaderPathButton = new QPushButton(QIcon::fromTheme("dots"),
                                                   QString(),
                                                   this);
    mShaderPathButton->setFocusPolicy(Qt::NoFocus);

    mShaderLayout->addWidget(mShaderLabel);
    mShaderLayout->addWidget(mShaderPath);
    mShaderLayout->addWidget(mShaderPathButton);

    addWidget(mShaderWidget);

    connect(mShaderPathButton, &QPushButton::pressed,
            this, [this]() {
        QString path = QFileDialog::getExistingDirectory(this,
                                                         tr("Select directory"),
                                                         QDir::homePath());
        if (QFile::exists(path)) { mShaderPath->setText(path); }
    });
}

void PluginsSettingsWidget::applySettings()
{
    AppSupport::setSettings("settings",
                            "CustomShaderPath",
                            mShaderPath->text());
}

void PluginsSettingsWidget::updateSettings()
{
    mShaderPath->setText(AppSupport::getAppShaderEffectsPath());
}
