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

#ifndef FRICTION_UPDATABLE_H
#define FRICTION_UPDATABLE_H

#include <QList>
#include <QEventLoop>

#include "etask.h"

class ExecController;

class CORE_EXPORT eCpuTask : public eTask
{
    e_OBJECT
public:
    Friction::Core::HardwareSupport hardwareSupport() const final;

    void processGpu(QGL33 * const gl,
                    SwitchableContext &context) final;

protected:
    void queTaskNow() final;
};

class CORE_EXPORT eHddTask : public eTask
{
    e_OBJECT
public:
    Friction::Core::HardwareSupport hardwareSupport() const;

    void processGpu(QGL33 * const gl,
                    SwitchableContext &context);

protected:
    void queTaskNow();
};

class CORE_EXPORT eCustomCpuTask : public eCpuTask
{
    e_OBJECT
protected:
    using Func = std::function<void()>;
    eCustomCpuTask(const Func& before,
                   const Func& run,
                   const Func& after,
                   const Func& canceled);

    void beforeProcessing(const Friction::Core::Hardware) final;
    void afterProcessing() final;
    void afterCanceled() final;
    void process() final;

private:
    const Func mBefore;
    const Func mRun;
    const Func mAfter;
    const Func mCanceled;
};

template <typename T>
class CORE_EXPORT SPtrDisposer : public eCpuTask
{
    e_OBJECT
protected:
    SPtrDisposer(const T& ptr);

public:
    void beforeProcessing(const Friction::Core::Hardware) final;
    void process() final;
    static eTask* sDispose(const T& ptr);

protected:
    void afterProcessing() final;

private:
    T mPtr;
};

#endif // FRICTION_UPDATABLE_H
