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

#ifndef BOXESLISTACTIONBUTTON_H
#define BOXESLISTACTIONBUTTON_H

#include "../core_global.h"
#include "GUI/global.h"
#include "Private/esettings.h"

#include <functional>

#include <QDebug>
#include <QPaintEvent>
#include <QPushButton>

// LEGACY:
class CORE_EXPORT BoxesListActionButton : public QWidget {
    Q_OBJECT
public:
    BoxesListActionButton(QWidget * const parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

    bool mHover = false;
signals:
    void pressed();
};

class CORE_EXPORT PixmapActionButton : public BoxesListActionButton {
public:
    PixmapActionButton(QWidget * const parent = nullptr) :
        BoxesListActionButton(parent) {}

    void setPixmapChooser(const std::function<QPixmap*()>& func) {
        mPixmapChooser = func;
    }
protected:
    void paintEvent(QPaintEvent*);
private:
    std::function<QPixmap*()> mPixmapChooser;
};

// REFACTOR:
class CORE_EXPORT BoxesListIconActionButton : public QPushButton
{
    Q_OBJECT
public:
    BoxesListIconActionButton(QWidget *parent = nullptr) : QPushButton(parent)
    {
        setObjectName("FlatButton");
        eSizesUI::widget.add(this, [this](const int size) {
            setFixedSize(size, size);
            //if (eSettings::instance().fCurrentInterfaceDPI != 1.) {
                setIconSize(QSize(size, size));
            //}
        });
    }
};

class CORE_EXPORT IconActionButton : public BoxesListIconActionButton
{
public:
    IconActionButton(QWidget *parent = nullptr) : BoxesListIconActionButton(parent) {}
    void setIconChooser(const std::function<QIcon*()>& func)
    {
        mIconChooser = func;
    }
protected:
    void paintEvent(QPaintEvent *event)
    {
        if (!mIconChooser) { return; }
        const auto ico = mIconChooser();
        if (!ico) { return; }
        setIcon(*ico);
        BoxesListIconActionButton::paintEvent(event);
    }
private:
    std::function<QIcon*()> mIconChooser;
};

#endif // BOXESLISTACTIONBUTTON_H
