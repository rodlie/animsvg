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

#include "toolbar.h"
#include "GUI/global.h"
#include "appsupport.h"

#include <QMenu>

Friction::Ui::ToolBar::ToolBar(const QString &title,
                               QWidget *parent,
                               const bool &iconsOnly)
    : QToolBar(title, parent)
    , mIconsOnly(iconsOnly)
{
    setup();
}

Friction::Ui::ToolBar::ToolBar(const QString &title,
                               const QString &objectName,
                               QWidget *parent,
                               const bool iconsOnly)
    : QToolBar(title, parent)
    , mIconsOnly(iconsOnly)
{
    setObjectName(objectName);
    setup();
}

void Friction::Ui::ToolBar::setup()
{
#ifdef Q_OS_MAC
    mToolbar->setStyleSheet(QString("font-size: %1pt;").arg(font().pointSize()));
#endif

    if (objectName().isEmpty()) {
        setObjectName(windowTitle().replace(" ", "").simplified());
    }

    setMovable(AppSupport::getSettings("ui",
                                       QString("ToolBarMovable_%1").arg(objectName()),
                                       false).toBool());
    setFocusPolicy(Qt::NoFocus);

    setToolButtonStyle(mIconsOnly ? Qt::ToolButtonIconOnly : Qt::ToolButtonTextBesideIcon);
    connect(this, &QToolBar::orientationChanged,
            this, [this]() {
        if (mIconsOnly) { return; }
        setToolButtonStyle(orientation() == Qt::Horizontal ?
                               Qt::ToolButtonTextBesideIcon :
                               Qt::ToolButtonIconOnly);
    });

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested,
            this, &ToolBar::showContextMenu);

    eSizesUI::widget.add(this, [this](const int size) {
        this->setIconSize({size, size});
    });
}

void Friction::Ui::ToolBar::showContextMenu(const QPoint &pos)
{
    QMenu menu(this);

    const auto act = menu.addAction(QIcon::fromTheme("window"), windowTitle());
    act->setEnabled(false);
    menu.addSeparator();

    menu.addAction(QIcon::fromTheme(isMovable() ? "locked" : "unlocked"),
                   tr(isMovable() ? "Lock" : "Unlock"),
                   this, [this]() {
        setMovable(!isMovable());
        AppSupport::setSettings("ui",
                                QString("ToolBarMovable_%1").arg(objectName()),
                                isMovable());
    });

    menu.exec(mapToGlobal(pos));
}

QAction *Friction::Ui::ToolBar::addSpacer(QWidget *widget)
{
    if (!widget) { return nullptr; }
    widget->setSizePolicy(QSizePolicy::Expanding,
                          QSizePolicy::Minimum);
    return addWidget(widget);
}
