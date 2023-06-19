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

#include "gpurendertools.h"

GpuRenderTools::GpuRenderTools(QGL33 * const gl,
                               SwitchableContext &context,
                               sk_sp<SkImage> img,
                               const QRect& globalRect) :
    fGlobalRect(globalRect), mGL(gl), mContext(context) {
    imageToTexture(img, mSrcTexture);
}

GpuRenderTools::~GpuRenderTools() {
    mTargetTextureFbo.clear(mGL, mContext.grContext());
    mSrcTexture.clear(mContext.grContext());
}

GLuint GpuRenderTools::getSquareVAO() const {
    return mContext.textureSquareVAO();
}

void GpuRenderTools::swapTextures() {
    if(!validTargetFbo()) return;
    mTargetTextureFbo.swapTexture(mGL, mSrcTexture);
}

void GpuRenderTools::switchToOpenGL(QGL33 * const gl) {
    mContext.switchToOpenGL(gl);
}

void GpuRenderTools::switchToSkia() {
    mContext.switchToSkia();
}

SkCanvas *GpuRenderTools::requestTargetCanvas() {
    if(!mContext.skiaMode())
        RuntimeThrow("Requesting target canvas is "
                     "only available in Skia mode.\n"
                     "Use GpuRenderTools::switchToSkia "
                     "prior to this call.");
    if(!mCanvas) {
        requestTargetFbo();
        GrGLFramebufferInfo fboInfo;
        fboInfo.fFBOID = mTargetTextureFbo.fFBOId;
        fboInfo.fFormat = GR_GL_RGBA8;
        const auto grFbo = GrBackendRenderTarget(mTargetTextureFbo.fWidth,
                                                 mTargetTextureFbo.fHeight,
                                                 0, 8, fboInfo);
        const auto grContext = mContext.grContext();
        mSurface = SkSurface::MakeFromBackendRenderTarget(
                    grContext, grFbo, kBottomLeft_GrSurfaceOrigin,
                    kRGBA_8888_SkColorType, nullptr, nullptr);
        if(!mSurface) RuntimeThrow("Failed to make SkSurface.");

        mCanvas = mSurface->getCanvas();
    }
    return mCanvas;
}

eTexture &GpuRenderTools::getSrcTexture() {
    return mSrcTexture;
}

bool GpuRenderTools::imageToTexture(sk_sp<SkImage> img, eTexture& texture) {
    if(!img) return false;
    mContext.switchToSkia();
    if(!img->isTextureBacked())
        img = img->makeTextureImage(mContext.grContext(), GrMipMapped::kNo);
    const auto grTex = img->getBackendTexture(true);
    GrGLTextureInfo texInfo;
    grTex.getGLTextureInfo(&texInfo);
    texture.fId = texInfo.fID;
    texture.fWidth = grTex.width();
    texture.fHeight = grTex.height();
    return true;
}

eTextureFrameBuffer &GpuRenderTools::requestTargetFbo() {
    if(!validTargetFbo())
        mTargetTextureFbo.gen(mGL, mContext.grContext(),
                              mSrcTexture.fWidth, mSrcTexture.fHeight);
    return mTargetTextureFbo;
}

sk_sp<SkImage> GpuRenderTools::requestSrcTextureImageWrapper() {
    if(!mContext.skiaMode())
        RuntimeThrow("Wrapping texture image with SkImage is "
                     "only available in Skia mode.\n"
                     "Use GpuRenderTools::switchToSkia "
                     "prior to this call.");
    const auto grContext = mContext.grContext();
    return SkImage::MakeFromTexture(grContext,
                                    sourceBackedTexture(),
                                    kBottomLeft_GrSurfaceOrigin,
                                    kRGBA_8888_SkColorType,
                                    kPremul_SkAlphaType,
                                    nullptr);
}

bool GpuRenderTools::validTargetCanvas() const {
    return mCanvas;
}

bool GpuRenderTools::validTargetFbo() const {
    return mTargetTextureFbo.fFBOId != 0;
}

GrBackendTexture GpuRenderTools::sourceBackedTexture() {
    GrGLTextureInfo texInfo;
    texInfo.fID = mSrcTexture.fId;
    texInfo.fFormat = GR_GL_RGBA8;
    texInfo.fTarget = GR_GL_TEXTURE_2D;
    return GrBackendTexture(mSrcTexture.fWidth, mSrcTexture.fHeight,
                            GrMipMapped::kNo, texInfo);
}
