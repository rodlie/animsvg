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

#version 330 core
layout(location = 0) out vec4 fragColor;

in vec2 texCoord;
layout(pixel_center_integer) in vec4 gl_FragCoord;

uniform sampler2D texture;

uniform vec2 translate;
uniform float dotDistance;
uniform float dotRadius;
uniform float opacity;

uniform vec2 scenePos;

void main(void) {
    bool inDot;
    float mixAlpha;

    vec2 transformedCoord = scenePos - translate + gl_FragCoord.xy;

    vec2 dotID = floor(transformedCoord/dotDistance + 0.5);
    vec2 posInDot = transformedCoord - dotID*dotDistance;
    float distToCenter = sqrt(posInDot.x*posInDot.x +
                              posInDot.y*posInDot.y);
    float distToEdge = distToCenter - sqrt(dotRadius*dotRadius);
    if(distToEdge > 0.f) {
        inDot = false;
    } else {
        distToEdge = abs(distToEdge);
        inDot = true;
        if(distToEdge < 1.f) {
            mixAlpha = opacity + distToEdge*(1. - opacity);
        } else {
            mixAlpha = 1.f;
        }
    }
    vec4 texCol = texture2D(texture, texCoord);
    if(inDot) {
        fragColor =  vec4(mixAlpha*texCol.rgb, mixAlpha*texCol.a);
    } else {
        fragColor =  vec4(opacity*texCol.rgb, opacity*texCol.a);
    }
}
