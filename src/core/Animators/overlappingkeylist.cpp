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

#include "overlappingkeylist.h"

bool OverlappingKeyList::hasKey(const Key * const key, int *idP) const {
    int id = idAtFrame(key->getRelFrame());
    if(idP) *idP = id;
    if(id < 0) return false;
    if(id >= mList.count()) return false;
    return mList.at(id).hasKey(key);
}

bool OverlappingKeyList::isDuplicateAtIdex(const int index) const {
    return mList.at(index).hasMultiple();
}

void OverlappingKeyList::add(const stdsptr<Key> &key) {
    const int relFrame = key->getRelFrame();
    const auto notLess = lowerBound(relFrame);
    if(notLess == mList.end()) {
        mList.append(OverlappingKeys(key, mAnimator));
    } else {
        if(notLess->getFrame() == relFrame) notLess->addKey(key);
        else {
            const int insertId = notLess - mList.begin();
            mList.insert(insertId, OverlappingKeys(key, mAnimator));
        }
    }
}

void OverlappingKeyList::remove(const stdsptr<Key> &key) {
    const int relFrame = key->getRelFrame();
    const int removeId = idAtFrame(relFrame);
    if(removeId == -1) return;
    auto& ovrlp = mList[removeId];
    ovrlp.removeKey(key);
    if(ovrlp.isEmpty()) mList.removeAt(removeId);
}

std::pair<int, int> OverlappingKeyList::prevAndNextId(const int relFrame) const {
    if(mList.isEmpty()) return {-1, -1};
    const auto notLess = lowerBound(relFrame);
    if(notLess == mList.end())
        return {mList.count() - 1, -1};
    const int notLessId = notLess - mList.begin();
    if(notLess->getFrame() == relFrame) {
        if(notLessId == mList.count() - 1)
            return {notLessId - 1, -1};
        return {notLessId - 1, notLessId + 1};
    }
    // notLess is greater than relFrame
    return {notLessId - 1, notLessId};
}

bool OKeyFrameMore(const int relFrame, const OverlappingKeys& keys) {
    return keys.getFrame() > relFrame;
}

OverlappingKeyList::OKeyListCIter
OverlappingKeyList::upperBound(const int relFrame) const {
    return std::upper_bound(mList.begin(), mList.end(),
                            relFrame, OKeyFrameMore);
}

OverlappingKeyList::OKeyListIter
OverlappingKeyList::upperBound(const int relFrame) {
    return std::upper_bound(mList.begin(), mList.end(),
                            relFrame, OKeyFrameMore);
}

bool OKeyFrameLess(const OverlappingKeys& keys,
                   const int relFrame) {
    return keys.getFrame() < relFrame;
}

OverlappingKeyList::OKeyListCIter
    OverlappingKeyList::lowerBound(const int relFrame) const {
    return std::lower_bound(mList.begin(), mList.end(),
                            relFrame, OKeyFrameLess);
}

OverlappingKeyList::OKeyListIter
    OverlappingKeyList::lowerBound(const int relFrame) {
    return std::lower_bound(mList.begin(), mList.end(),
                            relFrame, OKeyFrameLess);
}

int OverlappingKeyList::idAtFrame(const int relFrame) const {
    const auto notPrevious = lowerBound(relFrame);
    if(notPrevious == mList.end()) return -1;
    if(notPrevious->getFrame() == relFrame)
        return notPrevious - mList.begin();
    return -1;
}
