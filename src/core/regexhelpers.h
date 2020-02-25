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

#ifndef REGEXHELPERS_H
#define REGEXHELPERS_H

#define REGEX_SPACES "\\s*"
#define REGEX_FLOAT "(-?[\\.|\\d]+)"
#define REGEX_SINGLE_FLOAT REGEX_SPACES REGEX_FLOAT REGEX_SPACES
#define REGEX_SEPARATOR REGEX_SPACES "[,|\\s+]" REGEX_SPACES
#define REGEX_INNER_FLOAT REGEX_FLOAT REGEX_SEPARATOR
#define REGEX_FIRST_FLOAT REGEX_SPACES REGEX_INNER_FLOAT
#define REGEX_LAST_FLOAT REGEX_FLOAT REGEX_SPACES

#endif // REGEXHELPERS_H
