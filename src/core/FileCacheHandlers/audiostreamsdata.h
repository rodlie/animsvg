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

#ifndef AUDIOSTREAMSDATA_H
#define AUDIOSTREAMSDATA_H
#include "soundreader.h"

struct CORE_EXPORT AudioStreamsData : public QObject {
private:
    explicit AudioStreamsData();

    ~AudioStreamsData();

    static void sDestroy(AudioStreamsData * const p) {
        delete p;
    }
protected:
    friend struct VideoStreamsData;
    static stdsptr<const AudioStreamsData> sOpen(
            const QString& path,
            AVFormatContext * const formatContext);
public:
    bool lock() {
        if(mLocked) return false;
        mLocked = true;
        return true;
    }

    bool unlock() {
        if(!mLocked) return false;
        mLocked = false;
        if(mUpdateSwrPlanned) updateSwrContext();
        return true;
    }

    QString fPath;
    bool fOpened = false;
    qreal fDurationSec = 0;
    AVFormatContext *fFormatContext = nullptr;
    int fAudioStreamIndex = -1;
    AVStream * fAudioStream = nullptr;
    AVPacket * fPacket = nullptr;
    AVFrame *fDecodedFrame = nullptr;
    AVCodecContext * fCodecContext = nullptr;
    struct SwrContext * fSwrContext = nullptr;
    int fLastDstSample = 0;

    void updateSwrContext();

    static stdsptr<AudioStreamsData> sOpen(const QString& path);
private:
    void open(const QString& path, AVFormatContext * const formatContext);
    void open(const QString& path);
    void open();
    void open(const char * const path);
    void open(AVFormatContext * const formatContext);

    void close();

    bool mLocked = false;
    bool mUpdateSwrPlanned = false;
};

#endif // AUDIOSTREAMSDATA_H
