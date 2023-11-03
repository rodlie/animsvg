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

#include "presetsettingswidget.h"
#include "appsupport.h"

#include <QGroupBox>
#include <QScrollArea>
#include <QHeaderView>
#include <QList>
#include <QPair>

PresetSettingsWidget::PresetSettingsWidget(QWidget *parent)
    : SettingsWidget(parent)
    , mTreeResolutions(nullptr)
    , mTreeFps(nullptr)
{
    setupResolutionPresetWidget();
    setupFpsPresetWidget();
}

void PresetSettingsWidget::applySettings()
{
    saveResolutionsPresets();
    saveFpsPresets();
}

void PresetSettingsWidget::updateSettings(bool restore)
{
    Q_UNUSED(restore)
    populateResolutionsPresets();
    populateFpsPresets();
}

void PresetSettingsWidget::setupResolutionPresetWidget()
{
    const auto area = new QScrollArea(this);
    const auto container = new QGroupBox(this);
    const auto containerLayout = new QVBoxLayout(container);
    const auto containerInner = new QWidget(this);
    const auto containerInnerLayout = new QVBoxLayout(containerInner);

    area->setWidget(containerInner);
    area->setWidgetResizable(true);
    area->setContentsMargins(0, 0, 0, 0);

    container->setTitle(tr("Scene Resolutions"));

    container->setContentsMargins(0, 0, 0, 0);

    containerInnerLayout->setMargin(0);
    containerLayout->setMargin(0);

    containerLayout->addWidget(area);

    mTreeResolutions = new QTreeWidget(this);
    mTreeResolutions->setHeaderLabels(QStringList() << tr("Width") << tr("Height"));
    mTreeResolutions->setAlternatingRowColors(true);
    mTreeResolutions->setSortingEnabled(false);
    mTreeResolutions->setHeaderHidden(true);
    mTreeResolutions->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    containerInnerLayout->addWidget(mTreeResolutions);

    addWidget(container);
}

void PresetSettingsWidget::populateResolutionsPresets()
{
    mTreeResolutions->setSortingEnabled(false);
    mTreeResolutions->clear();
    for (auto &resolution: AppSupport::getResolutionPresets()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(mTreeResolutions);
        item->setCheckState(0, Qt::Checked);
        item->setText(0, QString::number(resolution.first));
        item->setText(1, QString::number(resolution.second));
        mTreeResolutions->addTopLevelItem(item);
    }
    mTreeResolutions->setSortingEnabled(true);
    mTreeResolutions->sortByColumn(0, Qt::AscendingOrder);
}

void PresetSettingsWidget::saveResolutionsPresets()
{
    QList<QPair<int, int>> resolutions;
    for (int i = 0; i < mTreeResolutions->topLevelItemCount(); ++i) {
        const auto item = mTreeResolutions->topLevelItem(i);
        if (item->checkState(0) != Qt::Checked) { continue; }
        QPair<int, int> resolution;
        resolution.first = item->text(0).toInt();
        resolution.second = item->text(1).toInt();
        resolutions << resolution;
    }
    if (resolutions != AppSupport::getResolutionPresets()) {
        AppSupport::saveResolutionPresets(resolutions);
    }
}

void PresetSettingsWidget::setupFpsPresetWidget()
{
    const auto area = new QScrollArea(this);
    const auto container = new QGroupBox(this);
    const auto containerLayout = new QVBoxLayout(container);
    const auto containerInner = new QWidget(this);
    const auto containerInnerLayout = new QVBoxLayout(containerInner);

    area->setWidget(containerInner);
    area->setWidgetResizable(true);
    area->setContentsMargins(0, 0, 0, 0);

    container->setTitle(tr("Scene FPS"));

    container->setContentsMargins(0, 0, 0, 0);

    containerInnerLayout->setMargin(0);
    containerLayout->setMargin(0);

    containerLayout->addWidget(area);

    mTreeFps = new QTreeWidget(this);
    mTreeFps->setHeaderLabels(QStringList() << tr("FPS"));
    mTreeFps->setAlternatingRowColors(true);
    mTreeFps->setSortingEnabled(false);
    mTreeFps->setHeaderHidden(true);

    containerInnerLayout->addWidget(mTreeFps);

    addWidget(container);
}

void PresetSettingsWidget::populateFpsPresets()
{
    mTreeFps->setSortingEnabled(false);
    mTreeFps->clear();
    for (auto &fps: AppSupport::getFpsPresets()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(mTreeFps);
        item->setCheckState(0, Qt::Checked);
        item->setText(0, fps);
        mTreeFps->addTopLevelItem(item);
    }
    mTreeFps->setSortingEnabled(true);
    mTreeFps->sortByColumn(0, Qt::AscendingOrder);
}

void PresetSettingsWidget::saveFpsPresets()
{
    QStringList fps;
    for (int i = 0; i < mTreeFps->topLevelItemCount(); ++i) {
        const auto item = mTreeFps->topLevelItem(i);
        if (item->checkState(0) != Qt::Checked) { continue; }
        fps << item->text(0);
    }
    if (fps != AppSupport::getFpsPresets()) {
        AppSupport::saveFpsPresets(fps);
    }
}
