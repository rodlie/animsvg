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

#include "appsupport.h"

#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QPalette>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>

AppSupport::AppSupport(QObject *parent)
    : QObject{parent}
{

}

void AppSupport::setupTheme()
{
    QIcon::setThemeName(QString::fromUtf8("hicolor"));
    qApp->setStyle(QString::fromUtf8("fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(44, 44, 49));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(33, 33, 38));
    palette.setColor(QPalette::AlternateBase, QColor(44, 44, 49));
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(44, 44, 49));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(177, 16, 20));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);
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

const QString AppSupport::getAppVersion(bool html)
{
    QString version = ENVE2D_VERSION;
    if (version.isEmpty()) { version = tr("N/A"); }
    QString git;
#ifdef ENVE2D_GIT
    git = ENVE2D_GIT;
#endif
    if (!git.isEmpty()) {
        version.append(html ? QString::fromUtf8(" <a href=\"https://github.com/enve2d/enve2d/commit/%1\">%1</a>").arg(git) : QString::fromUtf8(" %1").arg(git));
    }
    return version;
}

const QString AppSupport::getAppConfigPath()
{
    QString path = QString::fromUtf8("%1/enve2d")
                   .arg(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation));
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
