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

#include "brushselectionwidget.h"
#include <QDir>
#include <QMenu>
#include "flowlayout.h"
#include "brushwidget.h"
#include <QDebug>
#include <QDockWidget>
#include "GUI/BoxesList/OptimalScrollArea/scrollarea.h"
#include "Private/document.h"

bool BrushSelectionWidget::sLoaded = false;
qsptr<BrushesContext> BrushSelectionWidget::sPaintContext;
qsptr<BrushesContext> BrushSelectionWidget::sOutlineContext;

BrushSelectionWidget::BrushSelectionWidget(BrushesContext& context,
                                           QWidget * const parent) :
    QTabWidget(parent), mContext(context) {
    setSizePolicy(QSizePolicy::Preferred, sizePolicy().verticalPolicy());

    //setupBookmarksTab();
    //updateBrushes();
}

void BrushSelectionWidget::setupBookmarksTab() {
    /*mBookmarksScroll = new ScrollArea(this);
    const auto tabWidget = new QWidget(this);
    const auto tabWidgetLay = new FlowLayout(tabWidget, 0, 0, 0);
    connect(&mContext, &BrushesContext::bookmarkAdded,
            this, [this, tabWidgetLay, tabWidget](BrushContexedWrapper* const brush) {
        const auto bWidget = new BrushWidget(brush, tabWidget);
        connect(bWidget, &BrushWidget::selected,
                this, &BrushSelectionWidget::brushCWrapperSelected);
        connect(&mContext, &BrushesContext::bookmarkRemoved,
                bWidget, [this, bWidget](BrushContexedWrapper* const brush) {
            if(brush == bWidget->getBrush()) {
                bWidget->deleteLater();
                setNumberBookmarked(mNumberBookmarked - 1);
            }
        });

        tabWidgetLay->addWidget(bWidget);
        setNumberBookmarked(mNumberBookmarked + 1);
    });

    tabWidget->setLayout(tabWidgetLay);
    mBookmarksScroll->setWidget(tabWidget);
    mBookmarksScroll->hide();*/
}

void BrushSelectionWidget::setNumberBookmarked(const int bookmarked) {
    Q_UNUSED(bookmarked)
    /*if(mNumberBookmarked == bookmarked) return;
    if(mNumberBookmarked == 0 && bookmarked > 0) {
        insertTab(0, mBookmarksScroll, "Bookmarks");
        mBookmarksScroll->show();
    } else if(bookmarked == 0) {
        removeTab(0);
        mBookmarksScroll->hide();
    }
    mNumberBookmarked = bookmarked;*/
}

void BrushSelectionWidget::updateBrushes() {
    /*for(const auto& coll : mContext.fCollections) {
        const auto tabScroll = new ScrollArea(this);
        const auto tabWidget = new QWidget(this);
        const auto tabWidgetLay = new FlowLayout(tabWidget, 0, 0, 0);
        for(const auto& brush : coll.fBrushes) {
            const auto bWidget = new BrushWidget(brush.get(), tabWidget);
            connect(bWidget, &BrushWidget::selected,
                    this, &BrushSelectionWidget::brushCWrapperSelected);
            connect(bWidget, &BrushWidget::triggered,
                    this, &BrushSelectionWidget::brushTriggered);
            tabWidgetLay->addWidget(bWidget);
        }
        tabWidget->setLayout(tabWidgetLay);
        tabScroll->setWidget(tabWidget);
        addTab(tabScroll, coll.fName);
    }*/
}

qsptr<BrushesContext> BrushSelectionWidget::sCreateNewContext() {
    /*if(!sLoaded) {
        const QString brushesDir = eSettings::sSettingsDir() + "/brushes";
        sLoadCollectionsFromDir(brushesDir);
        sLoadCollectionsFromDir(":/brushes");
        sLoaded = true;
    }
    return enve::make_shared<BrushesContext>(BrushCollectionData::sData);*/
    return nullptr;
}

void BrushSelectionWidget::setCurrentBrush(SimpleBrushWrapper * const wrapper) {
    Q_UNUSED(wrapper)
    //if(mSelected && mSelected->getSimpleBrush() == wrapper) return;
    //mContext.setSelectedWrapper(wrapper);
}

SimpleBrushWrapper *BrushSelectionWidget::getCurrentBrush() {
    //if(mSelected)
        //return mSelected->getSimpleBrush();
    return nullptr;
}

void BrushSelectionWidget::brushCWrapperSelected(BrushContexedWrapper *wrapper) {
    Q_UNUSED(wrapper)
    /*if(mSelected && mSelected != wrapper)
        mSelected->setSelected(false);
    mSelected = wrapper;
    emit currentBrushChanged(wrapper);*/
}

void loadBrushFromFile(const QString &path, BrushCollectionData& coll) {
    Q_UNUSED(path)
    Q_UNUSED(coll)
    /*QFile dataFile(path);
    if(!dataFile.exists()) return;
    if(!dataFile.open(QIODevice::ReadOnly)) return;
    const QByteArray wholeFile = dataFile.readAll();
    const char *data = wholeFile.constData();
    dataFile.close();

    MyPaintBrush* const brush = mypaint_brush_new();
    if(!mypaint_brush_from_string(brush, data)) {
        mypaint_brush_unref(brush);
        return;
    }

    QString iconFileName = path;
    iconFileName.replace(".myb", "_prev.png");
    QImage icon;
    if(icon.load(iconFileName)) {
        icon = icon.scaled(64, 64, Qt::IgnoreAspectRatio,
                           Qt::SmoothTransformation);
    }

    const QFileInfo fileInfo(dataFile);
    const auto brushName = fileInfo.baseName();
    auto brushWrapper = enve::make_shared<SimpleBrushWrapper>(
                coll.fName, brushName,
                brush, wholeFile);
    coll.fBrushes.append({ brushName, brushWrapper,
                           icon, wholeFile });*/
}

void loadCollectionFromDir(const QString &mainDirPath,
                           QList<BrushCollectionData> &data) {
    Q_UNUSED(mainDirPath)
    Q_UNUSED(data)
    /*BrushCollectionData collection;
    const QDir brushesDir(mainDirPath);
    collection.fName = brushesDir.dirName();
    const QDir::Filters filter = QDir::NoDotAndDotDot | QDir::AllEntries;
    const QFileInfoList entryList = brushesDir.entryInfoList(filter);
    for(const QFileInfo& fileInfo : entryList) {
        if(fileInfo.isDir()) {
            QString collDirPath = fileInfo.absoluteFilePath();
            loadCollectionFromDir(collDirPath, data);
        } else if(fileInfo.completeSuffix() == "myb") {
            loadBrushFromFile(fileInfo.absoluteFilePath(), collection);
        }
    }
    data << collection;*/
}

void BrushSelectionWidget::sLoadCollectionsFromDir(const QString &mainDirPath) {
    Q_UNUSED(mainDirPath)
    /*const QDir brushesDir(mainDirPath);
    if(!brushesDir.exists()) brushesDir.mkpath(mainDirPath);
    const QDir::Filters filter = QDir::NoDotAndDotDot | QDir::AllEntries;
    const QFileInfoList entryList = brushesDir.entryInfoList(filter);
    for(const QFileInfo& fileInfo : entryList) {
        if(fileInfo.isDir()) {
            const QString collName = fileInfo.fileName();
            const QString collDirPath = fileInfo.absoluteFilePath();
            loadCollectionFromDir(collDirPath, BrushCollectionData::sData);
        }
    }*/
}
