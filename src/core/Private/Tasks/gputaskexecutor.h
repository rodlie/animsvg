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

#ifndef GPUTASKEXECUTOR_H
#define GPUTASKEXECUTOR_H

#include "taskexecutor.h"
#include "offscreenqgl33c.h"

class CORE_EXPORT GpuTaskExecutor : public TaskExecutor,
                        private OffscreenQGL33c {
public:
    GpuTaskExecutor();

    static void sAddTask(const stdsptr<eTask>& ready);
    static void sAddTasks(const QList<stdsptr<eTask>>& ready);
    static int sUsageCount();
    static int sWaitingTasks();

    void initialize(QThread* const thread);

    bool unhandledException() const;
    std::exception_ptr handleException();
private:
    void processTask(eTask& task);
    void start();

    void setException(const std::exception_ptr& exception);

    void initializeContext();

    static QAtomicInt sUseCount;
    static QAtomicList<stdsptr<eTask>> sTasks;

    sk_sp<const GrGLInterface> mInterface;
    SwitchableContext mContext;
    std::exception_ptr mProcessException;
    bool mInitialized = false;
    GLuint _mTextureSquareVAO;
};

#endif // GPUTASKEXECUTOR_H
