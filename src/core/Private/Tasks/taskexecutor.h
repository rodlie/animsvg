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

#ifndef TASKEXECUTOR_H
#define TASKEXECUTOR_H

#include <QThread>

#include "Tasks/updatable.h"
#include "../qatomiclist.h"

class CORE_EXPORT TaskExecutor : public QObject {
    Q_OBJECT
public:
    TaskExecutor(QAtomicInt& count,
                 QAtomicList<stdsptr<eTask>>& tasks) :
        mUseCount(count), mTasks(tasks) {}

    static QAtomicInt sTaskFinishSignals;

    virtual void start();
    void stop();
signals:
    void finishedTask(const stdsptr<eTask>&);
protected:
    void processLoop();
private:
    virtual void processTask(eTask& task);

    std::atomic<bool> mStop;

    QAtomicInt& mUseCount;
    QAtomicList<stdsptr<eTask>>& mTasks;
};

class CORE_EXPORT CpuTaskExecutor : public TaskExecutor {
public:
    CpuTaskExecutor() : TaskExecutor(sUseCount, sTasks) {}

    static void sAddTask(const stdsptr<eTask>& ready);
    static void sAddTasks(const QList<stdsptr<eTask>>& ready);
    static int sUsageCount();
    static int sWaitingTasks();
private:
    static QAtomicInt sUseCount;
    static QAtomicList<stdsptr<eTask>> sTasks;
};

class CORE_EXPORT HddTaskExecutor : public TaskExecutor {
public:
    HddTaskExecutor() : TaskExecutor(sUseCount, sTasks) {}

    static void sAddTask(const stdsptr<eTask>& ready);
    static void sAddTasks(const QList<stdsptr<eTask>>& ready);
    static int sUsageCount();
    static int sWaitingTasks();
private:
    static QAtomicInt sUseCount;
    static QAtomicList<stdsptr<eTask>> sTasks;
};

#endif // TASKEXECUTOR_H
