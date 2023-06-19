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

#ifndef NOSHORTCUTACTION_H
#define NOSHORTCUTACTION_H
#include <QAction>

class NoShortcutAction : public QAction {
public:
    template <typename T1, typename T2>
    NoShortcutAction(const QString &text,
                     const T1 * const receiver,
                     const T2 &member,
                     const QKeySequence &shortcut = 0,
                     QObject * const parent = nullptr) :
        QAction(text, parent) {
        connect(this, &QAction::triggered, receiver, member);
        setShortcut(shortcut);
    }

    NoShortcutAction(const QString &text,
                     const QKeySequence &shortcut = 0,
                     QObject * const parent = nullptr) :
        QAction(text, parent) {
        setShortcut(shortcut);
    }
protected:
    bool event(QEvent *e);
};

#endif // NOSHORTCUTACTION_H
