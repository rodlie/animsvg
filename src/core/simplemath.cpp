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

#include "simplemath.h"
#include "skia/skqtconversions.h"

qreal signedSquare(const qreal val) {
    return val*val*SIGN(val);
}

qreal distSign(const QVector3D distPt) {
    const qreal val = signedSquare(distPt.x()) + signedSquare(distPt.y());
    if(val > 0) return sqrt(val);
    else return -sqrt(-val);
}

qreal gRandF(const qreal fMin, const qreal fMax) {
    qreal f = static_cast<qreal>(QRandomGenerator::global()->generate())/RAND_MAX;
    return fMin + f * (fMax - fMin);
}

float gSkRandF(const float fMin, const float fMax) {
    float f = static_cast<float>(QRandomGenerator::global()->generate())/(float)RAND_MAX;
    return fMin + f * (fMax - fMin);
}

qreal pointToLen(QVector3D point) {
    return qSqrt(point.x()*point.x() + point.y()*point.y());
}

float pointToLen(const SkPoint3 &point) {
    return SkScalarSqrt(point.x()*point.x() + point.y()*point.y());
}

QVector3D scalePointToNewLen(const QVector3D point,
                           const qreal newLen) {
    const qreal currLen = pointToLen(point);
    if(isZero4Dec(currLen)) return QVector3D(0, 0, 0);
    return point * newLen / currLen;
}

int clamp(const int val, const int min, const int max) {
    if(val > max) return max;
    if(val < min) return min;
    return val;
}

bool isZero4Dec(const float val) {
    return abs(val) < 0.0001f;
}

bool isZero4Dec(const double val) {
    return qAbs(val) < 0.0001;
}

bool isInteger4Dec(const double val) {
    return isZero4Dec(val - qRound(val));
}

double floor4Dec(const double val) {
    const double roundVal = qRound(val);
    if(isZero4Dec(val - roundVal)) return roundVal;
    return qFloor(val);
}

bool isZero6Dec(const double val) {
    return qAbs(val) < 0.000001;
}

bool isOne4Dec(const double val) {
    return isZero4Dec(val - 1);
}

bool isOne6Dec(const double val) {
    return isZero6Dec(val - 1);
}

bool isZero6Dec(const float val) {
    return qAbs(val) < 0.000001f;
}

bool isOne4Dec(const float val) {
    return isZero4Dec(val - 1);
}

bool isOne6Dec(const float val) {
    return isZero6Dec(val - 1);
}

bool isZeroOrOne6Dec(const double val) {
    if(isZero6Dec(val)) return true;
    if(isOne6Dec(val)) return true;
    return false;
}

bool isZero2Dec(const qreal val) {
    return qAbs(val) < 0.01;
}

bool isZero2Dec(const float val) {
    return qAbs(val) < 0.01f;
}

bool isZero1Dec(qreal val) {
    return qAbs(val) < 0.1;
}

bool isPointZero(QVector3D pos) {
    return pointToLen(pos) < 0.0001;
}

bool isNonZero(const float val) {
    return val > 0.0001f || val < - 0.0001f;
}

bool isNonZero(const double val) {
    return val > 0.0001 || val < - 0.0001;
}

void rotate(float rad_t, float *x_t, float *y_t) {
    float cos_hue = cosf(rad_t);
    float sin_hue = sinf(rad_t);
    float x_rotated_t = *x_t*cos_hue - *y_t*sin_hue;
    float y_rotated_t = *x_t*sin_hue + *y_t*cos_hue;
    *x_t = x_rotated_t;
    *y_t = y_rotated_t;
}

void rotate(qreal rad_t, qreal *x_t, qreal *y_t) {
    qreal cos_hue = cos(rad_t);
    qreal sin_hue = sin(rad_t);
    qreal x_rotated_t = *x_t*cos_hue - *y_t*sin_hue;
    qreal y_rotated_t = *x_t*sin_hue + *y_t*cos_hue;
    *x_t = x_rotated_t;
    *y_t = y_rotated_t;
}

unsigned char truncateU8(const int val) {
    if(val > 255) {
        return 255;
    } else if(val < 0) {
        return 0;
    }
    return static_cast<unsigned char>(val);
}

int clampInt(int val, int min, int max) {
    if(val > max) {
        return max;
    } else if(val < min) {
        return min;
    } else {
        return val;
    }
}

float clamp(float val, float min_t, float max_t) {
    if(val > max_t)
    {
        return max_t;
    }
    if(val < min_t)
    {
        return min_t;
    }
    return val;
}

double getAngleF(double x1, double y1, double x2, double y2) {
    double dot = x1*x2 + y1*y2;
    double det = x1*y2 - y1*x2;
    return atan2(det, dot)*RadToF + 0.5;
}

double getAngleDeg(double x1, double y1, double x2, double y2)
{
    double dot = x1*x2 + y1*y2;
    double det = x1*y2 - y1*x2;
    return atan2(det, dot)*RadToDeg + 180;
}

void normalize(qreal *x_t, qreal *y_t, qreal dest_len) {
    qreal x_val = *x_t;
    qreal y_val = *y_t;
    qreal curr_len = sqrt(x_val*x_val + y_val*y_val);
    *x_t = x_val*dest_len/curr_len;
    *y_t = y_val*dest_len/curr_len;
}



qreal qMin4(qreal v1, qreal v2, qreal v3, qreal v4) {
    return qMin(v1, qMin(v2, qMin(v3, v4) ) );
}

qreal qMax4(qreal v1, qreal v2, qreal v3, qreal v4) {
    return qMax(v1, qMax(v2, qMax(v3, v4) ) );
}

QRect3D QRect3D4Points(QVector3D p1, QVector3D c1,
                     QVector3D c2, QVector3D p2) {
    return QRect3D(QVector3D(qMin4(p1.x(), c1.x(), c2.x(), p2.x()),
                             qMin4(p1.y(), c1.y(), c2.y(), p2.y()),
                             0), // FIXME: z prop?
                  QVector3D(qMax4(p1.x(), c1.x(), c2.x(), p2.x()),
                            qMax4(p1.y(), c1.y(), c2.y(), p2.y()),
                            0)); // FIXME: z prop?
}

QVector3D rotateVector90Degrees(const QVector3D &pt) {
    return QVector3D(-pt.y(), pt.x(), pt.z()); // y is downwards
}

qreal degreesBetweenVectors(const QVector3D &pt1,
                            const QVector3D &pt2) {
    return radiansBetweenVectors(pt1, pt2)*180./M_PI;
}

qreal radiansBetweenVectors(const QVector3D &pt1,
                            const QVector3D &pt2) {
    qreal dot = pt1.x()*pt2.x() + pt1.y()*pt2.y();
    qreal det = pt1.x()*pt2.y() - pt1.y()*pt2.x();
    return atan2(det, dot);
}

QVector3D gRotPt(const QVector3D &pt, const qreal deg) {
    return {pt.x() * cos(deg*PI/180) - pt.y() * sin(deg*PI/180),
            pt.x() * sin(deg*PI/180) + pt.y() * cos(deg*PI/180)};
}

QVector3D gQPointFDisplace(const QVector3D pt, const qreal displ) {
    return QVector3D(pt.x() + gRandF(-displ, displ),
                     pt.y() + gRandF(-displ, displ),
                     pt.z() + gRandF(-displ, displ));
}
