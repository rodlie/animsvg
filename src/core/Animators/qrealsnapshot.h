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

#ifndef QREALSNAPSHOT_H
#define QREALSNAPSHOT_H
class QrealKey;
#include "../pointhelpers.h"
#include "../framerange.h"

class CORE_EXPORT QrealSnapshot {
    friend class Iterator;
    struct KeySnaphot {
        qreal fC0Frame;
        qreal fC0Value;

        qreal fFrame;
        qreal fValue;

        qreal fC1Frame;
        qreal fC1Value;
    };
public:
    class Iterator {
    public:
        Iterator(const qreal startFrame, const qreal sampleStep,
                 const QrealSnapshot * const snap);

        qreal getValueAndProgress(const qreal progress);

        bool staticValue() const;
    private:
        void updateSamples();

        bool mInterpolate;
        bool mStaticValue;
        const qreal mSampleFrameStep;
        const qreal mInvFrameSpan;
        qreal mPrevFrame;
        qreal mNextFrame;
        qreal mPrevValue;
        qreal mNextValue;
        qreal mCurrentFrame;
        const QrealSnapshot * const mSnapshot;
    };

    QrealSnapshot() {}
    QrealSnapshot(const qreal currentValue) :
        QrealSnapshot(currentValue, 1, 1) {}
    QrealSnapshot(const qreal currentValue,
                  const qreal frameMultiplier,
                  const qreal valueMultiplier) :
        mCurrentValue(currentValue*valueMultiplier),
        mFrameMultiplier(frameMultiplier),
        mValueMultiplier(valueMultiplier) {}

    void appendKey(const QrealKey * const key);
    void appendKey(const qreal c0Frame, const qreal c0Value,
                   const qreal pFrame, const qreal pValue,
                   const qreal c1Frame, const qreal c1Value);

    qreal getValue(const qreal relFrame) const;
protected:
    void getPrevAndNextKey(const qreal relFrame,
                           KeySnaphot const *& prevKey,
                           KeySnaphot const *& nextKey) const;
    void getPrevAndNextKey(const qreal relFrame,
                           KeySnaphot const *& prevKey,
                           KeySnaphot const *& nextKey,
                           const int minId, const int maxId) const;
    void getPrevAndNextKeyId(const qreal relFrame,
                             int & prevKey, int & nextKey) const;
    void getPrevAndNextKeyId(const qreal relFrame,
                             int & prevKey, int & nextKey,
                             const int minId, const int maxId) const;
    qreal mCurrentValue;
    QList<KeySnaphot> mKeys;

    qreal mFrameMultiplier;
    qreal mValueMultiplier;
};
#endif // QREALSNAPSHOT_H
