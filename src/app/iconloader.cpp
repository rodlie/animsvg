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

#include "iconloader.h"

#include <QDir>
#include <QSvgRenderer>
#include <QPainter>

#include "Private/esettings.h"
#include "exceptions.h"

void renderSvg(const QString& svgPath, QImage& image) {
    QSvgRenderer renderer(svgPath);
    QPainter painter(&image);
    renderer.viewBox();
    renderer.render(&painter);
}

bool IconLoader::generate(const QString& svgPath,
                          const qreal scale,
                          const QString& pngPath) {
    if(QFile(pngPath).exists()) return false;

    QSvgRenderer renderer(svgPath);
    const auto defSize = renderer.defaultSize();
    QImage image(qRound(scale*defSize.width()),
                 qRound(scale*defSize.height()),
                 QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    renderer.render(&painter);

    if(!image.save(pngPath)) RuntimeThrow("Failed to save icon file '" + pngPath + "'");
    return true;
}

bool generate(const QString& svgPath,
              const QImage& base,
              const QString& pngPath) {
    if(QFile(pngPath).exists()) return false;

    QImage image = base;
    renderSvg(svgPath, image);

    if(!image.save(pngPath)) RuntimeThrow("Failed to save icon file '" + pngPath + "'");
    return true;
}

void generateButtons(const int minWidgetDim, const int buttonDim) {
    const QSize buttonSize(buttonDim, buttonDim);

    const QDir eDir(eSettings::sSettingsDir());
    const QString mkPath = "icons/" + QString::number(minWidgetDim) + "/toolbarButtons";
    if(!eDir.mkpath(mkPath)) RuntimeThrow("Failed to mkpath '" + mkPath + "'");

    QImage uncheckedBase(buttonSize, QImage::Format_ARGB32);
    uncheckedBase.fill(Qt::transparent);
    renderSvg(":/icons/toolbarButtons/uncheckedBg.svg", uncheckedBase);

    QImage checkedBase(buttonSize, QImage::Format_ARGB32);
    checkedBase.fill(Qt::transparent);
    renderSvg(":/icons/toolbarButtons/checkedBg.svg", checkedBase);

    const QString dir = eSettings::sSettingsDir() + "/" + mkPath;
    QDirIterator checkableIt(":/icons/toolbarButtons/checkable");
    while(checkableIt.hasNext()) {
        const auto path = checkableIt.next();
        const auto fileName = checkableIt.fileName().chopped(4);
        const auto pngFileName = fileName + ".png";

        if(fileName.contains("Checked")) {
            const auto pngPath = dir + "/" + fileName + ".png";
            generate(path, checkedBase, pngPath);
        } else if(fileName.contains("Unchecked")) {
            const auto pngPath = dir + "/" + fileName + ".png";
            generate(path, uncheckedBase, pngPath);
        } else {
            generate(path, uncheckedBase, dir + "/" + fileName + "Unchecked.png");
            generate(path, checkedBase, dir + "/" + fileName + "Checked.png");
        }
    }

    QDirIterator plainIt(":/icons/toolbarButtons/plain");
    while(plainIt.hasNext()) {
        const auto path = plainIt.next();
        const auto fileName = plainIt.fileName().chopped(4);;
        generate(path, uncheckedBase, dir + "/" + fileName + ".png");
    }
}

void IconLoader::generateAll(const int minWidgetDim, const int buttonDim) {
    const QString minDimStr = QString::number(minWidgetDim);
    const QDir eDir(eSettings::sSettingsDir());
    QDir iconsDir(eSettings::sSettingsDir() + "/icons");
#ifdef QT_DEBUG
    iconsDir.removeRecursively();
#endif
    const QString mkPath = "icons/" + minDimStr;
    if(!eDir.mkpath(mkPath)) RuntimeThrow("Failed to mkpath '" + mkPath + "'");
    QDir iconsSizeDir(eSettings::sIconsDir());

    const int enveIconsVersion = 0;
    QFile verFile(iconsSizeDir.filePath("ver"));
    bool removeIcons = false;
    if(!verFile.exists()) {
        removeIcons = true;
    } else {
        const auto verData = verFile.readAll();
        const int foundVersion = verData.toInt();
        if(enveIconsVersion != foundVersion) {
            removeIcons = true;
        }
    }
    if(removeIcons) {
        iconsSizeDir.removeRecursively();
        if(!eDir.mkpath(mkPath)) RuntimeThrow("Failed to mkpath '" + mkPath + "'");
    }

    if(verFile.open(QIODevice::WriteOnly)) {
        const auto verIntData = reinterpret_cast<const char*>(&enveIconsVersion);
        const auto verData = QByteArray::fromRawData(verIntData, sizeof(int));
        verFile.write(verData);
        verFile.close();
    }

    QDirIterator noInterIt(":/icons/noInterpolation");
    while(noInterIt.hasNext()) {
        const auto path = noInterIt.next();
        const auto fileName = noInterIt.fileName();
        const QImage img(path);
        int targetWidth = qCeil(img.width()*minWidgetDim/22.);
        if(qAbs(targetWidth - img.width()) % 2 == 1) targetWidth--;
        const auto scaled = img.scaledToWidth(targetWidth, Qt::TransformationMode::SmoothTransformation);
        scaled.save(iconsSizeDir.filePath(fileName));
    }

    QDirIterator baseIt(":/icons");
    while(baseIt.hasNext()) {
        const auto path = baseIt.next();
        if(baseIt.fileInfo().isDir()) continue;
        const auto fileName = baseIt.fileName().chopped(4);
        const auto pngPath = iconsSizeDir.filePath(fileName + ".png");
        generate(path, minWidgetDim/22., pngPath);
    }
    generateButtons(minWidgetDim, buttonDim);
}
