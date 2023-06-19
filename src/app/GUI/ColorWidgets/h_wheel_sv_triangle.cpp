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

#include "h_wheel_sv_triangle.h"
#include "GUI/mainwindow.h"
#include "colorhelpers.h"
#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QWindow>
#include <QApplication>

qreal sign(qreal x, qreal y, QPointF p2, QPointF p3) {
    return (x - p3.x()) * (p2.y() - p3.y()) - (p2.x() - p3.x()) * (y - p3.y());
}

bool pointInTriangle(qreal x, qreal y, QPointF v1, QPointF v2, QPointF v3) {
    bool b1, b2, b3;

    qreal q0 = 0;
    b1 = sign(x, y, v1, v2) < q0;
    b2 = sign(x, y, v2, v3) < q0;
    b3 = sign(x, y, v3, v1) < q0;

    return ((b1 == b2) && (b2 == b3));
}

bool insideCircle(int r, int x_t, int y_t) {
    return x_t*x_t + y_t*y_t < r*r;
}

bool outsideCircle(int r, int x_t, int y_t) {
    return !insideCircle(r, x_t, y_t);
}

H_Wheel_SV_Triangle::H_Wheel_SV_Triangle(QWidget *parent_t) :
    ColorWidget(parent_t) {
    setFixedSize(200, 200);
}

void H_Wheel_SV_Triangle::resizeGL(int w, int h) {
    wheel_dim = static_cast<uint>(std::min(w, h));
    outer_circle_r = wheel_dim*0.5f;
    inner_circle_r = outer_circle_r - wheel_thickness;
    triangle_width = sqrt_3f*inner_circle_r;
    triangle_tex_width = static_cast<GLuint>(ceil(inner_circle_r*2));
    triangle_tex_height = static_cast<GLuint>(ceil(1.5f*inner_circle_r));
    ColorWidget::resizeGL(w, h);
}

void H_Wheel_SV_Triangle::drawTriangle() {

}

void H_Wheel_SV_Triangle::paintGL() {

}

void H_Wheel_SV_Triangle::wheelEvent(QWheelEvent *e) {
    if(e->delta() > 0) {
        mHue += 0.01;
        if(mHue > 1)
        {
            mHue -= 1;
        }
    } else {
        mHue -= 0.01;
        if(mHue < 0)
        {
            mHue += 1;
        }
    }
    //drawTriangle();
    update();
}

void H_Wheel_SV_Triangle::drawWheel()
{

}

void H_Wheel_SV_Triangle::wheelInteraction(const int x_t, const int y_t) {
    double radial_x = x_t - wheel_dim*0.5;
    double radial_y = y_t - wheel_dim*0.5;
    mHue = getAngleF(1, 0, -radial_x, radial_y);
    //drawTriangle();
    update();
}

void H_Wheel_SV_Triangle::triangleInteraction(int x_t, int y_t) {
    x_t--;
    y_t--;
    x_t -= wheel_thickness;
    y_t -= wheel_thickness;

    float tr_x_t = x_t - inner_circle_r;
    float tr_y_t = y_t - inner_circle_r;
    float hue_rad = (mHue + 1/12.f)*2*PIf;
    rotate(hue_rad, &tr_x_t, &tr_y_t);
    tr_x_t += inner_circle_r;
    tr_y_t += inner_circle_r;
    float row_width_t = tr_y_t*2/sqrt_3f;
    float row_x_0 = inner_circle_r - row_width_t*0.5f;

    mSaturation = clamp( (tr_x_t - row_x_0)/row_width_t , 0.f, 1.f);
    mValue = clamp(tr_y_t/(inner_circle_r*1.5f), 0.f, 1.f);

    update();
}

void H_Wheel_SV_Triangle::mousePressEvent(QMouseEvent *e) {
    mValueBlocked = true;

    if(e->button() == Qt::RightButton) {
        return;
    }
    grabMouse();
    if(isInTriangle(e->pos())) {
        value_focus = SV;
        triangleInteraction(e->x(), e->y() );
    } else if(isInWheel(e->pos())) {
        value_focus = H;
        wheelInteraction(e->x(), e->y() );
    } else {
        value_focus = NONE;
        return;
    }
    QApplication::setOverrideCursor(
                QCursor(QPixmap("pixmaps/cursor_crosshair_open.png") ) );
}

void H_Wheel_SV_Triangle::mouseReleaseEvent(QMouseEvent *) {
    releaseMouse();
    QApplication::restoreOverrideCursor();
    value_focus = NONE;
    mValueBlocked = false;
}

void H_Wheel_SV_Triangle::mouseMoveEvent(QMouseEvent *e) {
    if(value_focus == H) {
        wheelInteraction(e->x(), e->y());
    } else if(value_focus == SV) {
        triangleInteraction(e->x(), e->y());
    }
}

bool H_Wheel_SV_Triangle::isInWheel(const QPoint& pos_t) {
    int rel_center_x = pos_t.x() - static_cast<int>(outer_circle_r);
    int rel_center_y = pos_t.y() - static_cast<int>(outer_circle_r);
    if(!insideCircle(static_cast<int>(outer_circle_r),
                     rel_center_x, rel_center_y)) return false;
    return outsideCircle(static_cast<int>(inner_circle_r),
                         rel_center_x, rel_center_y);
}

bool H_Wheel_SV_Triangle::isInTriangle(const QPoint& pos_t) {
    float x_mar = (triangle_tex_width - triangle_width)*0.5f + wheel_thickness;
    float y_mar = wheel_thickness;
    float x1_t = triangle_width*0.5f + x_mar - outer_circle_r;
    float y1_t = y_mar - outer_circle_r;
    QPointF v1 = QPointF(static_cast<qreal>(x1_t),
                         static_cast<qreal>(y1_t));
    float x2_t = x_mar - outer_circle_r;
    float y2_t = triangle_tex_height + y_mar - outer_circle_r;
    QPointF v2 = QPointF(static_cast<qreal>(x2_t),
                         static_cast<qreal>(y2_t));
    float x3_t = triangle_width + x_mar - outer_circle_r;
    float y3_t = triangle_tex_height + y_mar - outer_circle_r;
    QPointF v3 = QPointF(static_cast<qreal>(x3_t),
                         static_cast<qreal>(y3_t));

    float x_t = pos_t.x() - outer_circle_r;
    float y_t = pos_t.y() - outer_circle_r;
    float hue_rad = (mHue + 1/12.f)*2*PIf;
    rotate(hue_rad, &x_t, &y_t);
    return pointInTriangle(static_cast<qreal>(x_t),
                           static_cast<qreal>(y_t),
                           v1, v2, v3);
}
