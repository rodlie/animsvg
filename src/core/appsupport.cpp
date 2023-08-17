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

AppSupport::AppSupport(QObject *parent)
    : QObject{parent}
{

}

void AppSupport::setupTheme()
{
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
    return QString::fromUtf8("graphics.friction");
}

const QString AppSupport::getAppUrl()
{
    return QString::fromUtf8("https://friction.graphics");
}

const QString AppSupport::getAppVersion(bool html)
{
    QString version = QString::fromUtf8("0.9.4"); // fallback, should not happen
#ifdef PROJECT_VERSION
    version = QString::fromUtf8(PROJECT_VERSION);
#endif
    QString git;
#ifdef PROJECT_GIT
    git = QString::fromUtf8(PROJECT_GIT);
#endif
    QString branch;
#ifdef PROJECT_BRANCH
    branch = QString::fromUtf8(PROJECT_BRANCH);
#endif
    if (!branch.isEmpty()) {
        version.append(QString::fromUtf8(" %1").arg(branch));
    }
    if (!git.isEmpty()) {
        if (branch.isEmpty()) { version.append(QString::fromUtf8(" ")); }
        else { version.append(QString::fromUtf8("/")); }
        version.append(html ? QString::fromUtf8("<a href=\"%2/%1\">%1</a>").arg(git,
                                                                                getAppCommitUrl()) : git);
    }
    return version;
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
    return QString::fromUtf8("https://github.com/friction2d/friction/releases/latest");
}

const QString AppSupport::getAppCommitUrl()
{
    return QString::fromUtf8("https://github.com/friction2d/friction/commit");
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

const QString AppSupport::getAppShaderEffectsPath()
{
    QString path = QString::fromUtf8("%1/ShaderEffects").arg(getAppConfigPath());
    QDir dir(path);
    if (!dir.exists()) { dir.mkpath(path); }
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
