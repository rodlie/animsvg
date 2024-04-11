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

#ifndef BRUSHCONTEXEDWRAPPER_H
#define BRUSHCONTEXEDWRAPPER_H
#include "Paint/simplebrushwrapper.h"
#include <QImage>

class BrushesContext;

struct CORE_EXPORT BrushData {
    QString fName;
    stdsptr<SimpleBrushWrapper> fWrapper;
    QImage fIcon;
    QByteArray fWholeFile;
};

class CORE_EXPORT BrushContexedWrapper : public SelfRef {
    Q_OBJECT
    e_OBJECT
public:
    void setSelected(const bool selected);

    bool selected() const;

    bool bookmarked() const;

    void setBookmarked(const bool bookmarked);

    const BrushData& getBrushData() const;

    SimpleBrushWrapper * getSimpleBrush();

    BrushesContext* getContext() const;

    void bookmark();
    void unbookmark();
protected:
    BrushContexedWrapper(BrushesContext* context,
                         const BrushData& brushData);
signals:
    void selectionChanged(bool);
    void bookmarkedChanged(bool);
private:
    bool mBookmarked = false;
    bool mSelected = false;
    const BrushData& mBrushData;
    BrushesContext* const mContext;
};

#endif // BRUSHCONTEXEDWRAPPER_H
