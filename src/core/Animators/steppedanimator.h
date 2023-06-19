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

#ifndef STEPPEDANIMATOR_H
#define STEPPEDANIMATOR_H

#include "animatort.h"

template <typename T>
class SteppedAnimator : public AnimatorT<T> {
protected:
    SteppedAnimator(const QString& name) :
        AnimatorT<T>(name) {}
public:
    FrameRange prp_getIdenticalRelRange(const int relFrame) const override;
protected:
    T getValueAtRelFrameK(const qreal frame,
                          const KeyT<T> * const prevKey,
                          const KeyT<T> * const nextKey) const {
        Q_UNUSED(frame)
        if(prevKey) return prevKey->getValue();
        return nextKey->getValue();
    }
};

template<typename T>
FrameRange SteppedAnimator<T>::prp_getIdenticalRelRange(const int relFrame) const {
    if(!this->anim_hasKeys())
        return {FrameRange::EMIN, FrameRange::EMAX};
    const auto pn = this->anim_getPrevAndNextKeyId(relFrame);
    const int prevId = pn.first;
    const int nextId = pn.second;

    Key *prevKey = this->anim_getKeyAtIndex(prevId);
    Key *nextKey = this->anim_getKeyAtIndex(nextId);
    const bool adjKeys = nextId - prevId == 1;
    Key * const keyAtRelFrame = adjKeys ? nullptr :
                                          this->anim_getKeyAtIndex(pn.first + 1);
    Key *prevPrevKey = keyAtRelFrame ? keyAtRelFrame : prevKey;
    Key *prevNextKey = keyAtRelFrame ? keyAtRelFrame : prevKey;

    int fId = relFrame;
    int lId = relFrame;

    while(true) {
        if(!prevKey) {
            fId = FrameRange::EMIN;
            break;
        }
        if(prevPrevKey) {
            if(prevKey->differsFromKey(prevPrevKey)) break;
        }
        fId = prevKey->getRelFrame();
        prevPrevKey = prevKey;
        prevKey = prevKey->getPrevKey();
    }

    while(true) {
        if(!nextKey) {
            lId = FrameRange::EMAX;
            break;
        }
        lId = nextKey->getRelFrame() - 1;
        if(prevNextKey) {
            if(nextKey->differsFromKey(prevNextKey)) break;
        } else break;
        prevNextKey = nextKey;
        nextKey = nextKey->getNextKey();
    }

    return {fId, lId};
}

#endif // STEPPEDANIMATOR_H
