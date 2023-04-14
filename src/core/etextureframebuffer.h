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

#ifndef ETEXTUREFRAMEBUFFER_H
#define ETEXTUREFRAMEBUFFER_H
#include "etexture.h"

struct CORE_EXPORT eTextureFrameBuffer {
    GrBackendRenderTarget fGrFBO;
    eTexture fTexture;
    GLuint fFBOId = 0;
    int fWidth = 0;
    int fHeight = 0;
    bool fBound = false;

    //! @brief Swaps underlying texture and bind FBO
    void swapTexture(QGL33 * const gl, eTexture& otherTexture);

    void clear(QGL33 * const gl, GrDirectContext* const context);

    void bind(QGL33 * const gl);
    void unbind();

    void bindTexture(QGL33 * const gl);

    //! @brief Generates and binds framebuffer and associated texture.
    void gen(QGL33 * const gl, GrDirectContext * const context,
             const int width, const int height);

    sk_sp<SkImage> toImage(QGL33 * const gl) const {
        return fTexture.imageSnapshot(gl);
    }

    SkBitmap toBitmap(QGL33 * const gl) const {
        return fTexture.bitmapSnapshot(gl);
    }
};

#endif // ETEXTUREFRAMEBUFFER_H
