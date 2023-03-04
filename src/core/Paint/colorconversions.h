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

#ifndef COLORCONVERSIONS_H
#define COLORCONVERSIONS_H

#include <qglobal.h>

#include <stdint.h>

void rgba8_to_rgba16(uint8_t* src,
                     const int srcWidth,
                     uint16_t* dst,
                     const int dstWidth,
                     const int height);
// Naive conversion code from the internal MyPaint format and 8 bit RGB
void rgba16_to_rgba8_unpremultiplied(uint16_t* src,
                    const int srcWidth,
                    uint8_t* dst,
                    const int dstWidth,
                    const int height);

void rgba16_to_rgba8_premultiplied(
        uint16_t* src,
        const int srcWidth,
        uint8_t* dst,
        const int dstWidth,
        const int height);

#endif // COLORCONVERSIONS_H
