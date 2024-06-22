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

#ifndef FRAMEBINDING_H
#define FRAMEBINDING_H

#include "propertybindingbase.h"

class CORE_EXPORT FrameBinding : public PropertyBindingBase {
    FrameBinding(const Property* const context);
public:
    static qsptr<FrameBinding> sCreate(const Property* const context);

    QJSValue getJSValue(QJSEngine& e);
    QJSValue getJSValue(QJSEngine& e, const qreal relFrame);

    FrameRange identicalRelRange(const int absFrame);
    FrameRange nextNonUnaryIdenticalRelRange(const int absFrame);
    QString path() const { return "$frame"; }
};

#endif // FRAMEBINDING_H
