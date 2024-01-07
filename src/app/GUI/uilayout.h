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

#ifndef UILAYOUT_H
#define UILAYOUT_H

#include <QWidget>
#include <QSplitter>

class UIDock : public QWidget
{
    Q_OBJECT

public:
    enum Position
    {
        Left,
        Right,
        Up,
        Down
    };
    explicit UIDock(QWidget *parent = nullptr,
                    QWidget *widget = nullptr,
                    const QString &label = tr("Dock"),
                    const Position &pos = Position::Left,
                    const bool &showLabel = true,
                    const bool &showHeader = true,
                    const bool &darkHeader = false);
    void setPosition(const Position &pos);
    const QString getLabel();

signals:
    void changePosition(const Position &pos,
                        const Position &trigger);

private:
    QWidget *mWidget;
    QString mLabel;
    Position mPos;
};

class UILayout : public QSplitter
{
    Q_OBJECT

public:
    explicit UILayout(QWidget *parent = nullptr);
    void readSettings();
    void writeSettings();
    void addDock(const UIDock::Position &pos,
                 const QString &label,
                 QWidget *widget,
                 const bool &showLabel = true,
                 const bool &showHeader = true,
                 const bool &darkHeader = false);

private:
    QSplitter *mLeft;
    QSplitter *mMiddle;
    QSplitter *mRight;
    QSplitter *mTop;
    QSplitter *mBottom;

    const QString getIdFromLabel(const QString &label);
};

#endif // UILAYOUT_H
