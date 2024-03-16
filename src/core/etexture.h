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

#ifndef ETEXTURE_H
#define ETEXTURE_H
#include "glhelpers.h"

struct CORE_EXPORT eTexture {
    GLuint fId = 0;
    int fWidth = 0;
    int fHeight = 0;
    GrBackendTexture fGrTex;

    void bind(QGL33 * const gl) const;

    void clear(GrContext* const context);

    //! @brief Generates, binds texture and sets data.
    void gen(GrContext* const context,
             const int width, const int height);

    void swap(eTexture& otherTexture);

    SkBitmap bitmapSnapshot(QGL33 * const gl) const;
    sk_sp<SkImage> imageSnapshot(QGL33 * const gl) const;
};

#endif // ETEXTURE_H
