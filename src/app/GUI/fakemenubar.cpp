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

#include "fakemenubar.h"

#include "GUI/global.h"

#include <QToolButton>
#include <QPushButton>
#include <QFrame>

class QActionButton : public QPushButton {
public:
    explicit QActionButton(QWidget *parent = nullptr) :
        QPushButton(parent) {
        /*eSizesUI::widget.add(this, [this](const int size) {
            setFixedHeight(size - 1);
        });*/
        //setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setObjectName("QActionButton");
        //setStyleSheet("QPushButton::menu-indicator { width:0px; }");
    }

    void setAction(QAction *action) {
        if(mAction == action) return;
        if(mAction) {
            disconnect(mAction, &QAction::changed,
                       this, &QActionButton::updateButtonStatusFromAction);
            disconnect(this, &QActionButton::clicked,
                       mAction, &QAction::trigger);
        }

        mAction = action;

        updateButtonStatusFromAction();

        connect(mAction, &QAction::changed,
                this, &QActionButton::updateButtonStatusFromAction);
        connect(this, &QActionButton::clicked,
                mAction, &QAction::trigger);
    }
private:
    void updateButtonStatusFromAction() {
        if(!mAction) return;
        const auto icon = mAction->icon();
        if(!icon.isNull()) setIcon(icon);
        else setText(mAction->text());
        setStatusTip(mAction->statusTip());
        setToolTip(mAction->toolTip());
        setEnabled(mAction->isEnabled());
        setCheckable(mAction->isCheckable());
        setChecked(mAction->isChecked());
    }

    QAction *mAction = nullptr;
};

FakeMenuBar::FakeMenuBar(QWidget* const parent) :
    QWidget(parent) {
    setObjectName("menuBarWidget");
    const auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mActsLayout = new QHBoxLayout();
    mActsLayout->setAlignment(Qt::AlignLeft);
    mActsLayout->setContentsMargins(0, 0, 0, 0);
    mActsLayout->setSpacing(0);

    mainLayout->addLayout(mActsLayout);

    setLayout(mainLayout);

    /*eSizesUI::widget.add(this, [this](const int size) {
        setFixedHeight(size);
    });*/
}

void FakeMenuBar::addSeparator() {
    const auto line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Plain);
    mActsLayout->addWidget(line);
}

QMenu* FakeMenuBar::addMenu(const QString &title) {
    const auto menu = new QMenu(title, this);
    addMenu(menu);
    return menu;
}

QMenu* FakeMenuBar::addMenu(const QIcon &icon, const QString &title) {
    const auto menu = new QMenu(this);
    menu->setIcon(icon);
    menu->menuAction()->setIconText(title);
    addMenu(menu);
    return menu;
}

void FakeMenuBar::addMenu(QMenu * const menu) {
    const auto button = new QActionButton(this);
    button->setAction(menu->menuAction());
    button->setMenu(menu);
    mActsLayout->addWidget(button);
}

QAction* FakeMenuBar::addAction(const QString &title) {
    const auto action = new QAction(title, this);
    addAction(action);
    return action;
}


QAction* FakeMenuBar::addAction(const QIcon &icon, const QString &title) {
    const auto action = new QAction(this);
    action->setIcon(icon);
    action->setIconText(title);
    addAction(action);
    return action;
}

void FakeMenuBar::addAction(QAction * const action) {
    const auto button = new QActionButton(this);
    button->setAction(action);
    mActsLayout->addWidget(button);
}
