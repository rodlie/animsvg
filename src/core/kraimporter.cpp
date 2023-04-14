// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "kraimporter.h"

#include "zipfileloader.h"

sk_sp<SkImage> ImportKRA::loadMergedKRAFile(
        const QString &filename, const bool useContained) {
    if(!useContained) return nullptr;
    ZipFileLoader fileProcessor;
    fileProcessor.setZipPath(filename);

    sk_sp<SkImage> result;
    fileProcessor.process("mergedimage.png", [&](QIODevice* const src) {
        const QByteArray qData = src->readAll();
        const auto data = SkData::MakeWithoutCopy(qData.data(), qData.size());
        result = SkImage::MakeFromEncoded(data);
    });
    return result;
}
