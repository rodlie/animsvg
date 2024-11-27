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

#include "bookmarkedwidget.h"
#include "Private/esettings.h"
#include "GUI/global.h"

BookmarkedWidget::BookmarkedWidget(const bool vertical,
                                   const int dimension,
                                   QWidget *parent) :
    QWidget(parent), mVertical(vertical), mDimension(dimension) {
    mUpArrow = new QPushButton(this);
    mDownArrow = new QPushButton(this);
    if(mVertical) {
        setFixedWidth(dimension);
        mUpArrow->setFixedWidth(dimension);
        mDownArrow->setFixedWidth(dimension);
    } else {
        setFixedHeight(dimension);
        mUpArrow->setFixedHeight(dimension);
        mDownArrow->setFixedHeight(dimension);
    }

    mUpArrow->setIcon(QIcon::fromTheme("uparrow"));
    mDownArrow->setIcon(QIcon::fromTheme("downarrow"));

    connect(mUpArrow, &QPushButton::pressed, this, [this]() {
        if(mFirstViewed == 0) return;
        mFirstViewed--;
        updateLayout();
    });
    connect(mDownArrow, &QPushButton::pressed, this, [this]() {
        if(mLastViewed == mWidgets.count() - 1) return;
        mFirstViewed++;
        updateLayout();
    });

    setStyleSheet("QPushButton { border-radius: 0; };");

    eSizesUI::widget.add(this, [this](int) {
        updateLayout();
    });
}

void BookmarkedWidget::addWidget(QWidget * const wid) {
    wid->setParent(this);
    wid->setFixedSize(mDimension, mDimension);
    mWidgets << wid;
    updateSize();
    updateLayout();
}

void BookmarkedWidget::removeWidget(QWidget * const wid) {
    mWidgets.removeOne(wid);
    wid->hide();
    wid->deleteLater();
    updateSize();
    updateLayout();
}

QWidget *BookmarkedWidget::getWidget(const int id) const {
    Q_ASSERT(id >= 0 && id < mWidgets.count());
    return mWidgets.at(id);
}

void BookmarkedWidget::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event)
    updateLayout();
}

void BookmarkedWidget::updateSize() {
    const int totWidDim = mWidgets.count()*mDimension;
    if(mVertical) resize(width(), qMin(maximumHeight(), totWidDim));
    else resize(qMin(maximumWidth(), totWidDim), height());
}

void BookmarkedWidget::updateLayout() {
    if(mWidgets.isEmpty()) return;
    std::function<void(QWidget*, const int)> setPos;
    if(mVertical) {
        setPos = [](QWidget* wid, const int y) { wid->move(0, y); };
    } else {
        setPos = [](QWidget* wid, const int x) { wid->move(x, 0); };
    }
    const int dim = mVertical ? height() : width();
    const int minArrowDim = eSizesUI::widget;
    const bool arrowsVisible = dim - mWidgets.count()*mDimension < 0;
    const int maxVisible = arrowsVisible ?
                (dim - 2*minArrowDim)/mDimension :
                dim/mDimension;
    const int nVisible = qMin(mWidgets.count(), maxVisible);
    const int arrowDim = (dim - nVisible*mDimension)/2;
    mFirstViewed = qMin(mWidgets.count() - nVisible, mFirstViewed);
    mLastViewed = mFirstViewed + nVisible - 1;
    int currXY = 0;
    if(arrowsVisible) {
        mUpArrow->show();
        setPos(mUpArrow, currXY);
        if(mVertical) mUpArrow->setFixedHeight(arrowDim);
        else mUpArrow->setFixedWidth(arrowDim);
        currXY += arrowDim;
        mUpArrow->setEnabled(mFirstViewed > 0);
    } else mUpArrow->hide();

    for(int i = 0; i < mWidgets.count(); i++) {
        const auto wid = mWidgets.at(i);
        if(i < mFirstViewed || i >= mFirstViewed + nVisible) {
            wid->hide();
            continue;
        }
        wid->show();
        wid->setFixedSize(mDimension, mDimension);
        setPos(wid, currXY);
        currXY += mDimension;
    }

    if(arrowsVisible) {
        mDownArrow->show();
        setPos(mDownArrow, currXY);
        if(mVertical) mDownArrow->setFixedHeight(dim - currXY);
        else mDownArrow->setFixedWidth(dim - currXY);
        mDownArrow->setEnabled(mLastViewed < mWidgets.count() - 1);
    } else mDownArrow->hide();
}
