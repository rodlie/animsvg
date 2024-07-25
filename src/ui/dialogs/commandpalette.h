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

#ifndef COMMANDPALETTE_H
#define COMMANDPALETTE_H

#include "ui_global.h"

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QList>
#include <QEvent>

class UI_EXPORT CommandPalette : public QDialog
{
public:
    explicit CommandPalette(QWidget *parent = nullptr);

private:
    QLineEdit *mUserInput;
    QListWidget *mSuggestions;

protected:
    bool eventFilter(QObject* obj, QEvent *e) override;
};

#endif // COMMANDPALETTE_H
