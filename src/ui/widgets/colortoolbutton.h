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
#include "widgets/colorsettingswidget.h"

#include <QWidgetAction>
#include <QPushButton>

namespace Friction
{
    namespace Ui
    {
        class UI_EXPORT ColorToolButton : public ToolButton
        {
        public:
            ColorToolButton(QWidget * const parent = nullptr,
                            const bool flatOnly = false);
            ColorToolButton(ColorAnimator * const colorTarget,
                            QWidget * const parent = nullptr);
            void setColorFillTarget(FillSettingsAnimator * const target);
            void setColorStrokeTarget(OutlineSettingsAnimator * const target);
            void setColorTarget(ColorAnimator * const target);
            void setColorType(const PaintType &type);
            void updateColorTypeWidgets(const PaintType &type);
            void updateColor();
            QColor color() const;

        private:
            bool mIsFlatOnly;
            QColor mColor;
            ColorLabel *mColorLabel;
            QPushButton *mColorNoneButton;
            QPushButton *mColorFlatButton;
            QPushButton *mColorGradientButton;
            ColorSettingsWidget *mColorWidget;
            QWidgetAction *mColorAct;
            ConnContextQPtr<ColorAnimator> mColorTarget;
            ConnContextQPtr<FillSettingsAnimator> mColorFillTarget;
            ConnContextQPtr<OutlineSettingsAnimator> mColorStrokeTarget;
        };
    }
}

#endif // FRICTION_COLOR_TOOLBUTTON_H
