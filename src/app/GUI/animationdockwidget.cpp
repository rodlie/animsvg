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

#include "animationdockwidget.h"

#include "GUI/global.h"
#include "keysview.h"
#include "widgets/actionbutton.h"
#include "Private/esettings.h"

AnimationDockWidget::AnimationDockWidget(QWidget *parent,
                                         KeysView *keysView)
    : QToolBar(parent)
{
    setObjectName(QString::fromUtf8("animationDockWidget"));
    setSizePolicy(QSizePolicy::Maximum,
                  QSizePolicy::Maximum);

    const QString iconsDir = eSettings::sIconsDir() + "/toolbarButtons";

    QAction *mLineButton = new QAction(QIcon::fromTheme("segmentLine"),
                                       tr("Make Segment Line"), this);
    connect(mLineButton, &QAction::triggered,
            keysView, &KeysView::graphMakeSegmentsLinearAction);

    QAction *mCurveButton = new QAction(QIcon::fromTheme("segmentCurve"),
                                        tr("Make Segment Curve"), this);
    connect(mCurveButton, &QAction::triggered,
            keysView, qOverload<>(&KeysView::graphMakeSegmentsSmoothAction));

    QAction *mSymmetricButton = new QAction(QIcon::fromTheme("nodeSymmetric"),
                                            tr("Symmetric Nodes"), this);
    connect(mSymmetricButton, &QAction::triggered,
            keysView, &KeysView::graphSetSymmetricCtrlAction);

    QAction *mSmoothButton = new QAction(QIcon::fromTheme("nodeSmooth"),
                                         tr("Smooth Nodes"), this);
    connect(mSmoothButton, &QAction::triggered,
            keysView, &KeysView::graphSetSmoothCtrlAction);

    QAction *mCornerButton = new QAction(QIcon::fromTheme("nodeCorner"),
                                         tr("Corner Nodes"), this);
    connect(mCornerButton, &QAction::triggered,
            keysView, &KeysView::graphSetCornerCtrlAction);

    QAction *mFitToHeightButton = new QAction(QIcon::fromTheme("zoom"),
                                              tr("Fit Vertical"), this);
    connect(mFitToHeightButton, &QAction::triggered,
            keysView, &KeysView::graphResetValueScaleAndMinShownAction);

    /*const auto valueLines = SwitchButton::sCreate2Switch(
                                "toolbarButtons/horizontalLinesChecked.png",
                                "toolbarButtons/horizontalLinesUnchecked.png",
                                gSingleLineTooltip("Show/Hide Value Lines"),
                                this);
    connect(valueLines, &SwitchButton::toggled,
            keysView, &KeysView::graphSetValueLinesDisabled);*/

    /*const auto selectedVisible = SwitchButton::sCreate2Switch(
                                     "toolbarButtons/onlySelectedVisibleUnchecked.png",
                                     "toolbarButtons/onlySelectedVisibleChecked.png",
                                     gSingleLineTooltip("View Only Selected Objects' Properties"),
                                     this);
    connect(selectedVisible, &SwitchButton::toggled,
            keysView, &KeysView::graphSetOnlySelectedVisible);*/

    QAction *onlySelectedAct = new QAction(QIcon::fromTheme("onlySelectedVisible"),
                                           QString(),
                                           this);
    onlySelectedAct->setCheckable(true);
    onlySelectedAct->setToolTip(tr("View only selected"));
    connect(onlySelectedAct, &QAction::triggered,
            keysView, &KeysView::graphSetOnlySelectedVisible);

    addAction(mLineButton);
    addAction(mCurveButton);
    addAction(mSymmetricButton);
    addAction(mSmoothButton);
    addAction(mCornerButton);
    addAction(mFitToHeightButton);
    //addWidget(valueLines);
    addAction(onlySelectedAct);
}
