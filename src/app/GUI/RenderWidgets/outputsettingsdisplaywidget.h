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

#ifndef OUTPUTSETTINGSDISPLAYWIDGET_H
#define OUTPUTSETTINGSDISPLAYWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QGroupBox>
#include "renderinstancesettings.h"

class OutputSettingsDisplayWidget : public QWidget {
    Q_OBJECT
public:
    explicit OutputSettingsDisplayWidget(QWidget *parent = nullptr);
    void setOutputSettings(const OutputSettings &settings);

    void setAlwaysShowAll(const bool bT) {
        mAlwaysShowAll = bT;
        setVideoLabelsVisible(true);
        setAudioLabelsVisible(true);
    }
private:
    void setVideoLabelsVisible(const bool bT) {
        mVideoCodecLabel->setVisible(bT);
        mVideoPixelFormatLabel->setVisible(bT);
        mVideoBitrateLabel->setVisible(bT);
    }

    void setAudioLabelsVisible(const bool bT) {
        mAudioCodecLabel->setVisible(bT);
        mAudioSampleRateLabel->setVisible(bT);
        mAudioSampleFormatLabel->setVisible(bT);
        mAudioBitrateLabel->setVisible(bT);
        mAudioChannelLayoutLabel->setVisible(bT);
    }

    void setOutputFormatText(const QString &txt) {
        mOutputFormatLabel->setText("<b>Format:</b><br>" + txt);
    }

    void setVideoCodecText(const QString &txt) {
        mVideoCodecLabel->setText("<b>Video codec:</b><br>" + txt);
    }

    const QString getVideoProfileName(const AVCodec *codec,
                                      int profile)
    {
        if (profile < 0 || !codec) { return QString(); }
        switch (codec->id) {
        case AV_CODEC_ID_H264:
            switch (profile) {
            case FF_PROFILE_H264_BASELINE:
                return tr("Baseline");
                break;
            case FF_PROFILE_H264_MAIN:
                return tr("Main");
                break;
            case FF_PROFILE_H264_HIGH:
                return tr("High");
                break;
            default:;
            }
            break;
        case AV_CODEC_ID_PRORES:
            switch (profile) {
            case FF_PROFILE_PRORES_PROXY:
                return tr("Proxy");
                break;
            case FF_PROFILE_PRORES_LT:
                return tr("LT");
                break;
            case FF_PROFILE_PRORES_STANDARD:
                return tr("Standard");
                break;
            case FF_PROFILE_PRORES_HQ:
                return tr("HQ");
                break;
            case FF_PROFILE_PRORES_4444:
                return tr("4444");
                break;
            case FF_PROFILE_PRORES_XQ:
                return tr("XQ");
                break;
            default:;
            }
            break;
        case AV_CODEC_ID_AV1:
            switch (profile) {
            case FF_PROFILE_AV1_MAIN:
                return tr("Main");
                break;
            case FF_PROFILE_AV1_HIGH:
                return tr("High");
                break;
            case FF_PROFILE_AV1_PROFESSIONAL:
                return tr("Professional");
                break;
            default:;
            }
            break;
        case AV_CODEC_ID_VP9:
            switch (profile) {
            case FF_PROFILE_VP9_0:
                return tr("0");
                break;
            case FF_PROFILE_VP9_1:
                return tr("1");
                break;
            case FF_PROFILE_VP9_2:
                return tr("2");
                break;
            case FF_PROFILE_VP9_3:
                return tr("3");
                break;
            default:;
            }
            break;
        case AV_CODEC_ID_MPEG4:
            switch (profile) {
            case FF_PROFILE_MPEG4_SIMPLE:
                return tr("Simple");
                break;
            case FF_PROFILE_MPEG4_CORE:
                return tr("Core");
                break;
            case FF_PROFILE_MPEG4_MAIN:
                return tr("Main");
                break;
            default:;
            }
            break;
        case AV_CODEC_ID_VC1:
            switch (profile) {
            case FF_PROFILE_VC1_SIMPLE:
                return tr("Simple");
                break;
            case FF_PROFILE_VC1_MAIN:
                return tr("Main");
                break;
            case FF_PROFILE_VC1_COMPLEX:
                return tr("Complex");
                break;
            case FF_PROFILE_VC1_ADVANCED:
                return tr("Advanced");
                break;
            default:;
            }
            break;
        default:;
        }
        return QString();
    }
    void setVideoCodecText(const QString &txt,
                           const AVCodec *codec,
                           int profile)
    {
        QString label = "<b>%1:</b><br>%2";
        QString profileName = getVideoProfileName(codec, profile);
        if (!profileName.isEmpty()) {
            label.append(QString(" (%1)").arg(profileName));
        }
        mVideoCodecLabel->setText(label.arg(tr("Video codec"), txt));
    }

    void setPixelFormatText(const QString &txt) {
        mVideoPixelFormatLabel->setText("<b>Pixel format:</b><br>" + txt);
    }

    void setVideoBitrateText(const QString &txt) {
        mVideoBitrateLabel->setText("<b>Video bitrate:</b><br>" + txt);
    }

    void setAudioCodecText(const QString &txt) {
        mAudioCodecLabel->setText("<b>Audio codec:</b><br>" + txt);
    }

    void setAudioSampleRateText(const QString &txt) {
        mAudioSampleRateLabel->setText("<b>Audio sample rate:</b><br>" + txt);
    }

    void setAudioSampleFormatText(const QString &txt) {
        mAudioSampleFormatLabel->setText("<b>Audio sample format:</b><br>" + txt);
    }

    void setAudioBitrateText(const QString &txt) {
        mAudioBitrateLabel->setText("<b>Audio bitrate:</b><br>" + txt);
    }

    void setAudioChannelLayoutText(const QString &txt) {
        mAudioChannelLayoutLabel->setText("<b>Audio channel layout:</b><br>" + txt);
    }

    QVBoxLayout *mMainLayout;

    QLabel *mOutputFormatLabel;

    QVBoxLayout *mAudioVideoLayout;

    //QGroupBox *mVideoGroupBox;
    QVBoxLayout *mVideoLayout;
    QLabel *mVideoCodecLabel;
    QLabel *mVideoPixelFormatLabel;
    QLabel *mVideoBitrateLabel;

    //QGroupBox *mAudioGroupBox;
    QVBoxLayout *mAudioLayout;
    QLabel *mAudioCodecLabel;
    QLabel *mAudioSampleRateLabel;
    QLabel *mAudioSampleFormatLabel;
    QLabel *mAudioBitrateLabel;
    QLabel *mAudioChannelLayoutLabel;

    bool mAlwaysShowAll = false;
};

#endif // OUTPUTSETTINGSDISPLAYWIDGET_H
