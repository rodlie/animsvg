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

#include "animationwidgetscrollbar.h"
#include <QMouseEvent>
#include <QPainter>
#include "GUI/global.h"
#include "colorhelpers.h"

#include "appsupport.h"

FrameScrollBar::FrameScrollBar(const int minSpan,
                               const int maxSpan,
                               const bool range,
                               const bool clamp,
                               const bool bottom,
                               QWidget *parent) :
    QWidget(parent) {
    mMinSpan = minSpan;
    mMaxSpan = maxSpan;
    mRange = range;
    mClamp = clamp;
    mBottom = bottom;
    setFramesSpan(0);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
}

qreal FrameScrollBar::posToFrame(int xPos) {
    return (xPos - eSizesUI::widget/2)*
            (mFrameRange.fMax - mFrameRange.fMin + (mRange ? 0 : 1) ) /
            (qreal(width()) - 2*eSizesUI::widget) + mFrameRange.fMin;
}

void FrameScrollBar::setCurrentCanvas(Canvas * const canvas) {
    mCurrentCanvas = canvas;
    if (mCurrentCanvas) { mDisplayTime = mCurrentCanvas->getDisplayTimecode(); }
}

void FrameScrollBar::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), QColor(25, 25, 25));

    const int dFrame = mFrameRange.fMax - mFrameRange.fMin + (mRange ? 0 : 1);
    if(dFrame <= 0) return;
    const qreal pixPerFrame = (width() - 2.*eSizesUI::widget)/dFrame;
    if(pixPerFrame < 0 || isZero2Dec(pixPerFrame)) return;

    const int f0 = -qCeil(0.5*eSizesUI::widget/pixPerFrame);
    const int minFrame = mFrameRange.fMin + f0;
    const qreal x0 = f0*pixPerFrame + eSizesUI::widget*0.5;

    const int f1 = qCeil(1.5*eSizesUI::widget/pixPerFrame);
    const int maxFrame = mFrameRange.fMax + f1;
    const qreal w1 = width() - 1.5*eSizesUI::widget + f1*pixPerFrame - x0;

    QColor col = mHandleColor;
    if(mPressed) {
        col.setHsv(col.hue(), col.saturation(), qMin(255, col.value() + 40));
    }
    QRect handleRect;
    const int hLeftFrames = mFirstViewedFrame - minFrame;

    const int handleFixedWidth = 16;
    const int handleWidth = qRound(mViewedFramesSpan*pixPerFrame);
    const int handleLeft = mBottom ? qRound(hLeftFrames*pixPerFrame + x0) : qRound((hLeftFrames*pixPerFrame + x0)+(handleWidth/2)-(handleFixedWidth/2));

    handleRect.setLeft(handleLeft);
    handleRect.setTop(mBottom ? 0 : 5);
    handleRect.setWidth(mBottom ? handleWidth : handleFixedWidth);
    handleRect.setBottom(mBottom ? 4 : height());
    p.fillRect(handleRect, col);

    if (mCurrentCanvas) {
        const qreal fps = mCurrentCanvas->getFps();
        mFps = fps;
        if (!mRange) {
            const int soundHeight = eSizesUI::widget/3;
            const int rasterHeight = eSizesUI::widget - soundHeight;
            const QRectF rasterRect(x0, 0, w1, rasterHeight);
            const auto& rasterCache = mCurrentCanvas->getSceneFramesHandler();
            rasterCache.drawCacheOnTimeline(&p, rasterRect, minFrame, maxFrame);

            const QRectF soundRect(x0, rasterHeight, w1, soundHeight);
            const auto& soundCache = mCurrentCanvas->getSoundCacheHandler();
            soundCache.drawCacheOnTimeline(&p, soundRect, minFrame, maxFrame, fps);
        }
    }

    p.setPen(Qt::white);

    const qreal inc = mDrawFrameInc*pixPerFrame;

    const int minMod = minFrame%mDrawFrameInc;
    qreal xL = (-minMod + (mRange ? 0. : 0.5))*pixPerFrame + x0;
    int currentFrame = minFrame - minMod;

    /*if(!mRange) {
        const int nEmpty = qCeil((70 - xL)/inc);
        currentFrame += nEmpty*mDrawFrameInc;
        xL += nEmpty*inc;
    }*/

    const qreal threeFourthsHeight = height()*0.75;
    const qreal maxX = width() + eSizesUI::widget;
    while(xL < maxX) {
        //if(!mRange) {
            p.drawLine(QPointF(xL, threeFourthsHeight + 2),
                       QPointF(xL, height()));
        //}
        QString drawValue = QString::number(currentFrame);
        qreal leftValue = xL;
        if (mDisplayTime && mFps > 0) {
            drawValue = AppSupport::getTimeCodeFromFrame(currentFrame, mFps);
            if (mRange) {
                if (currentFrame == mFrameRange.fMin) {
                    leftValue = xL + 40;
                } else if (currentFrame == mFrameRange.fMax) {
                    leftValue = xL - 40;
                }
            }
        }
        p.drawText(QRectF(leftValue - inc, 0, 2*inc, height()),
                   Qt::AlignCenter, drawValue);
        xL += inc;
        currentFrame += mDrawFrameInc;
    }

    /*p.setPen(QPen(Qt::white, 1));
    if(!mRange) {
        const QRectF textRect(eSizesUI::widget, 0,
                              5*eSizesUI::widget, height());
        p.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft,
                   QString::number(mFrameRange.fMin));
    }*/

    p.end();
}

void FrameScrollBar::setFramesSpan(int newSpan) {
    mViewedFramesSpan = clampInt(newSpan, mMinSpan, mMaxSpan);
    if(mClamp) setFirstViewedFrame(mFirstViewedFrame);
}

int FrameScrollBar::getMaxFrame() {
    return mFrameRange.fMax;
}

int FrameScrollBar::getMinFrame() {
    return mFrameRange.fMin;
}

void FrameScrollBar::wheelEvent(QWheelEvent *event) {
    if(mRange) {
        if(event->modifiers() & Qt::CTRL) {
            if(event->angleDelta().y() > 0) {
                setFirstViewedFrame(mFirstViewedFrame - mViewedFramesSpan/20);
            } else {
                setFirstViewedFrame(mFirstViewedFrame + mViewedFramesSpan/20);
            }
        } else {
            int newFramesSpan = mViewedFramesSpan;
            if(event->angleDelta().y() > 0) newFramesSpan *= 0.85;
            else newFramesSpan *= 1.15;
            setFramesSpan(newFramesSpan);
        }
    } else {
        if(event->angleDelta().y() > 0) {
            setFirstViewedFrame(mFirstViewedFrame - 1);
        } else {
            setFirstViewedFrame(mFirstViewedFrame + 1);
        }
    }

    emitTriggeredChange();
    update();
}

bool FrameScrollBar::setFirstViewedFrame(const int firstFrame) {
    if(mClamp) {
        if(mRange) {
            mFirstViewedFrame = clampInt(firstFrame, mFrameRange.fMin, mFrameRange.fMax -
                                          mViewedFramesSpan);
        } else {
            mFirstViewedFrame = clampInt(firstFrame, mFrameRange.fMin, mFrameRange.fMax);
        }
        return true;
    } else {
        mFirstViewedFrame = firstFrame;
        return true;
    }

}
#include <QMenu>
void FrameScrollBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        QMenu menu(this);
        /*QAction *clampAction = new QAction("Clamp", this);
        clampAction->setCheckable(true);
        clampAction->setChecked(mClamp);
        menu.addAction(clampAction);*/

        //menu.addSeparator();

        QAction *timeAction = new QAction(tr("Display Timecodes"), this);
        timeAction->setCheckable(true);
        timeAction->setChecked(mDisplayTime);
        menu.addAction(timeAction);

        QAction *framesAction = new QAction(tr("Display Frames"), this);
        framesAction->setCheckable(true);
        framesAction->setChecked(!mDisplayTime);
        menu.addAction(framesAction);

        QAction* selectedAction = menu.exec(event->globalPos());
        if (selectedAction) {
            /*if (selectedAction == clampAction) {
                mClamp = !mClamp;
            } else*/ if (selectedAction == framesAction) {
                mDisplayTime = false;
                update();
                if (mCurrentCanvas) {
                    mCurrentCanvas->setDisplayTimecode(mDisplayTime);
                }
            } else if (selectedAction == timeAction) {
                mDisplayTime = true;
                update();
                if (mCurrentCanvas) {
                    mCurrentCanvas->setDisplayTimecode(mDisplayTime);
                }
            }
        }
        return;
    }
    mPressed = true;
    mLastMousePressFrame = posToFrame(event->x() );
    if (mLastMousePressFrame < mFirstViewedFrame ||
        mLastMousePressFrame > mFirstViewedFrame + mViewedFramesSpan) {
        setFirstViewedFrame(qRound(mLastMousePressFrame - mViewedFramesSpan/2.));
        emitTriggeredChange();
    }
    mSavedFirstFrame = mFirstViewedFrame;
    update();
}

void FrameScrollBar::mouseMoveEvent(QMouseEvent *event) {
    qreal newFrame = posToFrame(event->x() );
    int moveFrame = qRound(newFrame - mLastMousePressFrame);
    if(setFirstViewedFrame(mSavedFirstFrame + moveFrame)) {
        emitTriggeredChange();
        update();
    }
}

void FrameScrollBar::mouseReleaseEvent(QMouseEvent *) {
    mPressed = false;
    update();
}

void FrameScrollBar::setDisplayedFrameRange(const FrameRange& range) {
    mFrameRange = range;

    const int dFrame = mFrameRange.fMax - mFrameRange.fMin + (mRange ? 0 : 1);
    int divInc = 3;
    mDrawFrameInc = 5000;
    while(mDrawFrameInc && dFrame/mDrawFrameInc < 3) {
        if(divInc == 3) {
            divInc = 0;
            mDrawFrameInc *= 4;
            mDrawFrameInc /= 10;
        } else {
            mDrawFrameInc /= 2;
        }
        divInc++;
    }
    mDrawFrameInc = qMax(1, mDrawFrameInc);
    mMaxSpan = range.span() - 1;
    setViewedFrameRange({mFirstViewedFrame, mFirstViewedFrame + mViewedFramesSpan});
}

void FrameScrollBar::setViewedFrameRange(const FrameRange& range) {
    setFirstViewedFrame(range.fMin);
    setFramesSpan(range.span() - 1);
    update();
    emitChange();
}

void FrameScrollBar::setCanvasFrameRange(const FrameRange &range) {
    mCanvasRange = range;
    update();
}

void FrameScrollBar::emitChange() {
    emit frameRangeChange(getViewedRange());
}

void FrameScrollBar::emitTriggeredChange() {
    emit triggeredFrameRangeChange(getViewedRange());
}

FrameRange FrameScrollBar::getViewedRange() const {
    return {mFirstViewedFrame, getLastViewedFrame()};
}

int FrameScrollBar::getFirstViewedFrame() const {
    return mFirstViewedFrame;
}

int FrameScrollBar::getLastViewedFrame() const {
    return mFirstViewedFrame + mViewedFramesSpan;
}
