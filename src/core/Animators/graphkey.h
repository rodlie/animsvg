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

#ifndef GRAPHKEY_H
#define GRAPHKEY_H

#include "key.h"
#include "clampedpoint.h"

enum class QrealPointType : short;
class QrealPoint;

class CORE_EXPORT GraphKey : public Key {
public:
    GraphKey(const int frame, Animator * const parentAnimator);
    GraphKey(Animator * const parentAnimator);

    virtual void setValueForGraph(const qreal value) = 0;
    virtual qreal getValueForGraph() const = 0;

    virtual void changeFrameAndValueBy(const QVector3D &frameValueChange);

    void startFrameTransform() final;
    void finishFrameTransform() final;
    void cancelFrameTransform() final;

    void writeKey(eWriteStream& dst);
    void readKey(eReadStream& src);

    void scaleFrameAndUpdateParentAnimator(
            const int relativeToFrame,
            const qreal scaleFactor,
            const bool useSavedFrame);
protected:
    void setRelFrame(const int frame);
public:
    const ClampedPoint& c0Clamped() const
    { return mC0Clamped; }
    const ClampedPoint& c1Clamped() const
    { return mC1Clamped; }

    void startCtrlPointsValueTransform();
    void finishCtrlPointsValueTransform();
    void cancelCtrlPointsValueTransform();

    qreal getC1Value() const;
    qreal getC0Value() const;

    void setC1Value(const qreal value);
    void setC0Value(const qreal value);

    void drawGraphKey(QPainter * const p,
                      const QColor &paintColor) const;

    void setC0Frame(const qreal frame);
    void setC1Frame(const qreal frame);

    QrealPoint *mousePress(const qreal frameT,
                           const qreal valueT,
                           const qreal pixelsPerFrame,
                           const qreal pixelsPerValue);

    void updateCtrlFromCtrl(const QrealPointType type,
                            const qreal pixelsPerFrame,
                            const qreal pixelsPerValue);
    void setCtrlsMode(const CtrlsMode mode);
    void setCtrlsModeAction(const CtrlsMode mode);
    void guessCtrlsMode();
    CtrlsMode getCtrlsMode() const;

    void constrainC0MinFrame(const qreal minRelFrame);
    void constrainC1MaxFrame(const qreal maxRelFrame);

    qreal getPrevKeyValueForGraph() const;
    qreal getNextKeyValueForGraph() const;

    void makeC0C1Smooth();
    bool isInsideRect(const QRect3D &valueFrameRect) const;

    void setC0FrameVar(const qreal startFrame);
    void setC1FrameVar(const qreal endFrame);

    qreal getC0Frame() const;
    qreal getC1Frame() const;

    qreal getC0AbsFrame() const;
    qreal getC1AbsFrame() const;

    void setC0EnabledAction(const bool enabled);
    void setC1EnabledAction(const bool enabled);

    void setC0Enabled(const bool enabled);
    void setC1Enabled(const bool enabled);

    bool getC1Enabled() const;
    bool getC0Enabled() const;

    void setC0ValueVar(const qreal value);
    void setC1ValueVar(const qreal value);
    void constrainC1Value(const qreal minVal,
                          const qreal maxVal);
    void constrainC0Value(const qreal minVal,
                          const qreal maxVal);

    void startFrameAndValueTransform();
    void finishFrameAndValueTransform();
    void cancelFrameAndValueTransform();
protected:
    void readC0Clamped(eReadStream& src) {
        src.read(&mC0Clamped, sizeof(ClampedPoint));
    }

    void readC1Clamped(eReadStream& src) {
        src.read(&mC1Clamped, sizeof(ClampedPoint));
    }
private:
    bool mC0Enabled = false;
    bool mC1Enabled = false;

    bool mSavedC0Enabled = false;
    bool mSavedC1Enabled = false;

    CtrlsMode mCtrlsMode = CtrlsMode::symmetric;

    ClampedPoint mC0Clamped = ClampedPoint(0, 0);
    ClampedPoint mC1Clamped = ClampedPoint(0, 0);

    stdsptr<QrealPoint> mC1Point;
    stdsptr<QrealPoint> mKeyPoint;
    stdsptr<QrealPoint> mC0Point;
};

#endif // GRAPHKEY_H
