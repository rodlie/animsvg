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

#ifndef RENDERDATAHANDLER_H
#define RENDERDATAHANDLER_H
#include "boxrenderdata.h"
#include <map>

class CORE_EXPORT RenderDataHandler {
public:
    void clear() { mFrameToData.clear(); }
    bool removeItem(const stdsptr<BoxRenderData> &item);
    bool removeItemAtRelFrame(const qreal frame);
    BoxRenderData *getItemAtRelFrame(const qreal frame) const;
    void addItemAtRelFrame(const stdsptr<BoxRenderData> &item);
private:
    int frameToKey(const qreal frame) const {
        return qRound(frame*1000);
    }

    std::map<int, stdsptr<BoxRenderData>> mFrameToData;
};

#endif // RENDERDATAHANDLER_H
