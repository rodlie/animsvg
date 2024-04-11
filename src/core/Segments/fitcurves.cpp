#include "fitcurves.h"

/*
An Algorithm for Automatically Fitting Digitized Curves
by Philip J. Schneider
from "Graphics Gems", Academic Press, 1990
*/

/*  fit_cubic.c	*/
/*	Piecewise cubic fitting code	*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

/* negates the input vector and returns it */
Vector2 *V2Negate(Vector2* const v)
//Vector2 *v;
{
    v->x = -v->x;  v->y = -v->y;
    return(v);
}

/* return the distance between two points */
double V2DistanceBetween2Points(Point2* const a, Point2* const b)
//Point2 *a, *b;
{
    double dx = a->x - b->x;
    double dy = a->y - b->y;
    return(sqrt((dx*dx)+(dy*dy)));
}

/* returns squared length of input vector */
double V2SquaredLength(Vector2* const a)
//Vector2 *a;
{
    return((a->x * a->x)+(a->y * a->y));
}

/* returns length of input vector */
double V2Length(Vector2* const a)
//Vector2 *a;
{
    return(sqrt(V2SquaredLength(a)));
}

/* scales the input vector to the new length and returns it */
Vector2 *V2Scale(Vector2* const v, const double newlen)
//Vector2 *v;
//double newlen;
{
    double len = V2Length(v);
    if (len != 0.0) { v->x *= newlen/len;   v->y *= newlen/len; }
    return(v);
}

/* return the dot product of vectors a and b */
double V2Dot(Vector2* const a, Vector2* const b)
//Vector2 *a, *b;
{
    return((a->x*b->x)+(a->y*b->y));
}

/* normalizes the input vector and returns it */
Vector2 *V2Normalize(Vector2* const v)
//Vector2 *v;
{
    double len = V2Length(v);
    if (len != 0.0) { v->x /= len;  v->y /= len; }
    return(v);
}

/* return vector sum c = a+b */
Vector2 *V2Add(Vector2* const a,
               Vector2* const b,
               Vector2* const c)
//Vector2 *a, *b, *c;
{
    c->x = a->x+b->x;  c->y = a->y+b->y;
    return(c);
}

/* Forward declarations */

static double FitCubic(Point2* const d,
                       const int first, const int last,
                       const double error,
                       const BezierHandler& bezierHandler,
                       const bool split);
static	double FitCubic(Point2* const d,
                        const int first, const int last,
                        Vector2 tHat1,
                        Vector2 tHat2,
                        const double error,
                        const BezierHandler& bezierHandler,
                        const bool split);
static	double		*Reparameterize(Point2* const d,
                                    const int first, const int last,
                                    double* const u,
                                    BezierCurve bezCurve);
static	double		NewtonRaphsonRootFind(BezierCurve Q,
                                          const Point2& P,
                                          const double u);
static	Point2		BezierII(const int degree,
                             Point2* const V,
                             const double t);
static	double 		B0(const double u), B1(const double u),
                    B2(const double u), B3(const double u);
static	Vector2		ComputeLeftTangent(Point2* const d, const int end);
static	Vector2		ComputeRightTangent(Point2* const d, const int end);
static	Vector2		ComputeCenterTangent(Point2* const d, const int center);
static	double		ComputeMaxError(Point2* const d,
                                    const int first, const int last,
                                    BezierCurve bezCurve,
                                    double* const u,
                                    int* const splitPoint);
static	double		*ChordLengthParameterize(Point2* const d,
                                             const int first, const int last);
static	BezierCurve	GenerateBezier(Point2* const d,
                                   const int first, const int last,
                                   double* const uPrime,
                                   Vector2 tHat1,
                                   Vector2 tHat2);
static	Vector2		V2AddII(const Vector2& a, const Vector2& b);
static	Vector2		V2ScaleIII(const Vector2& v, const double s);
static	Vector2		V2SubII(const Vector2& a, const Vector2& b);

/*
 *  FitCurve :
 *  	Fit a Bezier curve to a set of digitized points
 */

void FitCurve(Point2* const d, const double error,
              const BezierHandler& bezierHandler,
              const int min, const int max,
              const bool useTangents,
              const bool split)
//    Point2	*d;			/*  Array of digitized points	*/
//    int		nPts;		/*  Number of digitized points	*/
//    double	error;		/*  User-defined error squared	*/
{
    if(useTangents) {
        Vector2	tHat1, tHat2;	/*  Unit tangent vectors at endpoints */

        tHat1 = ComputeLeftTangent(d, min);
        tHat2 = ComputeRightTangent(d, max);
        FitCubic(d, min, max, tHat1, tHat2, error, bezierHandler, split);
    } else {
        FitCubic(d, min, max, error, bezierHandler, split);
    }
}

void FitCurves::FitCurve(QVector<QPointF>& data, const double error,
                         const BezierHandler& bezierHandler,
                         const bool useTangents,
                         const bool split)
{
    const auto pt2Data = reinterpret_cast<Point2*>(data.data());
    FitCurve(pt2Data, error, bezierHandler, 0, data.count() - 1, useTangents, split);
}

void FitCurves::FitCurve(QVector<QPointF>& data, const double error,
                         const BezierHandler& bezierHandler,
                         const int min, const int max,
                         const bool useTangents,
                         const bool split)
{
    const auto pt2Data = reinterpret_cast<Point2*>(data.data());
    FitCurve(pt2Data, error, bezierHandler, min, max, useTangents, split);
}


/*
 *  FitCubic :
 *  	Fit a Bezier curve to a (sub)set of digitized points
 */


static double FitCubic(Point2* const d,
                       const int first, const int last,
                       Vector2 tHat1,
                       Vector2 tHat2,
                       const double error,
                       const BezierHandler& bezierHandler,
                       const bool split,
                       int& splitPoint)
//    Point2	*d;			/*  Array of digitized points */
//    int		first, last;	/* Indices of first and last pts in region */
//    Vector2	tHat1, tHat2;	/* Unit tangent vectors at endpoints */
//    double	error;		/*  User-defined error squared	   */
{
    BezierCurve	bezCurve; /*Control points of fitted Bezier curve*/
    double	*u;		/*  Parameter values for point  */
    double	*uPrime;	/*  Improved parameter values */
    double	maxError;	/*  Maximum fitting error	 */
    int		nPts;		/*  Number of points in subset  */
    double	iterationError; /*Error below which you try iterating  */
    int		maxIterations = 8; /*  Max times to try iterating  */
    Vector2	tHatCenter;   	/* Unit tangent vector at splitPoint */
    int		i;

    iterationError = error * 4.0;	/* fixed issue 23 */
    nPts = last - first + 1;

    /*  Use heuristic if region only has two points in it */
    if (nPts == 2) {
        double dist = V2DistanceBetween2Points(&d[last], &d[first]) / 3.0;

        bezCurve = (Point2 *)malloc(4 * sizeof(Point2));
        bezCurve[0] = d[first];
        bezCurve[3] = d[last];
        V2Add(&bezCurve[0], V2Scale(&tHat1, dist), &bezCurve[1]);
        V2Add(&bezCurve[3], V2Scale(&tHat2, dist), &bezCurve[2]);
        bezierHandler(3, bezCurve);
        free((void *)bezCurve);
        return 0;
    }

    /*  Parameterize points, and attempt to fit curve */
    u = ChordLengthParameterize(d, first, last);
    bezCurve = GenerateBezier(d, first, last, u, tHat1, tHat2);

    /*  Find max deviation of points to fitted curve */
    maxError = ComputeMaxError(d, first, last, bezCurve, u, &splitPoint);

    /*  If error not too large, try some reparameterization  */
    /*  and iteration */
    if (maxError < iterationError || !split) {
        for (i = 0; i < maxIterations; i++) {
            uPrime = Reparameterize(d, first, last, u, bezCurve);
            free((void *)bezCurve);
            bezCurve = GenerateBezier(d, first, last, uPrime, tHat1, tHat2);
            maxError = ComputeMaxError(d, first, last,
                       bezCurve, uPrime, &splitPoint);
            if (maxError < error || !split) {
                bezierHandler(3, bezCurve);
                free((void *)u);
                free((void *)bezCurve);
                free((void *)uPrime);
                return maxError;
            }
            free((void *)u);
            u = uPrime;
        }
    }

    /* Fitting failed -- split at max error point and fit recursively */
    free((void *)u);
    free((void *)bezCurve);
    tHatCenter = ComputeCenterTangent(d, splitPoint);
    const double err1 = FitCubic(d, first, splitPoint, tHat1,
                                 tHatCenter, error, bezierHandler, true);
    V2Negate(&tHatCenter);
    const double err2 = FitCubic(d, splitPoint, last, tHatCenter,
                                 tHat2, error, bezierHandler, true);
    return std::max(err1, err2);
}

static double FitCubic(Point2* const d,
                       const int first, const int last,
                       Vector2 tHat1,
                       Vector2 tHat2,
                       const double error,
                       const BezierHandler& bezierHandler,
                       const bool split)
//    Point2	*d;			/*  Array of digitized points */
//    int		first, last;	/* Indices of first and last pts in region */
//    Vector2	tHat1, tHat2;	/* Unit tangent vectors at endpoints */
//    double	error;		/*  User-defined error squared	   */
{
    int		splitPoint;	/*  Point to split point set at	 */
    return FitCubic(d, first, last, tHat1, tHat2, error,
                    bezierHandler, split, splitPoint);
}

Point2 rotatePoint(const double angle, const Point2& p) {
  const double  s = std::sin(angle);
  const double  c = std::cos(angle);
  const double x = p.x * c - p.y * s;
  const double y = p.x * s + p.y * c;

  return {x, y};
}

static double FitCubic(Point2* const d,
                       const int first, const int last,
                       const double error,
                       const BezierHandler& bezierHandler,
                       const bool split)
//    Point2	*d;			/*  Array of digitized points */
//    int		first, last;	/* Indices of first and last pts in region */
//    double	error;		/*  User-defined error squared	   */
{
    double minError = DBL_MAX;
    int minErrorSplitPoint = 0;
    BezierCurve minErrorBez = (Point2 *)malloc(4 * sizeof(Point2));

    BezierCurve bez = (Point2 *)malloc(4 * sizeof(Point2));

    qreal r1Min = 0;
    qreal r1Max = 2*M_PI;
    qreal r2Min = 0;
    qreal r2Max = 2*M_PI;

    qreal bestR1 = r1Min;
    qreal bestR2 = r2Min;

    for(int inc = 0; inc < 4; inc++) {
        const qreal rInc = 0.25*M_PI/(inc + 1);
        for(qreal r1 = r1Min; r1 < r1Max; r1 += rInc) {
            for(qreal r2 = r2Min; r2 < r2Max; r2 += rInc) {
                const Vector2 tHat1 = rotatePoint(r1, Point2{1, 0});
                const Vector2 tHat2 = rotatePoint(r2, Point2{1, 0});
                int splitPoint;
                const auto bezSetter = [bez](const int, BezierCurve curve) {
                    memcpy(bez, curve, 4*sizeof(Point2));
                };
                const double err = FitCubic(d, first, last, tHat1, tHat2, error,
                                            bezSetter, false, splitPoint);
                if(err < minError) {
                    bestR1 = r1;
                    bestR2 = r2;
                    minError = err;
                    minErrorSplitPoint = splitPoint;
                    memcpy(minErrorBez, bez, 4*sizeof(Point2));
                }
            }
        }
        r1Min = bestR1 - rInc + rInc/2;
        r1Max = bestR1 + rInc;
        r2Min = bestR2 - rInc + rInc/2;
        r2Max = bestR2 + rInc;
    }
    free((void *)bez);

    if(minError < error || !split) {
        bezierHandler(3, minErrorBez);
        free((void *)minErrorBez);
        return minError;
    } else free((void *)minErrorBez);
    const double err1 = FitCubic(d, first, minErrorSplitPoint, error, bezierHandler, true);
    const double err2 = FitCubic(d, minErrorSplitPoint, last, error, bezierHandler, true);
    return std::max(err1, err2);
}


/*
 *  GenerateBezier :
 *  Use least-squares method to find Bezier control points for region.
 *
 */
static BezierCurve  GenerateBezier(Point2* const d,
                                   const int first, const int last,
                                   double* const uPrime,
                                   Vector2 tHat1,
                                   Vector2 tHat2)
//    Point2	*d;			/*  Array of digitized points	*/
//    int		first, last;		/*  Indices defining region	*/
//    double	*uPrime;		/*  Parameter values for region */
//    Vector2	tHat1, tHat2;	/*  Unit tangents at endpoints	*/
{
    int 	i;
    int 	nPts;			/* Number of pts in sub-curve */
    double 	C[2][2];			/* Matrix C		*/
    double 	X[2];			/* Matrix X			*/
    double 	det_C0_C1,		/* Determinants of matrices	*/
            det_C0_X,
            det_X_C1;
    double 	alpha_l,		/* Alpha values, left and right	*/
            alpha_r;
    Vector2 	tmp;			/* Utility variable		*/
    BezierCurve	bezCurve;	/* RETURN bezier curve ctl pts	*/
    double  segLength;
    double  epsilon;

    bezCurve = (Point2 *)malloc(4 * sizeof(Point2));
    nPts = last - first + 1;
    std::vector<Vector2> A;	/* Precomputed rhs for eqn	*/
    A.resize(nPts*2);

    /* Compute the A's	*/
    for (i = 0; i < nPts; i++) {
        Vector2		v1, v2;
        v1 = tHat1;
        v2 = tHat2;
        V2Scale(&v1, B1(uPrime[i]));
        V2Scale(&v2, B2(uPrime[i]));
        A[2*i] = v1;
        A[2*i + 1] = v2;
    }

    /* Create the C and X matrices	*/
    C[0][0] = 0.0;
    C[0][1] = 0.0;
    C[1][0] = 0.0;
    C[1][1] = 0.0;
    X[0]    = 0.0;
    X[1]    = 0.0;

    for (i = 0; i < nPts; i++) {
        C[0][0] += V2Dot(&A[2*i], &A[2*i]);
        C[0][1] += V2Dot(&A[2*i], &A[2*i + 1]);
/*					C[1][0] += V2Dot(&A[i][0], &A[i][1]);*/
        C[1][0] = C[0][1];
        C[1][1] += V2Dot(&A[2*i + 1], &A[2*i + 1]);

        tmp = V2SubII(d[first + i],
            V2AddII(
              V2ScaleIII(d[first], B0(uPrime[i])),
                V2AddII(
                    V2ScaleIII(d[first], B1(uPrime[i])),
                            V2AddII(
                            V2ScaleIII(d[last], B2(uPrime[i])),
                                V2ScaleIII(d[last], B3(uPrime[i]))))));


    X[0] += V2Dot(&A[2*i], &tmp);
    X[1] += V2Dot(&A[2*i + 1], &tmp);
    }

    /* Compute the determinants of C and X	*/
    det_C0_C1 = C[0][0] * C[1][1] - C[1][0] * C[0][1];
    det_C0_X  = C[0][0] * X[1]    - C[1][0] * X[0];
    det_X_C1  = X[0]    * C[1][1] - X[1]    * C[0][1];

    /* Finally, derive alpha values	*/
    alpha_l = (det_C0_C1 == 0) ? 0.0 : det_X_C1 / det_C0_C1;
    alpha_r = (det_C0_C1 == 0) ? 0.0 : det_C0_X / det_C0_C1;

    /* If alpha negative, use the Wu/Barsky heuristic (see text) */
    /* (if alpha is 0, you get coincident control points that lead to
     * divide by zero in any subsequent NewtonRaphsonRootFind() call. */
    segLength = V2DistanceBetween2Points(&d[last], &d[first]);
    epsilon = 1.0e-6 * segLength;
    if (alpha_l < epsilon || alpha_r < epsilon)
    {
        /* fall back on standard (probably inaccurate) formula, and subdivide further if needed. */
        double dist = segLength / 3.0;
        bezCurve[0] = d[first];
        bezCurve[3] = d[last];
        V2Add(&bezCurve[0], V2Scale(&tHat1, dist), &bezCurve[1]);
        V2Add(&bezCurve[3], V2Scale(&tHat2, dist), &bezCurve[2]);
        return (bezCurve);
    }

    /*  First and last control points of the Bezier curve are */
    /*  positioned exactly at the first and last data points */
    /*  Control points 1 and 2 are positioned an alpha distance out */
    /*  on the tangent vectors, left and right, respectively */
    bezCurve[0] = d[first];
    bezCurve[3] = d[last];
    V2Add(&bezCurve[0], V2Scale(&tHat1, alpha_l), &bezCurve[1]);
    V2Add(&bezCurve[3], V2Scale(&tHat2, alpha_r), &bezCurve[2]);
    return (bezCurve);
}


/*
 *  Reparameterize:
 *	Given set of points and their parameterization, try to find
 *   a better parameterization.
 *
 */
static double *Reparameterize(Point2* const d,
                              const int first, const int last,
                              double* const u,
                              BezierCurve bezCurve)
//    Point2	*d;			/*  Array of digitized points	*/
//    int		first, last;		/*  Indices defining region	*/
//    double	*u;			/*  Current parameter values	*/
//    BezierCurve	bezCurve;	/*  Current fitted curve	*/
{
    int 	nPts = last-first+1;
    int 	i;
    double	*uPrime;		/*  New parameter values	*/

    uPrime = (double *)malloc(nPts * sizeof(double));
    for (i = first; i <= last; i++) {
        uPrime[i-first] = NewtonRaphsonRootFind(bezCurve, d[i], u[i-first]);
    }
    return (uPrime);
}



/*
 *  NewtonRaphsonRootFind :
 *	Use Newton-Raphson iteration to find better root.
 */
static double NewtonRaphsonRootFind(BezierCurve Q,
                                    const Point2& P,
                                    const double u)
//    BezierCurve	Q;			/*  Current fitted curve	*/
//    Point2 		P;		/*  Digitized point		*/
//    double 		u;		/*  Parameter value for "P"	*/
{
    double 		numerator, denominator;
    Point2 		Q1[3], Q2[2];	/*  Q' and Q''			*/
    Point2		Q_u, Q1_u, Q2_u; /*u evaluated at Q, Q', & Q''	*/
    double 		uPrime;		/*  Improved u			*/
    int 		i;

    /* Compute Q(u)	*/
    Q_u = BezierII(3, Q, u);

    /* Generate control vertices for Q'	*/
    for (i = 0; i <= 2; i++) {
        Q1[i].x = (Q[i+1].x - Q[i].x) * 3.0;
        Q1[i].y = (Q[i+1].y - Q[i].y) * 3.0;
    }

    /* Generate control vertices for Q'' */
    for (i = 0; i <= 1; i++) {
        Q2[i].x = (Q1[i+1].x - Q1[i].x) * 2.0;
        Q2[i].y = (Q1[i+1].y - Q1[i].y) * 2.0;
    }

    /* Compute Q'(u) and Q''(u)	*/
    Q1_u = BezierII(2, Q1, u);
    Q2_u = BezierII(1, Q2, u);

    /* Compute f(u)/f'(u) */
    numerator = (Q_u.x - P.x) * (Q1_u.x) + (Q_u.y - P.y) * (Q1_u.y);
    denominator = (Q1_u.x) * (Q1_u.x) + (Q1_u.y) * (Q1_u.y) +
                  (Q_u.x - P.x) * (Q2_u.x) + (Q_u.y - P.y) * (Q2_u.y);
    if (denominator == 0.0f) return u;

    /* u = u - f(u)/f'(u) */
    uPrime = u - (numerator/denominator);
    return (uPrime);
}



/*
 *  Bezier :
 *  	Evaluate a Bezier curve at a particular parameter value
 *
 */
static Point2 BezierII(const int degree,
                       Point2* const V,
                       const double t)
//    int		degree;		/* The degree of the bezier curve	*/
//    Point2 	*V;		/* Array of control points		*/
//    double 	t;		/* Parametric value to find point for	*/
{
    int 	i, j;
    Point2 	Q;	        /* Point on curve at parameter t	*/
    Point2 	*Vtemp;		/* Local copy of control points		*/

    /* Copy array	*/
    Vtemp = (Point2 *)malloc((unsigned)((degree+1)
                * sizeof (Point2)));
    for (i = 0; i <= degree; i++) {
        Vtemp[i] = V[i];
    }

    /* Triangle computation	*/
    for (i = 1; i <= degree; i++) {
        for (j = 0; j <= degree-i; j++) {
            Vtemp[j].x = (1.0 - t) * Vtemp[j].x + t * Vtemp[j+1].x;
            Vtemp[j].y = (1.0 - t) * Vtemp[j].y + t * Vtemp[j+1].y;
        }
    }

    Q = Vtemp[0];
    free((void *)Vtemp);
    return Q;
}


/*
 *  B0, B1, B2, B3 :
 *	Bezier multipliers
 */
static double B0(const double u)
//    double	u;
{
    double tmp = 1.0 - u;
    return (tmp * tmp * tmp);
}


static double B1(const double u)
//    double	u;
{
    double tmp = 1.0 - u;
    return (3 * u * (tmp * tmp));
}

static double B2(const double u)
//    double	u;
{
    double tmp = 1.0 - u;
    return (3 * u * u * tmp);
}

static double B3(const double u)
//    double	u;
{
    return (u * u * u);
}



/*
 * ComputeLeftTangent, ComputeRightTangent, ComputeCenterTangent :
 *Approximate unit tangents at endpoints and "center" of digitized curve
 */
static Vector2 ComputeLeftTangent(Point2* const d, const int end)
//    Point2	*d;			/*  Digitized points*/
//    int		end;		/*  Index to "left" end of region */
{
    Vector2	tHat1;
    tHat1 = V2SubII(d[end+1], d[end]);
    tHat1 = *V2Normalize(&tHat1);
    return tHat1;
}

static Vector2 ComputeRightTangent(Point2* const d, const int end)
//    Point2	*d;			/*  Digitized points		*/
//    int		end;		/*  Index to "right" end of region */
{
    Vector2	tHat2;
    tHat2 = V2SubII(d[end-1], d[end]);
    tHat2 = *V2Normalize(&tHat2);
    return tHat2;
}


static Vector2 ComputeCenterTangent(Point2* const d, const int center)
//    Point2	*d;			/*  Digitized points			*/
//    int		center;		/*  Index to point inside region	*/
{
    Vector2	V1, V2, tHatCenter;

    V1 = V2SubII(d[center-1], d[center]);
    V2 = V2SubII(d[center], d[center+1]);
    tHatCenter.x = (V1.x + V2.x)/2.0;
    tHatCenter.y = (V1.y + V2.y)/2.0;
    tHatCenter = *V2Normalize(&tHatCenter);
    return tHatCenter;
}


/*
 *  ChordLengthParameterize :
 *	Assign parameter values to digitized points
 *	using relative distances between points.
 */
static double *ChordLengthParameterize(Point2* const d,
                                       const int first, const int last)
//    Point2	*d;			/* Array of digitized points */
//    int		first, last;		/*  Indices defining region	*/
{
    int		i;
    double	*u;			/*  Parameterization		*/

    u = (double *)malloc((unsigned)(last-first+1) * sizeof(double));

    u[0] = 0.0;
    for (i = first+1; i <= last; i++) {
        u[i-first] = u[i-first-1] +
                V2DistanceBetween2Points(&d[i], &d[i-1]);
    }

    for (i = first + 1; i <= last; i++) {
        u[i-first] = u[i-first] / u[last-first];
    }

    return(u);
}




/*
 *  ComputeMaxError :
 *	Find the maximum squared distance of digitized points
 *	to fitted curve.
*/
static double ComputeMaxError(Point2* const d,
                              const int first, const int last,
                              BezierCurve bezCurve,
                              double* const u,
                              int* const splitPoint)
//    Point2	*d;			/*  Array of digitized points	*/
//    int		first, last;		/*  Indices defining region	*/
//    BezierCurve	bezCurve;		/*  Fitted Bezier curve		*/
//    double	*u;			/*  Parameterization of points	*/
//    int		*splitPoint;		/*  Point of maximum error	*/
{
    int		i;
    double	maxDist;		/*  Maximum error		*/
    double	dist;		/*  Current error		*/
    Point2	P;			/*  Point on curve		*/
    Vector2	v;			/*  Vector from point to curve	*/

    *splitPoint = (last - first + 1)/2;
    maxDist = 0.0;
    for (i = first + 1; i < last; i++) {
        P = BezierII(3, bezCurve, u[i-first]);
        v = V2SubII(P, d[i]);
        dist = V2SquaredLength(&v);
        if (dist >= maxDist) {
            maxDist = dist;
            *splitPoint = i;
        }
    }
    return (maxDist);
}
static Vector2 V2AddII(const Vector2& a, const Vector2& b)
//    Vector2 a, b;
{
    Vector2	c;
    c.x = a.x + b.x;  c.y = a.y + b.y;
    return (c);
}
static Vector2 V2ScaleIII(const Vector2& v, const double s)
//    Vector2	v;
//    double	s;
{
    Vector2 result;
    result.x = v.x * s; result.y = v.y * s;
    return (result);
}

static Vector2 V2SubII(const Vector2& a, const Vector2& b)
//    Vector2	a, b;
{
    Vector2	c;
    c.x = a.x - b.x; c.y = a.y - b.y;
    return (c);
}
