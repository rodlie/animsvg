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

#ifndef APPSUPPORT_H
#define APPSUPPORT_H

#include "core_global.h"

#include <QObject>
#include <QVariant>
#include <QPalette>
#include <QPair>
#include <QStringList>

#include "hardwareenums.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#define QT_ENDL Qt::endl
#define QT_SKIP_EMPTY Qt::SkipEmptyParts
#else
#define QT_ENDL endl
#define QT_SKIP_EMPTY QString::SkipEmptyParts
#endif

class CORE_EXPORT AppSupport : public QObject
{
    Q_OBJECT

public:
    explicit AppSupport(QObject *parent = nullptr);
    static void setupTheme();
    static const QPalette getDarkPalette();
    static const QPalette getDarkerPalette();
    static const QPalette getNotSoDarkPalette();
    static QVariant getSettings(const QString &group,
                                const QString &key,
                                const QVariant &fallback = QVariant());
    static void setSettings(const QString &group,
                            const QString &key,
                            const QVariant &value,
                            bool append = false);
    static const QString getAppName();
    static const QString getAppDisplayName();
    static const QString getAppDomain();
    static const QString getAppID();
    static const QString getAppUrl();
    static const QString getAppVersion();
    static const QString getAppBuildInfo(bool html = false);
    static const QString getAppDesc();
    static const QString getAppCompany();
    static const QString getAppContributorsUrl();
    static const QString getAppIssuesUrl();
    static const QString getAppLatestReleaseUrl();
    static const QString getAppCommitUrl();
    static const QString getAppBranchUrl();
    static const QString getAppConfigPath();
    static const QString getAppOutputProfilesPath();
    static const QString getAppPathEffectsPath();
    static const QString getAppRasterEffectsPath();
    static const QString getAppShaderEffectsPath(bool restore = false);
    static const QString getAppShaderPresetsPath();
    static const QString getAppExPresetsPath();
    static const QString getAppUserExPresetsPath();
    static const QString getFileMimeType(const QString &path);
    static const QString getFileIcon(const QString &path);
    static const QPair<QString,QString> getShaderID(const QString &path);
    static const QStringList getFilesFromPath(const QString &path,
                                              const QStringList &suffix = QStringList());
    static const QString getTimeCodeFromFrame(int frame, float fps);
    static HardwareSupport getRasterEffectHardwareSupport(const QString &effect,
                                                          HardwareSupport fallback);
    static const QString getRasterEffectHardwareSupportString(const QString &effect,
                                                              HardwareSupport fallback);
    static const QByteArray filterShader(QByteArray data);
    static const QStringList getFpsPresets();
    static void saveFpsPresets(const QStringList &presets);
    static void saveFpsPreset(const double value);
    static bool removeFpsPreset(const double value);
    static QPair<bool, bool> getFpsPresetStatus();
    static const QStringList getResolutionPresetsList();
    static const QList<QPair<int, int>> getResolutionPresets();
    static void saveResolutionPresets(const QList<QPair<int, int>> &presets);
    static void saveResolutionPreset(const int w, const int h);
    static bool removeResolutionPreset(const int w, const int h);
    static QPair<bool, bool> getResolutionPresetStatus();
    static const QString filterTextAZW(const QString &text);
    static const QPair<QStringList,bool> hasWriteAccess();
};

#endif // APPSUPPORT_H
