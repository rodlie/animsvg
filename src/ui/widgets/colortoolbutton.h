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

#ifndef FRICTION_COLOR_TOOLBUTTON_H
#define FRICTION_COLOR_TOOLBUTTON_H

#include "ui_global.h"

#include "Animators/coloranimator.h"
#include "widgets/colorlabel.h"
#include "widgets/toolbutton.h"
#include "conncontextptr.h"

#include <QWidgetAction>

namespace Friction
{
    namespace Ui
    {
        class UI_EXPORT ColorToolButton : public ToolButton
        {
        public:
            ColorToolButton(QWidget * const parent = nullptr);
            ColorToolButton(ColorAnimator * const colorTarget,
                            QWidget * const parent = nullptr);
            void setColorTarget(ColorAnimator * const target);
            void updateColor();
            QColor color() const;

        private:
            QColor mColor;
            ColorLabel *mColorLabel;
            QWidget *mColorWidget;
            QWidgetAction *mColorAct;
            ConnContextQPtr<ColorAnimator> mColorTarget;
        };
    }
}

#endif // FRICTION_COLOR_TOOLBUTTON_H
