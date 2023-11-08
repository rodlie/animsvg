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

#include "renderwidget.h"
#include "canvas.h"
#include "GUI/global.h"
#include "renderinstancewidget.h"
#include "GUI/BoxesList/OptimalScrollArea/scrollarea.h"
#include "videoencoder.h"
#include "ReadWrite/basicreadwrite.h"
#include "renderhandler.h"
#include "videoencoder.h"
#include "appsupport.h"
#include "../mainwindow.h"

RenderWidget::RenderWidget(QWidget *parent)
    : QWidget(parent)
    , mMainLayout(nullptr)
    , mRenderProgressBar(nullptr)
    , mStartRenderButton(nullptr)
    , mStopRenderButton(nullptr)
    , mAddRenderButton(nullptr)
    , mContWidget(nullptr)
    , mContLayout(nullptr)
    , mScrollArea(nullptr)
    , mCurrentRenderedSettings(nullptr)
{
    mMainLayout = new QVBoxLayout(this);
    mMainLayout->setMargin(0);
    mMainLayout->setSpacing(0);
    setLayout(mMainLayout);

    QWidget *bottomWidget = new QWidget(this);
    bottomWidget->setContentsMargins(0, 0, 0, 0);
    const auto bottomLayout = new QHBoxLayout(bottomWidget);

    const auto darkPal= AppSupport::getDarkPalette();
    bottomWidget->setAutoFillBackground(true);
    bottomWidget->setPalette(darkPal);

    mRenderProgressBar = new QProgressBar(this);
    //mRenderProgressBar->setObjectName(QString::fromUtf8("RenderProgressBar"));
    setSizePolicy(QSizePolicy::Expanding,
                  QSizePolicy::Expanding);
    mRenderProgressBar->setFormat("%p%");
    mRenderProgressBar->setValue(0);

    mStartRenderButton = new QPushButton(QIcon::fromTheme("play"),
                                         tr("Render"),
                                         this);
    mStartRenderButton->setFocusPolicy(Qt::NoFocus);
    mStartRenderButton->setSizePolicy(QSizePolicy::Preferred,
                                      QSizePolicy::Preferred);
    connect(mStartRenderButton, &QPushButton::pressed,
            this, qOverload<>(&RenderWidget::render));

    mStopRenderButton = new QPushButton(QIcon::fromTheme("cancel"),
                                        QString(),
                                        this);
    mStopRenderButton->setToolTip(tr("Stop Rendering"));
    mStopRenderButton->setFocusPolicy(Qt::NoFocus);
    mStopRenderButton->setSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::Preferred);
    connect(mStopRenderButton, &QPushButton::pressed,
            this, &RenderWidget::stopRendering);

    mAddRenderButton = new QPushButton(QIcon::fromTheme("plus"),
                                       QString(),
                                       this);
    mAddRenderButton->setToolTip(tr("Add current scene to queue"));
    mAddRenderButton->setFocusPolicy(Qt::NoFocus);
    mAddRenderButton->setSizePolicy(QSizePolicy::Preferred,
                                    QSizePolicy::Preferred);
    connect(mAddRenderButton, &QPushButton::pressed,
            this, []() {
        MainWindow::sGetInstance()->addCanvasToRenderQue();
    });

    const auto mClearQueueButton = new QPushButton(QIcon::fromTheme("trash"),
                                                   QString(),
                                                   this);
    mClearQueueButton->setToolTip(tr("Clear Queue"));
    mClearQueueButton->setFocusPolicy(Qt::NoFocus);
    mClearQueueButton->setSizePolicy(QSizePolicy::Preferred,
                                     QSizePolicy::Preferred);
    connect(mClearQueueButton, &QPushButton::pressed,
            this, &RenderWidget::clearRenderQueue);

    mContWidget = new QWidget(this);
    mContWidget->setContentsMargins(0, 0, 0, 0);
    mContLayout = new QVBoxLayout(mContWidget);
    mContLayout->setAlignment(Qt::AlignTop);
    mContLayout->setMargin(0);
    mContLayout->setSpacing(0);
    mContWidget->setLayout(mContLayout);
    mScrollArea = new ScrollArea(this);
    mScrollArea->setWidget(mContWidget);
    mScrollArea->setWidgetResizable(true);
    mScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    bottomLayout->addWidget(mRenderProgressBar);
    bottomLayout->addWidget(mStartRenderButton);
    bottomLayout->addWidget(mStopRenderButton);
    bottomLayout->addWidget(mAddRenderButton);
    bottomLayout->addWidget(mClearQueueButton);

    mMainLayout->addWidget(mScrollArea);
    mMainLayout->addWidget(bottomWidget);

    const auto vidEmitter = VideoEncoder::sInstance->getEmitter();
    connect(vidEmitter, &VideoEncoderEmitter::encodingStarted,
            this, &RenderWidget::leaveOnlyInterruptionButtonsEnabled);

    connect(vidEmitter, &VideoEncoderEmitter::encodingFinished,
            this, &RenderWidget::leaveOnlyStartRenderButtonEnabled);
    connect(vidEmitter, &VideoEncoderEmitter::encodingFinished,
            this, &RenderWidget::sendNextForRender);

    connect(vidEmitter, &VideoEncoderEmitter::encodingInterrupted,
            this, &RenderWidget::clearAwaitingRender);
    connect(vidEmitter, &VideoEncoderEmitter::encodingInterrupted,
            this, &RenderWidget::leaveOnlyStartRenderButtonEnabled);

    connect(vidEmitter, &VideoEncoderEmitter::encodingFailed,
            this, &RenderWidget::leaveOnlyStartRenderButtonEnabled);
    connect(vidEmitter, &VideoEncoderEmitter::encodingFailed,
            this, &RenderWidget::sendNextForRender);

    connect(vidEmitter, &VideoEncoderEmitter::encodingStartFailed,
            this, &RenderWidget::leaveOnlyStartRenderButtonEnabled);
    connect(vidEmitter, &VideoEncoderEmitter::encodingStartFailed,
            this, &RenderWidget::sendNextForRender);
}

void RenderWidget::createNewRenderInstanceWidgetForCanvas(Canvas *canvas)
{
    const auto wid = new RenderInstanceWidget(canvas, this);
    addRenderInstanceWidget(wid);
}

void RenderWidget::addRenderInstanceWidget(RenderInstanceWidget *wid)
{
    mContLayout->addWidget(wid);
    connect(wid, &RenderInstanceWidget::destroyed,
            this, [this, wid]() {
        mRenderInstanceWidgets.removeOne(wid);
        mAwaitingSettings.removeOne(wid);
    });
    connect(wid, &RenderInstanceWidget::duplicate,
            this, [this](RenderInstanceSettings& sett) {
        addRenderInstanceWidget(new RenderInstanceWidget(sett, this));
    });
    mRenderInstanceWidgets << wid;
}

void RenderWidget::setRenderedFrame(const int frame)
{
    if (!mCurrentRenderedSettings) { return; }
    mRenderProgressBar->setValue(frame);
    emit progress(frame, mRenderProgressBar->maximum());
}

void RenderWidget::clearRenderQueue()
{
    if (mStopRenderButton->isEnabled()) { stopRendering(); }
    for (int i = mRenderInstanceWidgets.count() - 1; i >= 0; i--) {
        delete mRenderInstanceWidgets.at(i);
    }
    leaveOnlyStartRenderButtonEnabled();
}

void RenderWidget::write(eWriteStream &dst) const
{
    dst << mRenderInstanceWidgets.count();
    for (const auto widget : mRenderInstanceWidgets) {
        widget->write(dst);
    }
}

void RenderWidget::read(eReadStream &src)
{
    int nWidgets; src >> nWidgets;
    for (int i = 0; i < nWidgets; i++) {
        const auto wid = new RenderInstanceWidget(nullptr, this);
        wid->read(src);
        addRenderInstanceWidget(wid);
    }
}

void RenderWidget::render(RenderInstanceSettings &settings)
{
    const RenderSettings &renderSettings = settings.getRenderSettings();
    mRenderProgressBar->setRange(renderSettings.fMinFrame,
                                 renderSettings.fMaxFrame);
    mRenderProgressBar->setValue(renderSettings.fMinFrame);
    mCurrentRenderedSettings = &settings;
    RenderHandler::sInstance->renderFromSettings(&settings);
    connect(&settings, &RenderInstanceSettings::renderFrameChanged,
            this, &RenderWidget::setRenderedFrame);
    connect(&settings, &RenderInstanceSettings::stateChanged,
            this, [this](const RenderState state) {
        if (state == RenderState::finished) {
            mRenderProgressBar->setValue(mRenderProgressBar->maximum());
            emit rendererFinished();
        }
    });
}

void RenderWidget::leaveOnlyInterruptionButtonsEnabled()
{
    mStartRenderButton->setDisabled(true);
    mStopRenderButton->setEnabled(true);
}

void RenderWidget::leaveOnlyStartRenderButtonEnabled()
{
    mStartRenderButton->setEnabled(true);
    mStopRenderButton->setDisabled(true);
    mRenderProgressBar->setValue(0);
}

void RenderWidget::disableButtons()
{
    mStartRenderButton->setDisabled(true);
    mStopRenderButton->setDisabled(true);
}

void RenderWidget::enableButtons()
{
    mStartRenderButton->setEnabled(true);
    mStopRenderButton->setEnabled(true);
}

void RenderWidget::render()
{
    for (RenderInstanceWidget *wid : mRenderInstanceWidgets) {
        if (!wid->isChecked()) { continue; }
        mAwaitingSettings << wid;
        wid->getSettings().setCurrentState(RenderState::waiting);
    }
    sendNextForRender();
}

void RenderWidget::stopRendering()
{
    disableButtons();
    clearAwaitingRender();
    VideoEncoder::sInterruptEncoding();
    if (mCurrentRenderedSettings) {
        disconnect(mCurrentRenderedSettings, nullptr, this, nullptr);
        mCurrentRenderedSettings = nullptr;
    }
}

void RenderWidget::clearAwaitingRender()
{
    for (RenderInstanceWidget *wid : mAwaitingSettings) {
        wid->getSettings().setCurrentState(RenderState::none);
    }
    mAwaitingSettings.clear();
}

void RenderWidget::sendNextForRender()
{
    if (mAwaitingSettings.isEmpty()) { return; }
    const auto wid = mAwaitingSettings.takeFirst();
    if (wid->isChecked() && wid->getSettings().getTargetCanvas()) {
        disableButtons();
        wid->setDisabled(true);
        render(wid->getSettings());
    } else { sendNextForRender(); }
}

int RenderWidget::count()
{
    return mRenderInstanceWidgets.count();
}
