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

#ifndef SINGLEWIDGET_H
#define SINGLEWIDGET_H

#include <QWidget>
#include "smartPointers/stdpointer.h"
class SWT_Abstraction;
class ScrollWidgetVisiblePart;
class SingleWidgetTarget;

class SingleWidget : public QWidget {
    Q_OBJECT
public:
    SingleWidget(ScrollWidgetVisiblePart *parent = nullptr);
    virtual void setTargetAbstraction(SWT_Abstraction *abs);
    SWT_Abstraction *getTargetAbstraction();
    SingleWidgetTarget *getTarget() const;
protected:
    stdptr<SWT_Abstraction> mTarget;
    ScrollWidgetVisiblePart *mParent;
};

#endif // SINGLEWIDGET_H
