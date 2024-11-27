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

#ifndef EWIDGETS_H
#define EWIDGETS_H

#include "../core_global.h"

#include <functional>

class QObject;
class QWidget;

class QColor;
class ColorSetting;
class ColorAnimator;

class CORE_EXPORT eWidgets {
    template <typename T> using Func = std::function<T>;
    static eWidgets* sInstance;
protected:
    eWidgets();
public:
    static QWidget* sColorWidget(QWidget* const parent,
                                 const QColor& iniColor,
                                 QObject* const receiver,
                                 const Func<void(const ColorSetting&)>& slot) {
        return sInstance->colorWidget(parent, iniColor, receiver, slot);
    }

    static QWidget* sColorWidget(QWidget* const parent,
                                 ColorAnimator* const target) {
        return sInstance->colorWidget(parent, target);
    }
    static QWidget* sColorWidget(QWidget* const parent,
                                 ColorAnimator* const target,
                                 const bool showColorMode)
    {
        return sInstance->colorWidget(parent, target, showColorMode);
    }
protected:
    virtual QWidget* colorWidget(QWidget* const parent,
                                 const QColor& iniColor,
                                 QObject* const receiver,
                                 const Func<void(const ColorSetting&)>& slot) = 0;
    virtual QWidget* colorWidget(QWidget* const parent,
                                 ColorAnimator* const target) = 0;
    virtual QWidget* colorWidget(QWidget* const parent,
                                 ColorAnimator* const target,
                                 const bool showColorMode) = 0;
};

#endif // EWIDGETS_H
