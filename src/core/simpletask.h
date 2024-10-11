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

#ifndef SIMPLETASK_H
#define SIMPLETASK_H

#include "core_global.h"

#include <QObject>

#include <functional>
#include <memory>

class CORE_EXPORT SimpleTask : public QObject {
    Q_OBJECT
    typedef std::function<void(void)> Func;

    SimpleTask(const Func& func);
public:
    static SimpleTask *sScheduleContexted(
            const QPointer<const QObject>& ctxt,
            const Func& func);
    static SimpleTask *sSchedule(const Func& func);
    static void sProcessAll();
private:
    void process();

    const Func mFunc;
    static QList<std::shared_ptr<SimpleTask>> sTasks;
signals:
    void finished();
};

class CORE_EXPORT SimpleTaskScheduler : public QObject {
    typedef std::function<void(void)> Func;
public:
    SimpleTaskScheduler(const Func& func);

    void schedule();
    void operator()() { schedule(); }
private:
    bool mScheduled = false;
    const Func mFunc;
};

#endif // SIMPLETASK_H
