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

#ifndef SHADEREFFECTCALLER_H
#define SHADEREFFECTCALLER_H
#include "RasterEffects/rastereffectcaller.h"
#include "ShaderEffects/shadereffect.h"
#include "shadereffectprogram.h"
#include "../gpurendertools.h"
#include "shadereffectjs.h"

class CORE_EXPORT ShaderEffectCaller : public RasterEffectCaller {
    e_OBJECT
public:
    ShaderEffectCaller(std::unique_ptr<ShaderEffectJS>&& engine,
                       const ShaderEffectProgram& program,
                       const ShaderEffect* parentEffect,
                       const qreal& relFrame,
                       const qreal& resolution,
                       const qreal& influence);
    ~ShaderEffectCaller();

    void processGpu(QGL33 * const gl,
                    GpuRenderTools& renderTools);

    void calc(const ShaderEffect * pEff,
              const qreal relFrame,
              const qreal resolution,
              const qreal influence);

    ShaderEffectJS& getJSEngine()
    { return *mEngine; }

    UniformSpecifiers mUniformSpecifiers;
protected:
    QMargins getMargin(const SkIRect &srcRect);
private:
    void setupProgram(QGL33 * const gl);

    std::unique_ptr<ShaderEffectJS> mEngine;
    const GLuint mProgramId;
    const ShaderEffectProgram &mProgram;
};


#endif // SHADEREFFECTCALLER_H
