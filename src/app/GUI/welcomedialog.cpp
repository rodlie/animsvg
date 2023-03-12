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
#include <QFrame>
#include <QLabel>

#include "GUI/global.h"
#include "BoxesList/OptimalScrollArea/scrollarea.h"
#include "buttonslist.h"
#include "tipswidget.h"

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

    const auto headerLay = new QHBoxLayout;
    mainLay->addLayout(headerLay);

    const auto headerFrame = new QFrame(this);
    headerLay->addWidget(headerFrame);

    const auto logoLabel = new QLabel(this);
    logoLabel->setMinimumWidth(256);
    logoLabel->setMaximumWidth(256);
    logoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    logoLabel->setText(QString::fromUtf8("<div style=\"text-align: center; font-weight: normal;\">"
                                         "<p><img src=\":/icons/hicolor/256x256/apps/enve2d.png\" width=\"256\" height=\"256\"></p>"
                                         "<h1>enve2d<br><span style=\"font-size: large;\">version %1</span></h1>"
                                         "<p style=\"font-size: small;\">Copyright &copy; 2023 enve2d developers<br>Copyright &copy; 2016-2022 Maurycy Liebner</p>"
                                         "</div>")
                                        .arg(ENVE_VERSION));
    headerLay->addWidget(logoLabel);
    headerLay->addStretch();

    const auto buttonLay = new QHBoxLayout;
    sceneLay->addLayout(buttonLay);

    const auto newButton = new QPushButton("New", this);
    connect(newButton, &QPushButton::released, newFunc);
    buttonLay->addWidget(newButton);

    const auto openButton = new QPushButton("Open...", this);
    connect(openButton, &QPushButton::released, openFunc);
    buttonLay->addWidget(openButton);

    const auto homePath = QDir::homePath();

    const auto textTriggerGetter = [&](const int id) {
        const auto& path = recentPaths.at(id);
        QString ttPath = path;
        if(ttPath.left(homePath.count()) == homePath) {
            ttPath = "~" + ttPath.mid(homePath.count());
        }
        return ButtonsList::TextTrigger{
            ttPath, [path, openRecentFunc]() {
            openRecentFunc(path);
        }};
    };
    const int count = qMin(recentPaths.count(), 11);
    const auto recentWidget = new ButtonsList(textTriggerGetter, count, this);

    eSizesUI::widget.addSpacing(sceneLay);

    sceneLay->addWidget(recentWidget);

    const auto tipWidget = new TipsWidget(this);
    tipWidget->load();

    mainLay->addLayout(sceneLay);
    eSizesUI::widget.addSpacing(mainLay);
    mainLay->addWidget(tipWidget);
}

void WelcomeDialog::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(0, 0, width(), height(), Qt::black);
    p.end();
}
