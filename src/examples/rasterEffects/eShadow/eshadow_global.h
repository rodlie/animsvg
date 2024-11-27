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

#ifndef ESHADOW_GLOBAL_H
#define ESHADOW_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ESHADOW_LIBRARY)
#  define ESHADOW_EXPORT Q_DECL_EXPORT
#else
#  define ESHADOW_EXPORT Q_DECL_IMPORT
#endif

#include "enveCore/RasterEffects/customrastereffect.h"

extern "C" {

ESHADOW_EXPORT void eCreateNewestVersion(qsptr<CustomRasterEffect>& result);

ESHADOW_EXPORT void eCreate(const CustomIdentifier &identifier,
                            qsptr<CustomRasterEffect>& result);

ESHADOW_EXPORT void eName(QString& result);

ESHADOW_EXPORT void eIdentifier(CustomIdentifier& result);

ESHADOW_EXPORT bool eSupports(const CustomIdentifier &identifier);

}
#endif // ESHADOW_GLOBAL_H
