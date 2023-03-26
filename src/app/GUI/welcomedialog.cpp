// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "welcomedialog.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFontMetrics>
#include <QPainter>
#include <QDir>
#include <QLabel>
#include <QFileInfo>

#include "GUI/global.h"
#include "BoxesList/OptimalScrollArea/scrollarea.h"
#include "buttonslist.h"

WelcomeDialog::WelcomeDialog(const QStringList &recentPaths,
                             const std::function<void()>& newFunc,
                             const std::function<void()>& openFunc,
                             const std::function<void(QString)>& openRecentFunc,
                             QWidget * const parent)
    : QWidget(parent)
{
    const auto thisLay = new QVBoxLayout;

    const auto mainWid = new QWidget(this);
    /*eSizesUI::widget.add(mainWid, [mainWid](const int size) {
        mainWid->setMinimumWidth(24*size);
    });*/

    setLayout(thisLay);
    thisLay->addWidget(mainWid, 0, Qt::AlignHCenter | Qt::AlignVCenter);

    const auto mainLay = new QHBoxLayout;
    mainWid->setLayout(mainLay);

    const auto sceneLay = new QVBoxLayout;

    const auto logoLabel = new QLabel(this);
    int logoSize = 128;
    logoLabel->setMinimumWidth(logoSize*2);
    logoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    logoLabel->setText(QString::fromUtf8("<div style=\"margin: 0; padding: 0; text-align: center; font-weight: normal;\">"
                                         "<p style=\"margin: 0; padding: 0;\"><img src=\":/icons/hicolor/%2x%2/apps/enve2d.png\" width=\"%2\" height=\"%2\"></p>"
                                         "<h1 style=\"margin-top: 0; padding-top: 0;\">enve2d<br><span style=\"font-size: large;\">version %1</span></h1>"
                                         "</div>")
                                        .arg(ENVE_VERSION).arg(logoSize));
    sceneLay->addWidget(logoLabel);

    const auto buttonLay = new QHBoxLayout;
    sceneLay->addLayout(buttonLay);

    const auto newButton = new QPushButton(tr("New"), this);
    newButton->setObjectName(QString::fromUtf8("welcomeDialogNew"));
    connect(newButton, &QPushButton::released, newFunc);
    buttonLay->addWidget(newButton);

    const auto openButton = new QPushButton(tr("Open..."), this);
    openButton->setObjectName(QString::fromUtf8("welcomeDialogOpen"));
    connect(openButton, &QPushButton::released, openFunc);
    buttonLay->addWidget(openButton);

    const auto homePath = QDir::homePath();

    const auto textTriggerGetter = [&](const int id) {
        const auto& path = recentPaths.at(id);
        QFileInfo fileInfo(path);
        return ButtonsList::TextTrigger {
            fileInfo.baseName(), [path, openRecentFunc]() {
            openRecentFunc(path);
        }};
    };
    const int count = qMin(recentPaths.count(), 5);
    const auto recentWidget = new ButtonsList(textTriggerGetter, count, this);

    //eSizesUI::widget.addSpacing(sceneLay);

    sceneLay->addWidget(recentWidget);

    //const auto tipWidget = new TipsWidget(this);
    //tipWidget->load();

    mainLay->addLayout(sceneLay);
    //eSizesUI::widget.addSpacing(mainLay);
    //mainLay->addWidget(tipWidget);
}

void WelcomeDialog::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(0, 0, width(), height(), Qt::black);
    p.end();
}
