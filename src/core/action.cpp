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

#include "action.h"

using namespace Friction::Core;

Action::Action(const std::function<bool()>& canExecuteFunc,
               const std::function<void()>& executeFunc,
               const std::function<QString()>& textFunc,
               QObject *parent)
    : QObject(parent)
    , canExecute(canExecuteFunc)
    , execute(executeFunc)
    , text(textFunc)
{

}

Action::Action(const std::function<bool()>& canExecuteFunc,
               const std::function<void()>& executeFunc,
               const QString &textVal,
               QObject *parent)
    : Action(canExecuteFunc,
             executeFunc,
             [textVal]() { return textVal; },
             parent)
{

}

void Action::connect(QAction * const action)
{
    QObject::connect(action, &QAction::triggered, this, execute);
    QObject::connect(this, &Action::canExecuteChanged,
                     action, &QAction::setEnabled);
    QObject::connect(this, &Action::textChanged,
                     action, &QAction::setText);
    action->setText(text());
    action->setEnabled(canExecute());
}

void Action::operator()() const
{
    if (canExecute()) { execute(); }
}

void Action::raiseCanExecuteChanged()
{
    emit canExecuteChanged(canExecute());
}

void Action::raiseTextChanged()
{
    emit textChanged(text());
}

UndoableAction::UndoableAction(const std::function<bool()>& canExecuteFunc,
                               const std::function<void()>& executeFunc,
                               const std::function<QString()>& textFunc,
                               const std::function<void(const QString&)>& pushNameFunc,
                               QObject *parent)
    : Action(canExecuteFunc,
             [this, executeFunc, pushNameFunc]() {
                pushNameFunc(text());
                executeFunc();
             },
             textFunc,
             parent)
{

}

UndoableAction::UndoableAction(const std::function<bool()>& canExecuteFunc,
                               const std::function<void()>& executeFunc,
                               const QString &textVal,
                               const std::function<void(const QString&)>& pushNameFunc,
                               QObject *parent)
    : UndoableAction(canExecuteFunc, executeFunc,
                     [textVal]() { return textVal; },
                     pushNameFunc,
                     parent)
{

}
