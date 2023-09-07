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

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#include "welcomedialog.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QLabel>
#include <QToolButton>

#include "appsupport.h"

WelcomeDialog::WelcomeDialog(QMenu *recentMenu,
                             const std::function<void()> &newFunc,
                             const std::function<void()> &openFunc,
                             QWidget * const parent)
    : QWidget(parent)
{
    setObjectName(QString::fromUtf8("welcomeDialog"));

    const auto thisLay = new QVBoxLayout(this);

    const auto mainWid = new QWidget(this);
    const auto mainLay = new QHBoxLayout(mainWid);

    const auto sceneWid = new QWidget(this);
    const auto sceneLay = new QVBoxLayout(sceneWid);

    int logoSize = 96;
    const auto logoLabel = new QLabel(this);
    logoLabel->setOpenExternalLinks(false);
    logoLabel->setMinimumWidth(logoSize*2);
    logoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    logoLabel->setText(QString::fromUtf8("<div style=\"margin: 0; padding: 0; text-align: center; font-weight: normal;\">"
                                         "<p style=\"margin: 0; padding: 0;\"><img src=\":/icons/hicolor/%2x%2/apps/%4.png\" width=\"%2\" height=\"%2\"></p>"
                                         "<h1 style=\"font-weight: normal; margin-top: 0; padding-top: 0;\">%3<br><span style=\"font-size: x-large;\">%1</span></h1>"
                                         "</div>")
                                        .arg(AppSupport::getAppVersion(false),
                                             QString::number(logoSize),
                                             AppSupport::getAppDisplayName(),
                                             AppSupport::getAppName()));

    const auto buttonWid = new QWidget(this);
    const auto buttonLay = new QHBoxLayout(buttonWid);

    const auto newButton = new QPushButton(/*QIcon::fromTheme("file_blank"),*/
                                           tr("New"),
                                           this);
    newButton->setObjectName("WelcomeNewButton");
    newButton->setSizePolicy(QSizePolicy::Preferred,
                             QSizePolicy::Expanding);
    connect(newButton, &QPushButton::released, newFunc);

    const auto openButton = new QToolButton(this);
    openButton->setObjectName("WelcomeOpenButton");
    openButton->setText(tr("Open"));
    //openButton->setIcon(QIcon::fromTheme("file_folder"));
    openButton->setSizePolicy(QSizePolicy::Preferred,
                              QSizePolicy::Expanding);
    //openButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    openButton->setPopupMode(QToolButton::MenuButtonPopup);
    openButton->setMenu(recentMenu);
    connect(openButton, &QPushButton::released, openFunc);

    thisLay->addWidget(mainWid, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    sceneLay->addWidget(logoLabel);
    sceneLay->addWidget(buttonWid);
    buttonLay->addWidget(newButton);
    buttonLay->addWidget(openButton);
    mainLay->addWidget(sceneWid);
}

void WelcomeDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(0, 0, width(), height(), Qt::black);
    p.end();
}
