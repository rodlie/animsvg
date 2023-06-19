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
        mOutputFormatLabel->setText("<b>Format:</b> " + txt);
    }

    void setVideoCodecText(const QString &txt) {
        mVideoCodecLabel->setText("<b>Video codec:</b> " + txt);
    }

    void setPixelFormatText(const QString &txt) {
        mVideoPixelFormatLabel->setText("<b>Pixel format:</b> " + txt);
    }

    void setVideoBitrateText(const QString &txt) {
        mVideoBitrateLabel->setText("<b>Video bitrate:</b> " + txt);
    }

    void setAudioCodecText(const QString &txt) {
        mAudioCodecLabel->setText("<b>Audio codec:</b> " + txt);
    }

    void setAudioSampleRateText(const QString &txt) {
        mAudioSampleRateLabel->setText("<b>Audio sample rate:</b> " + txt);
    }

    void setAudioSampleFormatText(const QString &txt) {
        mAudioSampleFormatLabel->setText("<b>Audio sample format:</b> " + txt);
    }

    void setAudioBitrateText(const QString &txt) {
        mAudioBitrateLabel->setText("<b>Audio bitrate:</b> " + txt);
    }

    void setAudioChannelLayoutText(const QString &txt) {
        mAudioChannelLayoutLabel->setText("<b>Audio channel layout:</b> " + txt);
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
