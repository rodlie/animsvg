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

#include "Tasks/updatable.h"
#include "Private/Tasks/taskscheduler.h"
#include "Private/Tasks/taskexecutor.h"

using namespace Friction::Core;

HardwareSupport eCpuTask::hardwareSupport() const
{
    return HardwareSupport::cpuOnly;
}

void eCpuTask::processGpu(QGL33 * const gl,
                          SwitchableContext &context)
{
    Q_UNUSED(gl)
    Q_UNUSED(context)
}

void eCpuTask::queTaskNow()
{
    TaskScheduler::instance()->queCpuTask(ref<eTask>());
}

HardwareSupport eHddTask::hardwareSupport() const
{
    return HardwareSupport::cpuOnly;
}

void eHddTask::processGpu(QGL33 * const gl,
                          SwitchableContext &context)
{
    Q_UNUSED(gl)
    Q_UNUSED(context)
}

void eHddTask::queTaskNow()
{
    TaskScheduler::instance()->queHddTask(ref<eTask>());
}

eCustomCpuTask::eCustomCpuTask(const Func &before,
                               const Func &run,
                               const Func &after,
                               const Func &canceled)
    : mBefore(before)
    , mRun(run)
    , mAfter(after)
    , mCanceled(canceled)
{

}

void eCustomCpuTask::beforeProcessing(const Hardware)
{
    if (mBefore) { mBefore(); }
}

void eCustomCpuTask::afterProcessing()
{
    if (mAfter) { mAfter(); }
}

void eCustomCpuTask::afterCanceled()
{
    if (mCanceled) { mCanceled(); }
}

void eCustomCpuTask::process()
{
    if (mRun) { mRun(); }
}

template<typename T>
SPtrDisposer<T>::SPtrDisposer(const T &ptr)
    : mPtr(ptr)
{

}

template<typename T>
void SPtrDisposer<T>::beforeProcessing(const Hardware)
{

}

template<typename T>
void SPtrDisposer<T>::process()
{
    mPtr.reset();
}

template<typename T>
eTask *SPtrDisposer<T>::sDispose(const T &ptr)
{
    const auto disposer = enve::make_shared<SPtrDisposer<T>>(ptr);
    if (disposer->scheduleTask()) { return disposer.get(); }
    return nullptr;
}

template<typename T>
void SPtrDisposer<T>::afterProcessing()
{

}
