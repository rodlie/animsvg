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

#ifndef UPDATABLE_H
#define UPDATABLE_H
#include <QList>
#include <QEventLoop>

#include "etask.h"

class ExecController;

class CORE_EXPORT eCpuTask : public eTask {
    e_OBJECT
public:
    HardwareSupport hardwareSupport() const final {
        return HardwareSupport::cpuOnly;
    }

    void processGpu(QGL33 * const gl,
                    SwitchableContext &context) final {
        Q_UNUSED(gl)
        Q_UNUSED(context)
    }
protected:
    void queTaskNow() final;
};

class CORE_EXPORT eHddTask : public eTask {
    e_OBJECT
public:
    HardwareSupport hardwareSupport() const {
        return HardwareSupport::cpuOnly;
    }

    void processGpu(QGL33 * const gl,
                    SwitchableContext &context) {
        Q_UNUSED(gl)
        Q_UNUSED(context)
    }
protected:
    void queTaskNow();
};

class CORE_EXPORT eCustomCpuTask : public eCpuTask {
    e_OBJECT
protected:
    using Func = std::function<void()>;
    eCustomCpuTask(const Func& before,
                   const Func& run,
                   const Func& after,
                   const Func& canceled) :
        mBefore(before), mRun(run),
        mAfter(after), mCanceled(canceled) {}

    void beforeProcessing(const Hardware) final {
        if(mBefore) mBefore();
    }

    void afterProcessing() final {
        if(mAfter) mAfter();
    }

    void afterCanceled() final {
        if(mCanceled) mCanceled();
    }

    void process() final { if(mRun) mRun(); }
private:
    const Func mBefore;
    const Func mRun;
    const Func mAfter;
    const Func mCanceled;
};

template <typename T>
class CORE_EXPORT SPtrDisposer : public eCpuTask {
    e_OBJECT
protected:
    SPtrDisposer(const T& ptr) : mPtr(ptr) {}
public:
    void beforeProcessing(const Hardware) final {}
    void process() final { mPtr.reset(); }
    static eTask* sDispose(const T& ptr) {
        const auto disposer = enve::make_shared<SPtrDisposer<T>>(ptr);
        if(disposer->scheduleTask()) return disposer.get();
        return nullptr;
    }
protected:
    void afterProcessing() final {}
private:
    T mPtr;
};

#endif // UPDATABLE_H
