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

#ifndef H_WHEEL_SV_TRIANGLE_H
#define H_WHEEL_SV_TRIANGLE_H
#include <QOpenGLWidget>
#include "colorwidget.h"
#include "glhelpers.h"

enum VALUEFOCUS
{
    H,
    SV,
    NONE
};

class H_Wheel_SV_Triangle : public ColorWidget {
    Q_OBJECT
public:
    H_Wheel_SV_Triangle(QWidget *parent_t = nullptr);
private:
    void paintGL();
    void resizeGL(int w, int h);

    void drawTriangle();
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *e);
    bool isInTriangle(const QPoint &pos_t);
    bool isInWheel(const QPoint &pos_t);
    void wheelInteraction(const int x_t, const int y_t);
    void triangleInteraction(int x_t, int y_t);

    void wheelEvent(QWheelEvent *e);
    void drawWheel();

    GLuint wheel_dim = 128;
    GLuint wheel_thickness = 20;
    GLfloat triangle_width = 64;
    GLuint triangle_tex_width = 64;
    GLuint triangle_tex_height = 64;

    int update_c = 0;
    VALUEFOCUS value_focus = NONE;
    float outer_circle_r = 0;
    float inner_circle_r = 0;
};

#endif // H_WHEEL_SV_TRIANGLE_H
