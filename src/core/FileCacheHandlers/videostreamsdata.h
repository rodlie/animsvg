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

#ifndef VIDEOSTREAMSDATA_H
#define VIDEOSTREAMSDATA_H
#include "audiostreamsdata.h"

struct CORE_EXPORT VideoStreamsData {
private:
    explicit VideoStreamsData() {}

    ~VideoStreamsData() {
        if(fOpened) close();
    }

    static void sDestroy(VideoStreamsData * const p) {
        delete p;
    }
public:
    QString fPath;
    bool fOpened = false;
    qreal fFps = 0;
    int fTimeBaseNum = 0;
    int fTimeBaseDen = 1;
    int fFrameCount = 0;
    AVFormatContext *fFormatContext = nullptr;
    int fVideoStreamIndex = -1;
    AVStream * fVideoStream = nullptr;
    AVPacket * fPacket = nullptr;
    AVFrame *fDecodedFrame = nullptr;
    AVCodecContext * fCodecContext = nullptr;
    struct SwsContext * fSwsContext = nullptr;
    int fLastFrame = 0;
    int fWidth = 0;
    int fHeight = 0;

    stdsptr<const AudioStreamsData> fAudioData;

    static stdsptr<VideoStreamsData> sOpen(const QString& path);
private:
    void open(const QString& path);
    void open();
    void open(const char * const path);
    void close();
};
#endif // VIDEOSTREAMSDATA_H
