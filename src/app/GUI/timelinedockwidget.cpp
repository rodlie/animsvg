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
    , mFrameStartSpin(nullptr)
    , mFrameEndSpin(nullptr)
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

    mRenderProgress = new QProgressBar(this);
    mRenderProgress->setSizePolicy(QSizePolicy::Preferred,
                                   QSizePolicy::Preferred);
    mRenderProgress->setMinimumWidth(150);
    mRenderProgress->setFormat(tr("Cache %p%"));

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

    mRenderProgressAct->setVisible(false);

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

    connect(&mDocument, &Document::activeSceneSet,
            this, &TimelineDockWidget::updateSettingsForCurrentCanvas);

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
    mCurrentFrameSpin->setValue(frame);
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
    } else {
        return false;
    }
    return true;
}

void TimelineDockWidget::previewFinished()
{
    //setPlaying(false);
    mFrameStartSpin->setEnabled(true);
    mFrameEndSpin->setEnabled(true);
    mCurrentFrameSpinAct->setEnabled(true);
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
    mFrameStartSpin->setEnabled(false);
    mFrameEndSpin->setEnabled(false);
    mCurrentFrameSpinAct->setEnabled(false);
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
    mFrameStartSpin->setEnabled(false);
    mFrameEndSpin->setEnabled(false);
    mCurrentFrameSpinAct->setEnabled(false);
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
    mFrameStartSpin->setEnabled(true);
    mFrameEndSpin->setEnabled(true);
    mCurrentFrameSpinAct->setEnabled(true);
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
    Q_UNUSED(mode)
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
