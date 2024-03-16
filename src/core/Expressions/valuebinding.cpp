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

#include "valuebinding.h"

ValueBinding::ValueBinding(const Property* const context) :
    PropertyBindingBase(context) {}

qsptr<ValueBinding> ValueBinding::sCreate(const Property* const context) {
    const auto result = new ValueBinding(context);
    return qsptr<ValueBinding>(result);
}

QJSValue ValueBinding::getJSValue(QJSEngine& e) {
    if(mContext) return mContext->prp_getBaseJSValue(e);
    else return QJSValue::NullValue;
}

QJSValue ValueBinding::getJSValue(QJSEngine& e, const qreal relFrame) {
    if(mContext) return mContext->prp_getBaseJSValue(e, relFrame);
    else return QJSValue::NullValue;
}

FrameRange ValueBinding::identicalRelRange(const int absFrame) {
    Q_UNUSED(absFrame)
    return FrameRange::EMINMAX;
}

FrameRange ValueBinding::nextNonUnaryIdenticalRelRange(const int absFrame) {
    const int relFrame = mContext->prp_absFrameToRelFrame(absFrame);
    return mContext->prp_nextNonUnaryIdenticalRelRange(relFrame);
}
