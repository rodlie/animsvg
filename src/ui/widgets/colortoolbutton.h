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

#ifndef FRICTION_COLOR_TOOLBUTTON_H
#define FRICTION_COLOR_TOOLBUTTON_H

#include "ui_global.h"

#include "Private/document.h"
#include "Animators/coloranimator.h"
#include "widgets/colorlabel.h"
#include "conncontextptr.h"
#include "widgets/colorsettingswidget.h"
#include "widgets/fillstrokesettings.h"

#include <QFrame>
#include <QToolButton>
#include <QWidgetAction>
#include <QPushButton>
#include <QScrollArea>
#include <QMouseEvent>
#include <QWheelEvent>

namespace Friction
{
    namespace Ui
    {
        class UI_EXPORT ColorToolButton : public QToolButton
        {
            Q_OBJECT
        public:
            ColorToolButton(Document& document,
                            QWidget *parent = nullptr,
                            const bool fillOnly = false,
                            const bool strokeOnly = false,
                            const bool flatOnly = false);
            void setCurrentBox(BoundingBox *target);
            void setColorFillTarget(FillSettingsAnimator * const target);
            void setColorStrokeTarget(OutlineSettingsAnimator * const target);
            void setColorTarget(ColorAnimator * const target);
            void updateColor();
            QColor color() const;

        signals:
            void message(const QString &msg);

        protected:
            void mousePressEvent(QMouseEvent *e) override;
            void wheelEvent(QWheelEvent *e) override;

        private:
            bool mIsFillOnly;
            bool mIsStrokeOnly;
            bool mIsFlatOnly;
            QFrame *mPop;
            QScrollArea *mScroll;
            QColor mColor;
            ColorLabel *mColorLabel;
            ColorSettingsWidget *mBackgroundWidget;
            FillStrokeSettingsWidget *mFillStrokeWidget;
            QWidgetAction *mColorAct;
            Document& mDocument;
            ConnContextQPtr<ColorAnimator> mColorTarget;
            ConnContextQPtr<FillSettingsAnimator> mColorFillTarget;
            ConnContextQPtr<OutlineSettingsAnimator> mColorStrokeTarget;
        };
    }
}

#endif // FRICTION_COLOR_TOOLBUTTON_H
