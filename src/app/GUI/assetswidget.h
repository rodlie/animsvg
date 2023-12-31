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

#ifndef ASSETSWIDGET_H
#define ASSETSWIDGET_H

#include <QObject>
#include <QWidget>
#include <QList>
#include <QDebug>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>

#include "FileCacheHandlers/filecachehandler.h"
#include "fileshandler.h"

class AssetsTreeWidget : public QTreeWidget
{
public:
    explicit AssetsTreeWidget(QWidget *parent);
    void addAssets(const QList<QUrl> &urls);

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
};

class AssetsWidgetItem : public QTreeWidgetItem
{
public:
    explicit AssetsWidgetItem(QTreeWidget *parent = nullptr,
                              FileCacheHandler *cache = nullptr);
    FileCacheHandler* getCache();

private:
    FileCacheHandler *mCache;
    void setCache(const QString &path,
                  bool missing);
};

class AssetsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AssetsWidget(QWidget *parent = nullptr);
    void addCacheHandler(FileCacheHandler *handler);
    void removeCacheHandler(FileCacheHandler *handler);
    void addAssets(const QList<QUrl> &urls);

private:
    AssetsTreeWidget *mTree;
    QList<FileCacheHandler*> mCacheList;
};

#endif // ASSETSWIDGET_H
