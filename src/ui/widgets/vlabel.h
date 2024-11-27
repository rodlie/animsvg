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

#ifndef VLABEL_H
#define VLABEL_H

#include "ui_global.h"

#include <QLabel>
#include <QStylePainter>
#include <QPaintEvent>

class UI_EXPORT VLabel : public QLabel
{
    Q_OBJECT

public:
    explicit VLabel(QWidget *parent = nullptr,
                    const Qt::Orientation &orientation = Qt::Vertical)
        : QLabel(parent)
        , mOrientation(orientation)
    {}
    explicit VLabel(const QString &text,
                    QWidget *parent = nullptr,
                    const Qt::Orientation &orientation = Qt::Vertical)
        : QLabel(text, parent)
        , mOrientation(orientation)
    {}
    void setOrientation(Qt::Orientation orientation)
    {
        mOrientation = orientation;
        update();
    }

protected:
    void paintEvent(QPaintEvent *e)
    {
        if (mOrientation == Qt::Horizontal) {
            QLabel::paintEvent(e);
            return;
        }
        QStylePainter painter(this);
        painter.rotate(90);
        painter.drawText(0, -(sizeHint().width() / 2)-2, text());
    }
    QSize sizeHint() const
    {
        if (mOrientation == Qt::Horizontal) { return QLabel::sizeHint(); }
        const auto s = QLabel::sizeHint();
        return QSize(s.height(), s.width());
    }
    QSize minimumSizeHint() const
    {
        if (mOrientation == Qt::Horizontal) { return QLabel::minimumSizeHint(); }
        const auto s = QLabel::minimumSizeHint();
        return QSize(s.height(), s.width());
    }

private:
    Qt::Orientation mOrientation;
};

#endif // VLABEL_H
