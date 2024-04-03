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

#ifndef RENDEROUTPUTWIDGET_H
#define RENDEROUTPUTWIDGET_H

#include "ui_global.h"

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSpinBox>

class UI_EXPORT RenderOutputWidget : public QDialog {
    Q_OBJECT
public:
    explicit RenderOutputWidget(const int canvasWidth,
                                const int canvasHeight,
                                QWidget *parent = nullptr);

private:
    qreal mCurrentResolutionFrac = 1.;
    qreal getCurrentResolution();

    int mCanvasWidth;
    int mCanvasHeight;

    QHBoxLayout *mPathLayout;
    QLabel *mPathLabel;
    QPushButton *mSelectPathButton;

    QVBoxLayout *mMainLayout;

    QPushButton *mRenderButton;

    QHBoxLayout *mSizeLayout;
    QLabel *mWidthLabel;
    QSpinBox *mWidthSpinBox;
    QLabel *mHeightLabel;
    QSpinBox *mHeightSpinBox;

    QLabel *mResoultionLabel;
    QComboBox *mResolutionComboBox;

    void connectSignals();
    void disconnectSignals();
signals:
    void renderOutput(QString, qreal);
public slots:
    void emitRender();
    void chooseDir();
private slots:
    void updateSizeBoxesFromResolution();
    void updateSizeBoxesFromHeight();
    void updateSizeBoxesFromWidth();
};

#endif // RENDEROUTPUTWIDGET_H
