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

#ifndef TASKQUEHANDLER_H
#define TASKQUEHANDLER_H
#include "taskque.h"

class CORE_EXPORT TaskQueHandler {
public:
    int countQues() const;
    bool isEmpty() const;

    void clear();

    stdsptr<eTask> takeQuedForGpuProcessing();
    stdsptr<eTask> takeQuedForCpuProcessing();

    void beginQue();

    void addTask(const stdsptr<eTask>& task);

    void endQue();

    int taskCount() const { return mTaskCount; }
private:
    void queDone(const TaskQue * const que, const int queId);

    int mTaskCount = 0;
    QList<stdsptr<TaskQue>> mQues;
    TaskQue * mCurrentQue = nullptr;
};
#endif // TASKQUEHANDLER_H
