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

#include "taskexecutor.h"

QAtomicInt TaskExecutor::sTaskFinishSignals = 0;

void TaskExecutor::processTask(eTask& task) {
    task.process();
}

QAtomicList<stdsptr<eTask>> CpuTaskExecutor::sTasks;
QAtomicInt CpuTaskExecutor::sUseCount = 0;

void CpuTaskExecutor::sAddTask(const stdsptr<eTask>& ready) {
    sTasks.appendAndNotifyAll(ready);
}

void CpuTaskExecutor::sAddTasks(const QList<stdsptr<eTask>>& ready) {
    sTasks.appendAndNotifyAll(ready);
}

int CpuTaskExecutor::sUsageCount() {
    return sUseCount;
}

int CpuTaskExecutor::sWaitingTasks() {
    return sTasks.count();
}

void TaskExecutor::start() {
    processLoop();
}

void TaskExecutor::stop() {
    mStop = true;
}

void TaskExecutor::processLoop() {
    mStop = false;
    while(!mStop) {
        stdsptr<eTask> task;
        if(!mTasks.waitTakeFirst(task, mStop)) break;
        mUseCount++;
        try {
            processTask(*task);
        } catch(...) {
            task->setException(std::current_exception());
        }

        const bool nextStep = !task->waitingToCancel() &&
                              task->nextStep();
        if(!nextStep) {
            sTaskFinishSignals++;
            emit finishedTask(task);
        }
        mUseCount--;
    }
}

QAtomicList<stdsptr<eTask>> HddTaskExecutor::sTasks;
QAtomicInt HddTaskExecutor::sUseCount = 0;

void HddTaskExecutor::sAddTask(const stdsptr<eTask>& ready) {
    sTasks.appendAndNotifyAll(ready);
}

void HddTaskExecutor::sAddTasks(const QList<stdsptr<eTask>>& ready) {
    sTasks.appendAndNotifyAll(ready);
}

int HddTaskExecutor::sUsageCount() {
    return sUseCount;
}

int HddTaskExecutor::sWaitingTasks() {
    return sTasks.count();
}
