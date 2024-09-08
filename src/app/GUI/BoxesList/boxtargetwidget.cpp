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

#include "boxtargetwidget.h"
#include <QMimeData>
#include <QPainter>
#include <QMenu>
#include "Boxes/containerbox.h"
#include "Properties/boxtargetproperty.h"
#include "GUI/mainwindow.h"
#include "Properties/emimedata.h"
#include "themesupport.h"

using namespace Friction::Core;

BoxTargetWidget::BoxTargetWidget(QWidget *parent) : QWidget(parent) {
    setAcceptDrops(true);
    setMaximumWidth(150);
}

void BoxTargetWidget::setTargetProperty(BoxTargetProperty *property) {
    auto& conn = mProperty.assign(property);
    if(property) conn << connect(property, &BoxTargetProperty::targetSet,
                                 this, qOverload<>(&QWidget::update));
    update();
}

void BoxTargetWidget::dropEvent(QDropEvent *event) {
    if(!mProperty) return;
    const auto mimeData = event->mimeData();
    mProperty->SWT_drop(mimeData);
    mDragging = false;
    update();
    Document::sInstance->actionFinished();
}

void BoxTargetWidget::dragEnterEvent(QDragEnterEvent *event) {
    if(!mProperty) return;
    const auto mimeData = event->mimeData();
    const bool support = mProperty->SWT_dropSupport(mimeData);
    if(!support) return;
    event->acceptProposedAction();
    mDragging = true;
    update();
}

void BoxTargetWidget::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

void BoxTargetWidget::dragLeaveEvent(QDragLeaveEvent *event) {
    mDragging = false;
    update();
    event->accept();
}

void BoxTargetWidget::mousePressEvent(QMouseEvent *event) {
    if(!mProperty) return;
    if(event->button() == Qt::LeftButton) {
        const auto parentBox = mProperty->getFirstAncestor<BoundingBox>();
        if(!parentBox) return;
        const auto srcGroup = parentBox->getParentGroup();
        if(!srcGroup) return;
        const auto& boxes = srcGroup->getContainedBoxes();
        QMenu menu(this);

        const auto currentTarget = mProperty->getTarget();
        {
            const auto act = menu.addAction("-none-");
            connect(act, &QAction::triggered, this, [this]() {
                mProperty->setTargetAction(nullptr);
                Document::sInstance->actionFinished();
            });
            if(!currentTarget) {
                act->setCheckable(true);
                act->setChecked(true);
                act->setDisabled(true);
            }
        }
        for(const auto& box : boxes) {
            if(box == parentBox) continue;
            const auto& validator = mProperty->validator();
            if(validator && !validator(box)) continue;
            const auto act = menu.addAction(box->prp_getName());
            connect(act, &QAction::triggered, this, [this, box]() {
                mProperty->setTargetAction(box);
                Document::sInstance->actionFinished();
            });
            if(currentTarget == box) {
                act->setCheckable(true);
                act->setChecked(true);
                act->setDisabled(true);
            }
        }
        menu.exec(mapToGlobal(QPoint(0, height())));
    } else if(event->button() == Qt::RightButton) {

    }
}

void BoxTargetWidget::paintEvent(QPaintEvent *) {
    if(!mProperty) return;
    QPainter p(this);
    if(mProperty->SWT_isDisabled()) p.setOpacity(.5);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(ThemeSupport::getThemeButtonBorderColor());
    if(mDragging) {
        p.setPen(ThemeSupport::getThemeHighlightSelectedColor());
    } else {
        p.setPen(ThemeSupport::getThemeButtonBaseColor());
    }
    p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 5., 5.);

    p.setPen(Qt::white);
    const auto target = mProperty->getTarget();
    if(!target) {
        p.drawText(rect(), Qt::AlignCenter, "-none-");
    } else {
        p.drawText(rect(), Qt::AlignCenter, target->prp_getName());
    }

    p.end();
}
