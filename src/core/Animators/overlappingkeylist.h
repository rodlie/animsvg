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

#ifndef OVERLAPPINGKEYLIST_H
#define OVERLAPPINGKEYLIST_H

#include "overlappingkeys.h"

class CORE_EXPORT OverlappingKeyList {
    typedef QList<OverlappingKeys>::const_iterator OKeyListCIter;
    typedef QList<OverlappingKeys>::iterator OKeyListIter;
public:
    OverlappingKeyList(Animator * const animator) :
        mAnimator(animator) {}

    class iterator {
    public:
        iterator(const int id,
                const QList<OverlappingKeys> * const list) :
            mList(list) {
            setId(id);
        }

        iterator& operator++() {
            setId(mId + 1);
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return this->mId != other.mId;
        }

        Key * const & operator->() const { return mKey; }
        Key * const &operator*() const { return mKey; }
    protected:
        int mId;
    private:
        void setId(const int id) {
            mId = id;
            if(mId < 0 || mId >= mList->count()) mKey = nullptr;
            else mKey = mList->at(mId).getKey();
        }
        const QList<OverlappingKeys> * mList = nullptr;
        Key * mKey = nullptr;
    };

    iterator begin() const
    { return iterator(0, &mList); }

    iterator end() const
    { return iterator(mList.count(), &mList); }

    int count() const
    { return mList.count(); }
    bool isEmpty() const
    { return mList.isEmpty(); }

    bool hasKey(const Key* const key, int* idP = nullptr) const;
    bool isDuplicateAtIdex(const int index) const;

    void add(const stdsptr<Key>& key);
    void remove(const stdsptr<Key>& key);

    template <class T = Key>
    T* atId(const int id) const;
    template <class T = Key>
    T* atRelFrame(const int relFrame) const;

    std::pair<int, int> prevAndNextId(const int relFrame) const;

    template <class T = Key>
    T* first() const
    { return atId<T>(0); }
    template <class T = Key>
    T* last() const
    { return atId<T>(mList.count() - 1); }

    void mergeAll()
    { for(auto& oKey : mList) oKey.merge(); }
private:
    OKeyListCIter upperBound(const int relFrame) const;
    OKeyListCIter lowerBound(const int relFrame) const;
    OKeyListIter upperBound(const int relFrame);
    OKeyListIter lowerBound(const int relFrame);

    int idAtFrame(const int relFrame) const;

    Animator * const mAnimator;
    QList<OverlappingKeys> mList;
};

template <class T>
T* OverlappingKeyList::atId(const int id) const {
    if(id < 0) return nullptr;
    if(id >= mList.count()) return nullptr;
    return static_cast<T*>(mList.at(id).getKey());
}

template <class T>
T* OverlappingKeyList::atRelFrame(const int relFrame) const {
    const int id = idAtFrame(relFrame);
    const auto kAtId = atId<T>(id);
    if(!kAtId) return nullptr;
    if(kAtId->getRelFrame() == relFrame) return kAtId;
    return nullptr;
}

#endif // OVERLAPPINGKEYLIST_H
