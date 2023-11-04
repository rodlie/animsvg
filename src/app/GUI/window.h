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

#ifndef WINDOW_H
#define WINDOW_H

#include <QDialog>
#include <QKeyEvent>

class Window : public QDialog
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent,
                    QWidget *child,
                    const QString title,
                    const QString id,
                    bool visible = true,
                    bool blockEscKey = true,
                    bool forwardKeys = false);
    ~Window();
    void focusWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void loadState(bool visible = true);
    void saveState();
    bool mBlockEscKey;
    bool mForwardKeys;
};

#endif // WINDOW_H
