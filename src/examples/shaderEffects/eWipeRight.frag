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

#version 330 core
layout(location = 0) out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D texture;

uniform float sharpness;
uniform float time;

void main(void) {
    float width = 2 - sharpness;
    float margin = 0.5*(width - 1);
    float x0 = width * (1 - time) - margin;
    float x1 = x0 + 1 - sharpness;
    float alpha;
    float fragX = texCoord.x + 0.5*(1 - sharpness);
    if(fragX < x0) {
        alpha = 1;
    } else if(fragX > x1) {
        alpha = 0;
    } else {
        alpha = 0.5*(cos(3.14159*(fragX - x0)/(1 - sharpness)) + 1);
    }
    fragColor = texture2D(texture, texCoord) * alpha;
}