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

#include "followpatheffect.h"

#include "Boxes/pathbox.h"
#include "Animators/qrealanimator.h"
#include "Animators/transformanimator.h"
#include "svgexporter.h"

FollowPathEffect::FollowPathEffect() :
    TargetTransformEffect("follow path", TransformEffectType::followPath) {
    targetProperty()->setValidator<PathBox>();

    mRotate = enve::make_shared<BoolProperty>("rotate");
    mLengthBased = enve::make_shared<BoolProperty>("length based");
    mComplete = enve::make_shared<QrealAnimator>(0, 0, 1, 0.01, "complete");
    mInfluence = enve::make_shared<QrealAnimator>(1, -1, 1, 0.01, "influence");

    ca_addChild(mRotate);
    ca_addChild(mLengthBased);
    ca_addChild(mComplete);
    ca_addChild(mInfluence);
}

void calculateFollowRotPosChange(
        const SkPath relPath,
        const QMatrix transform,
        const bool lengthBased,
        const bool rotate,
        const qreal infl,
        qreal per,
        qreal& rotChange,
        qreal& posXChange,
        qreal& posYChange) {
    SkPath path;
    relPath.transform(toSkMatrix(transform), &path);
    const QPainterPath qpath = toQPainterPath(path);

    if(lengthBased) {
        const qreal length = qpath.length();
        per = qpath.percentAtLength(per*length);
    }
    const auto p1 = qpath.pointAtPercent(per);

    if(rotate) {
        qreal t2 = per + 0.0001;
        const bool reverse = t2 > 1;
        if(reverse) t2 = 0.9999;
        const auto p2 = qpath.pointAtPercent(t2);

        const QLineF baseLine(QPointF(0., 0.), QPointF(100., 0.));
        QLineF l;
        if(reverse) l = QLineF(p2, p1);
        else l = QLineF(p1, p2);
        qreal trackAngle = l.angleTo(baseLine);
        if(trackAngle > 180) trackAngle -= 360;

        rotChange = trackAngle*infl;
    } else rotChange = 0;
    posXChange = p1.x();
    posYChange = p1.y();
}

void FollowPathEffect::setRotScaleAfterTargetChange(
        BoundingBox* const oldTarget, BoundingBox* const newTarget) {
    const bool rotate = mRotate->getValue();
    if(!rotate) return;
    const auto parent = getFirstAncestor<BoundingBox>();
    if(!parent) return;
    const auto oldTargetP = static_cast<PathBox*>(oldTarget);
    const auto newTargetP = static_cast<PathBox*>(newTarget);

    const qreal infl = mInfluence->getEffectiveValue();
    const qreal per = mComplete->getEffectiveValue();
    const bool lengthBased = mLengthBased->getValue();
    const qreal relFrame = parent->anim_getCurrentRelFrame();
    const auto parentTransform = parent->getInheritedTransformAtFrame(relFrame);

    qreal rot = 0.;
    if(oldTargetP) {
        const auto relPath = oldTargetP->getRelativePath();
        const auto targetTransform = oldTargetP->getTotalTransform();
        const auto transform = targetTransform*parentTransform.inverted();

        qreal rotChange;
        qreal posXChange;
        qreal posYChange;
        calculateFollowRotPosChange(relPath, transform,
                                    lengthBased, rotate, infl, per,
                                    rotChange, posXChange, posYChange);

        rot += rotChange;
    }

    if(newTargetP) {
        const auto relPath = newTargetP->getRelativePath();
        const auto targetTransform = newTargetP->getTotalTransform();
        const auto transform = targetTransform*parentTransform.inverted();

        qreal rotChange;
        qreal posXChange;
        qreal posYChange;
        calculateFollowRotPosChange(relPath, transform,
                                    lengthBased, rotate, infl, per,
                                    rotChange, posXChange, posYChange);

        rot -= rotChange;
    }

    parent->startRotTransform();
    parent->rotateBy(rot);
}

void FollowPathEffect::applyEffect(const qreal relFrame,
        qreal& pivotX, qreal& pivotY,
        qreal& posX, qreal& posY,
        qreal& rot,
        qreal& scaleX, qreal& scaleY,
        qreal& shearX, qreal& shearY,
        QMatrix& postTransform,
        BoundingBox* const parent) {
    Q_UNUSED(pivotX)
    Q_UNUSED(pivotY)
    Q_UNUSED(scaleX)
    Q_UNUSED(scaleY)
    Q_UNUSED(shearX)
    Q_UNUSED(shearY)
    Q_UNUSED(postTransform)

    if(!isVisible()) return;

    if(!parent) return;
    const auto target = static_cast<PathBox*>(targetProperty()->getTarget());
    if(!target) return;
    const qreal absFrame = prp_relFrameToAbsFrameF(relFrame);
    const qreal targetRelFrame = target->prp_absFrameToRelFrameF(absFrame);

    const auto parentTransform = parent->getInheritedTransformAtFrame(relFrame);
    const auto targetTransform = target->getTotalTransformAtFrame(targetRelFrame);

    const auto transform = targetTransform*parentTransform.inverted();

    const auto relPath = target->getRelativePath(targetRelFrame);
    const qreal infl = mInfluence->getEffectiveValue(relFrame);
    qreal per = mComplete->getEffectiveValue(relFrame);
    const bool rotate = mRotate->getValue();
    const bool lengthBased = mLengthBased->getValue();

    qreal rotChange;
    qreal posXChange;
    qreal posYChange;

    calculateFollowRotPosChange(relPath, transform,
                                lengthBased, rotate, infl, per,
                                rotChange, posXChange, posYChange);

    if(rotate) rot += rotChange;

    posX += posXChange; //p1.x()*infl;
    posY += posYChange; //p1.y()*infl;
}

QDomNode findNodeRecursive(const QDomNode &node, const std::function<bool(const QDomNode&)> &predicate) {
    // Verificar si el nodo actual cumple con el criterio
    if (predicate(node)) {
        return node;
    }

    // Recorrer los hijos del nodo actual
    QDomNodeList children = node.childNodes();
    for (int i = 0; i < children.size(); ++i) {
        QDomNode found = findNodeRecursive(children.at(i), predicate);
        if (!found.isNull()) {
            return found; // Retornar el nodo si se encuentra
        }
    }

    // Retornar un nodo nulo si no se encuentra
    return QDomNode();
}

QDomElement FollowPathEffect::saveFollowPathSVG(SvgExporter &exp,
                                                const FrameRange &visRange,
                                                QDomElement &childElement,
                                                QDomElement &parentElement) const
{
    const auto target = targetProperty()->getTarget();
    if (!target) { return parentElement; }
    mComplete->saveQrealSVG(exp,
                            childElement,
                            visRange,
                            "transform",
                            1.,
                            false,
                            "",
                            "%1",
                            "",
                            "",
                            true,
                            mRotate->getValue(),
                            target->prp_getName());
    const auto transform = target->getBoxTransformAnimator();
    const auto transformed = transform->saveSVG(exp,
                                                visRange,
                                                parentElement);
    
    // Make a mutable copy  of `transformed`
    QDomElement mutableTransformed = transformed;

    // Recursive search of <animateMotion> node
    QDomNode animateMotionNode = findNodeRecursive(mutableTransformed, [](const QDomNode &node) {
        return node.isElement() && node.toElement().tagName() == "animateMotion";
    });

    // Recursive search of comment `<!--wrapper-end-->` (added in transformeffectcollection.cpp)
    QDomNode wrapperEndComment = findNodeRecursive(mutableTransformed, [](const QDomNode &node) {
        return node.isComment() && node.nodeValue().trimmed() == "wrapper-end";
    });

    if (!animateMotionNode.isNull() && !wrapperEndComment.isNull()) {
        // Get nodes parents
        QDomNode animateMotionParent = animateMotionNode.parentNode();
        QDomNode commentParent = wrapperEndComment.parentNode();

        // Check that parents aren't null
        if (!animateMotionParent.isNull() && !commentParent.isNull()) {
            // Detete animateMotionNode from his parent
            animateMotionParent.removeChild(animateMotionNode);

            // Insert animateMotionNode right before wrapperEndComment
            commentParent.insertBefore(animateMotionNode, wrapperEndComment);

            // Delete `<!--wrapper-end-->` comment
            commentParent.removeChild(wrapperEndComment);
        }
    }

    return mutableTransformed;
}
