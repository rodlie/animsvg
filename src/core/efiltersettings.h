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

#ifndef EFILTERSETTINGS_H
#define EFILTERSETTINGS_H

#include <QObject>

#include "core_global.h"
#include "skia/skiaincludes.h"
#include "simplemath.h"

class CORE_EXPORT eFilterSettings : public QObject {
    Q_OBJECT
public:
    eFilterSettings();

    static eFilterSettings* sInstance;

    static void sSetEnveRenderFilter(const SkFilterQuality filter) {
        sInstance->setEnveRenderFilter(filter);
    }

    static void sSetOutputRenderFilter(const SkFilterQuality filter) {
        sInstance->setOutputRenderFilter(filter);
    }

    static void sSetDisplayFilter(const SkFilterQuality filter) {
        sSetSmartDisplay(false);
        sInstance->mDisplayFilter = filter;
    }

    static void sSetSmartDisplay(const bool smart) {
        sInstance->mSmartDisplay = smart;
    }

    static SkFilterQuality sRender() {
        return sInstance->mRender;
    }

    static SkFilterQuality sDisplay() {
        return sInstance->mDisplayFilter;
    }

    static bool sSmartDisplat() {
        return sInstance->mSmartDisplay;
    }

    static SkFilterQuality sDisplay(const qreal zoom,
                                    const qreal resolution) {
        if(sInstance->mSmartDisplay) {
            const qreal scale = zoom/resolution;
            if(isOne4Dec(scale)) return kNone_SkFilterQuality;
            else if(scale > 2.5) return kNone_SkFilterQuality;
            else if(scale < 0.5) return kMedium_SkFilterQuality;
            return kLow_SkFilterQuality;
        } else return sDisplay();
    }

    static void sSwitchToEnveRender() {
        sInstance->mCurrentRender = RenderFilter::enve;
        sInstance->updateRenderFilter();
    }

    static void sSwitchToOutputRender() {
        sInstance->mCurrentRender = RenderFilter::output;
        sInstance->updateRenderFilter();
    }

    void setEnveRenderFilter(const SkFilterQuality filter);
    void setOutputRenderFilter(const SkFilterQuality filter);
signals:
    void renderFilterChanged(const SkFilterQuality filter);
private:
    void updateRenderFilter() {
        SkFilterQuality newFilter;
        if(mCurrentRender == RenderFilter::enve) newFilter = mEnveRender;
        else newFilter = mOutputRender;
        if(newFilter == mRender) return;
        mRender = newFilter;
        emit renderFilterChanged(newFilter);
    }

    SkFilterQuality mEnveRender = SkFilterQuality::kHigh_SkFilterQuality;
    SkFilterQuality mOutputRender = SkFilterQuality::kHigh_SkFilterQuality;

    enum class RenderFilter { enve, output };
    RenderFilter mCurrentRender = RenderFilter::enve;
    SkFilterQuality mRender = mEnveRender;

    bool mSmartDisplay = true;
    SkFilterQuality mDisplayFilter = SkFilterQuality::kNone_SkFilterQuality;
};

#endif // EFILTERSETTINGS_H
