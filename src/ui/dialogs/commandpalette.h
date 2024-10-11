/*
#
# Friction - https://friction.graphics
#
# Copyright (c) Ole-André Rodlie and contributors
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

#include "Private/document.h"
#include "ui_global.h"

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QList>
#include <QEvent>
#include <QLabel>

class UI_EXPORT CommandPalette : public QDialog
{
    Q_OBJECT
public:
    explicit CommandPalette(Document &document,
                            QWidget *parent = nullptr);

private:
    Document& mDocument;
    QLineEdit *mUserInput;
    QLabel *mLabel;
    QListWidget *mSuggestions;
    int mHistoryCounter;

    bool skipItem(const QString &title,
                  const QString &input);
    bool isCmd(const QString &input);
    void parseCmd(const QString &input);
    bool showToolTip(const QString& input);
    void updateToolTip(const QString &input);
    void getHistory(const bool append = false);
    void appendHistory(const QString &input);
    QAction *getAction(const int index);
    bool isIntOrDouble(const QString &input);
    bool isActionCmd(const QString &input);

protected:
    bool eventFilter(QObject* obj, QEvent *e) override;
};

#endif // COMMANDPALETTE_H
