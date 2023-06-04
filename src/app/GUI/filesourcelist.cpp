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

#include "filesourcelist.h"
#include "GUI/global.h"
#include <QPainter>
#include <QScrollBar>
#include <QMenu>
#include "mainwindow.h"
#include <QApplication>
#include <QDrag>
#include "FileCacheHandlers/videocachehandler.h"
#include "FileCacheHandlers/imagecachehandler.h"
#include "FileCacheHandlers/imagesequencecachehandler.h"

FileSourceWidget::FileSourceWidget(FileSourceListVisibleWidget *parent) :
    QWidget(parent) {
    mParentVisibleWidget = parent;
    eSizesUI::widget.add(this, [this](const int size) {
        setFixedHeight(size);
    });
}

void FileSourceWidget::setTargetCache(FileCacheHandlerAbstraction *target) {
    mTargetCache = target;
    if(!mTargetCache) {
        setToolTip("");
    } else {
        setToolTip(mTargetCache->getName());
    }
}

void FileSourceWidget::mouseMoveEvent(QMouseEvent *event) {
    if(!(event->buttons() & Qt::LeftButton)) {
        return;
    }
    if((event->pos() - mDragStartPos).manhattanLength()
         < QApplication::startDragDistance()) {
        return;
    }
    QDrag *drag = new QDrag(this);

    QMimeData *mimeData = new QMimeData();
    const QString path = mTargetCache->getName();
    const QUrl url = QUrl::fromLocalFile(path);
    mimeData->setUrls(QList<QUrl>() << url);
    drag->setMimeData(mimeData);

    drag->installEventFilter(MainWindow::sGetInstance());
    drag->exec(Qt::CopyAction | Qt::MoveAction);
}


void FileSourceWidget::paintEvent(QPaintEvent *) {
    if(!mTargetCache || width() <= 2*eSizesUI::widget) return;
    QPainter p(this);

    QString pathString = mTargetCache->getName();
    if(mFileNameOnly) pathString = pathString.split("/").last();
    const auto fm = p.fontMetrics();
    const int spaceForPath = int(width() - 1.5*eSizesUI::widget);
    pathString = fm.elidedText(pathString, Qt::ElideLeft, spaceForPath);
    const int pathWidth = fm.width(pathString);

    if(mTargetCache->fSelected) {
        p.fillRect(QRect(0.5*eSizesUI::widget, 0,
                         pathWidth + eSizesUI::widget, eSizesUI::widget),
                   QColor(180, 180, 180));
        p.setPen(Qt::black);
    }
    if(mTargetCache->isFileMissing()) p.setPen(Qt::red);

    p.drawText(rect().adjusted(eSizesUI::widget, 0,
                               -0.5*eSizesUI::widget, 0),
               Qt::AlignVCenter | Qt::AlignLeft,
               pathString);

    p.end();
}

void FileSourceWidget::switchFileNameOnly() {
    mFileNameOnly = !mFileNameOnly;
}

void FileSourceWidget::mousePressEvent(QMouseEvent *event) {
    mDragStartPos = event->pos();
}

void FileSourceWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(!mTargetCache) return;
    if(event->button() == Qt::LeftButton) {
        if(event->modifiers() & Qt::ShiftModifier) {
            mTargetCache->switchSelected();
        } else {
            mParentVisibleWidget->clearSelected();
            mTargetCache->setSelected(true);
        }
    } else {
        if(!mTargetCache->fSelected) {
            mParentVisibleWidget->clearSelected();
            mTargetCache->setSelected(true);
        }
        mParentVisibleWidget->showContextMenu(event->globalPos());
    }

    update();
}

FileSourceListScrollWidget::FileSourceListScrollWidget(ScrollArea *parent) :
    MinimalScrollWidget(new FileSourceListVisibleWidget(this), parent) {
    //updateHeight();
}

void FileSourceListScrollWidget::updateHeight() {
    const auto visWid = static_cast<FileSourceListVisibleWidget*>(
                visiblePartWidget());
    setFixedHeight((visWid->getCacheListCount() + 0.5) * eSizesUI::widget);
}

FileSourceListVisibleWidget::FileSourceListVisibleWidget(MinimalScrollWidget *parent) :
    ScrollVisiblePartBase(parent) {
    connect(FilesHandler::sInstance, &FilesHandler::addedCacheHandler,
            this, &FileSourceListVisibleWidget::addCacheHandlerToList);
    connect(FilesHandler::sInstance, &FilesHandler::removedCacheHandler,
            this, &FileSourceListVisibleWidget::removeCacheHandlerFromList);
}

void FileSourceListVisibleWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);

    int currY = eSizesUI::widget;
    p.setPen(QPen(QColor(40, 40, 40), 1.));
    while(currY < height()) {
        p.drawLine(0, currY, width(), currY);

        currY += eSizesUI::widget;
    }

//    if(mDragging) {
//        p.setPen(QPen(Qt::white, 3.));
//        p.drawLine(0, mCurrentDragPosId*MIN_WIDGET_HEIGHT,
//                   width(), mCurrentDragPosId*MIN_WIDGET_HEIGHT);
//    }

    p.end();
}

void FileSourceListVisibleWidget::updateVisibleWidgetsContent() {
    int firstVisibleId = visibleTop()/eSizesUI::widget;

    int iTarget = firstVisibleId;
    const auto& wids = widgets();
    const int nWidgets =  wids.count();
    for(int iWidget = 0; iWidget < nWidgets; iWidget++) {
        const auto fsw = static_cast<FileSourceWidget*>(wids.at(iWidget));
        if(iTarget < mCacheList.count()) {
            fsw->setTargetCache(mCacheList.at(iTarget++).get());
            fsw->show();
        } else {
            fsw->setTargetCache(nullptr);
            fsw->hide();
        }
    }
}

QWidget *FileSourceListVisibleWidget::createNewSingleWidget() {
    return new FileSourceWidget(this);
}

void FileSourceListVisibleWidget::addCacheHandlerToList(FileCacheHandler * const handler) {
    mCacheList << std::make_shared<FileCacheHandlerAbstraction>(handler, this);
    scheduleContentUpdate();
}

void FileSourceListVisibleWidget::removeCacheHandlerFromList(
        FileCacheHandler * const handler) {
    for(int i = 0; i < mCacheList.count(); i++) {
        const auto& abs = mCacheList.at(i);
        if(abs->fTarget == handler) {
            if(abs->fSelected) removeFromSelectedList(abs.get());
            mCacheList.removeAt(i);
            scheduleContentUpdate();
            return;
        }
    }
}

void FileSourceListVisibleWidget::showContextMenu(const QPoint &globalPos) {
    QMenu menu;
    menu.addAction("Reload");
    if(mSelectedList.count() == 1) menu.addAction("Replace...");
    menu.addSeparator();
    menu.addAction("Delete");
    const auto selected_action = menu.exec(globalPos);
    if(selected_action) {
        if(selected_action->text() == "Reload") {
            for(const auto& abs : mSelectedList) {
                abs->fTarget->reloadAction();
            }
        } else if(selected_action->text() == "Replace...") {
            mSelectedList.first()->fTarget->replace();
        } else if(selected_action->text() == "Delete") {
            for(const auto& abs : mSelectedList) {
                abs->fTarget->deleteAction();
            }
        }

        Document::sInstance->actionFinished();
    }
}

FileSourceList::FileSourceList(QWidget *parent) : ScrollArea(parent) {
    mScrollWidget = new FileSourceListScrollWidget(this);
    setWidget(mScrollWidget);

    connect(verticalScrollBar(), &QScrollBar::valueChanged,
            mScrollWidget, &FileSourceListScrollWidget::changeVisibleTop);
    connect(this, &FileSourceList::heightChanged,
            mScrollWidget, &FileSourceListScrollWidget::changeVisibleHeight);
    connect(this, &FileSourceList::widthChanged,
            mScrollWidget, &FileSourceListScrollWidget::setWidth);

    verticalScrollBar()->setSingleStep(eSizesUI::widget);
    setAcceptDrops(true);
}

void FileSourceList::dropEvent(QDropEvent *event) {
    if(event->mimeData()->hasUrls()) {
        const QList<QUrl> urlList = event->mimeData()->urls();
        for(const QUrl &url : urlList) {
            if(url.isLocalFile()) {
                const QString urlStr = url.toLocalFile();
                const QFileInfo fInfo(urlStr);
                const QString ext = fInfo.suffix();
                const auto filesHandler = FilesHandler::sInstance;
                if(fInfo.isDir()) {
                    filesHandler->getFileHandler<ImageSequenceFileHandler>(urlStr);
                } else if(isSoundExt(ext)) {
                    filesHandler->getFileHandler<SoundFileHandler>(urlStr);
                } else if(isImageExt(ext) || isLayersExt(ext)) {
                    filesHandler->getFileHandler<ImageFileHandler>(urlStr);
                } else if(isVideoExt(ext)) {
                    filesHandler->getFileHandler<VideoFileHandler>(urlStr);
                }
            }
        }
        event->acceptProposedAction();
    }
    Document::sInstance->actionFinished();
}

void FileSourceList::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

void FileSourceList::dragEnterEvent(QDragEnterEvent *event) {
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void FileCacheHandlerAbstraction::setSelected(const bool bT) {
    if(bT == fSelected) return;
    fSelected = bT;
    if(fSelected) {
        fParentVisibleWidget->addToSelectedList(this);
    } else {
        fParentVisibleWidget->removeFromSelectedList(this);
    }
}
