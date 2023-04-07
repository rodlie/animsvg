/*
# enve2d - https://github.com/enve2d
#
# Copyright (c) enve2d developers
# Copyright (C) 2016-2020 Maurycy Liebner
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
*/

#ifndef WELCOMEDIALOG_H
#define WELCOMEDIALOG_H

#include <QWidget>
#include <functional>

class WelcomeDialog : public QWidget
{
public:
    WelcomeDialog(const QStringList &recentPaths,
                  const std::function<void()> &newFunc,
                  const std::function<void()> &openFunc,
                  const std::function<void(QString)> &openRecentFunc,
                  QWidget * const parent = nullptr);

protected:
    void paintEvent(QPaintEvent *);
};

#endif // WELCOMEDIALOG_H
