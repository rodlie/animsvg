#include "centralwidget.h"
#include "bookmarkedwidget.h"
#include "GUI/global.h"
#include <QResizeEvent>

CentralWidget::CentralWidget(BookmarkedWidget *left,
                             QWidget *central,
                             BookmarkedWidget *right,
                             QWidget *parent)
    : QWidget(parent)
    , mLeft(left)
    , mCentral(central)
    , mRight(right)
{
    setMinimumWidth(600);
    setContentsMargins(0, 0, 0, 0);
    central->setParent(this);
    left->setParent(this);
    left->move(0, eSizesUI::widget);
    right->setParent(this);
    right->move(width() - right->width(), eSizesUI::widget);
}

void CentralWidget::setSidesVisibilitySetting(const bool vis) {
    mVisibilitySetting = vis;
    updateSideWidgetsVisibility();
}

void CentralWidget::setCanvasMode(const CanvasMode mode) {
    mMode = mode;
    updateSideWidgetsVisibility();
}

void CentralWidget::resizeEvent(QResizeEvent *event) {
    updateSizeWidgetsSize();
    mCentral->resize(event->size());
}

void CentralWidget::updateSideWidgetsVisibility() {
    const bool vis = mMode == CanvasMode::paint && mVisibilitySetting;
    mLeft->setVisible(vis);
    mRight->setVisible(vis);
    updateSizeWidgetsSize();
}

void CentralWidget::updateSizeWidgetsSize() {
    if(mLeft->isVisible()) {
        mLeft->setMaximumHeight(height() - eSizesUI::widget);
        mLeft->updateSize();
    }
    if(mRight->isVisible()) {
        mRight->move(width() - mRight->width(), eSizesUI::widget);
        mRight->setMaximumHeight(height() - eSizesUI::widget);
        mRight->updateSize();
    }
}
