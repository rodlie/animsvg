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

#ifndef EEVENT_H
#define EEVENT_H

#include "core_global.h"

#include <QMouseEvent>

class eEvent {
protected:
    eEvent(const QPointF& pos,
           const QPointF& lastPos,
           const QPointF& lastPressPos,
           const bool mouseGrabbing,
           const qreal scale,
           const QPoint& globalPos,
           const Qt::MouseButtons buttons,
           const Qt::KeyboardModifiers modifiers,
           const ulong& timestamp,
           std::function<void()> releaseMouse,
           std::function<void()> grabMouse,
           QWidget * const widget);
public:
    bool shiftMod() const {
        return fModifiers & Qt::SHIFT;
    }

    bool ctrlMod() const {
        return fModifiers & Qt::CTRL;
    }

    QPointF fPos;
    QPointF fLastPos;
    QPointF fLastPressPos;
    bool fMouseGrabbing;
    qreal fScale;
    QPoint fGlobalPos;
    Qt::MouseButtons fButtons;
    Qt::KeyboardModifiers fModifiers;
    ulong fTimestamp;
    std::function<void()> fReleaseMouse;
    std::function<void()> fGrabMouse;
    QWidget* fWidget;
};

class CORE_EXPORT eMouseEvent : public eEvent {
protected:
    eMouseEvent(const bool synth,
                const QPointF& pos,
                const QPointF& lastPos,
                const QPointF& lastPressPos,
                const bool mouseGrabbing,
                const qreal scale,
                const QPoint& globalPos,
                const Qt::MouseButton button,
                const Qt::MouseButtons buttons,
                const Qt::KeyboardModifiers modifiers,
                const ulong& timestamp,
                std::function<void()> releaseMouse,
                std::function<void()> grabMouse,
                QWidget * const widget);
public:
    eMouseEvent(const QPointF& pos,
                const QPointF& lastPos,
                const QPointF& lastPressPos,
                const bool mouseGrabbing,
                const qreal scale,
                const QMouseEvent * const e,
                std::function<void()> releaseMouse,
                std::function<void()> grabMouse,
                QWidget * const widget);

    bool fSynth;
    Qt::MouseButton fButton;
};

class CORE_EXPORT eKeyEvent : public eMouseEvent {
public:
    eKeyEvent(const QPointF& pos,
              const QPointF& lastPos,
              const QPointF& lastPressPos,
              const bool mouseGrabbing,
              const qreal scale,
              const QPoint globalPos,
              const Qt::MouseButtons buttons,
              const QKeyEvent * const e,
              std::function<void()> releaseMouse,
              std::function<void()> grabMouse,
              QWidget * const widget);

    bool fAutorepeat;
    QEvent::Type fType;
    int fKey;
};

#endif // EEVENT_H
