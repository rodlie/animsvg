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

#ifndef CLAMPEDVALUE_H
#define CLAMPEDVALUE_H
#include "../pointhelpers.h"

class CORE_EXPORT ClampedValue {
    qreal mValue;
    qreal mSavedValue;
    qreal mMin = -DBL_MAX;
    qreal mMax = DBL_MAX;
public:
    ClampedValue(const qreal value);

    void saveValue();
    void restoreSavedValue();

    qreal getRawSavedValue() const;
    qreal getRawValue() const;

    qreal getClampedSavedValue() const;
    qreal getClampedValue() const;

    void setValue(const qreal value);
    void clampValue();

    void setRange(const qreal min, const qreal max);
    void setMax(const qreal max);
    void setMin(const qreal min);
};

#endif // CLAMPEDVALUE_H
