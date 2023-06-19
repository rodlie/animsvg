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

#ifndef FILESOURCELIST_H
#define FILESOURCELIST_H
#include "FileCacheHandlers/filecachehandler.h"
#include "filesourcescache.h"
#include <QWidget>
#include "GUI/BoxesList/OptimalScrollArea/scrollvisiblepartbase.h"
#include "GUI/BoxesList/OptimalScrollArea/scrollarea.h"
#include "GUI/BoxesList/OptimalScrollArea/minimalscrollwidget.h"
#include "fileshandler.h"
class FileSourceListVisibleWidget;
struct FileCacheHandlerAbstraction : public QObject {
    FileCacheHandlerAbstraction(
            FileCacheHandler *target,
            FileSourceListVisibleWidget *parent) :
        QObject(target), fTarget(target),
        fParentVisibleWidget(parent) {}

    FileCacheHandler * const fTarget;
    FileSourceListVisibleWidget * const fParentVisibleWidget;
    bool fSelected = false;

    void switchSelected() {
        setSelected(!fSelected);
    }

    void setSelected(const bool bT);

    const QString &getName() {
        return fTarget->path();
    }

    bool isFileMissing() {
        return fTarget->fileMissing();
    }

};

class FileSourceWidget : public QWidget {
public:
    FileSourceWidget(FileSourceListVisibleWidget *parent = nullptr);

    void setTargetCache(FileCacheHandlerAbstraction *target);

    void switchFileNameOnly();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
private:
    bool mFileNameOnly = true;
    qptr<FileCacheHandlerAbstraction> mTargetCache;
    FileSourceListVisibleWidget *mParentVisibleWidget = nullptr;
    QPoint mDragStartPos;
};

class FileSourceListScrollWidget : public MinimalScrollWidget {
public:
    FileSourceListScrollWidget(ScrollArea *parent);

    void updateHeight();
};

class FileSourceListVisibleWidget : public ScrollVisiblePartBase {
public:
    FileSourceListVisibleWidget(MinimalScrollWidget *parent = nullptr);

    void updateVisibleWidgetsContent();
    QWidget *createNewSingleWidget();

    void addCacheHandlerToList(FileCacheHandler * const handler);
    void removeCacheHandlerFromList(FileCacheHandler * const handler);

    void addToSelectedList(FileCacheHandlerAbstraction *item) {
        mSelectedList << item;
        update();
    }

    void removeFromSelectedList(FileCacheHandlerAbstraction *item) {
        mSelectedList.removeOne(item);
        update();
    }

    void clear() {
        clearSelected();
        mCacheList.clear();
    }

    void clearSelected() {
        for(const auto& abs : mSelectedList) {
            abs->fSelected = false;
        }

        mSelectedList.clear();
        update();
    }

    void showContextMenu(const QPoint &globalPos);

    int getCacheListCount() {
        return mCacheList.count();
    }
protected:
    QList<FileCacheHandlerAbstraction*> mSelectedList;
    QList<stdsptr<FileCacheHandlerAbstraction>> mCacheList;
    void paintEvent(QPaintEvent *);
};

class FileSourceList : public ScrollArea {
    Q_OBJECT
public:
    FileSourceList(QWidget *parent = nullptr);
signals:
    void doubleClicked();
protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *) { emit doubleClicked(); }
private:
    FileSourceListScrollWidget *mScrollWidget;
};

#endif // FILESOURCELIST_H
