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

#ifndef CUSTOMHANDLER_H
#define CUSTOMHANDLER_H
#include <QList>
#include <QLibrary>
#include "smartPointers/ememory.h"
#include "typemenu.h"
#include "customidentifier.h"

template <typename B, typename C>
class CustomHandler {
public:
    typedef qsptr<C> (*CCreatorNewFunc)();
    typedef qsptr<C> (*CCreatorFunc)(const CustomIdentifier &identifier);
    typedef QString (*CNameFunc)();
    typedef CustomIdentifier (*CIdentifierFunc)();
    typedef bool (*CSupport)(const CustomIdentifier&);

    CustomHandler(const CCreatorNewFunc& creatorNew,
                  const CCreatorFunc& creator,
                  const CNameFunc& name,
                  const CIdentifierFunc& identifier,
                  const CSupport& support) :
        mCreatorNew(creatorNew), mCreator(creator), mName(name),
        mIdentifier(identifier), mSupport(support) {}

    static void sLoadCustom(const QString& libPath);

    static qsptr<C> sCreateForIdentifier(
            const CustomIdentifier &identifier) {
        for(const auto& creator : sCreators) {
            if(!creator.mSupport(identifier)) continue;
            return creator.mCreator(identifier);
        }
        return nullptr;
    }

    template <typename U>
    static void sAddToMenu(PropertyMenu * const menu,
                           void (U::*adder)(const qsptr<B>&)) {
        for(const auto& creator : sCreators)
            sAddToMenu(menu, creator, adder);
    }

    template <typename T> using Func = std::function<T>;
    template <typename T> using Creator = Func<qsptr<T>()>;
    using CCreator = Creator<RasterEffect>;
    using CAdder = Func<void(const QString&, const QString&, const CCreator&)>;
    static void sForEveryEffect(const CAdder& add) {
        for(const auto& creator : sCreators) {
            const auto cCreator = [creator]() { return creator.mCreatorNew(); };
            add(creator.mName(), "", cCreator);
        }
    }
private:
    template <typename U>
    static void sAddToMenu(PropertyMenu * const menu,
                           const CustomHandler<B, C>& creator,
                           void (U::*adder)(const qsptr<B>&)) {
        menu->addPlainAction<U>(QIcon::fromTheme("preferences"), creator.mName(),
        [adder, creator](U * const obj) {
            const auto cEffect = creator.mCreatorNew();
            (obj->*adder)(qSharedPointerCast<B>(cEffect));
        });
    }
    static QList<CustomHandler<B, C>> sCreators;

    CCreatorNewFunc mCreatorNew;
    CCreatorFunc mCreator;
    CNameFunc mName;
    CIdentifierFunc mIdentifier;
    CSupport mSupport;
};

template <typename B, typename C>
QList<CustomHandler<B, C>> CustomHandler<B, C>::sCreators;

template <typename B, typename C>
void CustomHandler<B, C>::sLoadCustom(const QString &libPath) {
    QLibrary lib(libPath);

    if(!lib.load()) RuntimeThrow("Could not load the library");

    const auto creatorNewFunc = reinterpret_cast<CCreatorNewFunc>(
                lib.resolve("eCreateNewestVersion"));
    if(!creatorNewFunc) RuntimeThrow("Could not resolve 'eCreateNewestVersion' symbol");

    const auto creatorFunc = reinterpret_cast<CCreatorFunc>(
                lib.resolve("eCreate"));
    if(!creatorFunc) RuntimeThrow("Could not resolve 'eCreate' symbol");

    const auto nameFunc = reinterpret_cast<CNameFunc>(
                lib.resolve("eName"));
    if(!nameFunc) RuntimeThrow("Could not resolve 'eName' symbol");

    const auto identifierFunc = reinterpret_cast<CIdentifierFunc>(
                lib.resolve("eIdentifier"));
    if(!identifierFunc) RuntimeThrow("Could not resolve 'eIdentifier' symbol");

    const auto supportFunc = reinterpret_cast<CSupport>(
                lib.resolve("eSupports"));
    if(!supportFunc) RuntimeThrow("Could not resolve 'eSupports' symbol");

    CustomHandler handler(creatorNewFunc, creatorFunc, nameFunc,
                          identifierFunc, supportFunc);
    sCreators.append(handler);
}


#endif // CUSTOMHANDLER_H
