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

#include "memorydatahandler.h"
#include "CacheHandlers/cachecontainer.h"

MemoryDataHandler *MemoryDataHandler::sInstance = nullptr;

MemoryDataHandler::MemoryDataHandler() {
    Q_ASSERT(!sInstance);
    sInstance = this;
}

void MemoryDataHandler::addContainer(CacheContainer * const cont) {
    mContainers << cont;
}

void MemoryDataHandler::removeContainer(CacheContainer * const cont) {
    mContainers.removeOne(cont);
}

void MemoryDataHandler::containerUpdated(CacheContainer * const cont) {
    removeContainer(cont);
    addContainer(cont);
}

CacheContainer *MemoryDataHandler::takeFirst() {
    const auto cont = mContainers.takeFirst();
    cont->mHandledByMemoryHandler = false;
    return cont;
}
