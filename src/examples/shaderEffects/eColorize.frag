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

#version 330 core
layout(location = 0) out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D texture;

uniform float hue;
uniform float saturation;
uniform float lightness;

vec3 HUEtoRGB(in float H) {
    float R = abs(H * 6 - 3) - 1;
    float G = 2 - abs(H * 6 - 2);
    float B = 2 - abs(H * 6 - 4);
    return clamp(vec3(R,G,B), 0., 1.);
}

float Epsilon = 1e-10;

vec3 RGBtoHCV(in vec3 RGB) {
    // Based on work by Sam Hocevar and Emil Persson
    vec4 P = (RGB.g < RGB.b) ? vec4(RGB.bg, -1.0, 2.0/3.0) : vec4(RGB.gb, 0.0, -1.0/3.0);
    vec4 Q = (RGB.r < P.x) ? vec4(P.xyw, RGB.r) : vec4(RGB.r, P.yzx);
    float C = Q.x - min(Q.w, Q.y);
    float H = abs((Q.w - Q.y) / (6 * C + Epsilon) + Q.z);
    return vec3(H, C, Q.x);
}

vec3 HSLtoRGB(in vec3 HSL) {
    vec3 RGB = HUEtoRGB(HSL.x);
    float C = (1 - abs(2 * HSL.z - 1)) * HSL.y;
    return (RGB - 0.5) * C + HSL.z;
}

vec3 RGBtoHSL(in vec3 RGB) {
    vec3 HCV = RGBtoHCV(RGB);
    float L = HCV.z - HCV.y * 0.5;
    float S = HCV.y / (1 - abs(L * 2 - 1) + Epsilon);
    return vec3(HCV.x, S, L);
}

void main(void) {
    vec4 color = texture2D(texture, texCoord);
    if(color.a < 0.00001f) {
        fragColor = color;
    } else {
        float h = mod(hue, 360.)/360.;
        color.rgb /= color.a;
        color.xyz = RGBtoHSL(color.rgb);
        color.x = h;
        color.y = saturation;
        color.z = clamp(color.z + lightness, 0., 1.);
        color.rgb = HSLtoRGB(color.xyz) * color.a;
        fragColor = color;
    }
}
