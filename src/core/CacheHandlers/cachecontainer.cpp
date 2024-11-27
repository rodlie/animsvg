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

#include "cachecontainer.h"
#include "memorydatahandler.h"

CacheContainer::CacheContainer() {
    addToMemoryManagment();
}

CacheContainer::~CacheContainer() {
    if(!MemoryDataHandler::sInstance) return;
    removeFromMemoryManagment();
}

int CacheContainer::free_RAM_k() {
    const int bytes = getByteCount();
    noDataLeft_k();
    return bytes;
}

void CacheContainer::addToMemoryManagment() {
    if(mHandledByMemoryHandler || mInUse) return;
    MemoryDataHandler::sInstance->addContainer(this);
    mHandledByMemoryHandler = true;
}

void CacheContainer::removeFromMemoryManagment() {
    if(!mHandledByMemoryHandler) return;
    MemoryDataHandler::sInstance->removeContainer(this);
    mHandledByMemoryHandler = false;
}

void CacheContainer::updateInMemoryManagment() {
    if(!mHandledByMemoryHandler) addToMemoryManagment();
    else MemoryDataHandler::sInstance->containerUpdated(this);
}

void CacheContainer::incInUse() {
    mInUse++;
    removeFromMemoryManagment();
}

void CacheContainer::decInUse() {
    mInUse--;
    Q_ASSERT(mInUse >= 0);
    if(!mInUse) addToMemoryManagment();
}
