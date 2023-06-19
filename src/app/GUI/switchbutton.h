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

#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include "buttonbase.h"

class SwitchButton : public ButtonBase {
    Q_OBJECT
public:
    SwitchButton(const QString &toolTip, QWidget *parent);

    void toggle();

    static SwitchButton* sCreate2Switch(
            const QString &icon0, const QString &icon1,
            const QString &toolTip, QWidget *parent);

    int addState(const QString& icon);

    bool setState(const int state);
private:
    void updateIcon();

    QList<QImage> mStates;
    int mCurrentState = -1;
signals:
    void toggled(int);
};

#endif // SWITCHBUTTON_H
