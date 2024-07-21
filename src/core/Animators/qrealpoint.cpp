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

#include "qrealpoint.h"
#include "graphkey.h"

QrealPoint::QrealPoint(QrealPointType type,
                       GraphKey * const parentKey,
                       const qreal radius) {
    mRadius = radius;
    mType = type;
    mParentKey = parentKey;
}

qreal QrealPoint::getRelFrame() {
    if(mType == QrealPointType::keyPt) return mParentKey->getRelFrame();
    if(mType == QrealPointType::c0Pt) return mParentKey->getC0Frame();
    /*if(mType == END_POINT)*/ return mParentKey->getC1Frame();
}

void QrealPoint::setRelFrame(const qreal frame) {
    if(mType == QrealPointType::keyPt) {
        return mParentKey->setRelFrameAndUpdateParentAnimator(qRound(frame));
    }
    if(mType == QrealPointType::c0Pt) return mParentKey->setC0Frame(frame);
    if(mType == QrealPointType::c1Pt) return mParentKey->setC1Frame(frame);
}

void QrealPoint::setAbsFrame(const qreal absFrame) {
    setRelFrame(mParentKey->absFrameToRelFrameF(absFrame));
}

void QrealPoint::startTransform() {
    mParentKey->startFrameAndValueTransform();
}

void QrealPoint::finishTransform() {
    mParentKey->finishFrameAndValueTransform();
}

qreal QrealPoint::getValue() {
    if(mType == QrealPointType::keyPt) return mParentKey->getValueForGraph();
    if(mType == QrealPointType::c0Pt) return mParentKey->getC0Value();
    /*if(mType == END_POINT)*/ return mParentKey->getC1Value();
}

void QrealPoint::setValue(const qreal value) {
    if(mType == QrealPointType::keyPt) return mParentKey->setValueForGraph(value);
    if(mType == QrealPointType::c0Pt) return mParentKey->setC0Value(value);
    if(mType == QrealPointType::c1Pt) return mParentKey->setC1Value(value);
}

bool QrealPoint::isSelected() {
    if(mType == QrealPointType::keyPt) return mParentKey->isSelected();
    return mIsSelected;
}

bool QrealPoint::isNear(const qreal absFrame, const qreal value,
                        const qreal pixelsPerFrame, const qreal pixelsPerValue) {
    if(qAbs(getAbsFrame() - absFrame)*pixelsPerFrame > mRadius) return false;
    if(qAbs(getValue() - value)*pixelsPerValue > mRadius) return false;
    return true;
}

void QrealPoint::moveBy(const qreal dFrame, const qreal dValue,
                        const qreal pixelsPerFrame,
                        const qreal pixelsPerValue) {
    if(mType == QrealPointType::keyPt) {
        mParentKey->changeFrameAndValueBy({dFrame, dValue});
    } else if(mType == QrealPointType::c0Pt) {
        const auto& pt = mParentKey->c0Clamped();
        const auto relTo = pt.getRawSavedValue();
        mParentKey->setC0FrameVar(relTo.x() + dFrame);
        mParentKey->setC0ValueVar(relTo.y() + dValue);
        mParentKey->updateCtrlFromCtrl(mType, pixelsPerFrame, pixelsPerValue);
    } else if(mType == QrealPointType::c1Pt) {
        const auto& pt = mParentKey->c1Clamped();
        const auto relTo = pt.getRawSavedValue();
        mParentKey->setC1FrameVar(relTo.x() + dFrame);
        mParentKey->setC1ValueVar(relTo.y() + dValue);
        mParentKey->updateCtrlFromCtrl(mType, pixelsPerFrame, pixelsPerValue);
    }
}

void QrealPoint::moveTo(const qreal frame, const qreal value,
                        const qreal pixelsPerFrame,
                        const qreal pixelsPerValue) {
    setAbsFrame(frame);
    setValue(value);
    if(isKeyPt()) return;
    mParentKey->updateCtrlFromCtrl(mType, pixelsPerFrame, pixelsPerValue);
}

qreal QrealPoint::getAbsFrame() {
    return mParentKey->relFrameToAbsFrameF(getRelFrame());
}

QPointF QrealPoint::getSavedFrameAndValue() const {
    const QPointF keySaved(mParentKey->getRelFrame(),
                           mParentKey->getValueForGraph());
    if(mType == QrealPointType::keyPt) {
        return keySaved;
    } else if(mType == QrealPointType::c0Pt) {
        const auto& pt = mParentKey->c0Clamped();
        return pt.getClampedSavedValue(keySaved);
    } else /*if(mType == END_POINT)*/ {
        const auto& pt = mParentKey->c1Clamped();
        return pt.getClampedSavedValue(keySaved);
    }
}

qreal QrealPoint::getSavedFrame() const {
    return getSavedFrameAndValue().x();
}

qreal QrealPoint::getSavedValue() const {
    return getSavedFrameAndValue().y();
}

void QrealPoint::setFrameAndValue(const qreal relFrame, const qreal value,
                                  const qreal pixelsPerFrame,
                                  const qreal pixelsPerValue) {
    setRelFrame(relFrame);
    setValue(value);
    if(isKeyPt()) return;
    mParentKey->updateCtrlFromCtrl(mType, pixelsPerFrame, pixelsPerValue);
}

void QrealPoint::draw(QPainter * const p, const QColor &paintColor) {
    const QPointF center(getAbsFrame(), getValue());

    p->setBrush(Qt::black);
    if(mHovered) gDrawCosmeticEllipse(p, center, mRadius + 1, mRadius + 1);
    else gDrawCosmeticEllipse(p, center, mRadius, mRadius);

    //p->setBrush(paintColor);
    p->setBrush(Qt::white);

    if(!isSelected()) p->setBrush(paintColor.lighter());

    gDrawCosmeticEllipse(p, center, mRadius - 1, mRadius - 1);
}

void QrealPoint::setSelected(const bool selected) {
    mIsSelected = selected;
}

bool QrealPoint::isEnabled() {
    if(isKeyPt()) return true;
    if(isC0Pt()) return mParentKey->getC0Enabled();
    /*if(isEndPoint())*/ return mParentKey->getC1Enabled();
}
