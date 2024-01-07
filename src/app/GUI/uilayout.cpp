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
#include <QRegularExpression>

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

void UIDock::setPosition(const Position &pos)
{
    mPos = pos;
}

const QString UIDock::getLabel()
{
    return mLabel;
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

void UILayout::addDock(const UIDock::Position &pos,
                       const QString &label,
                       QWidget *widget,
                       const bool &showLabel,
                       const bool &showHeader,
                       const bool &darkHeader)
{
    if (!widget) { return; }
    const auto dock = new UIDock(this,
                                 widget,
                                 label,
                                 pos,
                                 showLabel,
                                 showHeader,
                                 darkHeader);
    int loc = pos;
    QString id = QString("dock_%1").arg(getIdFromLabel(label));
    const auto conf = AppSupport::getSettings("uiLayout", id);
    if (conf.isValid() && conf.toInt() != loc) { loc = conf.toInt(); }
    switch (loc) {
    case UIDock::Position::Left:
        mLeft->addWidget(dock);
        dock->setPosition(UIDock::Position::Left);
        break;
    case UIDock::Position::Right:
        mRight->addWidget(dock);
        dock->setPosition(UIDock::Position::Right);
        break;
    case UIDock::Position::Up:
        mTop->addWidget(dock);
        dock->setPosition(UIDock::Position::Up);
        break;
    case UIDock::Position::Down:
        mBottom->addWidget(dock);
        dock->setPosition(UIDock::Position::Down);
        break;
    }
    connect(dock, &UIDock::changePosition,
            this, [this, dock](const UIDock::Position &pos,
                               const UIDock::Position &trigger) {
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
        QString id = QString("dock_%1").arg(getIdFromLabel(dock->getLabel()));
        switch (pos) {
        case UIDock::Position::Left:
            if (trigger == UIDock::Position::Down && (index + 1) < count) {
                mLeft->insertWidget(index + 1, dock);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Left);
            } else if (trigger == UIDock::Position::Up && index > 0) {
                mLeft->insertWidget(index - 1, dock);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Left);
            } else if (trigger == UIDock::Position::Right) {
                mTop->addWidget(dock);
                dock->setPosition(UIDock::Position::Up);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Up);
            } else if (trigger == UIDock::Position::Left) {
                mRight->addWidget(dock);
                dock->setPosition(UIDock::Position::Right);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Right);
            }
            break;
        case UIDock::Position::Right:
            if (trigger == UIDock::Position::Down && (index + 1) < count) {
                mRight->insertWidget(index + 1, dock);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Right);
            } else if (trigger == UIDock::Position::Up && index > 0) {
                mRight->insertWidget(index - 1, dock);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Right);
            } else if (trigger == UIDock::Position::Right) {
                mLeft->addWidget(dock);
                dock->setPosition(UIDock::Position::Left);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Left);
            } else if (trigger == UIDock::Position::Left) {
                mTop->addWidget(dock);
                dock->setPosition(UIDock::Position::Up);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Up);
            }
            break;
        case UIDock::Position::Up:
            if (trigger == UIDock::Position::Right && (index + 1) < count) {
                mTop->insertWidget(index + 1, dock);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Up);
            } else if (trigger == UIDock::Position::Right && (index + 1) >= count) {
                mRight->insertWidget(index + 1, dock);
                dock->setPosition(UIDock::Position::Right);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Right);
            } else if (trigger == UIDock::Position::Left && index > 0) {
                mTop->insertWidget(index - 1, dock);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Up);
            } else if (trigger == UIDock::Position::Left && index == 0) {
                mLeft->insertWidget(index - 1, dock);
                dock->setPosition(UIDock::Position::Left);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Left);
            } else if (trigger == UIDock::Position::Down) {
                mBottom->addWidget(dock);
                dock->setPosition(UIDock::Position::Down);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Down);
            }
            break;
        case UIDock::Position::Down:
            if (trigger == UIDock::Position::Right && (index + 1) < count) {
                mBottom->insertWidget(index + 1, dock);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Down);
            } else if (trigger == UIDock::Position::Right && (index + 1) >= count) {
                mRight->insertWidget(index + 1, dock);
                dock->setPosition(UIDock::Position::Right);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Right);
            } else if (trigger == UIDock::Position::Left && index > 0) {
                mBottom->insertWidget(index - 1, dock);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Down);
            } else if (trigger == UIDock::Position::Left && index == 0) {
                mLeft->insertWidget(index - 1, dock);
                dock->setPosition(UIDock::Position::Left);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Left);
            } else if (trigger == UIDock::Position::Up) {
                mTop->addWidget(dock);
                dock->setPosition(UIDock::Position::Up);
                AppSupport::setSettings("uiLayout", id, UIDock::Position::Up);
            }
            break;
        }
    });
}

const QString UILayout::getIdFromLabel(const QString &label)
{
    QRegularExpression regex("\\s|\\W");
    QString text = label;
    return text.replace(regex, "");
}
