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

#ifndef GPURENDERTOOLS_H
#define GPURENDERTOOLS_H

#include <QRect>

#include "glhelpers.h"
#include "switchablecontext.h"
#include "etextureframebuffer.h"

class CORE_EXPORT GpuRenderTools {
    friend class SkiaRenderTools;
    friend class OpenGLRenderTools;
public:
    GpuRenderTools(QGL33* const gl,
                   SwitchableContext& context,
                   sk_sp<SkImage> img,
                   const QRect& globalRect);

    ~GpuRenderTools();

    //! @brief Swaps the source and the target texture if valid.
    void swapTextures();

    // Skia
    void switchToSkia();
    //! @brief Returns SkCanvas associated with the target texture.
    //! If there is no SkCanvas new SkCanvas is created.
    SkCanvas* requestTargetCanvas();
    sk_sp<SkImage> requestSrcTextureImageWrapper();
    // Skia

    // OpenGL
    void switchToOpenGL(QGL33* const gl);

    GLuint getSquareVAO() const;

    //! @brief Returned texture may be used as one wishes,
    //!  but has to be valid.
    eTexture& getSrcTexture();

    bool imageToTexture(sk_sp<SkImage> img, eTexture& texture);

    //! @brief Returns TextureFrameBuffer associated with the target texture.
    //! If there is no SkCanvas new SkCanvas is created.
    eTextureFrameBuffer& requestTargetFbo();
    // OpenGL

    bool validTargetCanvas() const;
    bool validTargetFbo() const;

    const QRect fGlobalRect;
private:
    GrBackendTexture sourceBackedTexture();

    QGL33* const mGL;
    SwitchableContext& mContext;

    eTexture mSrcTexture;
    eTextureFrameBuffer mTargetTextureFbo;

    sk_sp<SkSurface> mSurface;
    SkCanvas* mCanvas = nullptr;
};

#endif // GPURENDERTOOLS_H
