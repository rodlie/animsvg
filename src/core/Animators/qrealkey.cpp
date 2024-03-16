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

#include "qrealkey.h"
#include "qrealanimator.h"
#include "complexanimator.h"
#include "qrealpoint.h"

QrealKey::QrealKey(const qreal value, const int frame,
                   QrealAnimator * const parentAnimator) :
    GraphKey(frame, parentAnimator) {
    mValue = value;
    setC0ValueVar(mValue);
    setC1ValueVar(mValue);
}

QrealKey::QrealKey(QrealAnimator * const parentAnimator) :
    QrealKey(0, 0, parentAnimator) { }

void QrealKey::incValue(const qreal incBy) {
    setValue(mValue + incBy);
}

QrealAnimator *QrealKey::getParentQrealAnimator() const {
    return static_cast<QrealAnimator*>(mParentAnimator.data());
}

//bool QrealKey::isNear(qreal frameT, qreal valueT,
//                        qreal pixelsPerFrame, qreal pixelsPerValue) {
//    qreal value = getValue();
//    qreal frame = getFrame();
//    if(qAbs(frameT - frame)*pixelsPerFrame > mRadius) return false;
//    if(qAbs(valueT - value)*pixelsPerValue > mRadius) return false;
//    return true;
//}


qreal QrealKey::getValue() const { return mValue; }

void QrealKey::setValue(const qreal value) {
    const qreal dVal = value - mValue;
    setC0ValueVar(c0Clamped().getRawYValue() + dVal);
    setC1ValueVar(c1Clamped().getRawYValue() + dVal);

    mValue = value;
    if(!this->mParentAnimator) return;
    mParentAnimator->anim_updateAfterChangedKey(this);
}

void QrealKey::writeKey(eWriteStream& dst) {
    GraphKey::writeKey(dst);
    dst << mValue;
}

void QrealKey::readKey(eReadStream& src) {
    GraphKey::readKey(src);
    src >> mValue;

    if(src.evFileVersion() < 3) {
        bool c0Enabled; src >> c0Enabled;
        setC0Enabled(c0Enabled);
        readC0Clamped(src);

        bool c1Enabled; src >> c1Enabled;
        setC1Enabled(c1Enabled);
        readC1Clamped(src);
    }
}

void QrealKey::finishValueTransform() {
    UndoRedo ur;
    const qreal oldValue = mSavedValue;
    const qreal newValue = mValue;
    ur.fUndo = [this, oldValue]() {
        setValue(oldValue);
    };
    ur.fRedo = [this, newValue]() {
        setValue(newValue);
    };
    addUndoRedo(ur);

    finishCtrlPointsValueTransform();
}

void QrealKey::startValueTransform() {
    mSavedValue = mValue;
    startCtrlPointsValueTransform();
}

void QrealKey::cancelValueTransform() {
    setValue(mSavedValue);
    cancelCtrlPointsValueTransform();
}

bool QrealKey::differsFromKey(Key *key) const {
    if(key == this) return false;
    const auto qaKey = static_cast<QrealKey*>(key);
    if(isZero4Dec(qaKey->getValue() - mValue)) {
        if(key->getRelFrame() > mRelFrame) {
            if(qaKey->getC0Enabled() || getC1Enabled()) return true;
        } else {
            if(qaKey->getC1Enabled() || getC0Enabled()) return true;
        }
        return false;
    }
    return true;
}

void QrealKey::changeFrameAndValueBy(const QPointF &frameValueChange) {
    setValue(frameValueChange.y() + mSavedValue);
    GraphKey::changeFrameAndValueBy(frameValueChange);
}
