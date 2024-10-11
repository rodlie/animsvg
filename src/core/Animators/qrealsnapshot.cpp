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

#include "qrealsnapshot.h"
#include "qrealkey.h"

void QrealSnapshot::appendKey(const QrealKey * const key) {
    appendKey(key->getC0Frame(), key->getC0Value(),
              key->getRelFrame(), key->getValue(),
              key->getC1Frame(), key->getC1Value());
}

void QrealSnapshot::appendKey(const qreal c0Frame, const qreal c0Value,
                              const qreal pFrame, const qreal pValue,
                              const qreal c1Frame, const qreal c1Value) {
    mKeys.append({c0Frame*mFrameMultiplier, c0Value*mValueMultiplier,
                  pFrame*mFrameMultiplier, pValue*mValueMultiplier,
                  c1Frame*mFrameMultiplier, c1Value*mValueMultiplier});
}

qreal QrealSnapshot::getValue(const qreal relFrame) const {
    const KeySnaphot * prevKey = nullptr;
    const KeySnaphot * nextKey = nullptr;

    getPrevAndNextKey(relFrame, prevKey, nextKey);

    if(prevKey && nextKey && prevKey != nextKey) {
        const qCubicSegment2D seg{{qreal(prevKey->fFrame), prevKey->fValue},
                                  {prevKey->fC1Frame, prevKey->fC1Value},
                                  {nextKey->fC0Frame, nextKey->fC0Value},
                                  {qreal(nextKey->fFrame), nextKey->fValue}};
        auto frameSeg =  seg.xSeg();
        const qreal t = frameSeg.tAtLength(relFrame - frameSeg.p0());
        const auto valSeg = seg.ySeg();
        return valSeg.valAtT(t);
    } else if(prevKey) {
        return prevKey->fValue;
    } else if(nextKey) {
        return nextKey->fValue;
    }
    return mCurrentValue;
}

void QrealSnapshot::getPrevAndNextKey(const qreal relFrame,
                                      const QrealSnapshot::KeySnaphot *&prevKey,
                                      const QrealSnapshot::KeySnaphot *&nextKey) const {
    getPrevAndNextKey(relFrame, prevKey, nextKey, 0, mKeys.count() - 1);
}

void QrealSnapshot::getPrevAndNextKey(const qreal relFrame,
                                      const QrealSnapshot::KeySnaphot *&prevKey,
                                      const QrealSnapshot::KeySnaphot *&nextKey,
                                      const int minId, const int maxId) const {
    int prevId;
    int nextId;
    getPrevAndNextKeyId(relFrame, prevId, nextId, minId, maxId);
    prevKey = prevId == -1 ? nullptr : &mKeys.at(prevId);
    nextKey = nextId == -1 ? nullptr : &mKeys.at(nextId);
}

void QrealSnapshot::getPrevAndNextKeyId(const qreal relFrame,
                                        int &prevKey, int &nextKey) const {
    getPrevAndNextKeyId(relFrame, prevKey, nextKey, 0, mKeys.count() - 1);
}

void QrealSnapshot::getPrevAndNextKeyId(const qreal relFrame,
                                        int &prevKey, int &nextKey,
                                        const int minId, const int maxId) const {
    if(maxId < minId) {
        prevKey = -1;
        nextKey = -1;
        return;
    }
    if(maxId - minId == 0) {
        prevKey = minId;
        nextKey = minId;
        return;
    } else if(maxId - minId == 1) {
        const auto minKey = &mKeys[minId];
        const auto maxKey = &mKeys[maxId];
        prevKey = maxKey->fFrame > relFrame ? minId : -1;
        nextKey = minKey->fFrame < relFrame ? maxId : -1;
        return;
    }
    const int guessId = (minId + maxId)/2;
    const auto& guessKey = &mKeys[guessId];
    if(guessKey->fFrame > relFrame) {
        getPrevAndNextKeyId(relFrame, prevKey, nextKey, minId, guessId);
        return;
    } else if(guessKey->fFrame < relFrame) {
        getPrevAndNextKeyId(relFrame, prevKey, nextKey, guessId, maxId);
        return;
    }
    prevKey = guessId;
    nextKey = guessId;
    return;
}

QrealSnapshot::Iterator::Iterator(const qreal startFrame,
                                  const qreal sampleStep,
                                  const QrealSnapshot * const snap) :
    mSampleFrameStep(sampleStep),
    mInvFrameSpan(1/sampleStep),
    mSnapshot(snap) {
    mNextFrame = startFrame;
    mNextValue = mSnapshot->getValue(mNextFrame);
    mCurrentFrame = startFrame;
    updateSamples();
}

qreal QrealSnapshot::Iterator::getValueAndProgress(const qreal progress) {
    if(mStaticValue) return mPrevValue;
    qreal result;
    if(mInterpolate) {
        const qreal fracNext = (mCurrentFrame - mPrevFrame)*mInvFrameSpan;
        result = (mNextValue - mPrevValue)*fracNext + mPrevValue;
    } else result = mPrevValue;
    if(progress < 0) return result;
    mCurrentFrame += progress;
    if(mCurrentFrame > mNextFrame) updateSamples();
    return result;
}

bool QrealSnapshot::Iterator::staticValue() const {
    return mStaticValue;
}

void QrealSnapshot::Iterator::updateSamples() {
    if(mSnapshot->mKeys.count() < 2) {
        mPrevFrame = -TEN_MIL;
        mNextFrame = TEN_MIL;
        mPrevValue = mSnapshot->getValue(mCurrentFrame);
        mNextValue = mPrevValue;
        mStaticValue = true;
    } else {
        mPrevFrame = mNextFrame;
        mNextFrame += mSampleFrameStep;
        mPrevValue = mNextValue;
        mNextValue = mSnapshot->getValue(mNextFrame);
        mInterpolate = !isZero4Dec(mNextValue - mPrevValue);
        mStaticValue = false;
    }
}
