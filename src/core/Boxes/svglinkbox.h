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

#ifndef SVGLINKBOX_H
#define SVGLINKBOX_H

#include "externallinkboxt.h"
#include "Boxes/containerbox.h"
#include "FileCacheHandlers/svgfilecachehandler.h"

using SvgLinkBoxBase =
    ExternalLinkBoxT<ContainerBox,
                     SvgFileCacheHandler>;

class CORE_EXPORT SvgLinkBox : public SvgLinkBoxBase {
public:
    SvgLinkBox();

    void changeSourceFile();
private:
    void updateContent();
    void fileHandlerConnector(ConnContext &conn, SvgFileCacheHandler *obj);
    void fileHandlerAfterAssigned(SvgFileCacheHandler *obj);

    QList<qsptr<Gradient>> mGradients;
};

#endif // SVGLINKBOX_H
