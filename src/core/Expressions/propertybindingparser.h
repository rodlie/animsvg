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

#ifndef PROPERTYBINDINGPARSER_H
#define PROPERTYBINDINGPARSER_H

#include "propertybinding.h"

using PropertyBindingMap = std::map<QString, qsptr<PropertyBindingBase>>;

namespace PropertyBindingParser {
    CORE_EXPORT
    qsptr<PropertyBindingBase> parseBinding(
            QString& name,
            const QString& exp,
            const PropertyBinding::Validator& validator,
            const Property* const context);
    CORE_EXPORT
    PropertyBindingMap parseBindings(
            QString exp,
            const PropertyBinding::Validator& validator,
            const Property* const context);
};

#endif // PROPERTYBINDINGPARSER_H
