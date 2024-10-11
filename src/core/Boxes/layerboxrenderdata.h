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

#ifndef CONTAINERBOXRENDERDATA_H
#define CONTAINERBOXRENDERDATA_H
#include "boxrenderdata.h"

struct CORE_EXPORT PathClipOp {
    SkPath fClipPath;
    SkClipOp fClipPathOp;
    bool fClipPathAA;

    void clip(SkCanvas * const canvas) const {
        canvas->clipPath(fClipPath, fClipPathOp, fClipPathAA);
    }
};

struct CORE_EXPORT PathClip {
    int fTargetIndex;
    bool fAbove;
    BoundingBox* fTargetBox = nullptr;
    QList<PathClipOp> fClipOps;

    void clip(SkCanvas * const canvas) const {
        for(const auto& op : fClipOps) op.clip(canvas);
    }
};

struct CORE_EXPORT ChildRenderData {
    template <typename T>
    ChildRenderData(const stdsptr<T>& data) :
        fData(data) {}

    inline BoxRenderData* operator->() const
    { return fData.operator->(); }

    stdsptr<BoxRenderData> fData;
    bool fIsMain = false;
    PathClip fClip;
};

struct CORE_EXPORT ContainerBoxRenderData : public BoxRenderData {
    e_OBJECT
public:
    ContainerBoxRenderData(BoundingBox * const parentBox);

    QList<ChildRenderData> fChildrenRenderData;
protected:
    void drawSk(SkCanvas * const canvas);
    void transformRenderCanvas(SkCanvas& canvas) const final;
    void updateRelBoundingRect();
};

#endif // CONTAINERBOXRENDERDATA_H
