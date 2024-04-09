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

#ifndef COLORWIDGETSHADERS_H
#define COLORWIDGETSHADERS_H

#include "ui_global.h"

#include "glhelpers.h"

struct UI_EXPORT ColorProgram {
    GLuint fID;
    GLint fHSVColorLoc;
    GLint fRGBColorLoc;
    GLint fHSLColorLoc;

    GLint fCurrentValueLoc;
    GLint fHandleWidthLoc;
    GLint fLightHandleLoc;
    GLint fMeshSizeLoc;
};

struct UI_EXPORT PlainColorProgram {
    GLuint fID;
    GLint fRGBAColorLoc;
    GLint fMeshSizeLoc;
};

struct UI_EXPORT GradientProgram {
    GLuint fID;
    GLint fRGBAColor1Loc;
    GLint fRGBAColor2Loc;
    GLint fMeshSizeLoc;
};

struct UI_EXPORT BorderProgram {
    GLuint fID;
    GLint fBorderSizeLoc;
    GLint fBorderColorLoc;
};

struct UI_EXPORT DoubleBorderProgram {
    GLuint fID;
    GLint fInnerBorderSizeLoc;
    GLint fInnerBorderColorLoc;
    GLint fOuterBorderSizeLoc;
    GLint fOuterBorderColorLoc;
};

extern UI_EXPORT ColorProgram HUE_PROGRAM;
extern UI_EXPORT ColorProgram HSV_SATURATION_PROGRAM;
extern UI_EXPORT ColorProgram VALUE_PROGRAM;

extern UI_EXPORT ColorProgram HSL_SATURATION_PROGRAM;
extern UI_EXPORT ColorProgram LIGHTNESS_PROGRAM;

extern UI_EXPORT ColorProgram RED_PROGRAM;
extern UI_EXPORT ColorProgram GREEN_PROGRAM;
extern UI_EXPORT ColorProgram BLUE_PROGRAM;

extern UI_EXPORT ColorProgram ALPHA_PROGRAM;

extern UI_EXPORT PlainColorProgram PLAIN_PROGRAM;
extern UI_EXPORT GradientProgram GRADIENT_PROGRAM;

extern UI_EXPORT BorderProgram BORDER_PROGRAM;
extern UI_EXPORT DoubleBorderProgram DOUBLE_BORDER_PROGRAM;

extern UI_EXPORT void iniColorPrograms(QGL33 * const gl);

#endif // COLORWIDGETSHADERS_H
