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
#include "appsupport.h"

TimelineDockWidget::TimelineDockWidget(Document& document,
                                       LayoutHandler * const layoutH,
                                       MainWindow * const parent)
    : QWidget(parent)
    , mDocument(document)
    , mMainWindow(parent)
    , mTimelineLayout(layoutH->timelineLayout())
    , mToolBar(nullptr)
    , mFrictionButton(nullptr)
    , mFrameStartSpin(nullptr)
    , mFrameEndSpin(nullptr)
    //, mNodeVisibilityAct(nullptr)
    //, mNodeVisibility(nullptr)
    , mFrameRewindAct(nullptr)
    , mFrameFastForwardAct(nullptr)
    , mCurrentFrameSpinAct(nullptr)
    , mCurrentFrameSpin(nullptr)
    , mRenderProgressAct(nullptr)
    , mRenderProgress(nullptr)
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

    const QSize iconSize(AppSupport::getSettings("ui",
                                                 "timelineToolbarIconSize",
                                                 QSize(24, 24)).toSize());

    mFrameRewindAct = new QAction(QIcon::fromTheme("rewind"),
                                  tr("Rewind"),
                                  this);
    mFrameRewindAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                      "rewind",
                                                                      "Shift+Left").toString()));
    connect(mFrameRewindAct, &QAction::triggered,
            this, [this]() {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return; }
        scene->anim_setAbsFrame(scene->getFrameRange().fMin);
        mDocument.actionFinished();
    });

    mFrameFastForwardAct = new QAction(QIcon::fromTheme("fastforward"),
                                       tr("Fast Forward"),
                                       this);
    mFrameFastForwardAct->setShortcut(QKeySequence(AppSupport::getSettings("shortcuts",
                                                                           "fastForward",
                                                                           "Shift+Right").toString()));
    connect(mFrameFastForwardAct, &QAction::triggered,
            this, [this]() {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return; }
        scene->anim_setAbsFrame(scene->getFrameRange().fMax);
        mDocument.actionFinished();
    });

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

    /*mLocalPivotAct = new QAction(mDocument.fLocalPivot ? QIcon::fromTheme("pivotLocal") : QIcon::fromTheme("pivotGlobal"),
                                 tr("Pivot Global / Local"),
                                 this);
    connect(mLocalPivotAct, &QAction::triggered,
            this, [this]() {
        TimelineDockWidget::setLocalPivot(!mDocument.fLocalPivot);
        mLocalPivotAct->setIcon(mDocument.fLocalPivot ? QIcon::fromTheme("pivotLocal") : QIcon::fromTheme("pivotGlobal"));
    });*/

    mFrictionButton = new QToolButton(this);
    mFrictionButton->setObjectName(QString::fromUtf8("ToolButton"));
    mFrictionButton->setPopupMode(QToolButton::InstantPopup);
    mFrictionButton->setIcon(QIcon::fromTheme("friction"));
    mFrictionButton->setFocusPolicy(Qt::NoFocus);

    /*mNodeVisibility = new QToolButton(this);
    mNodeVisibility->setObjectName(QString::fromUtf8("ToolButton"));
    mNodeVisibility->setPopupMode(QToolButton::InstantPopup);
    QAction *nodeVisibilityAction1 = new QAction(QIcon::fromTheme("dissolvedAndNormalNodes"),
                                                 tr("Dissolved and normal nodes"),
                                                 this);
    nodeVisibilityAction1->setData(0);
    QAction *nodeVisibilityAction2 = new QAction(QIcon::fromTheme("dissolvedNodesOnly"),
                                                 tr("Dissolved nodes only"),
                                                 this);
    nodeVisibilityAction2->setData(1);
    QAction *nodeVisibilityAction3 = new QAction(QIcon::fromTheme("normalNodesOnly"),
                                                 tr("Normal nodes only"),
                                                 this);
    nodeVisibilityAction3->setData(2);
    mNodeVisibility->addAction(nodeVisibilityAction1);
    mNodeVisibility->addAction(nodeVisibilityAction2);
    mNodeVisibility->addAction(nodeVisibilityAction3);
    mNodeVisibility->setDefaultAction(nodeVisibilityAction1);
    connect(mNodeVisibility, &QToolButton::triggered,
            this, [this](QAction *act) {
            qDebug() << "set node visibility" << act->data().toInt();
            mNodeVisibility->setDefaultAction(act);
            mDocument.fNodeVisibility = static_cast<NodeVisiblity>(act->data().toInt());
            Document::sInstance->actionFinished();
    });*/

    mFrameStartSpin = new QSpinBox(this);
    mFrameStartSpin->setAlignment(Qt::AlignHCenter);
    mFrameStartSpin->setFocusPolicy(Qt::ClickFocus);
    mFrameStartSpin->setToolTip(tr("Scene frame start"));
    mFrameStartSpin->setRange(0, INT_MAX);
    connect(mFrameStartSpin,
            &QSpinBox::editingFinished,
            this, [this]() {
            const auto scene = *mDocument.fActiveScene;
            if (!scene) { return; }
            auto range = scene->getFrameRange();
            int frame = mFrameStartSpin->value();
            if (range.fMin == frame) { return; }
            if (frame >= range.fMax) {
                mFrameStartSpin->setValue(range.fMin);
                return;
            }
            range.fMin = frame;
            scene->setFrameRange(range);
    });

    mFrameEndSpin = new QSpinBox(this);
    mFrameEndSpin->setAlignment(Qt::AlignHCenter);
    mFrameEndSpin->setFocusPolicy(Qt::ClickFocus);
    mFrameEndSpin->setToolTip(tr("Scene frame end"));
    mFrameEndSpin->setRange(1, INT_MAX);
    connect(mFrameEndSpin,
            &QSpinBox::editingFinished,
            this, [this]() {
            const auto scene = *mDocument.fActiveScene;
            if (!scene) { return; }
            auto range = scene->getFrameRange();
            int frame = mFrameEndSpin->value();
            if (range.fMax == frame) { return; }
            if (frame <= range.fMin) {
                mFrameEndSpin->setValue(range.fMax);
                return;
            }
            range.fMax = frame;
            scene->setFrameRange(range);
    });

    mCurrentFrameSpin = new QSpinBox(this);
    mCurrentFrameSpin->setAlignment(Qt::AlignHCenter);
    mCurrentFrameSpin->setObjectName(QString::fromUtf8("SpinBoxNoButtons"));
    mCurrentFrameSpin->setFocusPolicy(Qt::ClickFocus);
    mCurrentFrameSpin->setToolTip(tr("Current frame"));
    mCurrentFrameSpin->setRange(-INT_MAX, INT_MAX);
    connect(mCurrentFrameSpin,
            &QSpinBox::editingFinished,
            this, [this]() {
        const auto scene = *mDocument.fActiveScene;
        if (!scene) { return; }
        scene->anim_setAbsFrame(mCurrentFrameSpin->value());
        mDocument.actionFinished();
    });

    mToolBar = new QToolBar(this);
    mToolBar->setMovable(false);

    mToolBar->setIconSize(iconSize);

//    mControlButtonsLayout->addWidget(mGoToPreviousKeyButton);
//    mGoToPreviousKeyButton->setFocusPolicy(Qt::NoFocus);
//    mControlButtonsLayout->addWidget(mGoToNextKeyButton);
//    mGoToNextKeyButton->setFocusPolicy(Qt::NoFocus);

    mRenderProgress = new QProgressBar(this);
    mRenderProgress->setFormat(tr("Render"));

    mToolBar->addWidget(mFrictionButton);
    mToolBar->addWidget(mFrameStartSpin);

    QWidget *spacerWidget1 = new QWidget(this);
    spacerWidget1->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Minimum);
    mToolBar->addWidget(spacerWidget1);

    mToolBar->addAction(mFrameRewindAct);
    mToolBar->addAction(mPlayFromBeginningButton);
    mToolBar->addAction(mFrameFastForwardAct);
    mRenderProgressAct = mToolBar->addWidget(mRenderProgress);
    mCurrentFrameSpinAct = mToolBar->addWidget(mCurrentFrameSpin);
    mToolBar->addAction(mPlayButton);
    mToolBar->addAction(mStopButton);
    mToolBar->addAction(mLoopButton);
    //mToolBar->addAction(mLocalPivotAct);
    //mNodeVisibilityAct = mToolBar->addWidget(mNodeVisibility);

    mRenderProgressAct->setVisible(false);

    mToolBar->addAction(QIcon::fromTheme("render_animation"),
                        tr("Add to Render Queue"), this, [] {
        MainWindow::sGetInstance()->addCanvasToRenderQue();
    });

    setupDrawPathSpins();

    QWidget *spacerWidget2 = new QWidget(this);
    spacerWidget2->setSizePolicy(QSizePolicy::Expanding,
                                 QSizePolicy::Minimum);
    mToolBar->addWidget(spacerWidget2);

    mToolBar->addWidget(mFrameEndSpin);

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
    mDocument.fDrawPathManual = false;
    mDrawPathAuto = new QAction(mDocument.fDrawPathManual ? QIcon::fromTheme("drawPathAutoUnchecked") : QIcon::fromTheme("drawPathAutoChecked"),
                                tr("Automatic/Manual Fitting"),
                                this);
    connect(mDrawPathAuto, &QAction::triggered,
            this, [this]() {
        mDocument.fDrawPathManual = !mDocument.fDrawPathManual;
        qDebug() << "manual fitting?" << mDocument.fDrawPathManual;
        mDrawPathMaxErrorAct->setDisabled(mDocument.fDrawPathManual);
        mDrawPathAuto->setIcon(mDocument.fDrawPathManual ? QIcon::fromTheme("drawPathAutoUnchecked") : QIcon::fromTheme("drawPathAutoChecked"));
    });
    mToolBar->addAction(mDrawPathAuto);

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

void TimelineDockWidget::updateFrameRange(const FrameRange &range)
{
    mRenderProgress->setRange(range.fMin, range.fMax);
    if (range.fMin != mFrameStartSpin->value()) {
        mFrameStartSpin->blockSignals(true);
        mFrameStartSpin->setValue(range.fMin);
        mFrameStartSpin->blockSignals(false);
    }
    if (range.fMax != mFrameEndSpin->value()) {
        mFrameEndSpin->blockSignals(true);
        mFrameEndSpin->setValue(range.fMax);
        mFrameEndSpin->blockSignals(false);
    }
}

void TimelineDockWidget::handleCurrentFrameChanged(int frame)
{
    mCurrentFrameSpin->blockSignals(true);
    mCurrentFrameSpin->setValue(frame);
    mCurrentFrameSpin->blockSignals(false);
    if (mRenderProgress->isVisible()) { mRenderProgress->setValue(frame); }
}

void TimelineDockWidget::showRenderStatus(bool show)
{
    if (!show) { mRenderProgress->setValue(0); }
    mCurrentFrameSpinAct->setVisible(!show);
    mRenderProgressAct->setVisible(show);
}

void TimelineDockWidget::setLoop(const bool loop)
{
    RenderHandler::sInstance->setLoop(loop);
}

bool TimelineDockWidget::processKeyPress(QKeyEvent *event)
{
    //const CanvasMode mode = mDocument.fCanvasMode;
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
    } /*else if (key == Qt::Key_P &&
               !(mods & Qt::ControlModifier) && !(mods & Qt::AltModifier)) {
        mLocalPivotAct->trigger();
    } else if (mode == CanvasMode::pointTransform && key == Qt::Key_N &&
               !(mods & Qt::ControlModifier) && !(mods & Qt::AltModifier)) {
        mNodeVisibility->toggle();
    }*/ else {
        return false;
    }
    return true;
}

void TimelineDockWidget::previewFinished()
{
    //setPlaying(false);
    showRenderStatus(false);
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
    showRenderStatus(false);
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
    showRenderStatus(true);
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
    showRenderStatus(false);
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
    /*if (mLocalPivotAct) {
        mLocalPivotAct->setVisible(mode == CanvasMode::pointTransform ||
                                   mode == CanvasMode::boxTransform);
    }*/
    /*if (mNodeVisibilityAct) {
        mNodeVisibilityAct->setVisible(mode == CanvasMode::pointTransform);
    }*/

    const bool drawPathMode = mode == CanvasMode::drawPath;

    if (mDrawPathAuto) { mDrawPathAuto->setVisible(drawPathMode); }
    if (mDrawPathMaxErrorAct) { mDrawPathMaxErrorAct->setVisible(drawPathMode); }
    if (mDrawPathSmoothAct) { mDrawPathSmoothAct->setVisible(drawPathMode); }
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

/*void TimelineDockWidget::setLocalPivot(const bool local)
{
    mDocument.fLocalPivot = local;
    for (const auto& scene : mDocument.fScenes) { scene->updatePivot(); }
    Document::sInstance->actionFinished();
}*/

void TimelineDockWidget::updateSettingsForCurrentCanvas(Canvas* const canvas)
{
    if (!canvas) { return; }

    const auto range = canvas->getFrameRange();
    updateFrameRange(range);
    handleCurrentFrameChanged(canvas->anim_getCurrentAbsFrame());

    connect(canvas,
            &Canvas::newFrameRange,
            this, [this](const FrameRange range) {
            updateFrameRange(range);
    });
    connect(canvas, &Canvas::currentFrameChanged,
            this, &TimelineDockWidget::handleCurrentFrameChanged);
}

void TimelineDockWidget::setActions(const QList<QAction *> actions)
{
    mFrictionButton->addActions(actions);
}
