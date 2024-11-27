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

uniform int steps;
uniform int directions;
uniform vec2 displacementXY;

const float PIx2 = 6.28318530718;

void main(void) {
    vec4 Color = texture2D(texture, texCoord);
	float invStep = 1.0/float(steps);
    for(int i = 0; i < directions; i++) {
		float d = i*PIx2/float(directions);
		vec2 displ = vec2(cos(d), sin(d))*displacementXY;
        for(int j = 1; j <= steps; j++) {
            Color += texture2D(texture, texCoord + displ*j*invStep);
        }
    }
    Color /= float(steps)*float(directions) + 1.0;
    fragColor = Color;
}
