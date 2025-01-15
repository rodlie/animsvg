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

#include "smoothcurves.h"

class MovingAverage {
public:
    MovingAverage(const int w);

    void add(const QVector3D);
    const QVector3D average() const;
private:
    const int mWindow;
    QVector<QVector3D> mWindowData;
    QVector3D mSum;
    QVector3D mAverage;
    int mIndex;
    bool mFilterComplete;
};

MovingAverage::MovingAverage(const int w) : mWindow(w){
    mFilterComplete = false;
    mIndex = -1;
    mSum = QVector3D{0, 0};
    mAverage = QVector3D{0, 0};
    mWindowData = QVector<QVector3D>(mWindow, QVector3D{0, 0});
}

void MovingAverage::add(const QVector3D x) {
    mIndex = (mIndex + 1) % mWindow;
    mSum -= mWindowData[mIndex];
    mWindowData[mIndex] = x;
    mSum += x;
    if(!mFilterComplete && mIndex == mWindow - 1) {
        mFilterComplete = true;
    }
    if(mFilterComplete) {
        mAverage = mSum/mWindow;
    } else {
        mAverage = mSum/(mIndex + 1);
    }
}

const QVector3D MovingAverage::average() const {
    return mAverage;
}

void SmoothCurves::movingAverage(const QVector<QVector3D>& data,
                                 QVector<QVector3D>& smooth,
                                 const bool fixedStart,
                                 const bool fixedEnd,
                                 const int window) {
    smooth.clear();
    if(data.isEmpty()) return;
    MovingAverage ma(window);
    int iMin;
    int smoothCount;
    if(fixedStart) {
        iMin = 0;
        smoothCount = data.count();
        for(int i = 0; i < window; i++) ma.add(data.first());
    } else {
        iMin = window;
        smoothCount = data.count() - window;
        for(int i = 0; i < window; i++) ma.add(data.at(i));
    }
    if(smoothCount <= 0) return;
    smooth.reserve(smoothCount);
    for(int i = iMin; i < data.count(); i++) {
        ma.add(data.at(i));
        smooth << ma.average();
    }
    if(fixedEnd) {
        for(int i = 0; i < window; i++) {
            ma.add(data.last());
            smooth << ma.average();
        }
    }
}
