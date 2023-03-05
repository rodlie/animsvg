// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DABTEST_GLOBAL_H
#define DABTEST_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DABTEST_LIBRARY)
#  define DABTEST_EXPORT Q_DECL_EXPORT
#else
#  define DABTEST_EXPORT Q_DECL_IMPORT
#endif

#include "enveCore/RasterEffects/customrastereffect.h"

extern "C" {

DABTEST_EXPORT void eCreateNewestVersion(qsptr<CustomRasterEffect>& result);

DABTEST_EXPORT void eCreate(const CustomIdentifier &identifier,
                            qsptr<CustomRasterEffect>& result);

DABTEST_EXPORT void eName(QString& result);

DABTEST_EXPORT void eIdentifier(CustomIdentifier& result);

DABTEST_EXPORT bool eSupports(const CustomIdentifier &identifier);

}
#endif // DABTEST_GLOBAL_H
