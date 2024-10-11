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

#include "memorychecker.h"


#if defined(Q_OS_WIN)
    #include "windowsincludes.h"
#elif defined(Q_OS_UNIX)
    #include "gperftools/tcmalloc.h"
    #include "../gperftools/include/gperftools/malloc_extension.h"
    #if defined(Q_OS_LINUX)
        #include <sys/sysinfo.h>
        #include <unistd.h>
    #elif defined(Q_OS_MACOS)
        #include <mach/mach_host.h>
        #include <mach/mach_init.h>
        #include <mach/host_info.h>
        #include <mach/task.h>
    #endif
#endif

#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "exceptions.h"
#include "hardwareinfo.h"
MemoryChecker *MemoryChecker::mInstance;

MemoryChecker::MemoryChecker(QObject * const parent) : QObject(parent) {
    mInstance = this;

    const intKB totRam = HardwareInfo::sRamKB();
    mLowFreeKB = totRam*20/100;
    mVeryLowFreeKB = totRam*15/100;
    mCriticalFreeKB = totRam*10/100;
}

char MemoryChecker::sLine[256];

void MemoryChecker::sGetFreeKB(intKB& procFreeKB, intKB& sysFreeKB) {
    const auto usageCap = eSettings::sInstance->fRamMBCap;

    longB enveUsedB(0);
    qint64 freeInternal = 0;
    intKB freeExternal(0);
#if defined(Q_OS_WIN)
    const auto processID = GetCurrentProcessId();
    const auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                      FALSE, processID);

    if(hProcess != NULL) {
        PROCESS_MEMORY_COUNTERS pmc;
        if(GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
            enveUsedB = longB(pmc.WorkingSetSize);
        }
    }

    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    const longB availPhysB(statex.ullAvailPhys);
    freeExternal = intKB(availPhysB);
#elif defined(Q_OS_UNIX)
    //    qDebug() << "";
    size_t virtual_memory_used;
    size_t physical_memory_used;
    size_t bytes_in_use_by_app;
    MallocExtension::instance()->eMemoryStats(&virtual_memory_used,
                                              &physical_memory_used,
                                              &bytes_in_use_by_app);
//    qDebug() << "virtual_memory_used" << intMB(longB(virtual_memory_used)).fValue;
//    qDebug() << "physical_memory_used" << intMB(longB(physical_memory_used)).fValue;
//    qDebug() << "bytes_in_use_by_app" << intMB(longB(bytes_in_use_by_app)).fValue;

    enveUsedB = longB(static_cast<qint64>(bytes_in_use_by_app));

    freeInternal = physical_memory_used - bytes_in_use_by_app;
    #if defined(Q_OS_LINUX)
        int found = 0;
        FILE * const meminfo = fopen("/proc/meminfo", "r");
        if(!meminfo) RuntimeThrow("Failed to open /proc/meminfo");
        while(fgets(sLine, sizeof(sLine), meminfo)) {
            int ramPartKB;
            if(sscanf(sLine, "MemFree: %d kB", &ramPartKB) == 1) {
    //            qDebug() << "MemFree" << intMB(intKB(ramPartKB)).fValue;
            } else if(sscanf(sLine, "Cached: %d kB", &ramPartKB) == 1) {
    //            qDebug() << "Cached" << intMB(intKB(ramPartKB)).fValue;
            } else if(sscanf(sLine, "Buffers: %d kB", &ramPartKB) == 1) {
    //            qDebug() << "Buffers" << intMB(intKB(ramPartKB)).fValue;
            } else continue;
            freeExternal.fValue += ramPartKB;
            if(++found == 3) break;
        }
        fclose(meminfo);
        if(found != 3) RuntimeThrow("Entries missing from /proc/meminfo");
    #elif defined(Q_OS_MACOS)
        mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
        vm_statistics_data_t vmstat;
        const auto ret = host_statistics(mach_host_self(), HOST_VM_INFO,
                                         (host_info_t)&vmstat, &count);
        if(ret != KERN_SUCCESS) RuntimeThrow("Could not retrieve memory usage");
        // auto total = vmstat.wire_count + vmstat.active_count + vmstat.inactive_count + vmstat.free_count;
        const int pageSize = 4;
        freeExternal.fValue += vmstat.inactive_count * 4;
        freeExternal.fValue += vmstat.free_count * 4;
    #endif
#endif

    const intKB enveUsedKB(enveUsedB);
    if(usageCap.fValue > 0) {
        procFreeKB = intKB(usageCap) - enveUsedKB;
    } else {
        procFreeKB = HardwareInfo::sRamKB() - enveUsedKB;
    }

    sysFreeKB = intKB(longB(freeInternal)) + freeExternal;

//    qDebug() << "free" << intMB(sysFreeKB).fValue;
//    qDebug() << "usage" << 100 - 100*sysFreeKB.fValue/HardwareInfo::sRamKB().fValue;
    const qint64 releaseBytes = 500L*1024L*1024L;
    if(freeInternal > releaseBytes) {
#if defined(Q_OS_WIN)

#elif defined(Q_OS_UNIX)
    MallocExtension::instance()->ReleaseToSystem(releaseBytes);
//        qDebug() << "released";
#endif
    }
}

void MemoryChecker::checkMemory() {
    intKB procFreeKB;
    intKB sysFreeKB;
    sGetFreeKB(procFreeKB, sysFreeKB);

    if(sysFreeKB < mLowFreeKB) {
        const intKB toFree = mLowFreeKB - sysFreeKB;
        if(sysFreeKB < mCriticalFreeKB) {
            emit handleMemoryState(CRITICAL_MEMORY_STATE, longB(toFree));
            mLastMemoryState = CRITICAL_MEMORY_STATE;
        } else if(sysFreeKB < mVeryLowFreeKB) {
            emit handleMemoryState(VERY_LOW_MEMORY_STATE, longB(toFree));
            mLastMemoryState = VERY_LOW_MEMORY_STATE;
        } else {
            emit handleMemoryState(LOW_MEMORY_STATE, longB(toFree));
            mLastMemoryState = LOW_MEMORY_STATE;
        }
    } else if(procFreeKB.fValue < 0) {
        emit handleMemoryState(LOW_MEMORY_STATE, longB(-procFreeKB));
        mLastMemoryState = LOW_MEMORY_STATE;
    } else/* if(mLastMemoryState != NORMAL_MEMORY_STATE)*/ {
        emit handleMemoryState(NORMAL_MEMORY_STATE, longB(0));
        mLastMemoryState = NORMAL_MEMORY_STATE;
    }

    emit memoryCheckedKB(sysFreeKB, HardwareInfo::sRamKB());
}
