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

#include "themesupport.h"

#include <QFile>
#include <QIcon>
#include <QApplication>

const QColor ThemeSupport::getThemeBaseColor(int alpha)
{
    return alpha == 255 ? QColor(26, 26, 30) : QColor(26, 26, 30, alpha);
}

SkColor ThemeSupport::getThemeBaseSkColor(int alpha)
{
    return SkColorSetARGB(alpha, 26, 26, 30);
}

const QColor ThemeSupport::getThemeBaseDarkColor(int alpha)
{
    return alpha == 255 ? QColor(25, 25, 25) : QColor(25, 25, 25, alpha);
}

const QColor ThemeSupport::getThemeBaseDarkerColor(int alpha)
{
    return alpha == 255 ? QColor(19, 19, 21) : QColor(19, 19, 21, alpha);
}

const QColor ThemeSupport::getThemeAlternateColor(int alpha)
{
    return alpha == 255 ? QColor(33, 33, 39) : QColor(33, 33, 39, alpha);
}

const QColor ThemeSupport::getThemeHighlightColor(int alpha)
{
    return alpha == 255 ? QColor(104, 144, 206) : QColor(104, 144, 206, alpha);
}

const QColor ThemeSupport::getThemeHighlightAlternativeColor(int alpha)
{
    return alpha == 255 ? QColor(167, 185, 222) : QColor(167, 185, 222, alpha);
}

const QColor ThemeSupport::getThemeHighlightSelectedColor(int alpha)
{
    return alpha == 255 ? QColor(150, 191, 255) : QColor(150, 191, 255, alpha);
}

SkColor ThemeSupport::getThemeHighlightSkColor(int alpha)
{
    return SkColorSetARGB(alpha, 104, 144, 206);
}

const QColor ThemeSupport::getThemeButtonBaseColor(int alpha)
{
    return alpha == 255 ? QColor(49, 49, 59) : QColor(49, 49, 59, alpha);
}

const QColor ThemeSupport::getThemeButtonBorderColor(int alpha)
{
    return alpha == 255 ? QColor(65, 65, 80) : QColor(65, 65, 80, alpha);
}

const QColor ThemeSupport::getThemeComboBaseColor(int alpha)
{
    return alpha == 255 ? QColor(36, 36, 53) : QColor(36, 36, 53, alpha);
}

const QColor ThemeSupport::getThemeTimelineColor(int alpha)
{
    return alpha == 255 ? QColor(44, 44, 49) : QColor(44, 44, 49, alpha);
}

const QColor ThemeSupport::getThemeRangeColor(int alpha)
{
    return alpha == 255 ? QColor(56, 73, 101) : QColor(56, 73, 101, alpha);
}

const QColor ThemeSupport::getThemeRangeSelectedColor(int alpha)
{
    return alpha == 255 ? QColor(104, 144, 206) : QColor(104, 144, 206, alpha);
}

const QPalette ThemeSupport::getDarkPalette(int alpha)
{
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, getThemeBaseColor(alpha));
    pal.setColor(QPalette::Base, getThemeBaseColor(alpha));
    pal.setColor(QPalette::Button, getThemeBaseColor(alpha));
    return pal;
}

const QPalette ThemeSupport::getDarkerPalette(int alpha)
{
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, getThemeBaseDarkerColor(alpha));
    pal.setColor(QPalette::Base, getThemeBaseDarkerColor(alpha));
    pal.setColor(QPalette::Button, getThemeBaseDarkerColor(alpha));
    return pal;
}

const QPalette ThemeSupport::getNotSoDarkPalette(int alpha)
{
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, getThemeAlternateColor(alpha));
    pal.setColor(QPalette::Base, getThemeBaseColor(alpha));
    pal.setColor(QPalette::Button, getThemeBaseColor(alpha));
    return pal;
}

const QString ThemeSupport::getThemeStyle(int iconSize)
{
    QString css;
    QFile stylesheet(QString::fromUtf8(":/styles/friction.qss"));
    if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text)) {
        css = stylesheet.readAll();
        stylesheet.close();
    }
    return css.arg(getThemeButtonBaseColor().name(),
                   getThemeButtonBorderColor().name(),
                   getThemeBaseDarkerColor().name(),
                   getThemeHighlightColor().name(),
                   getThemeBaseColor().name(),
                   getThemeAlternateColor().name(),
                   QString::number(iconSize));
}

void ThemeSupport::setupTheme(const int iconSize)
{
    QIcon::setThemeSearchPaths(QStringList() << QString::fromUtf8(":/icons"));
    QIcon::setThemeName(QString::fromUtf8("hicolor"));
    qApp->setStyle(QString::fromUtf8("fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, getThemeAlternateColor());
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, getThemeBaseColor());
    palette.setColor(QPalette::AlternateBase, getThemeAlternateColor());
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::ToolTipBase, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, getThemeBaseColor());
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::white);
    palette.setColor(QPalette::Highlight, getThemeHighlightColor());
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setPalette(palette);
    qApp->setStyleSheet(getThemeStyle(iconSize));
}
