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

#ifndef EXECCONTROLLER_H
#define EXECCONTROLLER_H

#include "taskexecutor.h"

class CORE_EXPORT ExecController : public QObject {
    Q_OBJECT
protected:
    ExecController(TaskExecutor * const executor,
                   QObject * const parent = nullptr);
public:
    void stop();
    void stopAndWait();
signals:
    void processTaskSignal(const stdsptr<eTask>&);
    void finishedTaskSignal(const stdsptr<eTask>&, ExecController*);
protected:
    void start();

    TaskExecutor * const mExecutor;
    QThread * const mThread;
private:
    void finishedTask(const stdsptr<eTask>& task);
};

class CORE_EXPORT CpuExecController : public ExecController {
public:
    CpuExecController(QObject * const parent = nullptr);
};

class CORE_EXPORT GpuExecController : public ExecController {
public:
    GpuExecController(QObject * const parent = nullptr);

    void initialize();
};

class CORE_EXPORT HddExecController : public ExecController {
public:
    HddExecController(QObject * const parent = nullptr);
};

#endif // EXECCONTROLLER_H
