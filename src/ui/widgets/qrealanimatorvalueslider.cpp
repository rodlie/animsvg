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

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#include "qrealanimatorvalueslider.h"
#include "Animators/qvector3danimator.h"
#include "themesupport.h"
#include "canvas.h"
#include "Private/document.h"

#include <QMenu>
#include <QMouseEvent>

QrealAnimatorValueSlider::QrealAnimatorValueSlider(qreal minVal, qreal maxVal,
                                                   qreal prefferedStep,
                                                   QWidget *parent) :
    QDoubleSlider(minVal, maxVal, prefferedStep, parent) {

}

QrealAnimatorValueSlider::QrealAnimatorValueSlider(qreal minVal,
                                                   qreal maxVal,
                                                   qreal prefferedStep,
                                                   QWidget *parent,
                                                   bool autoAdjust)
    : QDoubleSlider(minVal, maxVal, prefferedStep, parent, autoAdjust)
{

}

QrealAnimatorValueSlider::QrealAnimatorValueSlider(qreal minVal, qreal maxVal,
                                                   qreal prefferedStep,
                                                   QrealAnimator *animator,
                                                   QWidget *parent) :
    QDoubleSlider(minVal, maxVal, prefferedStep, parent) {
    setTarget(animator);
}

QrealAnimatorValueSlider::QrealAnimatorValueSlider(QrealAnimator *animator,
                                                   QWidget *parent) :
    QDoubleSlider(parent) {
    setTarget(animator);
}

QrealAnimatorValueSlider::QrealAnimatorValueSlider(QWidget *parent) :
    QrealAnimatorValueSlider(nullptr, parent) {

}

QrealAnimatorValueSlider::QrealAnimatorValueSlider(QString name,
                                                   qreal minVal, qreal maxVal,
                                                   qreal prefferedStep,
                                                   QWidget *parent) :
    QDoubleSlider(name, minVal, maxVal, prefferedStep, parent) {

}

QrealAnimator* QrealAnimatorValueSlider::getTransformTargetSibling() {
    if(mTransformTarget) {
        const auto parent = mTransformTarget->getParent();
        if(const auto qPA = enve_cast<QPointFAnimator*>(parent)) {
            const bool thisX = qPA->getXAnimator() == mTransformTarget;
            return thisX ? qPA->getYAnimator() :
                           qPA->getXAnimator();
        }
    }
    return nullptr;
}

void QrealAnimatorValueSlider::mouseMoveEvent(QMouseEvent *event) {
    if(event->modifiers() & Qt::ShiftModifier) {
        const auto other = getTransformTargetSibling();
        if(other) {
            if(!mouseMoved()) other->prp_startTransform();
            const qreal dValue = getDValueForMouseMove(event->globalX());
            other->incCurrentBaseValue(dValue);
        }
    }
    QDoubleSlider::mouseMoveEvent(event);
}

bool QrealAnimatorValueSlider::eventFilter(QObject *obj, QEvent *event) {
    const bool keyPress = event->type() == QEvent::KeyPress;
    const bool keyRelease = event->type() == QEvent::KeyRelease;

    if(keyPress || keyRelease) {
        const auto keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_Shift) {
            const auto other = getTransformTargetSibling();
            if(other) {
                if(keyPress) {
                    if(mouseMoved()) {
                        other->prp_startTransform();
                    }
                } else if(keyRelease) {
                    other->prp_cancelTransform();
                }
            }
        }
    }
    return QDoubleSlider::eventFilter(obj, event);
}

void QrealAnimatorValueSlider::startTransform(const qreal value) {
    if(mTarget) {
        mTransformTarget = mTarget;
        mTransformTarget->prp_startTransform();
    }
    QDoubleSlider::startTransform(value);
}

QString QrealAnimatorValueSlider::getEditText() const {
    if(mTarget && mTarget->hasExpression()) {
        return valueToText(mBaseValue);
    }
    return QDoubleSlider::getEditText();
}

void QrealAnimatorValueSlider::setValue(const qreal value) {
    if(mTransformTarget) {
        mTransformTarget->setCurrentBaseValue(value);
        emit valueEdited(this->value());
    } else QDoubleSlider::setValue(value);
}

void QrealAnimatorValueSlider::finishTransform(const qreal value) {
    if(mTransformTarget) {
        mTransformTarget->prp_finishTransform();
        mTransformTarget = nullptr;
        const auto other = getTransformTargetSibling();
        if(other) other->prp_finishTransform();
    }
    QDoubleSlider::finishTransform(value);
}

void QrealAnimatorValueSlider::cancelTransform() {
    if(mTransformTarget) {
        mTransformTarget->prp_cancelTransform();
        mTransformTarget = nullptr;
        const auto other = getTransformTargetSibling();
        if(other) other->prp_cancelTransform();
    }
    QDoubleSlider::cancelTransform();
}

qreal QrealAnimatorValueSlider::startSlideValue() const {
    if(mTarget && mTarget->hasExpression()) return mBaseValue;
    else return QDoubleSlider::startSlideValue();
}

void QrealAnimatorValueSlider::paint(QPainter *p) {
    if(!mTarget) {
        QDoubleSlider::paint(p);
    } else {
        bool rec = false;
        bool key = false;
        const auto aTarget = static_cast<Animator*>(*mTarget);
        rec = aTarget->anim_isRecording();
        key = aTarget->anim_getKeyOnCurrentFrame();
        if(rec) {
            const bool disabled = isTargetDisabled() || !isEnabled();
            QDoubleSlider::paint(p,
                                 disabled ? ThemeSupport::getThemeButtonBaseColor(200) : ThemeSupport::getThemeHighlightAlternativeColor(),
                                 key ? (disabled ? ThemeSupport::getThemeAlternateColor() : ThemeSupport::getThemeHighlightSelectedColor()) : (disabled ? ThemeSupport::getThemeAlternateColor() : ThemeSupport::getThemeHighlightColor()),
                                 key ? (disabled ? Qt::gray : ThemeSupport::getThemeHighlightSelectedColor()) : (disabled ? Qt::darkGray : ThemeSupport::getThemeButtonBorderColor()),
                                 disabled ? Qt::darkGray : Qt::black);
        } else {
            QDoubleSlider::paint(p, !isTargetDisabled() && isEnabled());
        }
        if(!textEditing() && mTarget->hasExpression()) {
            if(mTarget->hasValidExpression()) {
                p->setBrush(ThemeSupport::getThemeHighlightColor());
            } else {
                p->setBrush(QColor(255, 125, 0));
            }
            p->setPen(Qt::NoPen);
            p->setRenderHint(QPainter::Antialiasing);
            p->drawEllipse({7, height()/2}, 3, 3);
        }
    }
}

void QrealAnimatorValueSlider::targetHasExpressionChanged() {
    QObject::disconnect(mExprConn);
    if(mTarget) {
        const bool hasExpression = mTarget->hasExpression();
        if(hasExpression) {
            mExprConn = connect(mTarget, &QrealAnimator::baseValueChanged,
                    this, [this](const qreal value) {
                mBaseValue = value;
                setName(valueToText(mBaseValue));
            });
        }
        mBaseValue = mTarget->getCurrentBaseValue();
        setName(valueToText(mBaseValue));
        setNameVisible(hasExpression);
    } else setNameVisible(false);
}

void QrealAnimatorValueSlider::setTarget(QrealAnimator * const animator) {
    if(animator == mTarget) return;
    auto& conn = mTarget.assign(animator);
    targetHasExpressionChanged();
    if(animator) {
        conn << connect(animator, &QrealAnimator::effectiveValueChanged,
                        this, &QrealAnimatorValueSlider::setDisplayedValue);
        conn << connect(animator, &QrealAnimator::anim_changedKeyOnCurrentFrame,
                        this, qOverload<>(&QrealAnimatorValueSlider::update));
        conn << connect(animator, &QrealAnimator::expressionChanged,
                        this, &QrealAnimatorValueSlider::targetHasExpressionChanged);

        setNumberDecimals(animator->getNumberDecimals());
        setValueRange(animator->getMinPossibleValue(),
                      animator->getMaxPossibleValue());
        setPrefferedValueStep(animator->getPrefferedValueStep());
        setDisplayedValue(animator->getEffectiveValue());
    }
}

bool QrealAnimatorValueSlider::hasTarget() {
    return mTarget;
}

bool QrealAnimatorValueSlider::isTargetDisabled() {
    if(hasTarget()) return mTarget->SWT_isDisabled();
    return true;
}

void QrealAnimatorValueSlider::openContextMenu(
        const QPoint &globalPos) {
    if(!mTarget) return;
    const auto aTarget = *mTarget;
    QMenu menu(this);

    const bool keyOnFrame = aTarget->anim_getKeyOnCurrentFrame();
    const auto deleteKey = menu.addAction(
                "Delete Key", aTarget,
                &Animator::anim_deleteCurrentKeyAction);
    deleteKey->setEnabled(keyOnFrame);

    const auto addKey = menu.addAction(
                "Add Key", aTarget,
                &Animator::anim_saveCurrentValueAsKey);
    addKey->setEnabled(!keyOnFrame);

    menu.addSeparator();

    const auto setExpression = menu.addAction(tr("Set Expression"));
    connect(setExpression, &QAction::triggered, this, [aTarget]() {
        const auto scene = aTarget->getParentScene();
        if (scene) { scene->openExpressionDialog(aTarget); }
    });

    const auto applyExpression = menu.addAction(tr("Apply Expression"));
    connect(applyExpression, &QAction::triggered, this, [aTarget]() {
        const auto scene = aTarget->getParentScene();
        if (scene) { scene->openApplyExpressionDialog(aTarget); }
    });
    applyExpression->setEnabled(aTarget->hasExpression());


    const auto clearExpression = menu.addAction(
                "Clear Expression", aTarget,
                &QrealAnimator::clearExpressionAction);
    clearExpression->setEnabled(aTarget->hasExpression());

    menu.addSeparator();

    QAction * const recAct = menu.addAction("Recording");
    recAct->setCheckable(true);
    recAct->setChecked(aTarget->anim_isRecording());
    connect(recAct, &QAction::triggered,
            aTarget, &Animator::anim_setRecording);

    QAction * const selectedAction = menu.exec(globalPos);
    if(!selectedAction) return;
    else Document::sInstance->actionFinished();
}
