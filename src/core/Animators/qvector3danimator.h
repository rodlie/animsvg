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

#ifndef QVECTOR3DANIMATOR_H
#define QVECTOR3DANIMATOR_H

#include <QVector3D>
#include <QMatrix4x4>

#include "staticcomplexanimator.h"
#include "../skia/skiaincludes.h"

// TODO: rename to QVector3DAnimator
class CORE_EXPORT QVector3DAnimator : public StaticComplexAnimator {
    e_OBJECT
protected:
    QVector3DAnimator(const QString& name);
    QVector3DAnimator(const QVector3D& iniValue,
                    const QVector3D& minValue,
                    const QVector3D& maxValue,
                    const QVector3D& valueStep,
                    const QString& name);
    QVector3DAnimator(const QVector3D& iniValue,
                    const QVector3D& minValue,
                    const QVector3D& maxValue,
                    const QVector3D& valueStep,
                    const QString& nameX,
                    const QString& nameY,
                    const QString& nameZ,
                    const QString& name);
public:
    QJSValue prp_getBaseJSValue(QJSEngine& e) const;
    QJSValue prp_getBaseJSValue(QJSEngine& e, const qreal relFrame) const;
    QJSValue prp_getEffectiveJSValue(QJSEngine& e) const;
    QJSValue prp_getEffectiveJSValue(QJSEngine& e, const qreal relFrame) const;

    void setBaseValue(const float valX, const float valY) {
        setBaseValue(QVector3D(valX, valY, getSavedZValue()));
    }
    void setBaseValue(const float valX, const float valY, const float valZ) {
        setBaseValue(QVector3D(valX, valY, valZ));
    }
    void setBaseValue(const QPointF &val);
    void setBaseValue(const QVector3D &val);

    void setBaseValueWithoutCallingUpdater(const QPointF &val);
    void setBaseValueWithoutCallingUpdater(const QVector3D &val);
    void incBaseValuesWithoutCallingUpdater(const qreal x, const qreal y);
    void incBaseValuesWithoutCallingUpdater(const qreal x, const qreal y, const qreal z);
    void incBaseValues(const qreal x, const qreal y);
    void incBaseValues(const qreal x, const qreal y, const qreal z);
    void multCurrentValues(const qreal sx, const qreal sy);
    void multCurrentValues(const qreal sx, const qreal sy, const qreal sz);

    QPointF getSavedValue();
    QVector3D getSavedValue3();
    qreal getSavedXValue();
    qreal getSavedYValue();
    qreal getSavedZValue();

    void incAllBaseValues(const qreal x, const qreal y);
    void incAllBaseValues(const qreal x, const qreal y, const qreal z);

    QrealAnimator *getXAnimator();
    QrealAnimator *getYAnimator();
    QrealAnimator *getZAnimator();

    void multSavedValueToCurrentValue(const qreal sx,
                                      const qreal sy);
    void multSavedValueToCurrentValue(const qreal sx,
                                      const qreal sy,
                                      const qreal sz);
    void incSavedValueToCurrentValue(const qreal incXBy,
                                     const qreal incYBy);
    void incSavedValueToCurrentValue(const qreal incXBy,
                                     const qreal incYBy,
                                     const qreal incZBy);

    void setValuesRange(const qreal minVal, const qreal maxVal);

    QPointF getBaseValue() const;
    QVector3D getBaseValue3() const;
    QPointF getBaseValueAtAbsFrame(const qreal frame) const;
    QVector3D getBaseValueAtAbsFrame3(const qreal frame) const;
    QPointF getBaseValue(const qreal relFrame) const;
    QVector3D getBaseValue3(const qreal relFrame) const;

    void setPrefferedValueStep(const qreal valueStep);

    bool getBeingTransformed();

    QPointF getEffectiveValue() const;
    QVector3D getEffectiveValue3() const;

    QPointF getEffectiveValueAtAbsFrame(const qreal frame) const;
    QVector3D getEffectiveValueAtAbsFrame3(const qreal frame) const;

    QPointF getEffectiveValue(const qreal relFrame) const;
    QVector3D getEffectiveValue3(const qreal relFrame) const;

    qreal getEffectiveXValue(const qreal relFrame);
    qreal getEffectiveXValue();

    qreal getEffectiveYValue(const qreal relFrame);
    qreal getEffectiveYValue();

    qreal getEffectiveZValue(const qreal relFrame);
    qreal getEffectiveZValue();

    void applyTransform(const QMatrix &transform);
    void applyTransform(const QMatrix4x4 &transform);

    void saveQPointFSVG(SvgExporter& exp,
                        QDomElement& parent,
                        const FrameRange& visRange,
                        const QString& name,
                        const bool transform = false,
                        const QString& type = "") const;
    void saveQPointFSVGX(SvgExporter& exp,
                         QDomElement& parent,
                         const FrameRange& visRange,
                         const QString& name,
                         const qreal y,
                         const qreal multiplier,
                         const bool transform = false,
                         const QString& type = "",
                         const QString& beginEvent = "",
                         const QString& endEvent = "",
                         const bool motion = false,
                         const bool motionRotate = false,
                         const QString & motionPath = QString()) const;
    void saveQPointFSVGY(SvgExporter& exp,
                         QDomElement& parent,
                         const FrameRange& visRange,
                         const QString& name,
                         const qreal x,
                         const qreal multiplier,
                         const bool transform = false,
                         const QString& type = "",
                         const QString& beginEvent = "",
                         const QString& endEvent = "",
                         const bool motion = false,
                         const bool motionRotate = false,
                         const QString & motionPath = QString()) const;
    QDomElement prp_writePropertyXEV_impl(const XevExporter& exp) const;
protected:
    qsptr<QrealAnimator> mXAnimator;
    qsptr<QrealAnimator> mYAnimator;
    qsptr<QrealAnimator> mZAnimator;
};

#endif // QVECTOR3DANIMATOR_H
