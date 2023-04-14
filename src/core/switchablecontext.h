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

#ifndef SWITCHABLECONTEXT_H
#define SWITCHABLECONTEXT_H
#include "glhelpers.h"

class CORE_EXPORT SwitchableContext {
    friend class GpuTaskExecutor;
    enum class Mode { OpenGL, Skia };
public:
    //! @brief Returns the handled GrDirectContext,
    GrDirectContext* grContext() const {
        return mContext.get();
    }

    bool skiaMode() const { return mMode == Mode::Skia; }
    bool openGLMode() const { return mMode == Mode::OpenGL; }

    void switchToSkia();
    void switchToOpenGL(QGL33* const gl);

    GLuint textureSquareVAO() const { return mTexturedSquareVAO; }
private:
    void setContext(const sk_sp<GrDirectContext>& context,
                    const GLuint textureSquareVAO);

    sk_sp<GrDirectContext> mContext;
    Mode mMode = Mode::Skia;
    GLuint mTexturedSquareVAO;
};

#endif // SWITCHABLECONTEXT_H
