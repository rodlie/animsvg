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

#include "appsupport.h"
#include "hardwareinfo.h"

#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QPalette>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDomDocument>
#include <QDomElement>
#include <QDirIterator>
#include <QtMath>
#include <QRegularExpression>

AppSupport::AppSupport(QObject *parent)
    : QObject{parent}
{

}

void AppSupport::setupTheme()
{
    QIcon::setThemeSearchPaths(QStringList() << QString::fromUtf8(":/icons"));
    QIcon::setThemeName(QString::fromUtf8("hicolor"));
    qApp->setStyle(QString::fromUtf8("fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(40, 40, 47));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(33, 33, 38));
    palette.setColor(QPalette::AlternateBase, QColor(40, 40, 47));
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::ToolTipBase, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(33, 33, 38));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(177, 16, 20));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);
}

const QPalette AppSupport::getDarkPalette()
{
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, QColor(33, 33, 38));
    pal.setColor(QPalette::Base, QColor(33, 33, 38));
    pal.setColor(QPalette::Button, QColor(33, 33, 38));
    return pal;
}

const QPalette AppSupport::getDarkerPalette()
{
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, QColor(25, 25, 25));
    pal.setColor(QPalette::Base, QColor(25, 25, 25));
    pal.setColor(QPalette::Button, QColor(25, 25, 25));
    return pal;
}

const QPalette AppSupport::getNotSoDarkPalette()
{
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, QColor(40, 40, 47));
    pal.setColor(QPalette::Base, QColor(33, 33, 38));
    pal.setColor(QPalette::Button, QColor(33, 33, 38));
    return pal;
}

QVariant AppSupport::getSettings(const QString &group,
                                 const QString &key,
                                 const QVariant &fallback)
{
    QVariant variant;
    QSettings settings;
    settings.beginGroup(group);
    variant = settings.value(key, fallback);
    settings.endGroup();
    return variant;
}

void AppSupport::setSettings(const QString &group,
                             const QString &key,
                             const QVariant &value,
                             bool append)
{
    QSettings settings;
    settings.beginGroup(group);
    QVariant result;
    if (append) {
        QVariant orig = getSettings(group, key);
        QMetaType::Type type = static_cast<QMetaType::Type>(orig.type());
        if (!orig.isNull() && type == QMetaType::QStringList) {
            QStringList list = orig.toStringList();
            list.append(value.toString());
            result = list;
        } else if (!orig.isNull() && type == QMetaType::QString) {
            QString text = orig.toString();
            text.append(value.toString());
            result = text;
        } else { append = false; }
    }
    settings.setValue(key, append ? result : value);
    settings.endGroup();
}

const QString AppSupport::getAppName()
{
    return QString::fromUtf8("friction");
}

const QString AppSupport::getAppDisplayName()
{
    return QString::fromUtf8("Friction");
}

const QString AppSupport::getAppDomain()
{
    return QString::fromUtf8("friction.graphics");
}

const QString AppSupport::getAppID()
{
    return QString::fromUtf8("graphics.friction.Friction");
}

const QString AppSupport::getAppUrl()
{
    return QString::fromUtf8("https://friction.graphics");
}

const QString AppSupport::getAppVersion()
{
    QString version = QString::fromUtf8(PROJECT_VERSION);
#ifndef PROJECT_OFFICIAL
    version.append("-dev");
#endif
    return version;
}

const QString AppSupport::getAppBuildInfo(bool html)
{
#if defined(PROJECT_COMMIT) && defined(PROJECT_BRANCH)
    const auto commit = QString::fromUtf8(PROJECT_COMMIT);
    const auto branch = QString::fromUtf8(PROJECT_BRANCH);
    if (commit.isEmpty() || branch.isEmpty()) { return QString(); }
    if (!html) {
        return QString("%1 %2 %3 %4.").arg(tr("Built from"),
                                           commit,
                                           tr("on"),
                                           branch);
    } else {
        return QString("%1 <a href=\"%5/%2\">%2</a> %3 <a href=\"%6/%4\">%4</a>.")
                      .arg(tr("Built from commit"),
                           commit,
                           tr("on branch"),
                           branch,
                           getAppCommitUrl(),
                           getAppBranchUrl());
    }
#else
    Q_UNUSED(html)
#endif
    return QString();
}

const QString AppSupport::getAppDesc()
{
    return QString::fromUtf8("Motion Graphics");
}

const QString AppSupport::getAppCompany()
{
    return getAppName();
}

const QString AppSupport::getAppContributorsUrl()
{
    return QString::fromUtf8("https://github.com/friction2d/friction/graphs/contributors");
}

const QString AppSupport::getAppIssuesUrl()
{
    return QString::fromUtf8("https://github.com/friction2d/friction/issues");
}

const QString AppSupport::getAppLatestReleaseUrl()
{
    return QString::fromUtf8("https://friction.graphics/news");
}

const QString AppSupport::getAppCommitUrl()
{
    return QString::fromUtf8("https://github.com/friction2d/friction/commit");
}

const QString AppSupport::getAppBranchUrl()
{
    return QString::fromUtf8("https://github.com/friction2d/friction/tree");
}

const QString AppSupport::getAppConfigPath()
{
    QString path = QString::fromUtf8("%1/%2")
                   .arg(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation),
                        getAppName());
    QDir dir(path);
    if (!dir.exists()) { dir.mkpath(path); }
    return path;
}

const QString AppSupport::getAppOutputProfilesPath()
{
    QString path = QString::fromUtf8("%1/OutputProfiles").arg(getAppConfigPath());
    QDir dir(path);
    if (!dir.exists()) { dir.mkpath(path); }
    return path;
}

const QString AppSupport::getAppPathEffectsPath()
{
    QString path = QString::fromUtf8("%1/PathEffects").arg(getAppConfigPath());
    QDir dir(path);
    if (!dir.exists()) { dir.mkpath(path); }
    return path;
}

const QString AppSupport::getAppRasterEffectsPath()
{
    QString path = QString::fromUtf8("%1/RasterEffects").arg(getAppConfigPath());
    QDir dir(path);
    if (!dir.exists()) { dir.mkpath(path); }
    return path;
}

const QString AppSupport::getAppShaderEffectsPath(bool restore)
{
    QString def = QString::fromUtf8("%1/ShaderEffects").arg(getAppConfigPath());
    QString path = restore ? def : getSettings("settings",
                                               "CustomShaderPath",
                                               def).toString();
    QDir dir(path);
    if (!dir.exists() && path.startsWith(getAppConfigPath())) { dir.mkpath(path); }
    return path;
}

const QString AppSupport::getAppShaderPresetsPath()
{
    QString path1 = QApplication::applicationDirPath();
    QString path2 = path1;
    path1.append(QString::fromUtf8("/plugins/shaders"));
    path2.append(QString::fromUtf8("/../share/friction/plugins/shaders"));
    if (QFile::exists(path1)) { return path1; }
    if (QFile::exists(path2)) { return path2; }
    return QString();
}

const QString AppSupport::getAppExPresetsPath()
{
    QString path1 = QApplication::applicationDirPath();
    QString path2 = path1;
    path1.append(QString::fromUtf8("/presets/expressions"));
    path2.append(QString::fromUtf8("/../share/friction/presets/expressions"));
    if (QFile::exists(path1)) { return path1; }
    if (QFile::exists(path2)) { return path2; }
    return QString();
}

const QString AppSupport::getAppUserExPresetsPath()
{
    QString path = QString::fromUtf8("%1/ExPresets").arg(getAppConfigPath());
    QDir dir(path);
    if (!dir.exists()) { dir.mkpath(path); }
    return path;
}

const QString AppSupport::getFileMimeType(const QString &path)
{
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(path);
    return type.name();
}

const QString AppSupport::getFileIcon(const QString &path)
{
    QString fileIcon = "file_blank";
    QString mime = getFileMimeType(path);
    if (mime.startsWith("image")) { fileIcon = "file_image"; }
    else if (mime.startsWith("video")) { fileIcon = "file_movie"; }
    else if (mime.startsWith("audio")) { fileIcon = "file_sound"; }
    return fileIcon;
}

const QPair<QString, QString> AppSupport::getShaderID(const QString &path)
{
    QPair<QString,QString> result;

    QFile greFile(path);
    if (!greFile.exists()) { return result; }
    if (!greFile.open(QIODevice::ReadOnly)) { return result; }

    QDomDocument document;
    QString errMsg;
    if (!document.setContent(&greFile, &errMsg)) {
        greFile.close();
        return result;
    }
    greFile.close();

    QDomElement root = document.firstChildElement();
    if (root.tagName() != "ShaderEffect") { return result; }

    const QString effectName = root.attribute("name");
    const QString menuPath = root.attribute("menuPath");
    result.first = effectName;
    result.second = menuPath;

    return result;
}

const QStringList AppSupport::getFilesFromPath(const QString &path,
                                               const QStringList &suffix)
{
    QStringList result;
    if (path.isEmpty() || !QFile::exists(path)) { return result; }
    QDirIterator it(path, suffix, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) { result << it.next(); }
    return result;
}

const QString AppSupport::getTimeCodeFromFrame(int frame, float fps)
{
    if (fps <= 0) { return "00:00:00:00"; }
    bool negative = frame < 0;
    if (negative) { frame = qAbs(frame); }
    double totalSeconds = static_cast<double>(frame) / fps;
    int hours = static_cast<int>(totalSeconds / 3600);
    int minutes = static_cast<int>((totalSeconds - hours * 3600) / 60);
    int seconds = static_cast<int>(totalSeconds - hours * 3600 - minutes * 60);
    int frames = static_cast<int>((totalSeconds - static_cast<int>(totalSeconds)) * fps);
    QString timecode = QString("%1:%2:%3:%4")
                           .arg(hours, 2, 10, QChar('0'))
                           .arg(minutes, 2, 10, QChar('0'))
                           .arg(seconds, 2, 10, QChar('0'))
                           .arg(frames, 2, 10, QChar('0'));
    if (negative) { timecode.prepend("-"); }
    //qDebug() << frame << "=" << timecode;
    return timecode;
}

HardwareSupport AppSupport::getRasterEffectHardwareSupport(const QString &effect,
                                                           HardwareSupport fallback)
{
    if (effect.isEmpty()) { return fallback; }
    QVariant variant;
    QSettings settings;
    settings.beginGroup("RasterEffects");
    variant = settings.value(QString("%1HardwareSupport").arg(effect));
    settings.endGroup();
    if (variant.isValid()) {
        HardwareSupport supported = static_cast<HardwareSupport>(variant.toInt());
        if (supported == HardwareSupport::hardwareDefault) { return fallback; }
        else { return supported; }
    }
    return fallback;
}

const QString AppSupport::getRasterEffectHardwareSupportString(const QString &effect,
                                                               HardwareSupport fallback)
{
    HardwareSupport supported = getRasterEffectHardwareSupport(effect, fallback);
    QString result;
    switch (supported) {
    case HardwareSupport::cpuOnly:
        result = tr("CPU-only");
        break;
    case HardwareSupport::cpuPreffered:
        result = tr("CPU preffered");
        break;
    case HardwareSupport::gpuOnly:
        result = tr("GPU-only");
        break;
    case HardwareSupport::gpuPreffered:
        result = tr("GPU preffered");
        break;
    default:;
    }
    return result;
}

const QByteArray AppSupport::filterShader(QByteArray data)
{
#ifdef Q_OS_WIN
    if (HardwareInfo::sGpuVendor() == GpuVendor::intel) {
        return data.replace("texture2D", "texture");
    }
#endif
    return data;
}

const QStringList AppSupport::getFpsPresets()
{
    QStringList presets;
    QSettings settings;
    settings.beginGroup("presets");
    presets = settings.value("fps",
                             QStringList()
                             << "24"
                             << "25"
                             << "30"
                             << "50"
                             << "60").toStringList();
    settings.endGroup();
    return presets;
}

void AppSupport::saveFpsPresets(const QStringList &presets)
{
    QSettings settings;
    settings.beginGroup("presets");
    settings.setValue("fps", presets);
    settings.endGroup();
}

void AppSupport::saveFpsPreset(const double value)
{
    if (value <= 0) { return; }
    auto presets = getFpsPresets();
    if (presets.contains(QString::number(value))) { return; }
    presets << QString::number(value);
    QSettings settings;
    settings.beginGroup("presets");
    settings.setValue("fps", presets);
    settings.endGroup();
}

bool AppSupport::removeFpsPreset(const double value)
{
    if (value <= 0) { return false; }
    auto presets = getFpsPresets();
    if (!presets.removeAll(QString::number(value))) { return false; }
    QSettings settings;
    settings.beginGroup("presets");
    settings.setValue("fps", presets);
    settings.endGroup();
    return true;
}

QPair<bool, bool> AppSupport::getFpsPresetStatus()
{
    QPair<bool, bool> status;
    QSettings settings;
    settings.beginGroup("presets");
    status.first = settings.value("EnableFPS", true).toBool();
    status.second = settings.value("EnableFPSAuto", true).toBool();
    settings.endGroup();
    return status;
}

const QStringList AppSupport::getResolutionPresetsList()
{
    QStringList presets;
    QSettings settings;
    settings.beginGroup("presets");
    presets = settings.value("resolution",
                             QStringList()
                             << "1280x720"
                             << "1920x1080"
                             << "2560x1440"
                             << "3840x2160").toStringList();
    settings.endGroup();
    return presets;
}

const QList<QPair<int, int> > AppSupport::getResolutionPresets()
{
    const auto presets = getResolutionPresetsList();
    QList<QPair<int, int>> l;
    for (auto &v: presets) {
        auto res = v.split("x");
        if (res.count() != 2) { continue; }
        const auto val = QPair<int, int>(res.at(0).trimmed().toInt(),
                                         res.at(1).trimmed().toInt());
        if (l.contains(val) || val.first <= 0 || val.second <= 0) { continue; }
        l.append(val);
    }
    std::sort(l.begin(), l.end());
    return l;
}

void AppSupport::saveResolutionPresets(const QList<QPair<int, int> > &presets)
{
    QStringList list;
    for (const auto &preset: presets) {
        if (preset.first <= 0 || preset.second <= 0) { continue; }
        const auto val = QString("%1x%2").arg(preset.first).arg(preset.second);
        if (list.contains(val)) { continue; }
        list << val;
    }
    QSettings settings;
    settings.beginGroup("presets");
    settings.setValue("resolution", list);
    settings.endGroup();
}

void AppSupport::saveResolutionPreset(const int w,
                                      const int h)
{
    if (w <= 0 || h <= 0) { return; }
    const auto v = QString("%1x%2").arg(w).arg(h);
    auto presets = getResolutionPresetsList();
    if (presets.contains(v)) { return; }
    presets << v;
    QSettings settings;
    settings.beginGroup("presets");
    settings.setValue("resolution", presets);
    settings.endGroup();
}

bool AppSupport::removeResolutionPreset(const int w,
                                        const int h)
{
    if (w <= 0 || h <= 0) { return false; }
    const auto v = QString("%1x%2").arg(w).arg(h);
    auto presets = getResolutionPresetsList();
    if (!presets.removeAll(v)) { return false; }
    QSettings settings;
    settings.beginGroup("presets");
    settings.setValue("resolution", presets);
    settings.endGroup();
    return true;
}

QPair<bool, bool> AppSupport::getResolutionPresetStatus()
{
    QPair<bool, bool> status;
    QSettings settings;
    settings.beginGroup("presets");
    status.first = settings.value("EnableResolutions", true).toBool();
    status.second = settings.value("EnableResolutionsAuto", true).toBool();
    settings.endGroup();
    return status;
}

const QString AppSupport::filterTextAZW(const QString &text)
{
    QRegularExpression regex("\\s|\\W");
    QString output = text;
    return output.replace(regex, "");
}

const QPair<QStringList, bool> AppSupport::hasWriteAccess()
{
    QPair<QStringList,bool> result(QStringList(), true);

    QStringList dirs;
    dirs << getAppConfigPath();
    dirs << getAppOutputProfilesPath();
    dirs << getAppShaderEffectsPath();
    for (const auto &dir : dirs) {
        if (dir.isEmpty()) { continue; }
        QFileInfo info(dir);
        if (!info.isDir() ||
            !info.exists() ||
            !info.isReadable() ||
            !info.isWritable()) {
            result.second = false;
            result.first << info.absoluteFilePath();
        }
    }

    return result;
}
