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

#ifndef VLABEL_H
#define VLABEL_H

#include <QLabel>
#include <QStylePainter>
#include <QPaintEvent>

class VLabel : public QLabel
{
    Q_OBJECT

public:
    explicit VLabel(QWidget *parent = nullptr)
        : QLabel(parent) {}
    explicit VLabel(const QString &text,
                    QWidget *parent = nullptr)
        : QLabel(text, parent) {}

protected:
    void paintEvent(QPaintEvent*)
    {
        QStylePainter painter(this);
        painter.rotate(90);
        painter.drawText(0, -(sizeHint().width() / 2)-2, text());
    }
    QSize sizeHint() const
    {
        const auto s = QLabel::sizeHint();
        return QSize(s.height(), s.width());
    }
    QSize minimumSizeHint() const
    {
        const auto s = QLabel::minimumSizeHint();
        return QSize(s.height(), s.width());
    }
};

#endif // VLABEL_H
