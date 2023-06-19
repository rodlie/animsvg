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

#include "switchbutton.h"

#include "Private/esettings.h"
#include "GUI/global.h"

SwitchButton::SwitchButton(const QString &toolTip, QWidget *parent) :
    ButtonBase(toolTip, parent) {
    connect(&eSizesUI::widget, &SizeSetter::sizeChanged,
            this, [this](int) { mStates.clear(); });
}

void SwitchButton::toggle() {
    int state = mCurrentState + 1;
    if(state >= mStates.count()) state = 0;
    setState(state);
    emit toggled(mCurrentState);
}

SwitchButton *SwitchButton::sCreate2Switch(
        const QString &icon0, const QString &icon1,
        const QString &toolTip, QWidget *parent) {
    const auto butt = new SwitchButton(toolTip, parent);
    butt->addState(icon0); butt->addState(icon1);
    return butt;
}

bool SwitchButton::setState(const int state) {
    if(state < 0) return false;
    if(state >= mStates.count()) return false;
    mCurrentState = state;
    updateIcon();
    return true;
}

void SwitchButton::updateIcon() {
    if(mCurrentState < 0) return;
    if(mCurrentState >= mStates.count()) return;
    setCurrentIcon(mStates.at(mCurrentState));
}

int SwitchButton::addState(const QString &icon) {
    eSizesUI::widget.add(this, [this, icon](int) {
        const auto iconPath = eSettings::sIconsDir() + "/" + icon;
        mStates << QImage(iconPath);
        if(mCurrentState == mStates.count() - 1) updateIcon();
    });
    if(mStates.count() == 1) toggle();
    return mStates.count() - 1;
}
