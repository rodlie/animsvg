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

#include "outputsettings.h"

#include "appsupport.h"

QList<qsptr<OutputSettingsProfile>> OutputSettingsProfile::sOutputProfiles;
bool OutputSettingsProfile::sOutputProfilesLoaded = false;

const std::map<int, QString> OutputSettings::sSampleFormatNames = {
    {AV_SAMPLE_FMT_U8, "8 bits unsigned"},
    {AV_SAMPLE_FMT_S16, "16 bits signed"},
    {AV_SAMPLE_FMT_S32, "32 bits signed"},
    {AV_SAMPLE_FMT_FLT, "32 bits float"},
    {AV_SAMPLE_FMT_DBL, "64 bits double"},
    {AV_SAMPLE_FMT_U8P, "8 bits unsigned, planar"},
    {AV_SAMPLE_FMT_S16P, "16 bits signed, planar"},
    {AV_SAMPLE_FMT_S32P, "32 bits signed, planar"},
    {AV_SAMPLE_FMT_FLTP, "32 bits float, planar"},
    {AV_SAMPLE_FMT_DBLP, "64 bits double, planar"},
    {AV_SAMPLE_FMT_S64, "64 bits signed"},
    {AV_SAMPLE_FMT_S64P, "64 bits signed, planar"}
};

const std::map<uint64_t, QString> gChannelLayouts = {
    {AV_CH_LAYOUT_MONO, "Mono"},
    {AV_CH_LAYOUT_STEREO, "Stereo"},
    {AV_CH_LAYOUT_2POINT1, "2.1"},
    {AV_CH_LAYOUT_SURROUND, "3.0"},
    {AV_CH_LAYOUT_2_1, "3.0(back)"},
    {AV_CH_LAYOUT_4POINT0, "4.0"},
    {AV_CH_LAYOUT_QUAD, "Quad"},
    {AV_CH_LAYOUT_2_2, "Quad(side)"},
    {AV_CH_LAYOUT_3POINT1, "3.1"},
    {AV_CH_LAYOUT_5POINT0_BACK, "5.0"},
    {AV_CH_LAYOUT_5POINT0, "5.0(side)"},
    {AV_CH_LAYOUT_4POINT1, "4.1"},
    {AV_CH_LAYOUT_5POINT1_BACK, "5.1"},
    {AV_CH_LAYOUT_5POINT1, "5.1(side)"},
    {AV_CH_LAYOUT_6POINT0, "6.0"},
    {AV_CH_LAYOUT_6POINT0_FRONT, "6.0(front)"},
    {AV_CH_LAYOUT_HEXAGONAL, "Hexagonal"},
    {AV_CH_LAYOUT_6POINT1, "6.1"},
    {AV_CH_LAYOUT_6POINT1_BACK, "6.1(back)"},
    {AV_CH_LAYOUT_6POINT1_FRONT, "6.1(front)"},
    {AV_CH_LAYOUT_7POINT0, "7.0"},
    {AV_CH_LAYOUT_7POINT0_FRONT, "7.0(front)"},
    {AV_CH_LAYOUT_7POINT1, "7.1"},
    {AV_CH_LAYOUT_7POINT1_WIDE_BACK, "7.1(wide)"},
    {AV_CH_LAYOUT_7POINT1_WIDE, "7.1(wide-side)"},
    {AV_CH_LAYOUT_OCTAGONAL, "Octagonal"},
    {AV_CH_LAYOUT_HEXADECAGONAL, "Hexadecagonal"},
    {AV_CH_LAYOUT_STEREO_DOWNMIX, "Downmix"},
    {AV_CH_FRONT_LEFT, "Front left"},
    {AV_CH_FRONT_RIGHT, "Front right"},
    {AV_CH_FRONT_CENTER, "Front center"},
    {AV_CH_LOW_FREQUENCY, "Low frequency"},
    {AV_CH_BACK_LEFT, "Back left"},
    {AV_CH_BACK_RIGHT, "Back right"},
    {AV_CH_TOP_CENTER, "Top center"},
    {AV_CH_TOP_FRONT_LEFT, "Top front left"},
    {AV_CH_TOP_FRONT_CENTER, "Top front center"},
    {AV_CH_TOP_FRONT_RIGHT, "Top front right"},
    {AV_CH_TOP_BACK_LEFT, "Top back left"},
    {AV_CH_TOP_BACK_CENTER, "Top back center"},
    {AV_CH_TOP_BACK_RIGHT, "Top back right"},
    {AV_CH_STEREO_LEFT, "Stereo left"},
    {AV_CH_STEREO_RIGHT, "Stereo right"},
    {AV_CH_WIDE_LEFT, "Wide left"},
    {AV_CH_WIDE_RIGHT, "Wide right"},
    {AV_CH_SURROUND_DIRECT_LEFT, "Surround direct left"},
    {AV_CH_SURROUND_DIRECT_RIGHT, "Surround direct right"},
    {AV_CH_LOW_FREQUENCY_2, "Low frequency 2"},
    {AV_CH_LAYOUT_NATIVE, "Native"}
};

QString OutputSettings::sGetChannelsLayoutName(const uint64_t &layout) {
    const auto it = gChannelLayouts.find(layout);
    if(it == gChannelLayouts.end()) return "Unrecognized";
    return it->second;
}

uint64_t OutputSettings::sGetChannelsLayout(const QString &name) {
    for(const auto& it : gChannelLayouts) {
        if(it.second == name) return it.first;
    }
    return AV_CH_LAYOUT_STEREO;
}

void OutputSettings::write(eWriteStream &dst) const {
    dst << (fOutputFormat ? QString(fOutputFormat->name) : "");

    dst << fVideoEnabled;
    dst << (fVideoCodec ? fVideoCodec->id : -1);
    dst.write(&fVideoPixelFormat, sizeof(AVPixelFormat));
    dst << fVideoBitrate;

    dst << fAudioEnabled;
    dst << (fAudioCodec ? fAudioCodec->id : -1);
    dst.write(&fAudioSampleFormat, sizeof(AVSampleFormat));
    dst << fAudioChannelsLayout;
    dst << fAudioSampleRate;
    dst << fAudioBitrate;
}

void OutputSettings::read(eReadStream &src) {
    QString formatName; src >> formatName;
    fOutputFormat = av_guess_format(formatName.toUtf8().data(),
                                   nullptr, nullptr);

    src >> fVideoEnabled;
    int videoCodecId; src >> videoCodecId;
    const auto avVideoCodecId = static_cast<AVCodecID>(videoCodecId);
    fVideoCodec = avcodec_find_encoder(avVideoCodecId);
    src.read(&fVideoPixelFormat, sizeof(AVPixelFormat));
    src >> fVideoBitrate;

    src >> fAudioEnabled;
    int audioCodecId; src >> audioCodecId;
    const auto avAudioCodecId = static_cast<AVCodecID>(audioCodecId);
    fAudioCodec = avcodec_find_encoder(avAudioCodecId);
    src.read(&fAudioSampleFormat, sizeof(AVSampleFormat));
    src >> fAudioChannelsLayout;
    src >> fAudioSampleRate;
    src >> fAudioBitrate;
}

OutputSettingsProfile::OutputSettingsProfile() {}

void OutputSettingsProfile::setSettings(const OutputSettings &settings) {
    mSettings = settings;
    emit changed();
}

void OutputSettingsProfile::save() {
    const QString path = QString::fromUtf8("%1/%2.eProf")
                         .arg(AppSupport::getAppOutputProfilesPath(), mName);
    QFile file(path);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text |
                 QIODevice::Truncate)) {
        QTextStream stream(&file);
        stream << "Name: " << mName << QT_ENDL;

        stream << "Format: ";
        const QString formatName = mSettings.fOutputFormat ?
                    mSettings.fOutputFormat->name : "null";
        stream << formatName << QT_ENDL;

        stream << "Video enabled: ";
        stream << (mSettings.fVideoEnabled ? "true" : "false") << QT_ENDL;
        if(mSettings.fVideoEnabled) {
            stream << "Video codec: ";
            const QString codecName = mSettings.fVideoCodec ?
                        mSettings.fVideoCodec->name : "null";
            stream << codecName << QT_ENDL;

            stream << "Pixel format: ";
            stream << av_get_pix_fmt_name(mSettings.fVideoPixelFormat) << QT_ENDL;

            stream << "Video bitrate: ";
            stream << QString::number(mSettings.fVideoBitrate) << QT_ENDL;
        }

        stream << "Audio enabled: ";
        stream << (mSettings.fAudioEnabled ? "true" : "false") << QT_ENDL;
        if(mSettings.fAudioEnabled) {
            stream << "Audio codec: ";
            const QString codecName = mSettings.fAudioCodec ?
                        mSettings.fAudioCodec->name : "null";
            stream << codecName << QT_ENDL;

            stream << "Sample format: ";
            stream << av_get_sample_fmt_name(mSettings.fAudioSampleFormat) << QT_ENDL;

            stream << "Channel layout: ";
            stream << OutputSettings::sGetChannelsLayoutName(
                          mSettings.fAudioChannelsLayout) << QT_ENDL;

            stream << "Audio sample-rate: ";
            stream << QString::number(mSettings.fAudioSampleRate) << QT_ENDL;

            stream << "Audio bitrate: ";
            stream << QString::number(mSettings.fAudioBitrate) << QT_ENDL;
        }
    } else RuntimeThrow("Could not save OutputProfile " + mName + ".\n" +
                        "Failed to open file: '" + path + "'.");
    mPath = path;
}

void OutputSettingsProfile::load(const QString &path) {
    QFile file(path);
    mPath = path;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while(!stream.atEnd()) {
            const QString line = stream.readLine();
            if(line.isEmpty()) continue;
            const auto varVal = line.split(":");
            if(varVal.count() != 2)
                RuntimeThrow("Unrecognized line '" + line +
                             "' in '" + path + "'");
            const auto var = varVal.at(0).trimmed();
            const auto val = varVal.at(1).trimmed();
            if(var == "Name") {
                mName = val;
            } else if(var == "Format") {
                mSettings.fOutputFormat = av_guess_format(
                            val.toUtf8().data(), nullptr, nullptr);
            } else if(var == "Video enabled") {
                mSettings.fVideoEnabled = (val == "true");
            } else if(var == "Video codec") {
                mSettings.fVideoCodec = avcodec_find_encoder_by_name(
                            val.toUtf8().data());
            } else if(var == "Pixel format") {
                mSettings.fVideoPixelFormat = av_get_pix_fmt(
                            val.toUtf8().data());
            } else if(var == "Video bitrate") {
                mSettings.fVideoBitrate = val.toInt();
            } else if(var == "Audio enabled") {
                mSettings.fAudioEnabled = (val == "true");
            } else if(var == "Audio codec") {
                mSettings.fAudioCodec = avcodec_find_encoder_by_name(
                            val.toUtf8().data());
            } else if(var == "Sample format") {
                mSettings.fAudioSampleFormat = av_get_sample_fmt(
                            val.toUtf8().data());
            } else if(var == "Channel layout") {
                mSettings.fAudioChannelsLayout =
                        OutputSettings::sGetChannelsLayout(val);
            } else if(var == "Audio sample-rate") {
                mSettings.fAudioSampleRate = val.toInt();
            } else if(var == "Audio bitrate") {
                mSettings.fAudioBitrate = val.toInt();
            } else RuntimeThrow("Unrecognized line '" + line +
                                "' in '" + path + "'");
        }
    } else RuntimeThrow("Could not load OutputProfile.\n" +
                        "Failed to open file: '" + path + "'.");
    emit changed();
}

void OutputSettingsProfile::removeFile() {
    if(!wasSaved()) return;
    QFile(mPath).remove();
}

OutputSettingsProfile *OutputSettingsProfile::sGetByName(const QString &name) {
    for(const auto& profile : sOutputProfiles) {
        if(profile->getName() == name) return profile.get();
    }
    return nullptr;
}
