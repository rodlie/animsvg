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

#ifndef GLOBAL_H
#define GLOBAL_H

#include "sizesetter.h"

#include <QPixmap>
#include <QString>
#include <QFont>

namespace eSizesUI {
    //! @brief font height
    CORE_EXPORT extern SizeSetter font;
    //! @brief minimum widget dimension
    CORE_EXPORT extern SizeSetter widget;
    //! @brief minimum button dimension
    CORE_EXPORT extern SizeSetter button;
};

CORE_EXPORT
extern void connectAppFont(QWidget* const widget);

CORE_EXPORT
extern QFont OS_FONT;

CORE_EXPORT
extern int KEY_RECT_SIZE;

CORE_EXPORT
extern QPixmap* ALPHA_MESH_PIX;

CORE_EXPORT
QString gSingleLineTooltip(const QString& text);
CORE_EXPORT
QString gSingleLineTooltip(const QString& text, const QString& shortcut);

#endif // GLOBAL_H
