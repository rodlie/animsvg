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

#include "esoundsettings.h"

eSoundSettings* eSoundSettings::sInstance = nullptr;

eSoundSettings::eSoundSettings() {
    Q_ASSERT(!sInstance);
    sInstance = this;
}

int eSoundSettings::sSampleRate() {
    return sInstance->fSampleRate;
}

AVSampleFormat eSoundSettings::sSampleFormat() {
    return sInstance->fSampleFormat;
}

uint64_t eSoundSettings::sChannelLayout() {
    return sInstance->fChannelLayout;
}

bool eSoundSettings::sPlanarFormat() {
    return sInstance->planarFormat();
}

int eSoundSettings::sChannelCount() {
    return sInstance->channelCount();
}

int eSoundSettings::sBytesPerSample() {
    return sInstance->bytesPerSample();
}

eSoundSettingsData& eSoundSettings::sData() {
    return *sInstance;
}

void eSoundSettings::sSetSampleRate(const int sampleRate) {
    sInstance->setSampleRate(sampleRate);
}

void eSoundSettings::sSetSampleFormat(const AVSampleFormat format) {
    sInstance->setSampleFormat(format);
}

void eSoundSettings::sSetChannelLayout(const uint64_t layout) {
    sInstance->setChannelLayout(layout);
}

void eSoundSettings::sSave() {
    sInstance->save();
}

void eSoundSettings::sRestore() {
    sInstance->restore();
}

void eSoundSettings::save() {
    mSaved = sData();
}

void eSoundSettings::restore() {
    setAll(mSaved);
}

void eSoundSettings::setAll(const eSoundSettings::eSoundSettingsData &data) {
    if(eSoundSettingsData::operator==(data)) return;
    eSoundSettingsData::operator=(data);
    emit settingsChanged();
}

void eSoundSettings::setSampleRate(const int sampleRate) {
    if(sampleRate == fSampleRate) return;
    fSampleRate = sampleRate;
    emit settingsChanged();
}

void eSoundSettings::setSampleFormat(const AVSampleFormat format) {
    if(format == fSampleFormat) return;
    fSampleFormat = format;
    emit settingsChanged();
}

void eSoundSettings::setChannelLayout(const uint64_t layout) {
    if(layout == fChannelLayout) return;
    fChannelLayout = layout;
    emit settingsChanged();
}
