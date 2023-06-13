// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "renderwidget.h"
#include "canvas.h"
#include "GUI/global.h"
#include "renderinstancewidget.h"
#include "GUI/BoxesList/OptimalScrollArea/scrollarea.h"
#include "videoencoder.h"
#include "ReadWrite/basicreadwrite.h"

RenderWidget::RenderWidget(QWidget *parent) : QWidget(parent) {
    mMainLayout = new QVBoxLayout(this);
    mMainLayout->setMargin(0);
    setLayout(mMainLayout);
    mRenderProgressBar = new QProgressBar(this);
    /*mRenderProgressBar->setStyleSheet("border-top: 0;"
                                      "border-right: 0;"
                                      "border-left: 0;");*/
    //mRenderProgressBar->setFixedHeight(8);
    mRenderProgressBar->setValue(0);

    mButtonsLayout = new QHBoxLayout();
    //mButtonsLayout->setMargin(eSizesUI::widget/2);
    //eSizesUI::widget.addHalfSpacing(mButtonsLayout);
    /*mCurrentRenderLabel = new QLabel("Current Render", this);
    mCurrentRenderLabel->setStyleSheet("background: rgb(30, 30, 30);"
                                       "font-weight: bold;"
                                       "padding-left: 10px;");
    mButtonsLayout->addWidget(mCurrentRenderLabel);*/

    mStartRenderButton = new QPushButton("Render", this);
    mButtonsLayout->addWidget(mStartRenderButton, Qt::AlignRight);
    mStartRenderButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(mStartRenderButton, &QPushButton::pressed,
            this, qOverload<>(&RenderWidget::render));

    mPauseRenderButton = new QPushButton("Pause", this);
    mButtonsLayout->addWidget(mPauseRenderButton, Qt::AlignRight);
    mPauseRenderButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    mStopRenderButton = new QPushButton("Stop", this);
    mStopRenderButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    mButtonsLayout->addWidget(mStopRenderButton, Qt::AlignRight);
    //eSizesUI::widget.addHalfSpacing(mButtonsLayout);
    connect(mStopRenderButton, &QPushButton::pressed,
            this, &RenderWidget::stopRendering);

    mMainLayout->addLayout(mButtonsLayout);

    mContWidget = new QWidget(this);
    mContLayout = new QVBoxLayout(mContWidget);
    mContLayout->setAlignment(Qt::AlignTop);
    mContLayout->setMargin(0);
    mContLayout->setSpacing(0);
    mContWidget->setLayout(mContLayout);
    mScrollArea = new ScrollArea(this);
    mScrollArea->setWidget(mContWidget);
    mScrollArea->setWidgetResizable(true);
    mScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mMainLayout->addWidget(mScrollArea);
    mMainLayout->addWidget(mRenderProgressBar);

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

void RenderWidget::createNewRenderInstanceWidgetForCanvas(Canvas *canvas) {
    const auto wid = new RenderInstanceWidget(canvas, this);
    addRenderInstanceWidget(wid);
}

void RenderWidget::addRenderInstanceWidget(RenderInstanceWidget *wid) {
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

void RenderWidget::setRenderedFrame(const int frame) {
    if(!mCurrentRenderedSettings) return;
    mRenderProgressBar->setValue(frame);
}

void RenderWidget::clearRenderQueue() {
    if(mStopRenderButton->isEnabled()) stopRendering();
    for(int i = mRenderInstanceWidgets.count() - 1; i >= 0; i--) {
        delete mRenderInstanceWidgets.at(i);
    }
    leaveOnlyStartRenderButtonEnabled();
}

void RenderWidget::write(eWriteStream &dst) const {
    dst << mRenderInstanceWidgets.count();
    for(const auto widget : mRenderInstanceWidgets) {
        widget->write(dst);
    }
}

void RenderWidget::read(eReadStream &src) {
    int nWidgets; src >> nWidgets;
    for(int i = 0; i < nWidgets; i++) {
        const auto wid = new RenderInstanceWidget(nullptr, this);
        wid->read(src);
        addRenderInstanceWidget(wid);
    }
}

#include "renderhandler.h"
void RenderWidget::render(RenderInstanceSettings &settings) {
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
        if(state == RenderState::finished) {
            mRenderProgressBar->setValue(mRenderProgressBar->maximum());
        }
    });
}

void RenderWidget::leaveOnlyInterruptionButtonsEnabled() {
    mStartRenderButton->setDisabled(true);
    mPauseRenderButton->setEnabled(true);
    mStopRenderButton->setEnabled(true);
}

void RenderWidget::leaveOnlyStartRenderButtonEnabled() {
    mStartRenderButton->setEnabled(true);
    mPauseRenderButton->setDisabled(true);
    mStopRenderButton->setDisabled(true);
}

void RenderWidget::disableButtons() {
    mStartRenderButton->setDisabled(true);
    mPauseRenderButton->setDisabled(true);
    mStopRenderButton->setDisabled(true);
}

void RenderWidget::enableButtons() {
    mStartRenderButton->setEnabled(true);
    mPauseRenderButton->setEnabled(true);
    mStopRenderButton->setEnabled(true);
}

void RenderWidget::render() {
    for(RenderInstanceWidget *wid : mRenderInstanceWidgets) {
        if(!wid->isChecked()) continue;
        mAwaitingSettings << wid;
        wid->getSettings().setCurrentState(RenderState::waiting);
    }
    sendNextForRender();
}

#include "videoencoder.h"
void RenderWidget::stopRendering() {
    disableButtons();
    clearAwaitingRender();
    VideoEncoder::sInterruptEncoding();
    if(mCurrentRenderedSettings) {
        disconnect(mCurrentRenderedSettings, nullptr, this, nullptr);
        mCurrentRenderedSettings = nullptr;
    }
}

void RenderWidget::clearAwaitingRender() {
    for(RenderInstanceWidget *wid : mAwaitingSettings) {
        wid->getSettings().setCurrentState(RenderState::none);
    }
    mAwaitingSettings.clear();
}

void RenderWidget::sendNextForRender() {
    if(mAwaitingSettings.isEmpty()) return;
    const auto wid = mAwaitingSettings.takeFirst();
    if(wid->isChecked() && wid->getSettings().getTargetCanvas()) {
        disableButtons();
        wid->setDisabled(true);
        render(wid->getSettings());
    } else sendNextForRender();
}
