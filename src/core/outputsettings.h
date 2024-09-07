/*
#
# enve2d - https://github.com/enve2d
#
# Copyright (c) enve2d developers
# Copyright (C) 2016-2020 Maurycy Liebner
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
*/

#ifndef OUTPUTSETTINGS_H
#define OUTPUTSETTINGS_H

#include "core_global.h"

#include <QString>
#include "Private/esettings.h"
#include "smartPointers/ememory.h"
#include "ReadWrite/basicreadwrite.h"
#include "formatoptions.h"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/opt.h>
}

struct CORE_EXPORT OutputSettings
{
    static const std::map<int, QString> sSampleFormatNames;
    static QString sGetChannelsLayoutName(const uint64_t &layout);
    static uint64_t sGetChannelsLayout(const QString &name);

    void write(eWriteStream& dst) const;
    void read(eReadStream& src);

    const AVOutputFormat *fOutputFormat = nullptr;

    bool fVideoEnabled = false;
    const AVCodec *fVideoCodec = nullptr;
    AVPixelFormat fVideoPixelFormat = AV_PIX_FMT_NONE;
    int fVideoBitrate = 0;
    int fVideoProfile = FF_PROFILE_UNKNOWN;
    Friction::Core::FormatOptions fVideoOptions;

    bool fAudioEnabled = false;
    const AVCodec *fAudioCodec = nullptr;
    AVSampleFormat fAudioSampleFormat = AV_SAMPLE_FMT_NONE;
    uint64_t fAudioChannelsLayout = 0;
    int fAudioSampleRate = 0;
    int fAudioBitrate = 0;
};

class CORE_EXPORT OutputSettingsProfile : public SelfRef
{
    Q_OBJECT
    e_OBJECT

public:
    OutputSettingsProfile();

    const QString &getName() const { return mName; }
    void setName(const QString &name) { mName = name; }

    const OutputSettings &getSettings() const { return mSettings; }
    void setSettings(const OutputSettings &settings);

    void save();
    void load(const QString &path);

    bool wasSaved() const { return !mPath.isEmpty(); }
    void removeFile();
    const QString &path() const { return mPath; }

    static OutputSettingsProfile* sGetByName(const QString &name);
    static QList<qsptr<OutputSettingsProfile>> sOutputProfiles;
    static bool sOutputProfilesLoaded;

    static Friction::Core::FormatOptions toFormatOptions(const Friction::Core::FormatOptionsList &list);
    static Friction::Core::FormatOptionsList toFormatOptionsList(const Friction::Core::FormatOptions &options);

signals:
    void changed();

private:
    QString mPath;
    QString mName = tr("Untitled");
    OutputSettings mSettings;
};

#endif // OUTPUTSETTINGS_H
