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

#ifndef FRICTION_FORMAT_OPTIONS_H
#define FRICTION_FORMAT_OPTIONS_H

#include "core_global.h"

#include <QList>
#include <QStringList>

namespace Friction
{
    namespace Core
    {
        enum CORE_EXPORT FormatType
        {
            fTypeMeta,
            fTypeFormat,
            fTypeCodec
        };

        struct CORE_EXPORT FormatOption
        {
            QString fKey;
            QString fValue;
            int fType;
        };

        struct CORE_EXPORT FormatOptions
        {
            QList<FormatOption> fValues;
        };

        struct CORE_EXPORT FormatOptionsList
        {
            QStringList fTypes;
            QStringList fKeys;
            QStringList fValues;
        };
    }
}

#endif // FRICTION_FORMAT_OPTIONS_H
