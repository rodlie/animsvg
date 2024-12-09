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

#include "samples.h"

void Samples::write(eWriteStream& dst) const {
    dst.write(&fFormat, sizeof(AVSampleFormat));
    dst << fPlanar;
    dst << fSampleRate;
    dst << fSampleSize;
    dst << fChannelLayout;
    dst << fNChannels;
    dst << fSampleRange;
    const auto bytes = static_cast<ulong>(fSampleRange.span())*fSampleSize;
    if(fPlanar) {
        for(uint i = 0; i < fNChannels; i++) {
            dst.write(fData[i], static_cast<qint64>(bytes));
        }
    } else {
        const auto totBytes = bytes * fNChannels;
        dst.write(fData[0], static_cast<qint64>(totBytes));
    }
}

stdsptr<Samples> Samples::sRead(eReadStream& src) {
    AVSampleFormat format;
    bool planar;
    int sampleRate;
    uint sampleSize;
    uint64_t channelLayout;
    uint nChannels;
    SampleRange sampleRange;
    uchar ** data;

    src.read(&format, sizeof(AVSampleFormat));
    src >> planar;
    src >> sampleRate;
    src >> sampleSize;
    src >> channelLayout;
    src >> nChannels;
    src >> sampleRange;
    const auto bytes = static_cast<ulong>(sampleRange.span())*sampleSize;
    if(planar) {
        data = new uchar*[nChannels];
        for(uint i = 0; i < nChannels; i++) {
            src.read(data[i], static_cast<qint64>(bytes));
        }
    } else {
        data = new uchar*[1];
        const auto totBytes = bytes * nChannels;
        src.read(data[0], static_cast<qint64>(totBytes));
    }

    return enve::make_shared<Samples>(data, sampleRange, sampleRate,
                                      format, channelLayout);
}
