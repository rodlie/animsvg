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

uniform sampler2D texture;

uniform float sharpness;
uniform float direction;
uniform float time;

const float PI = 3.1415926535;
const float sqrt2 = 1.41421356237;

void main(void) {
    float radDir = direction * PI / 180;
    float x = texCoord.x;
    float y = texCoord.y;

    bool i = mod(radDir, PI) > 0.5*PI;
    if(i) {
        x = 1 - x;
        radDir = PI - radDir;
    }

    float a = sqrt(x*x + y*y);
    float b = radDir - asin(y / a);
    float c = 0.25*PI - radDir;

    float f = a * cos(b) / (cos(c) * sqrt2);

    bool ii = mod(radDir, 2 * PI) > PI;
    if(ii) f = 1 - f;

    f += 0.33333 * sqrt2 * (1 - sharpness);

    float width = 2 - sharpness;
    float margin = 0.5*(width - 1);
    float x0 = width * time - margin;
    float x1 = x0 + 1 - sharpness;

    float alpha;
    if(f < x0) {
        alpha = 0;
    } else if(f > x1) {
        alpha = 1;
    } else {
        alpha = 1 - 0.5*(cos(PI*(f - x0)/(1 - sharpness)) + 1);
    }
    fragColor = texture2D(texture, texCoord) * alpha;
}