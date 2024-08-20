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
#include "GUI/keysview.h"
#include "appsupport.h"

#include <QMenu>

AnimationDockWidget::AnimationDockWidget(QWidget *parent,
                                         KeysView *keysView)
    : QToolBar(parent)
    , mLineButton(nullptr)
    , mCurveButton(nullptr)
    , mSymmetricButton(nullptr)
    , mSmoothButton(nullptr)
    , mCornerButton(nullptr)
    , mFitToHeightButton(nullptr)
    , mOnlySelectedAct(nullptr)
{
    setObjectName(QString::fromUtf8("animationDockWidget"));
    setSizePolicy(QSizePolicy::Maximum,
                  QSizePolicy::Maximum);

    const auto easingButton = new QPushButton(QIcon::fromTheme("easing"),
                                              QString(), this);
    easingButton->setToolTip(tr("Ease between two keyframes"));
    easingButton->setFocusPolicy(Qt::NoFocus);
    generateEasingActions(easingButton, keysView);

    mLineButton = new QAction(QIcon::fromTheme("segmentLine"),
                              tr("Make Segment Line"), this);
    connect(mLineButton, &QAction::triggered,
            keysView, &KeysView::graphMakeSegmentsLinearAction);

    mCurveButton = new QAction(QIcon::fromTheme("segmentCurve"),
                               tr("Make Segment Curve"), this);
    connect(mCurveButton, &QAction::triggered,
            keysView, qOverload<>(&KeysView::graphMakeSegmentsSmoothAction));

    mSymmetricButton = new QAction(QIcon::fromTheme("nodeSymmetric"),
                                   tr("Symmetric Nodes"), this);
    connect(mSymmetricButton, &QAction::triggered,
            keysView, &KeysView::graphSetSymmetricCtrlAction);

    mSmoothButton = new QAction(QIcon::fromTheme("nodeSmooth"),
                                tr("Smooth Nodes"), this);
    connect(mSmoothButton, &QAction::triggered,
            keysView, &KeysView::graphSetSmoothCtrlAction);

    mCornerButton = new QAction(QIcon::fromTheme("nodeCorner"),
                                tr("Corner Nodes"), this);
    connect(mCornerButton, &QAction::triggered,
            keysView, &KeysView::graphSetCornerCtrlAction);

    mFitToHeightButton = new QAction(QIcon::fromTheme("zoom"),
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

    mOnlySelectedAct = new QAction(QIcon::fromTheme("onlySelectedVisible"),
                                           QString(),
                                           this);
    mOnlySelectedAct->setCheckable(true);
    mOnlySelectedAct->setToolTip(tr("View only selected"));
    connect(mOnlySelectedAct, &QAction::triggered,
            keysView, &KeysView::graphSetOnlySelectedVisible);

    addWidget(easingButton);
    addAction(mLineButton);
    addAction(mCurveButton);
    addAction(mSymmetricButton);
    addAction(mSmoothButton);
    addAction(mCornerButton);
    addAction(mFitToHeightButton);
    //addWidget(valueLines);
    addAction(mOnlySelectedAct);
}

void AnimationDockWidget::showGraph(const bool show)
{
    mLineButton->setVisible(show);
    mCurveButton->setVisible(show);
    mSymmetricButton->setVisible(show);
    mSmoothButton->setVisible(show);
    mCornerButton->setVisible(show);
    mFitToHeightButton->setVisible(show);
    mOnlySelectedAct->setVisible(show);
}

void AnimationDockWidget::generateEasingActions(QPushButton *button,
                                                KeysView *keysView)
{
    const auto presets = AppSupport::getEasingPresets();
    const auto menu = new QMenu(this);
    for (const auto &preset : presets) {
        QString title = preset;
        const auto presetAct = new QAction(QIcon::fromTheme("easing"),
                                           title.split("/").takeLast().remove(".js"),
                                           this);
        presetAct->setData(preset);
        menu->addAction(presetAct);
        connect(presetAct, &QAction::triggered,
                this, [presetAct, keysView]() {
            keysView->graphEasingAction(presetAct->data().toString());
        });
    }
    if (!menu->isEmpty()) { button->setMenu(menu); }
}
