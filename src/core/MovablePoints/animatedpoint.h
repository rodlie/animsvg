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

#ifndef ANIMATEDPOINT_H
#define ANIMATEDPOINT_H

#include "movablepoint.h"
#include <QPointF>
#include <QPainter>

#include "../qrect3d.h"
#include "Animators/qvector3danimator.h"
#include "skia/skiaincludes.h"

class BoundingBox;
class NodePoint;

class PropertyUpdater;

class QVector3DAnimator;

class CORE_EXPORT AnimatedPoint : public MovablePoint {
    e_OBJECT
protected:
    AnimatedPoint(QVector3DAnimator * const associatedAnimator,
                  const MovablePointType type);
    AnimatedPoint(QVector3DAnimator * const associatedAnimator,
                  BasicTransformAnimator * const trans,
                  const MovablePointType type);
public:
    QVector3D getRelativePos() const;
    void setRelativePos(const QVector3D &relPos);

    void startTransform();
    void finishTransform();
    void cancelTransform();

    QVector3DAnimator * getAnimator() const {
        return mAssociatedAnimator_k;
    }

    QrealAnimator * getXAnimator() const {
        return mAssociatedAnimator_k->getXAnimator();
    }

    QrealAnimator * getYAnimator() const {
        return mAssociatedAnimator_k->getYAnimator();
    }
protected:
    void setValue(const QVector3D value) {
        mAssociatedAnimator_k->setBaseValue(value);
    }

    QVector3D getValue() const {
        return mAssociatedAnimator_k->getBaseValue();
    }
private:
    QVector3DAnimator* const mAssociatedAnimator_k;
};

#endif // ANIMATEDPOINT_H
