/*
#
# enve2d - https://github.com/enve2d
#
# Copyright (c) enve2d developers
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

#ifndef APPSUPPORT_H
#define APPSUPPORT_H

#include "core_global.h"

#include <QObject>
#include <QVariant>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#define QT_ENDL Qt::endl
#else
#define QT_ENDL endl
#endif

class CORE_EXPORT AppSupport : public QObject
{
    Q_OBJECT

public:
    explicit AppSupport(QObject *parent = nullptr);
    static void setupTheme();
    static QVariant getSettings(const QString &group,
                                const QString &key,
                                const QVariant &fallback = QVariant());
    static void setSettings(const QString &group,
                            const QString &key,
                            const QVariant &value,
                            bool append = false);
    static const QString getAppVersion(bool html = false);
};

#endif // APPSUPPORT_H
