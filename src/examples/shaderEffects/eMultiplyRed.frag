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

// enve uses OpenGL 3.3
#version 330 core
// output color
layout(location = 0) out vec4 fragColor;

// processed texture coordinate, from the vertex shader
in vec2 texCoord;

// texture provided by enve
uniform sampler2D texture;
// the value for the 'red' property
uniform float red;

void main(void) {
    // texture pixel color at the coordinate
    vec4 color = texture2D(texture, texCoord);
    // assign value to the output color,
    // multiply red by the 'red' property value
    fragColor = vec4(color.r * red, color.g, color.b, color.a); 
}
