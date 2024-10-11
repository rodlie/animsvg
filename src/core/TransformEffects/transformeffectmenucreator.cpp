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

#include "TransformEffects/transformeffectmenucreator.h"

#include "TransformEffects/followobjecteffect.h"
#include "TransformEffects/followobjectrelativeeffect.h"
#include "TransformEffects/followpatheffect.h"
#include "TransformEffects/parenteffect.h"
#include "TransformEffects/trackeffect.h"

void TransformEffectMenuCreator::forEveryEffect(const EffectAdder &add)
{
    add(QObject::tr("Track"),
        []() { return enve::make_shared<TrackEffect>(); });
    add(QObject::tr("Follow Path"),
        []() { return enve::make_shared<FollowPathEffect>(); });
    add(QObject::tr("Follow Object"),
        []() { return enve::make_shared<FollowObjectEffect>(); });
    add(QObject::tr("Follow Object Relative"),
        []() { return enve::make_shared<FollowObjectRelativeEffect>(); });
    add(QObject::tr("Parent"),
        []() { return enve::make_shared<ParentEffect>(); });
}
