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

#ifndef PATHEFFECTSTASK_H
#define PATHEFFECTSTASK_H
#include "../skia/skiaincludes.h"
#include "../Tasks/updatable.h"
#include "../Boxes/pathbox.h"
#include "patheffectcaller.h"

class CORE_EXPORT PathEffectsTask : public eCpuTask {
    friend class PathBox;

    typedef QList<stdsptr<PathEffectCaller>> EffectsList;
public:
    PathEffectsTask(PathBoxRenderData* const target,
                    EffectsList&& pathEffects,
                    EffectsList&& fillEffects,
                    EffectsList&& outlineBaseEffects,
                    EffectsList&& outlineEffects);

    bool isEmpty() const {
        return mPathEffects.isEmpty() &&
               mFillEffects.isEmpty() &&
               mOutlineBaseEffects.isEmpty() &&
               mOutlineEffects.isEmpty();
    }

    void process();

    void afterProcessing() {
        if(!mTarget) return;
        mTarget->fPath = mPath;
        mTarget->fFillPath = mFillPath;
        mTarget->fOutlineBasePath = mOutlineBasePath;
        mTarget->fOutlinePath = mOutlinePath;
    }
private:
    const stdptr<PathBoxRenderData> mTarget;
    const SkStroke mStroker;

    const EffectsList mPathEffects;
    const EffectsList mFillEffects;
    const EffectsList mOutlineBaseEffects;
    const EffectsList mOutlineEffects;

    SkPath mPath;
    SkPath mFillPath;
    SkPath mOutlineBasePath;
    SkPath mOutlinePath;
};

#endif // PATHEFFECTSTASK_H
