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

#ifndef COMPLEXKEY_H
#define COMPLEXKEY_H
#include "key.h"

class ComplexAnimator;

class CORE_EXPORT ComplexKey : public Key {
    e_OBJECT
protected:
    ComplexKey(const int absFrame,
               ComplexAnimator * const parent);
public:
    bool isDescendantSelected() const;

    void startFrameTransform()
    { Q_ASSERT(false); }
    void finishFrameTransform()
    { Q_ASSERT(false); }
    void cancelFrameTransform()
    { Q_ASSERT(false); }

    bool isSelected() const;
    void addToSelection(QList<Animator *> &selectedAnimators);
    void removeFromSelection(QList<Animator*> &selectedAnimators);
    bool differsFromKey(Key *otherKey) const;

    void addChildKey(Key * const key);
    void removeChildKey(Key * const key);

    bool isEmpty() const;

    void moveAllKeysTo(ComplexKey * const target);
    bool hasKey(Key *key) const;

    int getChildKeysCount() const;
    bool hasSameKey(Key *otherKey) const;
private:
    QList<Key*> mKeys;
};


#endif // COMPLEXKEY_H
