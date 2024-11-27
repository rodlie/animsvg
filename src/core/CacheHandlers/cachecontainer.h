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

#ifndef MINIMALCACHECONTAINER_H
#define MINIMALCACHECONTAINER_H
#include "smartPointers/stdselfref.h"

class CORE_EXPORT CacheContainer : public StdSelfRef {
    friend class UsePointerBase;
    friend class UsedRange;
    friend class MemoryHandler;
    friend class MemoryDataHandler;
protected:
    CacheContainer();
public:
    ~CacheContainer();

    virtual int getByteCount() = 0;
protected:
    virtual void noDataLeft_k() = 0;
private:
    virtual int free_RAM_k();
public:
    bool handledByMemoryHandler() const
    { return mHandledByMemoryHandler; }

    bool inUse() const { return mInUse; }
protected:
    void addToMemoryManagment();
    void removeFromMemoryManagment();
    void updateInMemoryManagment();
private:
    void incInUse();
    void decInUse();

    bool mHandledByMemoryHandler = false;
    int mInUse = 0;
};

#endif // MINIMALCACHECONTAINER_H
