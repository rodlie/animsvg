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

#ifndef TRANSFORMANIMATOR_H
#define TRANSFORMANIMATOR_H
#include "Properties/comboboxproperty.h"
#include "staticcomplexanimator.h"
#include "../skia/skiaincludes.h"
#include "transformvalues.h"

#include <QMatrix4x4>

class TransformUpdater;
class BoxPathPoint;
class MovablePoint;
class QPointFAnimator;

class CORE_EXPORT BasicTransformAnimator : public StaticComplexAnimator {
    e_OBJECT
    Q_OBJECT
protected:
    BasicTransformAnimator();
public:
    virtual void reset();
    virtual QMatrix4x4 getRelativeTransformAtFrame(
            const qreal relFrame, QMatrix4x4* postTransform = nullptr) const;
    virtual QMatrix4x4 getInheritedTransformAtFrame(const qreal relFrame) const;
    virtual QMatrix4x4 getTotalTransformAtFrame(const qreal relFrame) const;

    FrameRange prp_getIdenticalRelRange(const int relFrame) const;

    void resetScale();
    void resetTranslation();
    void resetRotation();

    void setScale(const qreal sx, const qreal sy, const qreal sz);
    void setPosition(const qreal x, const qreal y, const qreal z);
    void setRotation(const qreal x_rot, const qreal y_rot, const qreal z_rot);

    void startRotTransform();
    void startPosTransform();
    void startScaleTransform();

    void setRelativePos(const QVector3D &relPos);
    void moveByAbs(const QVector3D &absTrans);

    void rotateRelativeToSavedValue(const qreal x_rotRel, const qreal y_rotRel, const qreal z_rotRel);
    void translate(const qreal dX, const qreal dY, const qreal dZ);
    void scale(const qreal sx, const qreal sy, const qreal sz);
    void moveRelativeToSavedValue(const qreal dX,
                                  const qreal dY,
                                  const qreal dZ);

    qreal dx();
    qreal dy();
    qreal dz();
    qreal x_rot();
    qreal y_rot();
    qreal z_rot();
    qreal xScale();
    qreal yScale();
    qreal zScale();
    QVector3D pos();

    QVector3D mapAbsPosToRel(const QVector3D &absPos) const;
    QVector3D mapRelPosToAbs(const QVector3D &relPos) const;

    QVector3D mapFromParent(const QVector3D &parentRelPos) const;
    SkPoint3 mapAbsPosToRel(const SkPoint3 &absPos) const;
    SkPoint3 mapRelPosToAbs(const SkPoint3 &relPos) const;
    SkPoint3 mapFromParent(const SkPoint3 &parentRelPos) const;

    void scaleRelativeToSavedValue(const qreal sx,
                                   const qreal sy,
                                   const qreal sz,
                                   const QVector3D &pivot);
    void rotateRelativeToSavedValue(const qreal x_rotRel,
                                    const qreal y_rotRel,
                                    const qreal z_rotRel,
                                    const QVector3D &pivot);

    void updateRelativeTransform(const UpdateReason reason);
    void updateInheritedTransform(const UpdateReason reason);

    const QMatrix4x4 &getInheritedTransform() const;
    const QMatrix4x4 &getTotalTransform() const;
    const QMatrix4x4 &getRelativeTransform() const;

    void setParentTransformAnimator(BasicTransformAnimator *parent);

    QPointFAnimator *getPosAnimator() const;
    QPointFAnimator *getScaleAnimator() const;
    QrealAnimator *getRotAnimator() const;

    void updateTotalTransform(const UpdateReason reason);
    void setSVGEventsVisibility(const bool visible);

protected:
    QList<qsptr<BasicTransformAnimator>> mChildBoxes;

    QMatrix4x4 mRelTransform;
    QMatrix4x4 mPostTransform;
    QMatrix4x4 mInheritedTransform;
    QMatrix4x4 mTotalTransform;

    ConnContextQPtr<BasicTransformAnimator> mParentTransform;

    qsptr<QPointFAnimator> mPosAnimator;
    qsptr<QPointFAnimator> mScaleAnimator;
    qsptr<QrealAnimator> mRotAnimator;
    qsptr<ComboBoxProperty> mSVGBeginProperty;
    qsptr<ComboBoxProperty> mSVGEndProperty;

private:
    bool rotationFlipped() const;
signals:
    void totalTransformChanged(const UpdateReason);
    void inheritedTransformChanged(const UpdateReason);
};

class CORE_EXPORT AdvancedTransformAnimator : public BasicTransformAnimator {
    e_OBJECT
protected:
    AdvancedTransformAnimator();
public:
    void reset();
    QMatrix4x4 getRelativeTransformAtFrame(
            const qreal relFrame, QMatrix* postTransform = nullptr) const;

    void applyTransformEffects(const qreal relFrame,
                               qreal& pivotX, qreal& pivotY, qreal& pivotZ,
                               qreal& posX, qreal& posY, qreal& posZ,
                               qreal& x_rot, qreal& y_rot, qreal& z_rot,
                               qreal& scaleX, qreal& scaleY, qreal& scaleZ,
                               qreal& shearX, qreal& shearY, qreal& shearZ,
                               QMatrix4x4& postTransform) const;

    void setValues(const TransformValues& values);

    QMatrix4x4 getRotScaleShearTransform();
    void startRotScaleShearTransform();
    void resetRotScaleShear();

    void resetShear();
    void resetPivot();
    void setPivotFixedTransform(const QVector3D &newPivot);

    QVector3D getPivot(const qreal relFrame);
    QVector3D getPivot();
    qreal getPivotX();
    qreal getPivotY();
    qreal getPivotZ();

    void startShearTransform();
    void setShear(const qreal shearX, const qreal shearY, const qreal shearZ);

    qreal getOpacity();

    void startTransformSkipOpacity();

    void startOpacityTransform();
    void setOpacity(const qreal newOpacity);
    void setPivot(const qreal x, const qreal y, const qreal z);
    void startPivotTransform();

    void finishPivotTransform();
    QVector3D getPivotAbs();
    QVector3D getPivotAbs(const qreal relFrame);

    qreal getOpacity(const qreal relFrame);

    bool posOrPivotRecording() const;
    bool rotOrScaleOrPivotRecording() const;

    QPointFAnimator *getShearAnimator() const {
        return mShearAnimator.get();
    }

    QPointFAnimator *getPivotAnimator() const {
        return mPivotAnimator.get();
    }

    QrealAnimator *getOpacityAnimator() const {
        return mOpacityAnimator.get();
    }
private:
    qsptr<QPointFAnimator> mPivotAnimator;
    qsptr<QPointFAnimator> mShearAnimator;
    qsptr<QrealAnimator> mOpacityAnimator;
};

class CORE_EXPORT BoxTransformAnimator : public AdvancedTransformAnimator {
    e_OBJECT
protected:
    BoxTransformAnimator();
public:
    static const QString getSVGPropertyAction(const int value);
    QDomElement saveSVG(SvgExporter& exp, const FrameRange& visRange,
                        const QDomElement& child) const;
};

#endif // TRANSFORMANIMATOR_H
