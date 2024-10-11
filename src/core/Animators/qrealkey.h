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

#ifndef QREALKEY_H
#define QREALKEY_H
#include "graphkey.h"
#include "../smartPointers/ememory.h"

class QPainter;
class ComplexAnimator;

class ComplexKey;

class QrealAnimator;
class KeysClipboard;

class CORE_EXPORT QrealKey : public GraphKey {
    e_OBJECT
public:
    QrealKey(QrealAnimator * const parentAnimator);
    QrealKey(const qreal value, const int frame,
             QrealAnimator * const parentAnimator);

    void changeFrameAndValueBy(const QPointF &frameValueChange);

    bool differsFromKey(Key *key) const;

    void startValueTransform();
    void finishValueTransform();
    void cancelValueTransform();
    void writeKey(eWriteStream& dst);
    void readKey(eReadStream& src);

    qreal getValueForGraph() const
    { return getValue(); }

    void setValueForGraph(const qreal value)
    { setValue(value); }

    qreal getValue() const;
    void setValue(const qreal value);

    QrealAnimator *getParentQrealAnimator() const;
    void incValue(const qreal incBy);
private:
    qreal mValue;
    qreal mSavedValue;
};

#endif // QREALKEY_H
