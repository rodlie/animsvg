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

#ifndef AUDIOHANDLER_H
#define AUDIOHANDLER_H

#include <QAudioOutput>
#include <QDebug>

struct eSoundSettingsData;

class AudioHandler : public QObject {
public:
    AudioHandler();

    static AudioHandler* sInstance;

    struct DataRequest {
        char* fData;
        int fSize;

        operator bool() const {
            return fSize > 0;
        }
    };

    DataRequest dataRequest() {
        if(mAudioOutput && mAudioOutput->state() != QAudio::StoppedState) {
            if(mAudioOutput->bytesFree() >= mAudioOutput->periodSize())
                return {mAudioBuffer.data(), mAudioOutput->periodSize()};
        }
        return {nullptr, 0};
    }

    void provideData(const DataRequest& request) {
        if(!request || !mAudioIOOutput) return;
        mAudioIOOutput->write(request.fData, request.fSize);
    }

    void initializeAudio(eSoundSettingsData &soundSettings);
    void startAudio();
    void pauseAudio();
    void resumeAudio();
    void stopAudio();
    void setVolume(const int value);

    QAudioOutput* audioOutput() const { return mAudioOutput; }
private:
    QAudioDeviceInfo mAudioDevice;
    QAudioOutput *mAudioOutput = nullptr;
    QIODevice *mAudioIOOutput = nullptr; // not owned
    QAudioFormat mAudioFormat;

    QByteArray mAudioBuffer;
};

#endif // AUDIOHANDLER_H
