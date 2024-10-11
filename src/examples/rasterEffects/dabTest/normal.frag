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

uniform sampler2D dabData;

void main(void) {
    float fX = texelFetch(dabData, ivec2(0, 0), 0).r; // 0
    float fY = texelFetch(dabData, ivec2(1, 0), 0).r; // 1
    float fRadius = texelFetch(dabData, ivec2(2, 0), 0).r; // 2
    float fAspectRatio = texelFetch(dabData, ivec2(3, 0), 0).r; // 3
    float fSinTh = texelFetch(dabData, ivec2(4, 0), 0).r; // 4
    float fCosTh = texelFetch(dabData, ivec2(5, 0), 0).r; // 5
    float fHardness = texelFetch(dabData, ivec2(6, 0), 0).r; // 6
    float fOpaque = texelFetch(dabData, ivec2(7, 0), 0).r; // 7
    float fRed = texelFetch(dabData, ivec2(8, 0), 0).r; // 8
    float fGreen = texelFetch(dabData, ivec2(9, 0), 0).r; // 9
    float fBlue = texelFetch(dabData, ivec2(10, 0), 0).r; // 10
    float fAlpha = texelFetch(dabData, ivec2(11, 0), 0).r; // 11
    float fSeg1Offset = texelFetch(dabData, ivec2(12, 0), 0).r; // 12
    float fSeg1Slope = texelFetch(dabData, ivec2(13, 0), 0).r; // 13
    float fSeg2Offset = texelFetch(dabData, ivec2(14, 0), 0).r; // 14
    float fSeg2Slope = texelFetch(dabData, ivec2(15, 0), 0).r; // 15

    float xp = texCoord.x;
    float yp = texCoord.y;

    float yy = (yp - fY);
    float xx = (xp - fX);
    float yyr = (yy*fCosTh - xx*fSinTh) * fAspectRatio;
    float xxr = yy*fSinTh + xx*fCosTh;
    float rr = (yyr*yyr + xxr*xxr)/(fRadius*fRadius);
	
    if(rr > 1.f) {
        fragColor = vec4(0.f, 0.f, 0.f, 0.f);
    } else {
        float fac = (rr <= fHardness ? fSeg1Slope : fSeg2Slope);
        float opa = (rr <= fHardness ? fSeg1Offset : fSeg2Offset) + rr*fac;
        opa *= fAlpha;
        fragColor = vec4(fRed*opa, fGreen*opa, fBlue*opa, opa);
    }
}
