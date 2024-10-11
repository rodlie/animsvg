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

#include "etextureframebuffer.h"

void eTextureFrameBuffer::swapTexture(QGL33 * const gl, eTexture &otherTexture) {
    fTexture.swap(otherTexture);

    bind(gl);
    // create a color attachment texture
    gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, fTexture.fId, 0);
}

void eTextureFrameBuffer::clear(QGL33 * const gl, GrContext* const context) {
    if(fFBOId) gl->glDeleteFramebuffers(1, &fFBOId);
    fFBOId = 0;
    fWidth = 0;
    fHeight = 0;
    unbind();

    fTexture.clear(context);
}

void eTextureFrameBuffer::bind(QGL33 * const gl) {
    if(fBound) return;
    gl->glBindFramebuffer(GL_FRAMEBUFFER, fFBOId);
    fBound = true;
}

void eTextureFrameBuffer::unbind() {
    fBound = false;
}

void eTextureFrameBuffer::bindTexture(QGL33 * const gl) {
    fTexture.bind(gl);
}

void eTextureFrameBuffer::gen(QGL33 * const gl, GrContext* const context,
                             const int width, const int height) {
    fWidth = width;
    fHeight = height;
    gl->glGenFramebuffers(1, &fFBOId);
    bind(gl);
    gl->glClearColor(0, 0, 0, 0);
    // create a color attachment texture
    fTexture.gen(context, width, height);
    gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, fTexture.fId, 0);
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if(gl->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        RuntimeThrow("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
}
