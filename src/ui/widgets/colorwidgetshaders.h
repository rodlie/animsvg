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
#include "glhelpers.h"

struct ColorProgram {
    GLuint fID;
    GLint fHSVColorLoc;
    GLint fRGBColorLoc;
    GLint fHSLColorLoc;

    GLint fCurrentValueLoc;
    GLint fHandleWidthLoc;
    GLint fLightHandleLoc;
    GLint fMeshSizeLoc;
};

struct PlainColorProgram {
    GLuint fID;
    GLint fRGBAColorLoc;
    GLint fMeshSizeLoc;
};

struct GradientProgram {
    GLuint fID;
    GLint fRGBAColor1Loc;
    GLint fRGBAColor2Loc;
    GLint fMeshSizeLoc;
};

struct BorderProgram {
    GLuint fID;
    GLint fBorderSizeLoc;
    GLint fBorderColorLoc;
};

struct DoubleBorderProgram {
    GLuint fID;
    GLint fInnerBorderSizeLoc;
    GLint fInnerBorderColorLoc;
    GLint fOuterBorderSizeLoc;
    GLint fOuterBorderColorLoc;
};

extern ColorProgram HUE_PROGRAM;
extern ColorProgram HSV_SATURATION_PROGRAM;
extern ColorProgram VALUE_PROGRAM;

extern ColorProgram HSL_SATURATION_PROGRAM;
extern ColorProgram LIGHTNESS_PROGRAM;

extern ColorProgram RED_PROGRAM;
extern ColorProgram GREEN_PROGRAM;
extern ColorProgram BLUE_PROGRAM;

extern ColorProgram ALPHA_PROGRAM;

extern PlainColorProgram PLAIN_PROGRAM;
extern GradientProgram GRADIENT_PROGRAM;

extern BorderProgram BORDER_PROGRAM;
extern DoubleBorderProgram DOUBLE_BORDER_PROGRAM;

extern void iniColorPrograms(QGL33 * const gl);

#endif // COLORWIDGETSHADERS_H
