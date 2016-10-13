#include "qpointfanimator.h"
#include "boxeslist.h"

QPointFAnimator::QPointFAnimator() : ComplexAnimator()
{
    mXAnimator.setName("x");
    mYAnimator.setName("y");
    addChildAnimator(&mXAnimator);
    addChildAnimator(&mYAnimator);

    mXAnimator.blockPointer();
    mYAnimator.blockPointer();
}

QPointF QPointFAnimator::getCurrentValue()
{
    return QPointF(mXAnimator.getCurrentValue(), mYAnimator.getCurrentValue());
}

qreal QPointFAnimator::getXValue()
{
    return mXAnimator.getCurrentValue();
}

qreal QPointFAnimator::getYValue()
{
    return mYAnimator.getCurrentValue();
}

void QPointFAnimator::setCurrentValue(QPointF val, bool finish)
{
    startNewUndoRedoSet();

    mXAnimator.setCurrentValue(val.x(), finish);
    mYAnimator.setCurrentValue(val.y(), finish);

    finishUndoRedoSet();
}

void QPointFAnimator::incCurrentValue(qreal x, qreal y)
{
    mXAnimator.incCurrentValue(x);
    mYAnimator.incCurrentValue(y);
}

void QPointFAnimator::incAllValues(qreal x, qreal y) {
    mXAnimator.incAllValues(x);
    mYAnimator.incAllValues(y);
}

QrealAnimator *QPointFAnimator::getXAnimator()
{
    return &mXAnimator;
}

QrealAnimator *QPointFAnimator::getYAnimator()
{
    return &mYAnimator;
}

void QPointFAnimator::multCurrentValue(qreal sx, qreal sy)
{
    mXAnimator.multCurrentValue(sx);
    mYAnimator.multCurrentValue(sy);
}

QPointF QPointFAnimator::getSavedValue()
{
    return QPointF(mXAnimator.getSavedValue(), mYAnimator.getSavedValue() );
}

qreal QPointFAnimator::getSavedXValue()
{
    return mXAnimator.getSavedValue();
}

qreal QPointFAnimator::getSavedYValue()
{
    return mYAnimator.getSavedValue();
}
