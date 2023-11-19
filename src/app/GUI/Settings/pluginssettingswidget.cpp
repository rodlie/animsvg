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
#include "effectsloader.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QTreeWidgetItem>
#include <QHeaderView>

#include "GUI/global.h"

PluginsSettingsWidget::PluginsSettingsWidget(QWidget *parent)
    : SettingsWidget(parent)
    , mShaderPath(nullptr)
    , mShaderTree(nullptr)
{
    mShadersList = EffectsLoader::sInstance->getLoadedShaderEffects();

    mShadersDisabled = AppSupport::getSettings("settings",
                                               "DisabledShaders").toStringList();

    const auto mShaderWidget = new QWidget(this);
    mShaderWidget->setContentsMargins(0, 0, 0, 0);
    const auto mShaderLayout = new QHBoxLayout(mShaderWidget);
    mShaderLayout->setContentsMargins(0, 0, 0, 0);

    const auto mShaderLabel = new QLabel(tr("Shaders Path"), this);
    mShaderLabel->setToolTip(tr("This location will be scanned for plugins during startup."));
    mShaderPath = new QLineEdit(this);
    mShaderPath->setText(AppSupport::getAppShaderEffectsPath());
    const auto mShaderPathButton = new QPushButton(QIcon::fromTheme("dots"),
                                                   QString(),
                                                   this);
    mShaderPathButton->setFocusPolicy(Qt::NoFocus);
    mShaderPathButton->setIconSize(QSize(eSizesUI::widget, eSizesUI::widget));
    mShaderPathButton->setFixedSize(QSize(eSizesUI::widget, eSizesUI::widget));

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

    mShaderTree = new QTreeWidget(this);
    mShaderTree->setHeaderLabels(QStringList() << tr("Shader") << tr("Group"));
    mShaderTree->setAlternatingRowColors(true);
    mShaderTree->setSortingEnabled(false);
    mShaderTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    addWidget(mShaderTree);
    populateShaderTree();

    const auto infoLabel = new QLabel(this);
    infoLabel->setText(tr("Any changes in this section will require a restart of Friction."));
    addWidget(infoLabel);
}

void PluginsSettingsWidget::applySettings()
{
    AppSupport::setSettings("settings",
                            "CustomShaderPath",
                            mShaderPath->text());
    QStringList disabledShaders;
    for (int i = 0; i < mShaderTree->topLevelItemCount(); ++i ) {
        const auto item = mShaderTree->topLevelItem(i);
        if (item->checkState(0) == Qt::Unchecked) { disabledShaders << item->data(0,
                                                                                  Qt::UserRole).toString(); }
    }
    AppSupport::setSettings("settings",
                            "DisabledShaders",
                            disabledShaders);
}

void PluginsSettingsWidget::updateSettings(bool restore)
{
    mShaderPath->setText(AppSupport::getAppShaderEffectsPath(restore));
    mShadersDisabled = AppSupport::getSettings("settings",
                                               "DisabledShaders").toStringList();
    if (restore) {
        mShadersDisabled.clear();
        populateShaderTree();
    }
}

void PluginsSettingsWidget::populateShaderTree()
{
    mShaderTree->setSortingEnabled(false);
    mShaderTree->clear();
    for (auto &shader: mShadersList) {
        QPair<QString, QString> shaderID = AppSupport::getShaderID(shader);
        QTreeWidgetItem *item = new QTreeWidgetItem(mShaderTree);
        item->setCheckState(0, mShadersDisabled.contains(shader) ? Qt::Unchecked : Qt::Checked);
        item->setToolTip(0, shader);
        item->setData(0, Qt::UserRole, shader);
        item->setText(0, shaderID.first);
        item->setText(1, shaderID.second);
        mShaderTree->addTopLevelItem(item);
    }
    mShaderTree->setSortingEnabled(true);
    mShaderTree->sortByColumn(0, Qt::AscendingOrder);
}
