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

#include "colorwidgetshaders.h"
#include "glhelpers.h"
#include "exceptions.h"

ColorProgram HUE_PROGRAM;
ColorProgram HSV_SATURATION_PROGRAM;
ColorProgram VALUE_PROGRAM;

ColorProgram HSL_SATURATION_PROGRAM;
ColorProgram LIGHTNESS_PROGRAM;

ColorProgram RED_PROGRAM;
ColorProgram GREEN_PROGRAM;
ColorProgram BLUE_PROGRAM;

ColorProgram ALPHA_PROGRAM;

PlainColorProgram PLAIN_PROGRAM;
GradientProgram GRADIENT_PROGRAM;

BorderProgram BORDER_PROGRAM;
DoubleBorderProgram DOUBLE_BORDER_PROGRAM;

void iniColorProgram(QGL33 * const gl,
                     ColorProgram& program,
                     const QString& vShaderPath,
                     const QString& fShaderPath) {
    try {
        gIniProgram(gl, program.fID, vShaderPath, fShaderPath);
        program.fHSVColorLoc = gl->glGetUniformLocation(
                    program.fID, "HSVColor"); // optional
        program.fRGBColorLoc = gl->glGetUniformLocation(
                    program.fID, "RGBColor"); // optional
        program.fHSLColorLoc = gl->glGetUniformLocation(
                    program.fID, "HSLColor"); // optional
        program.fCurrentValueLoc = gl->glGetUniformLocation(
                    program.fID, "currentValue");
        program.fHandleWidthLoc = gl->glGetUniformLocation(
                    program.fID, "handleWidth");
        program.fLightHandleLoc = gl->glGetUniformLocation(
                    program.fID, "lightHandle");

        program.fMeshSizeLoc = gl->glGetUniformLocation(
                    program.fID, "meshSize"); // optional

        checkGLErrors(gl);
    } catch(...) {
        RuntimeThrow("Error initializing color program.");
    }
}

void iniPlainColorProgram(QGL33 * const gl, const QString& colorShadersPath) {
    try {
        gIniProgram(gl, PLAIN_PROGRAM.fID, GL_PLAIN_VERT,
                   colorShadersPath + "plain.frag");
        PLAIN_PROGRAM.fRGBAColorLoc = gl->glGetUniformLocation(
                    PLAIN_PROGRAM.fID, "RGBAColor");
        PLAIN_PROGRAM.fMeshSizeLoc = gl->glGetUniformLocation(
                    PLAIN_PROGRAM.fID, "meshSize");
    } catch(...) {
        RuntimeThrow("Error initializing plain color program.");
    }
}

void iniBorderProgram(QGL33 * const gl, const QString& colorShadersPath) {
    try {
        gIniProgram(gl, BORDER_PROGRAM.fID, GL_PLAIN_VERT,
                   colorShadersPath + "border.frag");
        BORDER_PROGRAM.fBorderSizeLoc = gl->glGetUniformLocation(
                    BORDER_PROGRAM.fID, "borderSize");
        BORDER_PROGRAM.fBorderColorLoc = gl->glGetUniformLocation(
                    BORDER_PROGRAM.fID, "borderColor");
    } catch(...) {
        RuntimeThrow("Error initializing border program.");
    }
}

void iniDoubleBorderProgram(QGL33 * const gl, const QString& colorShadersPath) {
    try {
        gIniProgram(gl, DOUBLE_BORDER_PROGRAM.fID, GL_PLAIN_VERT,
                   colorShadersPath + "doubleborder.frag");
        DOUBLE_BORDER_PROGRAM.fInnerBorderSizeLoc = gl->glGetUniformLocation(
                    DOUBLE_BORDER_PROGRAM.fID, "innerBorderSize");
        DOUBLE_BORDER_PROGRAM.fInnerBorderColorLoc = gl->glGetUniformLocation(
                    DOUBLE_BORDER_PROGRAM.fID, "innerBorderColor");
        DOUBLE_BORDER_PROGRAM.fOuterBorderSizeLoc = gl->glGetUniformLocation(
                    DOUBLE_BORDER_PROGRAM.fID, "outerBorderSize");
        DOUBLE_BORDER_PROGRAM.fOuterBorderColorLoc = gl->glGetUniformLocation(
                    DOUBLE_BORDER_PROGRAM.fID, "outerBorderColor");
    } catch(...) {
        RuntimeThrow("Error initializing double border program.");
    }
}

void iniGradientProgram(QGL33 * const gl, const QString& colorShadersPath) {
    try {
        gIniProgram(gl, GRADIENT_PROGRAM.fID, GL_PLAIN_VERT,
                   colorShadersPath + "gradient.frag");
        GRADIENT_PROGRAM.fRGBAColor1Loc = gl->glGetUniformLocation(
                    GRADIENT_PROGRAM.fID, "RGBAColor1");
        GRADIENT_PROGRAM.fRGBAColor2Loc = gl->glGetUniformLocation(
                    GRADIENT_PROGRAM.fID, "RGBAColor2");
        GRADIENT_PROGRAM.fMeshSizeLoc = gl->glGetUniformLocation(
                    GRADIENT_PROGRAM.fID, "meshSize");
    } catch(...) {
        RuntimeThrow("Error initializing gradient program.");
    }
}

void iniColorPrograms(QGL33 * const gl) {
    const QString colorShadersPath = ":/colorwidgetshaders/";

    try {
        iniColorProgram(gl, HUE_PROGRAM, GL_PLAIN_VERT,
                        colorShadersPath + "hue.frag");
        iniColorProgram(gl, HSV_SATURATION_PROGRAM, GL_PLAIN_VERT,
                        colorShadersPath + "hsv_saturation.frag");
        iniColorProgram(gl, VALUE_PROGRAM, GL_PLAIN_VERT,
                        colorShadersPath + "value.frag");
        iniColorProgram(gl, HSL_SATURATION_PROGRAM, GL_PLAIN_VERT,
                        colorShadersPath + "hsl_saturation.frag");
        iniColorProgram(gl, LIGHTNESS_PROGRAM, GL_PLAIN_VERT,
                        colorShadersPath + "lightness.frag");
        iniColorProgram(gl, RED_PROGRAM, GL_PLAIN_VERT,
                        colorShadersPath + "red.frag");
        iniColorProgram(gl, GREEN_PROGRAM, GL_PLAIN_VERT,
                        colorShadersPath + "green.frag");
        iniColorProgram(gl, BLUE_PROGRAM, GL_PLAIN_VERT,
                        colorShadersPath + "blue.frag");

        iniColorProgram(gl, ALPHA_PROGRAM, GL_PLAIN_VERT,
                        colorShadersPath + "alpha.frag");
        iniPlainColorProgram(gl, colorShadersPath);
        iniGradientProgram(gl, colorShadersPath);
        iniBorderProgram(gl, colorShadersPath);
        iniDoubleBorderProgram(gl, colorShadersPath);
    } catch(...) {
        RuntimeThrow("Error initializing color programs.");
    }
}
