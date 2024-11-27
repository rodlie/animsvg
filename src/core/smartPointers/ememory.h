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

#ifndef EMEMORY_H
#define EMEMORY_H

#include "stdpointer.h"
#include "stdselfref.h"
#include "selfref.h"

#define enve_cast enve::cast

#if defined (Q_OS_WIN)
    template <bool T> using BoolConstant = std::bool_constant<T>;
#elif defined(Q_OS_LINUX)
    template <bool T> using BoolConstant = std::__bool_constant<T>;
#elif (defined (Q_OS_MAC))
    template <bool T> using BoolConstant = std::integral_constant<bool, T>;
#endif

class SingleWidgetTarget;

namespace enve {
    template <class T, typename... Args>
    inline auto make_shared(Args && ...args) {
        static_assert(std::is_base_of<StdSelfRef, T>::value ||
                      std::is_base_of<SelfRef, T>::value,
                      "enve::make_shared can only be used with StdSelfRef and SelfRef derived classes");

        return T::template sCreate<T>(std::forward<Args>(args)...);
    }

    template <class T>
    inline auto shared(T * const obj) {
        static_assert(std::is_base_of<StdSelfRef, T>::value ||
                      std::is_base_of<SelfRef, T>::value,
                      "enve::shared can only be used with StdSelfRef and SelfRef derived classes");

        return obj->template ref<T>();
    }

    template <class T, class B>
    inline auto shared(const B& obj) -> decltype(obj->template ref<T>()) {
        static_assert(std::is_base_of<StdSelfRef, T>::value ||
                      std::is_base_of<SelfRef, T>::value,
                      "enve::shared can only be used with StdSelfRef and SelfRef derived classes");
        if(obj) return obj->template ref<T>();
        return nullptr;
    }

    namespace {
        template<typename T, typename U>
        struct HasCastMethod
        {
            template<typename W, W* (*)(U&)> struct SFINAE {};
            template<typename W> static char Test(SFINAE<W, &W::sCast>*);
            template<typename W> static int Test(...);
            static const bool Has = sizeof(Test<T>(0)) == sizeof(char);
        };

        template <typename T, class U>
        inline T cast_q_object(U* const u, std::false_type) {
            return dynamic_cast<T>(u);
        }

        template <typename T, class U>
        inline T cast_q_object(U* const u, std::true_type) {
            return qobject_cast<T>(u);
        }

        template <typename T, class U>
        inline T cast_qobject(U* const u, std::false_type) {
            return dynamic_cast<T>(u);
        }

        template <typename T, class U>
        inline T cast_qobject(U* const u, std::true_type) {
            typedef typename std::remove_cv<typename std::remove_pointer<T>::type>::type TO;
            typedef QtPrivate::HasQ_OBJECT_Macro<TO> MacroCheck;
            return cast_q_object<T>(u, BoolConstant<MacroCheck::Value>());
        }

        template <typename T, class U>
        inline T cast_enve(U* const u, std::false_type) {
            typedef typename std::remove_cv<typename std::remove_pointer<T>::type>::type TO;
            return cast_qobject<T>(u, BoolConstant<std::is_base_of<QObject, TO>::value>());
        }

        template <typename T, class U>
        inline T cast_enve(U* const u, std::true_type) {
            typedef typename std::remove_cv<typename std::remove_pointer<T>::type>::type TO;
            return u ? TO::sCast(*u) : nullptr;
        }

        template <typename T, class U>
        inline T cast_swt(U* const u, std::false_type) {
            typedef typename std::remove_cv<typename std::remove_pointer<T>::type>::type TO;
            return cast_qobject<T>(u, BoolConstant<std::is_base_of<QObject, TO>::value>());

        }

        template <typename T, class U>
        inline T cast_swt(U* const u, std::true_type) {
            typedef typename std::remove_cv<typename std::remove_pointer<T>::type>::type TO;
            return cast_enve<T>(u, BoolConstant<HasCastMethod<TO, U>::Has>());
        }
    }

    //! @brief Use virtual function call, qobject_cast, or dynamic_cast,
    //! depending on availability.
    template <typename T, class U>
    inline T cast(U* const u) {
        typedef typename std::remove_cv<U>::type UNC;
        return cast_swt<T>(const_cast<UNC*>(u), BoolConstant<std::is_base_of<SingleWidgetTarget, UNC>::value>());
    }

    //! @brief Use virtual function call, qobject_cast, or dynamic_cast,
    //! depending on availability.
    template <typename T, class U>
    inline T cast(const qptr<U>& u) {
        return cast<T>(u.data());
    }

    //! @brief Use virtual function call, qobject_cast, or dynamic_cast,
    //! depending on availability.
    template <typename T, class U>
    inline T cast(const qsptr<U>& u) {
        return cast<T>(u.data());
    }
}

#endif // EMEMORY_H
