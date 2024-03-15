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

#ifndef RENDERSETTINGS_H
#define RENDERSETTINGS_H

#include "core_global.h"

#include <QPointF>
#include "ReadWrite/basicreadwrite.h"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/opt.h>
}

struct CORE_EXPORT RenderSettings {
    void write(eWriteStream& dst) const;
    void read(eReadStream& src);

    qreal fResolution = 1;
    qreal fBaseFps = 24;
    qreal fFps = 24;
    qreal fFrameInc = 1;
    AVRational fTimeBase = { 1, 24 }; // inverse of fps - 1/fps
    int fVideoWidth = 0;
    int fVideoHeight = 0;

    int fBaseWidth = 0;
    int fBaseHeight = 0;

    int fMinFrame = 0;
    int fMaxFrame = 0;
};

#endif // RENDERSETTINGS_H
