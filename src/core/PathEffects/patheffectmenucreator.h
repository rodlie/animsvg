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

#ifndef PATHEFFECTMENUCREATOR_H
#define PATHEFFECTMENUCREATOR_H

#include "core_global.h"

#include <QString>
#include "PathEffects/dashpatheffect.h"
#include "PathEffects/displacepatheffect.h"
#include "PathEffects/duplicatepatheffect.h"
#include "PathEffects/linespatheffect.h"
#include "PathEffects/solidifypatheffect.h"
#include "PathEffects/spatialdisplacepatheffect.h"
#include "PathEffects/subdividepatheffect.h"
#include "PathEffects/subpatheffect.h"
#include "PathEffects/sumpatheffect.h"
#include "PathEffects/zigzagpatheffect.h"
#include <functional>
#include "smartPointers/selfref.h"

class PathEffect;

struct CORE_EXPORT PathEffectMenuCreator
{
    template <typename T> using Func = std::function<T>;
    template <typename T> using Creator = Func<qsptr<T>()>;
    using EffectCreator = Creator<PathEffect>;
    using EffectAdder = Func<void(const QString&,
                                  const EffectCreator&)>;
    static void forEveryEffect(const EffectAdder& add)
    {
        add(QObject::tr("Displace"),
            []() { return enve::make_shared<DisplacePathEffect>(); });
        add(QObject::tr("Spatial Displace"),
            []() { return enve::make_shared<SpatialDisplacePathEffect>(); });
        add(QObject::tr("Dash"),
            []() { return enve::make_shared<DashPathEffect>(); });
        add(QObject::tr("Duplicate"),
            []() { return enve::make_shared<DuplicatePathEffect>(); });
        add(QObject::tr("Sub-Path"),
            []() { return enve::make_shared<SubPathEffect>(); });
        add(QObject::tr("Solidify"),
            []() { return enve::make_shared<SolidifyPathEffect>(); });
        add(QObject::tr("Sum"),
            []() { return enve::make_shared<SumPathEffect>(); });
        add(QObject::tr("Lines"),
            []() { return enve::make_shared<LinesPathEffect>(); });
        add(QObject::tr("ZigZag"),
            []() { return enve::make_shared<ZigZagPathEffect>(); });
        add(QObject::tr("Subdivide"),
            []() { return enve::make_shared<SubdividePathEffect>(); });
    }
};

#endif // PATHEFFECTMENUCREATOR_H
