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

#ifndef TARGETTRANSFORMEFFECT_H
#define TARGETTRANSFORMEFFECT_H

#include "transformeffect.h"

#include "Properties/boxtargetproperty.h"

class TargetTransformEffect : public TransformEffect {
public:
    TargetTransformEffect(const QString& name,
                          const TransformEffectType type);

    FrameRange prp_getIdenticalRelRange(const int relFrame) const;
protected:
    virtual void setRotScaleAfterTargetChange(
            BoundingBox* const oldTarget,
            BoundingBox* const newTarget);

    BoxTargetProperty* targetProperty() const;
private:
    QPointF mPosBeforeTargetChange;
    ConnContextQPtr<BoundingBox> mTargetConn;
    qsptr<BoxTargetProperty> mTarget;
};

#endif // TARGETTRANSFORMEFFECT_H
