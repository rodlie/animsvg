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

#include "markereditor.h"
#include "GUI/global.h"
#include "themesupport.h"

#include <QHBoxLayout>
#include <QPushButton>

using namespace Friction::Ui;

MarkerEditor::MarkerEditor(Canvas *scene,
                           QWidget *parent)
    : QWidget{parent}
    , mScene(scene)
    , mTree(nullptr)
{
    const auto lay = new QHBoxLayout(this);
    mTree = new QTreeWidget(this);
    lay->addWidget(mTree);
    setup();
    populate();
}

void MarkerEditor::setup()
{
    mTree->setPalette(
        ThemeSupport::getDefaultPalette(
            ThemeSupport::getThemeButtonBorderColor()));

    mTree->setHeaderLabels(QStringList() << tr("Frame") << tr("Comment"));
    mTree->setTabKeyNavigation(true);
    mTree->setAlternatingRowColors(true);
    mTree->setSortingEnabled(false);
    mTree->setHeaderHidden(false);
    mTree->setRootIsDecorated(false);
    mTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    const auto addButton = new QPushButton(QIcon::fromTheme("plus"),
                                           QString(),
                                           this);
    addButton->setObjectName("FlatButton");
    addButton->setFocusPolicy(Qt::NoFocus);

    const auto remButton = new QPushButton(QIcon::fromTheme("minus"),
                                           QString(),
                                           this);
    remButton->setObjectName("FlatButton");
    remButton->setFocusPolicy(Qt::NoFocus);

    mTree->addScrollBarWidget(remButton, Qt::AlignBottom);
    mTree->addScrollBarWidget(addButton, Qt::AlignBottom);

    eSizesUI::widget.add(addButton, [addButton,
                                     remButton](const int size) {
        addButton->setFixedHeight(size);
        remButton->setFixedHeight(size);
    });

    connect(addButton, &QPushButton::clicked,
            this, [this]() {
        auto item = new QTreeWidgetItem(mTree);
        const int frame = mScene ? mScene->getCurrentFrame() : 0;
        item->setText(1, QString::number(frame));
        item->setText(0, QString::number(frame));
        item->setData(0, Qt::UserRole, frame);
        item->setCheckState(0, Qt::Checked);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        mTree->addTopLevelItem(item);
    });

    connect(remButton, &QPushButton::pressed, this, [this]() {
        auto item = mTree->selectedItems().count() > 0 ?
                        mTree->selectedItems().at(0) : nullptr;
        if (!item) { return; }
        const int frame = item->text(0).toInt();
        if (mScene) { mScene->removeMarker(frame); }
        delete mTree->takeTopLevelItem(
            mTree->indexOfTopLevelItem(item));
    });

    connect(mTree, &QTreeWidget::itemChanged,
            this, [this](QTreeWidgetItem *item) {
        if (!mScene || item->text(0).isEmpty()) { return; }
        const int oframe = item->data(0, Qt::UserRole).toInt();
        const int frame = item->text(0).toInt();
        const QString title = item->text(1);
        const bool checked = item->checkState(0) == Qt::Checked;
        if (frame != oframe && duplicate(item, frame)) {
            mTree->blockSignals(true);
            item->setText(0, QString::number(oframe));
            mTree->blockSignals(false);
            return;
        }
        if (mScene->hasMarker(frame)) {
            mScene->editMarker(frame, title,
                               checked ? true : false);
        } else {
            if (frame != oframe) {
                mScene->removeMarker(oframe);
            }
            mScene->setMarker(title, frame);
        }
        mScene->updateMarkers();

        mTree->blockSignals(true);
        item->setData(0, Qt::UserRole, frame);
        mTree->blockSignals(false);
    });
}

void MarkerEditor::populate()
{
    if (!mScene) { return; }
    mTree->clear();
    const auto markers = mScene->getMarkers();
    mTree->blockSignals(true);
    for (const auto &marker : markers) {
        auto item = new QTreeWidgetItem(mTree);
        item->setText(1, marker.title);
        item->setText(0, QString::number(marker.frame));
        item->setData(0, Qt::UserRole, marker.frame);
        item->setCheckState(0, marker.enabled ? Qt::Checked : Qt::Unchecked);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        mTree->addTopLevelItem(item);
    }
    mTree->blockSignals(false);
}

bool MarkerEditor::duplicate(QTreeWidgetItem *item,
                             const int &frame)
{
    for (int i = 0; i < mTree->topLevelItemCount(); i++) {
        auto tItem = mTree->topLevelItem(i);
        if (!tItem) { continue; }
        if (tItem->text(0).toInt() == frame) {
            if (tItem != item) { return true ;}
        }
    }
    return false;
}
