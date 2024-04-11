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

#ifndef KEYFOCUSTARGET_H
#define KEYFOCUSTARGET_H

#include "ui_global.h"

#include <QKeyEvent>
#include <QWidget>

class UI_EXPORT KeyFocusTarget {
public:
    KeyFocusTarget();
    virtual ~KeyFocusTarget();

    bool KFT_hasFocus() {
        return KFT_mCurrentTarget == this;
    }

    static bool KFT_handleKeyEvent(QKeyEvent *e);
    virtual bool KFT_keyPressEvent(QKeyEvent *e) {
        Q_UNUSED(e)
        return false;
    }

    virtual bool KFT_keyReleaseEvent(QKeyEvent *e) {
        Q_UNUSED(e)
        return false;
    }

    void KFT_setFocus() {
        KFT_setCurrentTarget(this);
        KFT_setFocusToWidget();
    }

    static void KFT_sTab();
    static void KFT_sSetRandomTarget();
    static void KFT_sSetLastTarget();
    static KeyFocusTarget *KFT_getCurrentTarget();
    static void KFT_setCurrentTarget(KeyFocusTarget *target);
protected:
    virtual void KFT_setFocusToWidget() = 0;
    virtual void KFT_clearFocus() = 0;
private:
    bool visible() const {
        const auto wid = dynamic_cast<const QWidget*>(this);
        if(wid) return wid->isVisible();
        return false;
    }

    static KeyFocusTarget *KFT_mCurrentTarget;
    static KeyFocusTarget *KFT_mLastTarget;

    static QList<KeyFocusTarget*> KFT_mAllTargets;
};

#endif // KEYFOCUSTARGET_H
