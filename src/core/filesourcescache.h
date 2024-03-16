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

#ifndef FILESOURCESCACHE_H
#define FILESOURCESCACHE_H

#include <QStringList>
#include "core_global.h"

CORE_EXPORT
bool hasVideoExt(const QString &filename);
CORE_EXPORT
bool hasSoundExt(const QString &filename);
CORE_EXPORT
bool hasVectorExt(const QString &filename);
CORE_EXPORT
bool hasImageExt(const QString &filename);
CORE_EXPORT
bool hasEvExt(const QString &filename);

CORE_EXPORT
bool isVideoExt(const QString &extension);
CORE_EXPORT
bool isSoundExt(const QString &extension);
CORE_EXPORT
bool isVectorExt(const QString &extension);
CORE_EXPORT
bool isImageExt(const QString &extension);
CORE_EXPORT
bool isEvExt(const QString &extension);

CORE_EXPORT
bool isLayersExt(const QString& ext);

namespace FileExtensions {
    CORE_EXPORT
    extern QStringList image;
    CORE_EXPORT
    extern QStringList sound;
    CORE_EXPORT
    extern QStringList video;
    CORE_EXPORT
    extern QStringList layers;

    CORE_EXPORT
    QString filters(const QStringList& exts);

    CORE_EXPORT
    QString imageFilters();
    CORE_EXPORT
    QString soundFilters();
    CORE_EXPORT
    QString videoFilters();
    CORE_EXPORT
    QString layersFilters();
};

#endif // FILESOURCESCACHE_H
