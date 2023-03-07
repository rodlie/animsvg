/*
#
# enve2d - https://github.com/enve2d
#
# Copyright (c) 2023 enve2d developers
# Copyright (c) 2016-2022 Maurycy Liebner
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
*/

#include "appsupport.h"

#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QPalette>

AppSupport::AppSupport(QObject *parent)
    : QObject{parent}
{

}

void AppSupport::setupTheme()
{
    qDebug() << "setupTheme";

    QIcon::setThemeName(QString::fromUtf8("hicolor"));
    qApp->setStyle(QString::fromUtf8("fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(44, 44, 49));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(33, 33, 38));
    palette.setColor(QPalette::AlternateBase, QColor(44, 44, 49));
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(44, 44, 49));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(177, 16, 20));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);
}
