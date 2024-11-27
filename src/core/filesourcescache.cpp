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

#include "filesourcescache.h"
#include <QStringList>

QStringList FileExtensions::image{"png", "jpg", "jpeg",
                                  "bmp", "webp"};
QStringList FileExtensions::sound{"mp3", "wav", "aiff",
                                  "flac", "m4a", "oga"};
QStringList FileExtensions::video{"avi", "mp4", "mov",
                                  "mkv", "m4v", "webm",
                                  "flv", "vob", "wmv",
                                  "3gp", "rmvb", "gif",
                                  "mpeg", "mpg", "mp2",
                                  "mpv"};
//QStringList FileExtensions::layers{};

bool isVideoExt(const QString &extension) {
    return FileExtensions::video.contains(extension.toLower());
}

bool isSoundExt(const QString &extension) {
    return FileExtensions::sound.contains(extension.toLower());
}

bool isVectorExt(const QString &extension) {
    return extension.toLower() == "svg";
}

bool isImageExt(const QString &extension) {
    return FileExtensions::image.contains(extension.toLower());
}

bool isLayersExt(const QString &ext) {
    Q_UNUSED(ext)
    return false; //FileExtensions::layers.contains(ext.toLower());
}

bool isEvExt(const QString &extension) {
    return (extension.toLower() == "friction" || extension.toLower() == "ev");
}

bool hasVideoExt(const QString &filename) {
    return isVideoExt(filename.split(".").last());
}

bool hasSoundExt(const QString &filename) {
    return isSoundExt(filename.split(".").last());
}

bool hasVectorExt(const QString &filename) {
    return isVectorExt(filename.split(".").last());
}

bool hasImageExt(const QString &filename) {
    return isImageExt(filename.split(".").last());
}

bool hasEvExt(const QString &filename) {
    return isEvExt(filename.split(".").last());
}

QString FileExtensions::filters(const QStringList &exts) {
    QString result;
    for(const auto& ext : exts) {
        result += "*." + ext + " ";
    }
    return result;
}

QString FileExtensions::imageFilters() { return filters(image); }

QString FileExtensions::soundFilters() { return filters(sound); }

QString FileExtensions::videoFilters() { return filters(video); }

QString FileExtensions::layersFilters() { return QString(); /*filters(layers);*/ }
