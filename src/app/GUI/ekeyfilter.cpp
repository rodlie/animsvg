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

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#include "ekeyfilter.h"
#include "mainwindow.h"
#include "misc/keyfocustarget.h"

eKeyFilter::eKeyFilter(MainWindow * const window) :
    QObject(window), mMainWindow(window) {}

eKeyFilter *eKeyFilter::sCreateLineFilter(MainWindow * const window) {
    const auto filter = new eKeyFilter(window);
    filter->mAllow = [](const int key) {
        for(int i = Qt::Key_F1; i <= Qt::Key_F12; i++) {
            if(key == i) return false;
        }
        if(key == Qt::Key_Return) {
            KeyFocusTarget::KFT_sSetRandomTarget();
            return false;
        }
        return true;
    };
    return filter;
}

eKeyFilter *eKeyFilter::sCreateNumberFilter(MainWindow * const window) {
    const auto filter = new eKeyFilter(window);
    filter->mAllow = [](const int key) {
        for(int i = Qt::Key_F1; i <= Qt::Key_F12; i++) {
            if(key == i) return false;
        }
        for(int i = Qt::Key_A; i <= Qt::Key_Z; i++) {
            if(key == i) return false;
        }
        if(key == Qt::Key_Return) {
            KeyFocusTarget::KFT_sSetRandomTarget();
            return false;
        }
        return true;
    };
    return filter;
}

bool eKeyFilter::eventFilter(QObject *watched, QEvent *event) {
    Q_UNUSED(watched)
    if(event->type() == QEvent::KeyPress ||
       event->type() == QEvent::KeyRelease) {
        const auto kEvent = static_cast<QKeyEvent*>(event);
        if(mAllow(kEvent->key())) return false;
        if(mMainWindow->processKeyEvent(kEvent)) return true;
        return false;
    }
    return false;
}
