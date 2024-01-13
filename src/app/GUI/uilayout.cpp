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

#include "uilayout.h"
#include "appsupport.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QDebug>

UIDock::UIDock(QWidget *parent,
               QWidget *widget,
               const QString &label,
               const Position &pos,
               const bool &showLabel,
               const bool &showHeader,
               const bool &darkHeader)
    : QWidget{parent}
    , mWidget(widget)
    , mLabel(label)
    , mPos(pos)
    , mIndex(-1)
{
    setObjectName(mLabel);

    setContentsMargins(0, 0, 0, 0);
    const auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    if (showHeader) {
        const auto headerWidget = new QWidget(this);
        const auto headerLayout = new QHBoxLayout(headerWidget);

        if (darkHeader) {
            headerWidget->setAutoFillBackground(true);
            headerWidget->setPalette(AppSupport::getDarkPalette());
        }

        headerWidget->setContentsMargins(0, 0, 0, 0);
        headerWidget->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Fixed);
        headerLayout->setContentsMargins(0, 0, 0, 0);
        headerLayout->setMargin(0);
        headerLayout->setSpacing(0);

        const auto leftButton = new QPushButton(this);
        const auto rightButton = new QPushButton(this);
        const auto upButton = new QPushButton(this);
        const auto downButton = new QPushButton(this);

        leftButton->setIcon(QIcon::fromTheme("leftarrow"));
        rightButton->setIcon(QIcon::fromTheme("rightarrow"));
        upButton->setIcon(QIcon::fromTheme("uparrow"));
        downButton->setIcon(QIcon::fromTheme("downarrow"));

        leftButton->setFocusPolicy(Qt::NoFocus);
        rightButton->setFocusPolicy(Qt::NoFocus);
        upButton->setFocusPolicy(Qt::NoFocus);
        downButton->setFocusPolicy(Qt::NoFocus);

        leftButton->setObjectName("FlatButton");
        rightButton->setObjectName("FlatButton");
        upButton->setObjectName("FlatButton");
        downButton->setObjectName("FlatButton");

        leftButton->setToolTip(tr("Left"));
        rightButton->setToolTip(tr("Right"));
        upButton->setToolTip(tr("Up"));
        downButton->setToolTip(tr("Down"));

        leftButton->setSizePolicy(QSizePolicy::Fixed,
                                  QSizePolicy::Fixed);
        rightButton->setSizePolicy(QSizePolicy::Fixed,
                                   QSizePolicy::Fixed);
        upButton->setSizePolicy(QSizePolicy::Fixed,
                                QSizePolicy::Fixed);
        downButton->setSizePolicy(QSizePolicy::Fixed,
                                  QSizePolicy::Fixed);

        headerLayout->addWidget(leftButton);
        headerLayout->addWidget(rightButton);
        headerLayout->addStretch();

        if (showLabel) {
            const auto dockLabel = new QLabel(this);
            dockLabel->setText(mLabel);
            headerLayout->addWidget(dockLabel);
            headerLayout->addStretch();
        }

        headerLayout->addWidget(upButton);
        headerLayout->addWidget(downButton);

        mainLayout->addWidget(headerWidget);

        connect(leftButton, &QPushButton::clicked,
                this, [this]() { emit changePosition(mPos, Position::Left); });
        connect(rightButton, &QPushButton::clicked,
                this, [this]() { emit changePosition(mPos, Position::Right); });
        connect(upButton, &QPushButton::clicked,
                this, [this]() { emit changePosition(mPos, Position::Up); });
        connect(downButton, &QPushButton::clicked,
                this, [this]() { emit changePosition(mPos, Position::Down); });
    }
    mainLayout->addWidget(mWidget);
}

UIDock::~UIDock()
{
    writeSettings();
}

void UIDock::setPosition(const Position &pos)
{
    mPos = pos;
}

UIDock::Position UIDock::getPosition()
{
    return mPos;
}

void UIDock::setIndex(const int &index)
{
    mIndex = index;
}

int UIDock::getIndex()
{
    return mIndex;
}

const QString UIDock::getLabel()
{
    return mLabel;
}

const QString UIDock::getId()
{
    return AppSupport::filterTextAZW(mLabel);
}

void UIDock::writeSettings()
{
    qDebug() << "uidock write settings" << mLabel << mPos << mIndex;
    QSettings settings;
    settings.beginGroup("uiLayout");
    settings.setValue(QString("pos_%1").arg(getId()), mPos);
    settings.setValue(QString("index_%1").arg(getId()), mIndex);
    settings.endGroup();
}

UILayout::UILayout(QWidget *parent)
    : QSplitter{parent}
    , mLeft(nullptr)
    , mMiddle(nullptr)
    , mRight(nullptr)
    , mTop(nullptr)
    , mBottom(nullptr)
{
    setOrientation(Qt::Horizontal);

    mLeft = new QSplitter(this);
    mLeft->setOrientation(Qt::Vertical);
    addWidget(mLeft);

    mMiddle = new QSplitter(this);
    mMiddle->setOrientation(Qt::Vertical);
    addWidget(mMiddle);

    mRight = new QSplitter(this);
    mRight->setOrientation(Qt::Vertical);
    addWidget(mRight);

    mTop = new QSplitter(this);
    mTop->setOrientation(Qt::Horizontal);
    mMiddle->addWidget(mTop);

    mBottom = new QSplitter(this);
    mBottom->setOrientation(Qt::Horizontal);
    mMiddle->addWidget(mBottom);
}

UILayout::~UILayout()
{
    updateDocks();
    writeSettings();
}

void UILayout::readSettings()
{
    QSettings settings;
    settings.beginGroup("uiLayout");
    restoreState(AppSupport::getSettings("uiLayout",
                                         "uiMain").toByteArray());
    mLeft->restoreState(AppSupport::getSettings("uiLayout",
                                                "uiLeft").toByteArray());
    mMiddle->restoreState(AppSupport::getSettings("uiLayout",
                                                  "uiMiddle").toByteArray());
    mRight->restoreState(AppSupport::getSettings("uiLayout",
                                                 "uiRight").toByteArray());
    mTop->restoreState(AppSupport::getSettings("uiLayout",
                                               "uiTop").toByteArray());
    mBottom->restoreState(AppSupport::getSettings("uiLayout",
                                                  "uiBottom").toByteArray());
    settings.endGroup();
}

void UILayout::writeSettings()
{
    qDebug() << "uilayout write settings";
    QSettings settings;
    settings.beginGroup("uiLayout");
    settings.setValue("uiMain", saveState());
    settings.setValue("uiLeft", mLeft->saveState());
    settings.setValue("uiMiddle", mMiddle->saveState());
    settings.setValue("uiRight", mRight->saveState());
    settings.setValue("uiTop", mTop->saveState());
    settings.setValue("uiBottom", mBottom->saveState());
    settings.endGroup();
}

void UILayout::addDocks(std::vector<Item> items)
{
    if (items.size() <= 0) { return; }
    std::vector<Item> leftDock, rightDock, topDock, bottomDock;

    for (auto item : items) {
        if (!item.widget) { continue; }
        qDebug() << "==> new layout item" << item.label;

        QString keyPos = QString("pos_%1").arg(AppSupport::filterTextAZW(item.label));
        QString keyIndex = QString("index_%1").arg(AppSupport::filterTextAZW(item.label));

        const auto confIndex = AppSupport::getSettings("uiLayout", keyIndex);
        if (confIndex.isValid()) { item.index = confIndex.toInt(); }

        const auto confPos = AppSupport::getSettings("uiLayout", keyPos);
        if (confPos.isValid()) { item.pos = confPos.toInt(); }

        qDebug() << "index" << item.index << "pos" << item.pos;

        switch (item.pos) {
        case UIDock::Position::Left:
            leftDock.push_back(item);
            break;
        case UIDock::Position::Right:
            rightDock.push_back(item);
            break;
        case UIDock::Position::Up:
            topDock.push_back(item);
            break;
        case UIDock::Position::Down:
            bottomDock.push_back(item);
            break;
        }
    }

    if (leftDock.size() > 0) { std::sort(leftDock.begin(), leftDock.end()); }
    if (rightDock.size() > 0) { std::sort(rightDock.begin(), rightDock.end()); }
    if (topDock.size() > 0) { std::sort(topDock.begin(), topDock.end()); }
    if (bottomDock.size() > 0) { std::sort(bottomDock.begin(), bottomDock.end()); }

    for (const auto &item : leftDock) {
        qDebug() << "==> left dock" << item.label << item.index;
        addDock(item);
    }
    for (const auto &item : rightDock) {
        qDebug() << "==> right dock" << item.label << item.index;
        addDock(item);
    }
    for (const auto &item : topDock) {
        qDebug() << "==> top dock" << item.label << item.index;
        addDock(item);
    }
    for (const auto &item : bottomDock) {
        qDebug() << "==> bottom dock" << item.label << item.index;
        addDock(item);
    }

    updateDocks();
    readSettings();
}

void UILayout::addDock(const Item &item)
{
    if (!item.widget) { return; }
    qDebug() << "add dock for real" << item.pos << item.index << item.label << item.showLabel << item.showHeader << item.darkHeader;
    const auto dock = new UIDock(this,
                                 item.widget,
                                 item.label,
                                 static_cast<UIDock::Position>(item.pos),
                                 item.showLabel,
                                 item.showHeader,
                                 item.darkHeader);
    switch (item.pos) {
    case UIDock::Position::Left:
        mLeft->addWidget(dock);
        break;
    case UIDock::Position::Right:
        mRight->addWidget(dock);
        break;
    case UIDock::Position::Up:
        mTop->addWidget(dock);
        break;
    case UIDock::Position::Down:
        mBottom->addWidget(dock);
        break;
    }
    connectDock(dock);
}

void UILayout::connectDock(UIDock *dock)
{
    if (!dock) { return; }
    connect(dock,
            &UIDock::changePosition,
            this,
            [this, dock](const UIDock::Position &pos,
                         const UIDock::Position &trigger)
            {
                int index = -1;
                int count = -1;
                switch (pos) {
                case UIDock::Position::Left:
                    index = mLeft->indexOf(dock);
                    count = mLeft->count();
                    break;
                case UIDock::Position::Right:
                    index = mRight->indexOf(dock);
                    count = mRight->count();
                    break;
                case UIDock::Position::Up:
                    index = mTop->indexOf(dock);
                    count = mTop->count();
                    break;
                case UIDock::Position::Down:
                    index = mBottom->indexOf(dock);
                    count = mBottom->count();
                    break;
                }
                if (index < 0 || count <= 0) { return; }
                switch (pos) {
                case UIDock::Position::Left:
                    if (trigger == UIDock::Position::Down && (index + 1) < count) {
                        mLeft->insertWidget(index + 1, dock);
                    } else if (trigger == UIDock::Position::Up && index > 0) {
                        mLeft->insertWidget(index - 1, dock);
                    } else if (trigger == UIDock::Position::Right) {
                        mTop->addWidget(dock);
                    } else if (trigger == UIDock::Position::Left) {
                        mRight->addWidget(dock);
                    }
                    break;
                case UIDock::Position::Right:
                    if (trigger == UIDock::Position::Down && (index + 1) < count) {
                        mRight->insertWidget(index + 1, dock);
                    } else if (trigger == UIDock::Position::Up && index > 0) {
                        mRight->insertWidget(index - 1, dock);
                    } else if (trigger == UIDock::Position::Right) {
                        mLeft->addWidget(dock);
                    } else if (trigger == UIDock::Position::Left) {
                        mTop->addWidget(dock);
                    }
                    break;
                case UIDock::Position::Up:
                    if (trigger == UIDock::Position::Right && (index + 1) < count) {
                        mTop->insertWidget(index + 1, dock);
                    } else if (trigger == UIDock::Position::Right && (index + 1) >= count) {
                        mRight->insertWidget(index + 1, dock);
                    } else if (trigger == UIDock::Position::Left && index > 0) {
                        mTop->insertWidget(index - 1, dock);
                    } else if (trigger == UIDock::Position::Left && index == 0) {
                        mLeft->insertWidget(index - 1, dock);
                    } else if (trigger == UIDock::Position::Down) {
                        mBottom->addWidget(dock);
                    }
                    break;
                case UIDock::Position::Down:
                    if (trigger == UIDock::Position::Right && (index + 1) < count) {
                        mBottom->insertWidget(index + 1, dock);
                    } else if (trigger == UIDock::Position::Right && (index + 1) >= count) {
                        mRight->insertWidget(index + 1, dock);
                    } else if (trigger == UIDock::Position::Left && index > 0) {
                        mBottom->insertWidget(index - 1, dock);
                    } else if (trigger == UIDock::Position::Left && index == 0) {
                        mLeft->insertWidget(index - 1, dock);
                    } else if (trigger == UIDock::Position::Up) {
                        mTop->addWidget(dock);
                    }
                    break;
                }
                updateDocks();
            });
}

void UILayout::updateDock(QSplitter *container,
                          const UIDock::Position &pos)
{
    if (!container) { return; }
    container->setVisible(container->count() > 0);
    for (int i = 0; i < container->count(); ++i) {
        UIDock *dock = qobject_cast<UIDock*>(container->widget(i));
        if (!dock) { continue; }
        dock->setPosition(pos);
        dock->setIndex(container->indexOf(dock));
        qDebug() << "update dock" << dock->getLabel() << dock->getPosition() << dock->getIndex();
    }
}

void UILayout::updateDocks()
{
    qDebug() << "==> update docks";
    updateDock(mLeft, UIDock::Position::Left);
    updateDock(mRight, UIDock::Position::Right);
    updateDock(mTop, UIDock::Position::Up);
    updateDock(mBottom, UIDock::Position::Down);
}
