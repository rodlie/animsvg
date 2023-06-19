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

#include "tipswidget.h"

#include "Private/esettings.h"
#include "GUI/global.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

TipsWidget::TipsWidget(QWidget* const parent) : QWidget(parent) {
    const auto mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    const auto titleLayout = new QHBoxLayout;
    titleLayout->setContentsMargins(0, 0, 0, 0);

    const QString iconsDir = eSettings::sIconsDir();

    const QIcon leftIcon(iconsDir + "/left-arrow.png");
    const auto leftButton = new QPushButton(leftIcon, "");
    leftButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(leftButton, &QPushButton::released,
            this, &TipsWidget::showPreviousTip);
    titleLayout->addWidget(leftButton);

    mNumber = new QLabel(this);
    mNumber->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(mNumber);

    const QIcon rightIcon(iconsDir + "/right-arrow.png");
    const auto rightButton = new QPushButton(rightIcon, "");
    rightButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(rightButton, &QPushButton::released,
            this, &TipsWidget::showNextTip);
    titleLayout->addWidget(rightButton);

    mainLayout->addLayout(titleLayout);
    eSizesUI::widget.addSpacing(mainLayout);

    mTitle = new QLabel(this);
    mainLayout->addWidget(mTitle, 0, Qt::AlignTop);
    eSizesUI::widget.addHalfSpacing(mainLayout);

    mGif = new QLabel(this);

    mGif->setMovie(&mMovie);
    mainLayout->addWidget(mGif, 0, Qt::AlignTop);
    eSizesUI::widget.addHalfSpacing(mainLayout);

    mDesc = new QLabel(this);
    mDesc->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    mDesc->setWordWrap(true);
    mainLayout->addWidget(mDesc, 0, Qt::AlignTop);
    eSizesUI::widget.addHalfSpacing(mainLayout);

    setLayout(mainLayout);

    eSizesUI::widget.add(this, [this](const int size) {
        setFixedWidth(12*size);
        mGif->setFixedWidth(12*size);
        mGif->setFixedHeight(6*size);
        mDesc->setFixedHeight(6*size);
    });
}

void TipsWidget::load() {
    const QDir tipsDir(":/tips");
    if(!tipsDir.exists()) return;
    const QDir::Filters filter = QDir::NoDotAndDotDot | QDir::AllEntries;
    const QFileInfoList entryList = tipsDir.entryInfoList(filter, QDir::Name);
    std::map<QString, Tip> tips;
    for(const QFileInfo& fileInfo : entryList) {
        if(!fileInfo.isFile()) continue;
        const QString name = fileInfo.completeBaseName();
        const QString path = fileInfo.filePath();
        const QString suffix = fileInfo.suffix();

        auto& tip = tips[name];
        if(suffix == "txt") {
            QFile file(path);
            const bool succ = file.open(QIODevice::ReadOnly);
            if(!succ) continue;
            QTextStream stream(&file);
            tip.fTitle = stream.readLine();
            while(!stream.atEnd()) {
                if(!tip.fDesc.isEmpty()) tip.fDesc += "<br/>";
                tip.fDesc += stream.readLine();
            }
            file.close();
        } else if(suffix == "gif") {
            tip.fGifPath = path;
        }
    }

    mTips.clear();
    for(const auto& tip : tips) {
        mTips << tip.second;
    }
}

void TipsWidget::showPreviousTip() {
    int tip = mCurrentTip - 1;
    if(tip < 0) tip = mTips.count() - 1;
    setTip(tip);
}

void TipsWidget::showNextTip() {
    int tip = mCurrentTip + 1;
    if(tip >= mTips.count()) tip = 0;
    setTip(tip);
}

void TipsWidget::showRandomTip() {
    if(mTips.isEmpty()) return;
    setTip(qrand() % mTips.count());
}

void TipsWidget::showEvent(QShowEvent* const e) {
    QWidget::showEvent(e);
    showRandomTip();
}

void TipsWidget::setTip(const int id) {
    if(id < 0 || id >= mTips.count()) return;
    mCurrentTip = id;
    setTip(mTips.at(id));
}

void TipsWidget::setTip(const Tip& tip) {
    mNumber->setText(QString::number(mCurrentTip + 1) + "/" +
                     QString::number(mTips.count()));
    mTitle->setText("<h2>" + tip.fTitle + "</h2>");
    mMovie.stop();
    mMovie.setFileName(tip.fGifPath);
    mMovie.setScaledSize(mGif->size());
    mMovie.start();
    mDesc->setText(tip.fDesc);
}
