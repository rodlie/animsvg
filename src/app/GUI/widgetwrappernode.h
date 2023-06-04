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

#ifndef WIDGETWRAPPERNODE_H
#define WIDGETWRAPPERNODE_H

#include <QMenuBar>
#include "Private/esettings.h"
#include "wrappernode.h"
#include "fakemenubar.h"

class StackWrapperMenu : public FakeMenuBar {
    friend class StackWidgetWrapper;
protected:
    explicit StackWrapperMenu();
};

class WidgetWrapperCornerMenu;
class WidgetWrapperNode : public QWidget, public WrapperNode {
public:
    WidgetWrapperNode(const WidgetCreator& creator);

    void close();
    void splitV();
    void splitH();

    void setMenuBar(StackWrapperMenu * const menu);
    void setCentralWidget(QWidget * const widget);

    WidgetWrapperCornerMenu* cornerMenu() const {
        return mCornerMenu;
    }

    QWidget* widget() { return this; }
private:
    WidgetWrapperCornerMenu* mCornerMenu = nullptr;
    StackWrapperMenu* mMenuBar = nullptr;
    QWidget* mCenterWidget = nullptr;
    QHBoxLayout* mMenuLayout;
    QVBoxLayout* mLayout;
};

class WidgetWrapperCornerMenu : public FakeMenuBar {
public:
    WidgetWrapperCornerMenu(WidgetWrapperNode* const target)
    {
        mSplitV = addAction(QIcon::fromTheme("split_h"),
                            tr("Split Vertically"));
        mSplitH = addAction(QIcon::fromTheme("split_v"),
                            tr("Split Horizontally"));
        mClose = addAction(QIcon::fromTheme("close"),
                           tr("Close"));

        connect(mSplitV, &QAction::triggered,
                target, &WidgetWrapperNode::splitV);
        connect(mSplitH, &QAction::triggered,
                target, &WidgetWrapperNode::splitH);
        connect(mClose, &QAction::triggered,
                target, &WidgetWrapperNode::close);

        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    void disableClose() {
        mClose->setVisible(false);
    }

    void enableClose() {
        mClose->setVisible(true);
    }
private:
    QAction * mSplitV = nullptr;
    QAction * mSplitH = nullptr;
    QAction * mClose = nullptr;
};

#endif // WIDGETWRAPPERNODE_H
