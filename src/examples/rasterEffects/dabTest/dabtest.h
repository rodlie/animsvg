// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DABTEST_H
#define DABTEST_H

#include "dabtest_global.h"
#include "enveCore/gpurendertools.h"

struct Dab {
    Dab(const float x, const float y,
        const float radius, const float aspectRatio,
        const float angle, const float hardness, const float opaque,
        const float red, const float green, const float blue, const float alpha) :
        fX(x), fY(y),
        fRadius(radius), fAspectRatio(aspectRatio),
        fSinTh(sin(angle)), fCosTh(cos(angle)),
        fHardness(hardness), fOpaque(opaque),
        fRed(red), fGreen(green), fBlue(blue), fAlpha(alpha),
        fSeg1Offset(1), fSeg1Slope(-(1.f/fHardness - 1.f)),
        fSeg2Offset(fHardness/(1.f - fHardness)),
        fSeg2Slope(-fHardness/(1.f - fHardness)) {

    }

    Dab() {}

    float fX = 0.5f; // 0
    float fY = 0.5f; // 1
    float fRadius = 0.5f; // 2
    float fAspectRatio = 1; // 3
    float fSinTh = 0; // 4
    float fCosTh = 1; // 5
    float fHardness = 1; // 6
    float fOpaque = 1; // 7
    float fRed = 1; // 8
    float fGreen = 0; // 9
    float fBlue = 0; // 10
    float fAlpha = 1; // 11
    float fSeg1Offset = 1; // 12
    float fSeg1Slope = -(1.f/fHardness - 1.f); // 13
    float fSeg2Offset = fHardness/(1.f - fHardness); // 14
    float fSeg2Slope = -fHardness/(1.f - fHardness); // 15
};

class DabTestCaller000 : public RasterEffectCaller {
public:
    DabTestCaller000(const Dab& dab) :
        RasterEffectCaller(HardwareSupport::gpuOnly), mDab(dab) {}

    void processGpu(QGL33 * const gl,
                    GpuRenderTools& renderTools);
    void processCpu(CpuRenderTools& renderTools,
                    const CpuRenderData& data) {
        Q_UNUSED(renderTools)
        Q_UNUSED(data)
    }
private:
    static bool sInitialized;
    static void sInitialize(QGL33 * const gl);
    static GLuint sDataId;
    static GLuint sProgramId;

    const Dab mDab;
};

class DabTest000 : public CustomRasterEffect {
public:
    DabTest000();

    stdsptr<RasterEffectCaller> getEffectCaller(
            const qreal relFrame, const qreal resolution,
            const qreal influence, BoxRenderData * const data) const;

    CustomIdentifier getIdentifier() const;
private:
    qsptr<QrealAnimator> mRadius;
    qsptr<QrealAnimator> mHardness;
};

#endif // DABTEST_H
