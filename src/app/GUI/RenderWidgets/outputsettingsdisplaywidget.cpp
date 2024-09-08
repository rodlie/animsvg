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

#include "outputsettingsdisplaywidget.h"
#include "GUI/global.h"
#include "appsupport.h"

using namespace Friction::Core;

OutputSettingsDisplayWidget::OutputSettingsDisplayWidget(QWidget *parent)
    : QWidget(parent)
{
    mMainLayout = new QVBoxLayout(this);
    setLayout(mMainLayout);
    mAudioVideoLayout = new QVBoxLayout();
    mOutputFormatLabel = new QLabel("<b>Format:</b><br>", this);
    mVideoCodecLabel = new QLabel("<b>Video codec:</b><br>", this);
    mVideoPixelFormatLabel = new QLabel("<b>Pixel format:</b><br>", this);
    mVideoBitrateLabel = new QLabel("<b>Video bitrate:</b><br>", this);
    mAudioCodecLabel = new QLabel("<b>Audio codec:</b><br>", this);
    mAudioSampleRateLabel = new QLabel("<b>Audio sample rate:</b><br>", this);
    mAudioSampleFormatLabel = new QLabel("<b>Audio sample format:</b><br>", this);
    mAudioBitrateLabel = new QLabel("<b>Audio bitrate:</b><br>", this);
    mAudioChannelLayoutLabel = new QLabel("<b>Audio channel layout:</b><br>", this);

    mOutputFormatLabel->setWordWrap(true);
    mVideoCodecLabel->setWordWrap(true);
    mVideoCodecLabel->setWordWrap(true);
    mVideoPixelFormatLabel->setWordWrap(true);
    mVideoBitrateLabel->setWordWrap(true);
    mAudioCodecLabel->setWordWrap(true);
    mAudioSampleRateLabel->setWordWrap(true);
    mAudioSampleFormatLabel->setWordWrap(true);
    mAudioBitrateLabel->setWordWrap(true);
    mAudioChannelLayoutLabel->setWordWrap(true);

    mVideoLayout = new QVBoxLayout();
    mVideoLayout->addWidget(mVideoCodecLabel);
    mVideoLayout->addWidget(mVideoPixelFormatLabel);
    mVideoLayout->addWidget(mVideoBitrateLabel);

    mAudioLayout = new QVBoxLayout();
    mAudioLayout->addWidget(mAudioCodecLabel);
    mAudioLayout->addWidget(mAudioSampleRateLabel);
    mAudioLayout->addWidget(mAudioSampleFormatLabel);
    mAudioLayout->addWidget(mAudioBitrateLabel);
    mAudioLayout->addWidget(mAudioChannelLayoutLabel);

    mAudioVideoLayout->addLayout(mVideoLayout);
    mAudioVideoLayout->addLayout(mAudioLayout);

    //mAudioLayout->setSpacing(0);
    //mVideoLayout->setSpacing(0);

    mMainLayout->addWidget(mOutputFormatLabel);
    mMainLayout->addLayout(mAudioVideoLayout);
}

void OutputSettingsDisplayWidget::setOutputSettings(const OutputSettings &settings) {
    if(!settings.fOutputFormat) {
        setOutputFormatText("-");
    } else {
        setOutputFormatText(AppSupport::filterFormatsName(QString(settings.fOutputFormat->name)));
    }
    if(!mAlwaysShowAll) {
        setVideoLabelsVisible(settings.fVideoEnabled);
    }
    if(!settings.fVideoCodec) {
        setVideoCodecText("-");
    } else {
        setVideoCodecText(QString(settings.fVideoCodec->name),
                          settings.fVideoCodec,
                          settings.fVideoProfile);
    }
    const char *pixelFormat = av_get_pix_fmt_name(settings.fVideoPixelFormat);
    if(!pixelFormat) {
        setPixelFormatText("-");
    } else {
        setPixelFormatText(QString(pixelFormat));
    }
    setVideoBitrateText(QString::number(settings.fVideoBitrate/1000000) + " Mbps");
    if(!mAlwaysShowAll) {
        setAudioLabelsVisible(settings.fAudioEnabled);
    }
    if(!settings.fAudioCodec) {
        setAudioCodecText("-");
    } else {
        setAudioCodecText(QString(settings.fAudioCodec->name));
    }
    setAudioSampleRateText(QString::number(settings.fAudioSampleRate) + " Hz");
    int formatId = settings.fAudioSampleFormat;
    if(OutputSettings::sSampleFormatNames.find(formatId) ==
            OutputSettings::sSampleFormatNames.end()) {
        setAudioSampleFormatText("-");
    } else {
        setAudioSampleFormatText(OutputSettings::sSampleFormatNames.at(formatId));
    }
    setAudioBitrateText(QString::number(settings.fAudioBitrate/1000) + " kbps");

    const auto channLay = OutputSettings::sGetChannelsLayoutName(
                settings.fAudioChannelsLayout);
    setAudioChannelLayoutText(channLay);
}
