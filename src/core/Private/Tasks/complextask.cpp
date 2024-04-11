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

#include "complextask.h"

ComplexTask::ComplexTask(const int finishValue, const QString &name) :
    mFinishValue(finishValue), mName(name) {}

eTask* ComplexTask::addEmptyTask() {
    const auto emptyTask = enve::make_shared<eCustomCpuTask>(
                nullptr, nullptr, nullptr, nullptr);
    emptyTask->queTask();
    addTask(emptyTask);
    return emptyTask.get();
}

void ComplexTask::addTask(const stdsptr<eTask> &task) {
    const QPointer<ComplexTask> ptr = this;
    task->addDependent({[ptr]() { if(ptr) ptr->nextStep(); },
                        [ptr]() { if(ptr) ptr->cancel(); }});
    mTasks << task;
}

void ComplexTask::addTask(const qsptr<ComplexTask>& task) {
    const QPointer<ComplexTask> ptr = this;
    task->addDependent({[ptr]() { if(ptr) ptr->nextStep(); },
                        [ptr]() { if(ptr) ptr->cancel(); }});
    mComplexTasks << task;
}

void ComplexTask::cancel() {
    if(mDone) return;
    mDone = true;
    eTaskBase::cancel();
    emit canceled();
}

bool ComplexTask::setValue(const int value) {
    mValue = value;
    return finishedEmitters();
}

bool ComplexTask::finishedEmitters() {
    if(mDone) return false;
    emit finished(mValue);
    if(mValue >= finishValue()) {
        mDone = true;
        finishedProcessing();
        emit finishedAll();
        return true;
    }
    return false;
}
