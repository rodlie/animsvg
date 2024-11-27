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

#ifndef RASTEREFFECTCALLER_H
#define RASTEREFFECTCALLER_H
#include "../smartPointers/stdselfref.h"
#include "../glhelpers.h"
#include "../gpurendertools.h"
#include "../cpurendertools.h"
#include "../hardwareenums.h"

enum class HardwareSupport : short;

class CORE_EXPORT RasterEffectCaller : public StdSelfRef {
    e_OBJECT
public:
    RasterEffectCaller(const HardwareSupport hwSupport,
                       const bool forceMargin = false,
                       const QMargins& margin = QMargins());

    virtual void processGpu(QGL33 * const gl,
                            GpuRenderTools& renderTools) {
        Q_UNUSED(gl)
        Q_UNUSED(renderTools)
    }

    virtual void processCpu(CpuRenderTools& renderTools,
                            const CpuRenderData& data){
        Q_UNUSED(renderTools)
        Q_UNUSED(data)
    }

    virtual int cpuThreads(const int available, const int area) const;

    virtual bool srcDstSeparation() const { return true; }

    HardwareSupport hardwareSupport() const {
        return fHwSupport;
    }

    bool interchangeable() const {
        return fHwSupport != HardwareSupport::cpuOnly &&
               fHwSupport != HardwareSupport::gpuOnly;
    }

    void setSrcRect(const SkIRect& srcRect, const SkIRect& clampRect);

    const SkIRect& getDstRect() const { return  fDstRect; }
protected:
    virtual QMargins getMargin(const SkIRect& srcRect) {
        Q_UNUSED(srcRect)
        return fMargin;
    }

    const bool fForceMargin;
    const HardwareSupport fHwSupport;
    const QMargins fMargin;
    SkIRect fSrcRect;
    SkIRect fDstRect;
};

#endif // RASTEREFFECTCALLER_H
