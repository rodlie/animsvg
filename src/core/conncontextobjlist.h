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

#ifndef CONNCONTEXTOBJLIST_H
#define CONNCONTEXTOBJLIST_H
#include "conncontext.h"
#include <functional>

template <typename T>
class ConnContextObjList : private QList<T> {
public:
    ConnContextObjList() {}
    ConnContextObjList(ConnContextObjList&) = delete;

    //! @brief Appends the object and returns corresponding connection context
    ConnContext& addObj(const T& obj) {
        return insertObj(this->count(), obj);
    }

    //! @brief Adds the object at index and returns corresponding connection context
    ConnContext& insertObj(const int index, const T& obj) {
        const auto cctx = std::make_shared<ConnContext>();
        mCCtxs.insert(index, cctx);
        this->insert(index, obj);
        return *cctx.get();
    }

    //! @brief Removes(but does not delete) the object and corresponding connections
    bool removeObj(const T& obj) {
        for(int i = 0; i < this->count(); i++) {
            const auto iObj = this->at(i);
            if(iObj == obj) {
                mCCtxs.removeAt(i);
                this->removeAt(i);
                return true;
            }
        }
        return false;
    }

    void moveObj(const int from, const int to) {
        mCCtxs.move(from, to);
        this->move(from, to);
    }

    T takeObjAt(const int id) {
        mCCtxs.removeAt(id);
        return this->takeAt(id);
    }

    T takeObjLast() {
        mCCtxs.removeLast();
        return this->takeLast();
    }

    void clear() {
        mCCtxs.clear();
        QList<T>::clear();
    }

    const QList<T>& getList() const { return *this; }

    inline typename QList<T>::const_iterator begin() const {
        return cbegin();
    }

    inline typename QList<T>::const_iterator end() const {
        return cend();
    }

    inline typename QList<T>::const_reverse_iterator rbegin() const {
        return crbegin();
    }

    inline typename QList<T>::const_reverse_iterator rend() const {
        return crend();
    }

    template <typename Compare>
    void sort(const Compare& compare) {
        const auto p = sortPermutation(*this, compare);
        applyPermutation(*this, p);
        applyPermutation(mCCtxs, p);
    }

    using QList<T>::contains;
    using QList<T>::indexOf;
    using QList<T>::cbegin;
    using QList<T>::cend;
    using QList<T>::crbegin;
    using QList<T>::crend;
    using QList<T>::count;
    using QList<T>::isEmpty;
    using QList<T>::at;
    using QList<T>::first;
    using QList<T>::last;
private:
    template <typename U>
    void applyPermutation(QList<U>& vec,
                          const std::vector<std::size_t>& p) {
        const size_t vecSize = static_cast<size_t>(vec.size());
        std::vector<bool> done(vecSize);
        for(std::size_t i = 0; i < vecSize; ++i) {
            if(done[i]) continue;
            done[i] = true;
            std::size_t prev_j = i;
            std::size_t j = p[i];
            while(i != j) {
                std::swap(vec[prev_j], vec[j]);
                done[j] = true;
                prev_j = j;
                j = p[j];
            }
        }
    }

    template <typename U, typename Compare>
    std::vector<std::size_t> sortPermutation(const QList<U>& vec,
                                             const Compare& compare) {
        std::vector<std::size_t> p(vec.size());
        std::iota(p.begin(), p.end(), 0);
        const auto sorter = [&](const std::size_t& i, const std::size_t& j) {
            return compare(vec[i], vec[j]);
        };
        std::sort(p.begin(), p.end(), sorter);
        return p;
    }

    QList<std::shared_ptr<ConnContext>> mCCtxs;
};

#endif // CONNCONTEXTOBJLIST_H
