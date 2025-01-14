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

uniform sampler2D tex1;
uniform sampler2D tex2;

uniform vec4 rect2;
uniform float opacity2;

void main(void) {
    vec4 color1 = texture(tex1, texCoord);
    bool inRect2 = texCoord.x > rect2.x &&
                   texCoord.y > rect2.y &&
                   texCoord.x < rect2.z &&
                   texCoord.y < rect2.w;
    if(inRect2) {
        vec4 color2 = opacity2*texture(tex2, (texCoord - rect2.xy)/(rect2.zw - rect2.xy));
        if(color2.a > color1.a) {
            float m2 = 1 - color1.a/color2.a;
            fragColor = color1 + color2*m2;
        } else {
            fragColor = color1;
        }
    } else {
        fragColor = color1;
    }
}
