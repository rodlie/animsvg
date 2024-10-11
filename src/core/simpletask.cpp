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

#include "simpletask.h"

#include <QPointer>

#include "exceptions.h"

QList<std::shared_ptr<SimpleTask>> SimpleTask::sTasks;

SimpleTask::SimpleTask(const Func& func) : mFunc(func) {}

SimpleTask *SimpleTask::sScheduleContexted(
        const QPointer<const QObject> &ctxt,
        const Func &func) {
    return sSchedule([func, ctxt]() { if(ctxt) func(); });
}

SimpleTask* SimpleTask::sSchedule(const Func &func) {
    const auto task = new SimpleTask(func);
    sTasks << std::shared_ptr<SimpleTask>(task);
    return task;
}

void SimpleTask::sProcessAll() {
    auto tasks = sTasks;
    sTasks.clear();
    for(auto& task : tasks) {
        try {
            task->process();
        } catch(const std::exception& e) {
            gPrintExceptionCritical(e);
        }
        task.reset();
    }
    if(!sTasks.isEmpty()) sProcessAll();
}

void SimpleTask::process() {
    mFunc();
    emit finished();
}

SimpleTaskScheduler::SimpleTaskScheduler(const Func &func) :
    mFunc(func) {}

void SimpleTaskScheduler::schedule() {
    if(mScheduled) return;
    const auto task = SimpleTask::sScheduleContexted(this, mFunc);
    connect(task, &SimpleTask::destroyed, this, [this]() { mScheduled = false; });
    mScheduled = true;
}
