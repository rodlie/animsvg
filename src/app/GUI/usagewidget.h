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

#ifndef USAGEWIDGET_H
#define USAGEWIDGET_H

#include <QLabel>
#include <QThread>
#include <QStatusBar>

class HardwareUsageWidget;
class ComplexTaskWidget;
class ComplexTask;

class UsageWidget : public QStatusBar
{
public:
    explicit UsageWidget(QWidget * const parent = nullptr);
    void setThreadsUsage(const int threads);
    void setThreadsTotal(const int threads);
    void setHddUsage(const bool used);
    void setGpuUsage(const bool used);
    void setRamUsage(const qreal thisMB);
    void setTotalRam(const qreal totalRamMB);

    void addComplexTask(ComplexTask* const task);

private:
    HardwareUsageWidget* mGpuBar;
    HardwareUsageWidget* mCpuBar;
    HardwareUsageWidget* mHddBar;
    HardwareUsageWidget* mRamBar;
    QLabel* mRamLabel;
    QList<ComplexTaskWidget*> mTaskWidgets;
};

#endif // USAGEWIDGET_H
