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

#ifndef HARDWAREINFO_H
#define HARDWAREINFO_H

#include "core_global.h"

#include "Private/esettings.h"

class CORE_EXPORT HardwareInfo {
    HardwareInfo() = delete;
public:
    static void sUpdateInfo();

    static int sCpuThreads() { return mCpuThreads; }
    static intKB sRamKB() { return mRamKB; }
    static GpuVendor sGpuVendor() { return mGpuVendor; }
    static const QString sGpuVendorString() { return mGpuVendorString; }
    static const QString sGpuRendererString() { return mGpuRendererString; }
    static const QString sGpuVersionString() { return mGpuVersionString; }

private:
    static int mCpuThreads;
    static intKB mRamKB;
    static GpuVendor mGpuVendor;
    static QString mGpuVendorString;
    static QString mGpuRendererString;
    static QString mGpuVersionString;
};

#endif // HARDWAREINFO_H
