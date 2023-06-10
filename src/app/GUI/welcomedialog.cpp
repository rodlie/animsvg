/*
# enve2d - https://github.com/enve2d
#
# Copyright (c) enve2d developers
# Copyright (C) 2016-2020 Maurycy Liebner
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
*/

#include "welcomedialog.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFontMetrics>
#include <QPainter>
#include <QDir>
#include <QLabel>
#include <QFileInfo>
#include <QToolButton>
#include <QAction>

#include "GUI/global.h"
#include "BoxesList/OptimalScrollArea/scrollarea.h"
#include "buttonslist.h"
#include "appsupport.h"

WelcomeDialog::WelcomeDialog(const QStringList &recentPaths,
                             const std::function<void()> &newFunc,
                             const std::function<void()> &openFunc,
                             const std::function<void(QString)> &openRecentFunc,
                             QWidget * const parent)
    : QWidget(parent)
{
    setObjectName(QString::fromUtf8("welcomeDialog"));

    const auto thisLay = new QVBoxLayout;

    const auto mainWid = new QWidget(this);

    setLayout(thisLay);
    thisLay->addWidget(mainWid, 0, Qt::AlignHCenter | Qt::AlignVCenter);

    const auto mainLay = new QHBoxLayout;
    mainWid->setLayout(mainLay);

    const auto sceneLay = new QVBoxLayout;

    const auto logoLabel = new QLabel(this);
    int logoSize = 96;
    logoLabel->setOpenExternalLinks(false);
    logoLabel->setMinimumWidth(logoSize*2);
    logoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    logoLabel->setText(QString::fromUtf8("<div style=\"margin: 0; padding: 0; text-align: center; font-weight: normal;\">"
                                         "<p style=\"margin: 0; padding: 0;\"><img src=\":/icons/hicolor/%2x%2/apps/%4.png\" width=\"%2\" height=\"%2\"></p>"
                                         "<h1 style=\"font-weight: normal; margin-top: 0; padding-top: 0;\">%3<br><span style=\"font-size: large;\">%1</span></h1>"
                                         "</div>")
                                        .arg(AppSupport::getAppVersion(false),
                                             QString::number(logoSize),
                                             AppSupport::getAppDisplayName(),
                                             AppSupport::getAppName()));
    sceneLay->addWidget(logoLabel);

    const auto buttonLay = new QHBoxLayout;
    sceneLay->addLayout(buttonLay);

    const auto newButton = new QPushButton(tr("New"), this);
    connect(newButton, &QPushButton::released, newFunc);
    buttonLay->addWidget(newButton);

    const auto openButton = new QToolButton(this);
    openButton->setText(tr("Open"));
    openButton->setSizePolicy(QSizePolicy::Preferred,
                              QSizePolicy::Expanding);
    openButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    openButton->setPopupMode(QToolButton::MenuButtonPopup);
    connect(openButton, &QPushButton::released, openFunc);

    for(int i = 0; i < recentPaths.size(); ++i) {
        const QString path = recentPaths.at(i);
        if (!QFile::exists(path)) { continue; }
        QAction *action = new QAction(QIcon::fromTheme("friction"),
                                      path,
                                      this);
        connect(action,
                &QAction::triggered,
                this,
                [openRecentFunc, action]() {
            openRecentFunc(action->text());
        });
        openButton->addAction(action);
    }

    buttonLay->addWidget(openButton);
    mainLay->addLayout(sceneLay);
}

void WelcomeDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(0, 0, width(), height(), Qt::black);
    p.end();
}
