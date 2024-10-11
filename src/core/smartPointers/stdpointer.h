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

#ifndef STDPOINTER_H
#define STDPOINTER_H
#include <QtCore/QtCore>
#include "stdselfref.h"

template <class T>
class StdPointer {
    std::weak_ptr<StdSelfRef> wp;
    StdSelfRef* rp = nullptr;

    void updateRawPtr() {
        if(wp.expired()) {
            rp = nullptr;
        } else {
            const std::shared_ptr<StdSelfRef> sp = wp.lock();
            rp = sp.get();
        }
    }
public:
    inline StdPointer() {
        static_assert(std::is_base_of<StdSelfRef, T>::value,
                      "StdPointer can be used only for StdSelfRef derived classes");
    }
    inline StdPointer(T * const p) : rp(p) {
        static_assert(std::is_base_of<StdSelfRef, T>::value,
                      "StdPointer can be used only for StdSelfRef derived classes");
        if(p) wp = p->template weakRef<T>();
    }
    // compiler-generated copy/move ctor/assignment operators are fine!
    // compiler-generated dtor is fine!

    inline void swap(StdPointer &other) {
        wp.swap(other.wp);
        updateRawPtr();
        other.updateRawPtr();
    }

    inline StdPointer<T> &operator=(T* const p) {
        rp = p;
        if(!p) wp.reset();
        else wp = static_cast<StdSelfRef*>(p)->weakRef<StdSelfRef>();
        return *this;
    }

    inline T* data() const {
        if(wp.expired()) return nullptr;
        return static_cast<T*>(rp);
    }

    inline T* get() const { return data(); }

    inline T* operator->() const
    { return data(); }
    inline T& operator*() const
    { return *data(); }
    inline operator T*() const
    { return data(); }

    inline bool isNull() const
    { return wp.expired(); }

    inline void clear() {
        wp.reset();
        rp = nullptr;
    }
};

template <class T> Q_DECLARE_TYPEINFO_BODY(StdPointer<T>, Q_MOVABLE_TYPE);

template <class T>
inline bool operator==(const T *o, const StdPointer<T> &p)
{ return o == p.operator->(); }

template<class T>
inline bool operator==(const StdPointer<T> &p, const T *o)
{ return p.operator->() == o; }

template <class T>
inline bool operator==(T *o, const StdPointer<T> &p)
{ return o == p.operator->(); }

template<class T>
inline bool operator==(const StdPointer<T> &p, T *o)
{ return p.operator->() == o; }

template<class T>
inline bool operator==(const StdPointer<T> &p1, const StdPointer<T> &p2)
{ return p1.operator->() == p2.operator->(); }

template<class T1, class T2>
inline bool operator==(const StdPointer<T1> &p1, const StdPointer<T2> &p2)
{ return p1.operator->() == p2.operator->(); }

template <class T>
inline bool operator!=(const T *o, const StdPointer<T> &p)
{ return o != p.operator->(); }

template<class T>
inline bool operator!= (const StdPointer<T> &p, const T *o)
{ return p.operator->() != o; }

template <class T>
inline bool operator!=(T *o, const StdPointer<T> &p)
{ return o != p.operator->(); }

template<class T>
inline bool operator!= (const StdPointer<T> &p, T *o)
{ return p.operator->() != o; }

template<class T>
inline bool operator!= (const StdPointer<T> &p1, const StdPointer<T> &p2)
{ return p1.operator->() != p2.operator->() ; }


#endif // STDPOINTER_H
