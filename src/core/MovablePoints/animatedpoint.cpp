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

#include "animatedpoint.h"
#include "skia/skqtconversions.h"
#include "pointhelpers.h"

AnimatedPoint::AnimatedPoint(
        QVector3DAnimator * const associatedAnimator,
        const MovablePointType type) :
    MovablePoint(type),
    mAssociatedAnimator_k(associatedAnimator) {}

AnimatedPoint::AnimatedPoint(QVector3DAnimator * const associatedAnimator,
                             BasicTransformAnimator * const trans,
                             const MovablePointType type) :
    AnimatedPoint(associatedAnimator, type) {
    setTransform(trans);
}

void AnimatedPoint::setRelativePos(const QVector3D &relPos) {
    setValue(relPos);
}

QVector3D AnimatedPoint::getRelativePos() const {
    return getValue();
}

void AnimatedPoint::startTransform() {
    mAssociatedAnimator_k->prp_startTransform();
    MovablePoint::startTransform();
}

void AnimatedPoint::finishTransform() {
    mAssociatedAnimator_k->prp_finishTransform();
}

void AnimatedPoint::cancelTransform() {
    mAssociatedAnimator_k->prp_cancelTransform();
}
