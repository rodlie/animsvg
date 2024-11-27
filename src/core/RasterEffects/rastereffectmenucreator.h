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

#ifndef RASTEREFFECTMENUCREATOR_H
#define RASTEREFFECTMENUCREATOR_H

#include <functional>
#include "smartPointers/selfref.h"
#include "typemenu.h"

class RasterEffect;

struct CORE_EXPORT RasterEffectMenuCreator {
    template <typename T> using Func = std::function<T>;
    template <typename T> using Creator = Func<qsptr<T>()>;
    using EffectCreator = Creator<RasterEffect>;
    using EffectAdder = Func<void(const QString&, const QString&,
                                  const EffectCreator&)>;

    template <typename Target>
    static void addEffects(PropertyMenu * const menu,
                           void (Target::*addToTarget)(const qsptr<RasterEffect>&)) {
        const auto adder =
        [menu, addToTarget](const QString& name, const QString& path,
                            const EffectCreator& creator) {
            const auto adder = [creator, addToTarget](Target * target) {
                (target->*addToTarget)(creator());
            };
            if(path.isEmpty()) {
                menu->addPlainAction<Target>(QIcon::fromTheme("effect"), name, adder);
            } else {
                const auto pathList = path.split('/');
                auto childMenu = menu->childMenu(pathList);
                if(!childMenu) childMenu = menu->addMenu(pathList);
                childMenu->addPlainAction<Target>(QIcon::fromTheme("effect"), name, adder);
            }
        };
        forEveryEffect(adder);
    }
    static void forEveryEffect(const EffectAdder& add);
    static void forEveryEffectCore(const EffectAdder& add);
    static void forEveryEffectCustom(const EffectAdder& add);
    static void forEveryEffectShader(const EffectAdder& add);
};

#endif // RASTEREFFECTMENUCREATOR_H
