#ifndef FITCURVES_H
#define FITCURVES_H

#include <functional>
#include <QtCore>

#include "../core_global.h"

typedef struct Point2Struct {	/* 2d point */
    double x, y;
    } Point2;
typedef Point2 Vector2;
typedef Point2 *BezierCurve;
typedef std::function<void(const int n, const BezierCurve curve)> BezierHandler;

namespace FitCurves {
    CORE_EXPORT
    extern void FitCurve(QVector<QVector3D>& data, const double error,
                         const BezierHandler& bezierHandler,
                         const bool useTangents,
                         const bool split);
    CORE_EXPORT
    extern void FitCurve(QVector<QVector3D>& data, const double error,
                         const BezierHandler& bezierHandler,
                         const int min, const int max,
                         const bool useTangents,
                         const bool split);

};

#endif // FITCURVES_H
