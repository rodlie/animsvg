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

#include "rastereffectcaller.h"


RasterEffectCaller::RasterEffectCaller(const HardwareSupport hwSupport,
                                       const bool forceMargin,
                                       const QMargins &margin) :
    fForceMargin(forceMargin),
    fHwSupport(hwSupport), fMargin(margin) {}

int RasterEffectCaller::cpuThreads(const int available,
                                   const int area) const {
    return qMin(area/(150*150) + 1, available);
}

#include "skia/skqtconversions.h"
void RasterEffectCaller::setSrcRect(const SkIRect &srcRect,
                                    const SkIRect &clampRect) {
    const int sl = srcRect.left();
    const int st = srcRect.top();
    const int sr = srcRect.right();
    const int sb = srcRect.bottom();

    const auto margins = getMargin(srcRect);
    const int ml = margins.left();
    const int mt = margins.top();
    const int mr = margins.right();
    const int mb = margins.bottom();

    const int l = sl - ml;
    const int t = st - mt;
    const int r = sr + mr;
    const int b = sb + mb;

    const int cl = clampRect.left();
    const int ct = clampRect.top();
    const int cr = clampRect.right();
    const int cb = clampRect.bottom();

    if(fForceMargin) {
        fSrcRect = SkIRect::MakeLTRB(qMax(l, cl - ml),
                                     qMax(t, ct - mt),
                                     qMin(r, cr + mr),
                                     qMin(b, cb + mb));
        fDstRect = fSrcRect;
    } else {
        fSrcRect = srcRect;
        fDstRect = SkIRect::MakeLTRB(qMax(l, cl), qMax(t, ct),
                                     qMin(r, cr), qMin(b, cb));
    }
}
