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

#ifndef CUBICNODE_H
#define CUBICNODE_H
#include <QPointF>

#include "../core_global.h"
struct CORE_EXPORT CubicNode {
public:
    CubicNode(const QPointF& c1, const QPointF& p, const QPointF& c2) {
        mC1 = c1; mP = p; mC2 = c2;
    }
private:
    QPointF mC1;
    QPointF mP;
    QPointF mC2;
};

#endif // CUBICNODE_H
