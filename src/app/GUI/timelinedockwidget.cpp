/*
#
# Friction - https://friction2d.com
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

#include "timelinedockwidget.h"

#include <QKeyEvent>
#include <QScrollBar>

#include "Private/document.h"
#include "GUI/global.h"
#include "GUI/BoxesList/boxscrollwidget.h"
#include "GUI/BoxesList/boxsinglewidget.h"

#include "mainwindow.h"
#include "canvaswindow.h"
#include "canvas.h"
#include "animationdockwidget.h"
#include "widgetstack.h"
#include "actionbutton.h"
#include "timelinewidget.h"
#include "animationwidgetscrollbar.h"
#include "renderinstancesettings.h"
#include "layouthandler.h"
#include "memoryhandler.h"
#include "switchbutton.h"
#include "editablecombobox.h"
#include "appsupport.h"

TimelineDockWidget::TimelineDockWidget(Document& document,
                                       LayoutHandler * const layoutH,
                                       MainWindow * const parent,
                                       FontsWidget *fontwidget)
    : QWidget(parent)
    , mDocument(document)
    , mMainWindow(parent)
    , mTimelineLayout(layoutH->timelineLayout())
    , mFontWidget(fontwidget)
{
    connect(RenderHandler::sInstance, &RenderHandler::previewFinished,
            this, &TimelineDockWidget::previewFinished);
    connect(RenderHandler::sInstance, &RenderHandler::previewBeingPlayed,
            this, &TimelineDockWidget::previewBeingPlayed);
    connect(RenderHandler::sInstance, &RenderHandler::previewBeingRendered,
            this, &TimelineDockWidget::previewBeingRendered);
    connect(RenderHandler::sInstance, &RenderHandler::previewPaused,
            this, &TimelineDockWidget::previewPaused);

    connect(Document::sInstance, &Document::canvasModeSet,
            this, &TimelineDockWidget::updateButtonsVisibility);

    setFocusPolicy(Qt::NoFocus);

    mMainLayout = new QVBoxLayout(this);
    setLayout(mMainLayout);
    mMainLayout->setSpacing(0);
    mMainLayout->setMargin(0);

    mResolutionComboBox = new EditableComboBox(this);
    mResolutionComboBox->addItem("100 %");
    mResolutionComboBox->addItem("75 %");
    mResolutionComboBox->addItem("50 %");
    mResolutionComboBox->addItem("25 %");
    mResolutionComboBox->lineEdit()->setInputMask("D00 %");
    mResolutionComboBox->setCurrentText("100 %");
    MainWindow::sGetInstance()->installNumericFilter(mResolutionComboBox);
    mResolutionComboBox->setInsertPolicy(QComboBox::NoInsert);
    mResolutionComboBox->setSizePolicy(QSizePolicy::Maximum,
                                       QSizePolicy::Maximum);
    connect(mResolutionComboBox, &QComboBox::currentTextChanged,
            this, &TimelineDockWidget::setResolutionText);

    const QSize iconSize(AppSupport::getSettings("ui",
                                                 "timelineToolbarIconSize",
                                                 QSize(24, 24)).toSize());
    const QString iconsDir = eSettings::sIconsDir() + "/toolbarButtons";

    mPlayFromBeginningButton = new QAction(QIcon::fromTheme("preview"),
                                           tr("Play From the Beginning"),
                                           this);
    connect(mPlayFromBeginningButton, &QAction::triggered,
            this, [this]() {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return; }
        scene->anim_setAbsFrame(scene->getFrameRange().fMin);
        renderPreview();
    });

    mPlayButton = new QAction(QIcon::fromTheme("play"),
                              tr("Render Preview"),
                              this);

    mStopButton = new QAction(QIcon::fromTheme("stop"),
                              tr("Stop Preview"),
                              this);

    connect(mStopButton, &QAction::triggered,
            this, &TimelineDockWidget::interruptPreview);

    mLoopButton = new QAction(QIcon::fromTheme("loop"),
                              tr("Loop"),
                              this);
    mLoopButton->setCheckable(true);
    connect(mLoopButton, &QAction::triggered,
            this, &TimelineDockWidget::setLoop);

    mLocalPivot = SwitchButton::sCreate2Switch(
                "toolbarButtons/pivotGlobal.png", "toolbarButtons/pivotLocal.png",
                gSingleLineTooltip("Pivot Global / Local", "P"), this);
    mLocalPivot->setState(mDocument.fLocalPivot);
    connect(mLocalPivot, &SwitchButton::toggled,
            this, &TimelineDockWidget::setLocalPivot);

    mNodeVisibility = new SwitchButton(
                gSingleLineTooltip("Node visibility", "N"), this);
    mNodeVisibility->addState("toolbarButtons/dissolvedAndNormalNodes.png");
    mNodeVisibility->addState("toolbarButtons/dissolvedNodesOnly.png");
    mNodeVisibility->addState("toolbarButtons/normalNodesOnly.png");
    connect(mNodeVisibility, &SwitchButton::toggled,
            this, [this](const int state) {
        mDocument.fNodeVisibility = static_cast<NodeVisiblity>(state);
        Document::sInstance->actionFinished();
    });

    mToolBar = new QToolBar(this);
    mToolBar->setMovable(false);

    mToolBar->setIconSize(iconSize);
    mToolBar->addSeparator();

//    mControlButtonsLayout->addWidget(mGoToPreviousKeyButton);
//    mGoToPreviousKeyButton->setFocusPolicy(Qt::NoFocus);
//    mControlButtonsLayout->addWidget(mGoToNextKeyButton);
//    mGoToNextKeyButton->setFocusPolicy(Qt::NoFocus);
    QAction *resA = mToolBar->addAction(tr("Resolution:"));
    resA->setToolTip(tr("Preview resolution"));
    mToolBar->widgetForAction(resA)->setObjectName("inactiveToolButton");

    mToolBar->addWidget(mResolutionComboBox);

    //mResolutionComboBox->setFocusPolicy(Qt::NoFocus);

    mToolBar->addAction(mPlayFromBeginningButton);
    mToolBar->addAction(mPlayButton);
    mToolBar->addAction(mStopButton);
    //mToolBar->addSeparator();
    mToolBar->addAction(mLoopButton);

    mLocalPivotAct = mToolBar->addWidget(mLocalPivot);
    mNodeVisibilityAct = mToolBar->addWidget(mNodeVisibility);

    setupDrawPathSpins();

    QWidget *spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Minimum);
    mToolBar->addWidget(spacerWidget);

    if (mFontWidget) {
        QLabel *fontLabel = new QLabel(tr("Font"), this);
        mToolBar->addWidget(fontLabel);
        mToolBar->addWidget(mFontWidget);
        mToolBar->addSeparator();
    }

    mMainLayout->addWidget(mToolBar);

    mPlayFromBeginningButton->setEnabled(false);
    mPlayButton->setEnabled(false);
    mStopButton->setEnabled(false);

    connect(&mDocument, &Document::activeSceneSet,
            this, [this](Canvas* const scene) {
        mPlayFromBeginningButton->setEnabled(scene);
        mPlayButton->setEnabled(scene);
        mStopButton->setEnabled(scene);
    });

    mMainLayout->addWidget(mTimelineLayout);

    previewFinished();
    //addNewBoxesListKeysViewWidget(1);
    //addNewBoxesListKeysViewWidget(0);

    connect(&mDocument, &Document::activeSceneSet,
            this, &TimelineDockWidget::updateSettingsForCurrentCanvas);

}

QAction* addSlider(const QString& name,
                   QDoubleSlider* const slider,
                   QToolBar* const toolBar)
{
    const auto widget = new QWidget;
    widget->setContentsMargins(5, 0, 5, 0);
    const auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);
    const auto label = new QLabel(name/* + ": "*/);

    layout->addWidget(label);
    layout->addWidget(slider);
    return toolBar->addWidget(widget);
}

void TimelineDockWidget::setupDrawPathSpins()
{
    mDrawPathAuto = SwitchButton::sCreate2Switch(
                "toolbarButtons/drawPathAutoUnchecked.png",
                "toolbarButtons/drawPathAutoChecked.png",
                gSingleLineTooltip("Manual/Automatic Fitting"), this);
    mDrawPathAuto->toggle();
    connect(mDrawPathAuto, &SwitchButton::toggled,
            &mDocument, [this](const int i) {
        mDocument.fDrawPathManual = i == 0;
        mDrawPathMaxErrorAct->setDisabled(i == 0);
    });
    mDrawPathAutoAct = mToolBar->addWidget(mDrawPathAuto);

    mDrawPathMaxError = new QDoubleSlider(1, 200, 1, this);
    mDrawPathMaxError->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mDrawPathMaxError->setNumberDecimals(0);
    mDrawPathMaxError->setDisplayedValue(mDocument.fDrawPathMaxError);
    connect(mDrawPathMaxError, &QDoubleSlider::valueEdited,
            this, [this](const qreal value) {
        mDocument.fDrawPathMaxError = qFloor(value);
    });
    mDrawPathMaxErrorAct = addSlider(tr("Max Error"), mDrawPathMaxError, mToolBar);

    mDrawPathSmooth = new QDoubleSlider(1, 200, 1, this);
    mDrawPathSmooth->setNumberDecimals(0);
    mDrawPathSmooth->setDisplayedValue(mDocument.fDrawPathSmooth);
    connect(mDrawPathSmooth, &QDoubleSlider::valueEdited,
            this, [this](const qreal value) {
        mDocument.fDrawPathSmooth = qFloor(value);
    });
    mDrawPathSmoothAct = addSlider(tr("Smooth"), mDrawPathSmooth, mToolBar);
}

void TimelineDockWidget::setResolutionText(QString text)
{
    text = text.remove(" %");
    const qreal res = clamp(text.toDouble(), 1, 200)/100;
    mMainWindow->setResolutionValue(res);
}

void TimelineDockWidget::setLoop(const bool loop)
{
    RenderHandler::sInstance->setLoop(loop);
}

bool TimelineDockWidget::processKeyPress(QKeyEvent *event)
{
    const CanvasMode mode = mDocument.fCanvasMode;
    const int key = event->key();
    const auto mods = event->modifiers();
    if (key == Qt::Key_Escape) {
        const auto state = RenderHandler::sInstance->currentPreviewState();
        if (state == PreviewSate::stopped) { return false; }
        interruptPreview();
    } else if (key == Qt::Key_Space) {
        const auto state = RenderHandler::sInstance->currentPreviewState();
        switch(state) {
            case PreviewSate::stopped: renderPreview(); break;
            case PreviewSate::rendering: playPreview(); break;
            case PreviewSate::playing: pausePreview(); break;
            case PreviewSate::paused: resumePreview(); break;
        }
    } else if (key == Qt::Key_Right && !(mods & Qt::ControlModifier)) {
        mDocument.incActiveSceneFrame();
    } else if (key == Qt::Key_Left && !(mods & Qt::ControlModifier)) {
        mDocument.decActiveSceneFrame();
    } else if (key == Qt::Key_Down && !(mods & Qt::ControlModifier)) {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return false; }
        int targetFrame;
        const int frame = mDocument.getActiveSceneFrame();
        if (scene->anim_prevRelFrameWithKey(frame, targetFrame)) {
            mDocument.setActiveSceneFrame(targetFrame);
        }
    } else if (key == Qt::Key_Up && !(mods & Qt::ControlModifier)) {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return false; }
        int targetFrame;
        const int frame = mDocument.getActiveSceneFrame();
        if (scene->anim_nextRelFrameWithKey(frame, targetFrame)) {
            mDocument.setActiveSceneFrame(targetFrame);
        }
    } else if (key == Qt::Key_P &&
               !(mods & Qt::ControlModifier) && !(mods & Qt::AltModifier)) {
        mLocalPivot->toggle();
    } else if (mode == CanvasMode::pointTransform && key == Qt::Key_N &&
               !(mods & Qt::ControlModifier) && !(mods & Qt::AltModifier)) {
        mNodeVisibility->toggle();
    } else {
        return false;
    }
    return true;
}

void TimelineDockWidget::previewFinished()
{
    //setPlaying(false);
    mPlayFromBeginningButton->setDisabled(false);
    mStopButton->setDisabled(true);
    mPlayButton->setIcon(QIcon::fromTheme("play"));
    mPlayButton->setText(tr("Render Preview"));
    disconnect(mPlayButton, nullptr, this, nullptr);
    connect(mPlayButton, &QAction::triggered,
            this, &TimelineDockWidget::renderPreview);
}

void TimelineDockWidget::previewBeingPlayed()
{
    mPlayFromBeginningButton->setDisabled(true);
    mStopButton->setDisabled(false);
    mPlayButton->setIcon(QIcon::fromTheme("pause"));
    mPlayButton->setText(tr("Pause Preview"));
    disconnect(mPlayButton, nullptr, this, nullptr);
    connect(mPlayButton, &QAction::triggered,
            this, &TimelineDockWidget::pausePreview);
}

void TimelineDockWidget::previewBeingRendered()
{
    mPlayFromBeginningButton->setDisabled(true);
    mStopButton->setDisabled(false);
    mPlayButton->setIcon(QIcon::fromTheme("play"));
    mPlayButton->setText(tr("Play Preview"));
    disconnect(mPlayButton, nullptr, this, nullptr);
    connect(mPlayButton, &QAction::triggered,
            this, &TimelineDockWidget::playPreview);
}

void TimelineDockWidget::previewPaused()
{
    mPlayFromBeginningButton->setDisabled(true);
    mStopButton->setDisabled(false);
    mPlayButton->setIcon(QIcon::fromTheme("play"));
    mPlayButton->setText(tr("Resume Preview"));
    disconnect(mPlayButton, nullptr, this, nullptr);
    connect(mPlayButton, &QAction::triggered,
            this, &TimelineDockWidget::resumePreview);
}

void TimelineDockWidget::resumePreview()
{
    RenderHandler::sInstance->resumePreview();
}

void TimelineDockWidget::updateButtonsVisibility(const CanvasMode mode)
{
    mLocalPivotAct->setVisible(mode == CanvasMode::pointTransform ||
                               mode == CanvasMode::boxTransform);
    mNodeVisibilityAct->setVisible(mode == CanvasMode::pointTransform);

    const bool drawPathMode = mode == CanvasMode::drawPath;
    mDrawPathAutoAct->setVisible(drawPathMode);
    mDrawPathMaxErrorAct->setVisible(drawPathMode);
    mDrawPathSmoothAct->setVisible(drawPathMode);
}

void TimelineDockWidget::pausePreview()
{
    RenderHandler::sInstance->pausePreview();
}

void TimelineDockWidget::playPreview()
{
    RenderHandler::sInstance->playPreview();
}

void TimelineDockWidget::renderPreview()
{
    RenderHandler::sInstance->renderPreview();
}

void TimelineDockWidget::interruptPreview()
{
    RenderHandler::sInstance->interruptPreview();
}

void TimelineDockWidget::setLocalPivot(const bool local)
{
    mDocument.fLocalPivot = local;
    for (const auto& scene : mDocument.fScenes) { scene->updatePivot(); }
    Document::sInstance->actionFinished();
}

void TimelineDockWidget::updateSettingsForCurrentCanvas(Canvas* const canvas)
{
    if (!canvas) { return; }
    disconnect(mResolutionComboBox, &QComboBox::currentTextChanged,
               this, &TimelineDockWidget::setResolutionText);
    mResolutionComboBox->setCurrentText(QString::number(canvas->getResolution()*100) + " %");
    connect(mResolutionComboBox, &QComboBox::currentTextChanged,
            this, &TimelineDockWidget::setResolutionText);
}
