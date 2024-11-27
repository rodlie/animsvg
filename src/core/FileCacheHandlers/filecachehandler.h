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

#ifndef FILECACHEHANDLER_H
#define FILECACHEHANDLER_H
#include "filedatacachehandler.h"
#include "smartPointers/ememory.h"
#include "conncontextptr.h"
#include "Animators/eboxorsound.h"

class CORE_EXPORT FileCacheHandler : public SelfRef {
    friend class FilesHandler;
    friend class FileHandlerObjRefBase;
    Q_OBJECT
protected:
    FileCacheHandler();

    virtual void reload() = 0;
public:
    virtual void replace() = 0;

    void reloadAction();
    bool deleteAction();

    const QString& path() const { return mPath; }
    bool fileMissing() const { return mFileMissing; }

    int refCount() const { return mReferenceCount; }
signals:
    void pathChanged(const QString& newPath);
    void reloaded();
    void deleteApproved(qsptr<FileCacheHandler>);
protected:
    void setPath(const QString& path);
    void setMissing(const bool missing);
private:
    void updateFileMissing();

    bool mFileMissing = false;
    QString mPath; // filename / dirname
private:
    int mReferenceCount = 0;
};

#endif // FILECACHEHANDLER_H
