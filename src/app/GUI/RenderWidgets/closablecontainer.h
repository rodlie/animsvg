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

#ifndef CLOSABLECONTAINER_H
#define CLOSABLECONTAINER_H
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
class QCheckBox;

class ClosableContainer : public QWidget {
public:
    explicit ClosableContainer(QWidget *parent = nullptr);

    void setLabelWidget(QWidget *widget);
    void addContentWidget(QWidget *widget);
    void setCheckable(const bool check);
    void setChecked(const bool check);
    bool isChecked() const;

    void setContentVisible(const bool visible);
protected:
    QCheckBox *mCheckBox = nullptr;
    QHBoxLayout *mMainLayout = new QHBoxLayout();
    QVBoxLayout *mVLayout = new QVBoxLayout();
    QWidget *mLabelWidget = nullptr;
    QWidget* mContWidget = new QWidget(this);
    QVBoxLayout *mContLayout = new QVBoxLayout();
    QList<QWidget*> mContWidgets;
    QPushButton *mContentArrow = new QPushButton();
};

#endif // CLOSABLECONTAINER_H
