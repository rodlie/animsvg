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

#include "patheffectstask.h"

PathEffectsTask::PathEffectsTask(PathBoxRenderData * const target,
                                 EffectsList&& pathEffects,
                                 EffectsList&& fillEffects,
                                 EffectsList&& outlineBaseEffects,
                                 EffectsList&& outlineEffects) :
    mTarget(target), mStroker(target->fStroker),

    mPathEffects(std::move(pathEffects)),
    mFillEffects(std::move(fillEffects)),
    mOutlineBaseEffects(std::move(outlineBaseEffects)),
    mOutlineEffects(std::move(outlineEffects)),

    mPath(target->fPath), mFillPath(target->fFillPath),
    mOutlineBasePath(target->fOutlineBasePath),
    mOutlinePath(target->fOutlinePath) {}

void PathEffectsTask::process() {
    const bool pathReady = mPathEffects.isEmpty();
    const bool fillReady = pathReady && mFillEffects.isEmpty();
    const bool outlineBaseReady = pathReady && mOutlineBaseEffects.isEmpty();

    for(const auto& effect : mPathEffects) {
        effect->apply(mPath);
    }

    if(!fillReady) {
        mFillPath = mPath;
        for(const auto& effect : mFillEffects) {
            effect->apply(mFillPath);
        }
    }

    if(!outlineBaseReady) {
        mOutlineBasePath = mPath;
        for(const auto& effect : mOutlineBaseEffects) {
            effect->apply(mOutlineBasePath);
        }
        mStroker.strokePath(mOutlineBasePath, &mOutlinePath);
    }

    for(const auto& effect : mOutlineEffects) {
        effect->apply(mOutlinePath);
    }
}
