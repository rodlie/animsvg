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

#ifndef EMIMEDATA_H
#define EMIMEDATA_H
#include <QMimeData>
#include "exceptions.h"
#include "smartPointers/ememory.h"

class CORE_EXPORT eDraggedObjects {
public:
    template <typename T>
    eDraggedObjects(const QList<T*>& objs) {
        for(const auto& obj : objs) mObjects << obj;
    }

    template <typename T>
    bool hasType() const {
        return getObjects<T>().count() != 0;
    }

    template <typename T>
    QList<T*> getObjects() const {
        QList<T*> result;
        for(const auto& obj : mObjects) {
            const auto ObjT = enve_cast<T*>(obj);
            if(ObjT) result << ObjT;
        }
        return result;
    }
private:
    QList<QObject*> mObjects;
};

class CORE_EXPORT eMimeData : public QMimeData, public eDraggedObjects {
public:
    template <typename T>
    eMimeData(const QList<T*>& objs) : eDraggedObjects(objs) {}

    template <typename T>
    static bool sHasType(const QMimeData* const data) {
        if(!data->hasFormat("eMimeData")) return false;
        return static_cast<const eMimeData*>(data)->hasType<T>();
    }
protected:
    bool hasFormat(const QString &mimetype) const {
        if(mimetype == "eMimeData") return true;
        return false;
    }
};

#endif // EMIMEDATA_H
