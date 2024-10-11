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

#ifndef GLWINDOW_H
#define GLWINDOW_H

#include "ui_global.h"

#include <QOpenGLWidget>
#include "glhelpers.h"
#include <string>

#include "skia/skiaincludes.h"

#include <QResizeEvent>
#include <QOpenGLPaintDevice>

struct ShaderEffectCreator;
struct ShaderEffectProgram;

class UI_EXPORT GLWindow : public QOpenGLWidget, protected QGL33 {
public:
    GLWindow(QWidget * const parent = nullptr);
protected:
    virtual void renderSk(SkCanvas * const canvas) = 0;
    void resizeGL(int, int) final;
    void initializeGL() final;
    void paintGL() final;
    void showEvent(QShowEvent *e);

    void initialize();
    void bindSkia(const int w, const int h);
    void updateFix();

    bool mRebind = false;
    sk_sp<GrContext> mGrContext;
    sk_sp<SkSurface> mSurface;
    SkCanvas *mCanvas = nullptr;
};

#endif // GLWINDOW_H
