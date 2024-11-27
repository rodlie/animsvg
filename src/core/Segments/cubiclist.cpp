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

#include "cubiclist.h"
#include "pointhelpers.h"

CubicList::CubicList() {

}

CubicList::CubicList(const QList<qCubicSegment2D> &segs) {
    mSegments = segs;
    updateClosed();
}

CubicList::CubicList(const CubicList &src) {
    mSegments = src.getSegments();
    updateClosed();
}

CubicList CubicList::getFragment(const qreal minLenFrac,
                                 const qreal maxLenFrac) const {
    if(minLenFrac > maxLenFrac) return CubicList();
    const qreal totLen = getTotalLength();
    if(isZero4Dec(totLen)) return *this;
    //Q_ASSERT(minLenFrac > 0);
    //Q_ASSERT(maxLenFrac < 1);
    const qreal minLen = totLen*CLAMP01(minLenFrac);
    const qreal maxLen = totLen*CLAMP01(maxLenFrac);
    qreal minT = 0, maxT = 1;
    int minI = -1, maxI = mSegments.count() - 1;
    qreal currLen = 0;
    const int iMax = mSegments.count();
    for(int i = 0; i < iMax; i++) {
        auto& seg = mSegments[i];
        const qreal lastLen = currLen;
        currLen += seg.length();
        if(minI == -1) {
            if(currLen > minLen) {
                minT = seg.tAtLength(minLen - lastLen);
                minI = i;
            }
        }
        if(currLen > maxLen) {
            maxT = seg.tAtLength(maxLen - lastLen);
            maxI = i;
            break;
        }
    }
    if(minI == -1) return CubicList();

    QList<qCubicSegment2D> fragSegs;
    if(minI == maxI) {
        fragSegs << mSegments[minI].tFragment(minT, maxT);
    } else {
        fragSegs << mSegments[minI].dividedAtT(minT).second;
        for(int i = minI + 1; i < maxI; i++) {
            fragSegs << static_cast<qCubicSegment2D>(mSegments[i]);
        }
        fragSegs << mSegments[maxI].dividedAtT(maxT).first;
    }
    return CubicList(fragSegs);
}

CubicList CubicList::getFragmentUnbound(const qreal minLenFrac,
                                        const qreal maxLenFrac) const {
    if(minLenFrac > maxLenFrac) return CubicList();
    const qreal shiftToPos = floor4Dec(minLenFrac);
    const qreal posMinLenFrac = minLenFrac - shiftToPos; // always between 0 and 1
    const qreal posMaxLenFrac = maxLenFrac - shiftToPos;
    QList<qCubicSegment2D> fragSegs;
    if(isInteger4Dec(posMinLenFrac) && isInteger4Dec(posMaxLenFrac)) {
        const int nFull = qRound(qAbs(posMaxLenFrac - posMinLenFrac));
        for(int i = 0; i < nFull; i++) fragSegs << mSegments;
        return fragSegs;
    }
    const qreal minRel = posMinLenFrac - qFloor(posMinLenFrac);
    const qreal maxRel = posMaxLenFrac - qFloor(posMaxLenFrac);
    int nFull = qFloor(posMaxLenFrac - posMinLenFrac - maxRel + minRel - 1);
    if(isZero4Dec(minLenFrac) && maxLenFrac > 1) nFull++;

    if(nFull <= 0 && qFloor(posMinLenFrac) == qFloor(posMaxLenFrac)) {
        return getFragment(minRel, maxRel).getSegments();
    }

    if(!isInteger4Dec(posMinLenFrac)) {
        fragSegs << getFragment(minRel, 1).getSegments();
    }

    for(int i = 0; i < nFull; i++) fragSegs << mSegments;

    if(!isInteger4Dec(posMaxLenFrac)) {
        fragSegs << getFragment(0, maxRel).getSegments();
    }
    return fragSegs;
}

QList<CubicList> CubicList::sMakeFromSkPath(const SkPath &src) {
    QList<CubicList> result;
    QList<qCubicSegment2D> segs;

    QPointF lastMovePos;
    QPointF lastPos;
    SkPath::Iter iter(src, false);
    for(;;) {
        SkPoint pts[4];
        switch(iter.next(pts)) {
        case SkPath::kLine_Verb: {
            QPointF pt1 = toQPointF(pts[1]);
            segs << qCubicSegment2D(lastPos, lastPos, pt1, pt1);
            lastPos = pt1;
        } break;
        case SkPath::kQuad_Verb: {
            QPointF pt2 = toQPointF(pts[2]);
            segs << qCubicSegment2D::sFromQuad(lastPos, toQPointF(pts[1]), pt2);
            lastPos = pt2;
        } break;
        case SkPath::kConic_Verb: {
            QPointF pt2 = toQPointF(pts[2]);
            segs << qCubicSegment2D::sFromConic(lastPos, toQPointF(pts[1]), pt2,
                                               toQreal(iter.conicWeight()));
            lastPos = pt2;
        } break;
        case SkPath::kCubic_Verb: {
            QPointF pt3 = toQPointF(pts[3]);
            segs << qCubicSegment2D(lastPos,
                                    toQPointF(pts[1]),
                                    toQPointF(pts[2]),
                                    pt3);
            lastPos = pt3;
        } break;
        case SkPath::kClose_Verb: {
//            SkCubicSegment2D seg{lastPos, lastPos, lastMovePos, lastMovePos};
//            segs << qCubicSegment2D(seg);
        } break;
        case SkPath::kMove_Verb: {
            if(!segs.isEmpty()) {
                result << segs;
                segs.clear();
            }
            lastMovePos = toQPointF(pts[0]);
            lastPos = lastMovePos;
        } break;
        case SkPath::kDone_Verb: {
            if(!segs.isEmpty()) {
                result << segs;
                segs.clear();
            }
            return result;
        }
        }
    }
}

qreal CubicList::getTotalLength() const {
    if(!mTotalLengthUpToDate) updateTotalLength();
    return mTotalLength;
}

bool CubicList::isEmpty() const { return mSegments.isEmpty(); }

qreal CubicList::minDistanceTo(const QPointF &p,
                               qreal * const pBestT,
                               QPointF * const pBestPos) const {
    qreal bestT = 0;
    QPointF bestPos;
    qreal smallestDist = DBL_MAX;
    const int iMax = mSegments.count();
    for(int i = 0; i < iMax; i++) {
        auto& seg = mSegments[i];
        qreal thisT;
        QPointF thisPos;
        qreal thisDist = seg.minDistanceTo(p, &thisT, &thisPos);
        if(thisDist < smallestDist) {
            bestT = thisT;
            bestPos = thisPos;
            smallestDist = thisDist;
        }
    }
    if(pBestT) *pBestT = bestT;
    if(pBestPos) *pBestPos = bestPos;

    return smallestDist;
}

void CubicList::opSmoothOut(const qreal smoothness) {
    if(mSegments.count() < 2) return;
    qCubicSegment2D * prevSeg = nullptr;
    QPointF lastC2;
    if(mClosed) {
        prevSeg = &mSegments[mSegments.count() - 2];
        auto& seg = mSegments.last();
        QPointF c1 = prevSeg->c2();
        QPointF c2 = seg.c1();
        gSmoothyAbsCtrlsForPtBetween(prevSeg->p0(), seg.p0(), seg.p3(),
                                     c1, c2, smoothness);
        lastC2 = c2;
        prevSeg = &seg;
    }

    for(int i = 0; i < mSegments.count(); i++) {
        auto& seg = mSegments[i];
        if(!mClosed) {
            if(i == 0) {
                prevSeg = &seg;
                lastC2 = prevSeg->c1();
                continue;
            }
        }
        QPointF c1 = prevSeg->c2();
        QPointF c2 = seg.c1();
        gSmoothyAbsCtrlsForPtBetween(prevSeg->p0(), seg.p0(), seg.p3(),
                                     c1, c2, smoothness);
        prevSeg->setC1(lastC2);
        prevSeg->setC2(c1);
        lastC2 = c2;
        prevSeg = &seg;
    }
    if(!mClosed) {
        auto& seg = mSegments.last();
        QPointF c1;
        if(smoothness > 0) {
            c1 = (seg.c2() - seg.p3())*(1 - smoothness) + seg.p3();
        } else {
            c1 = (seg.c2() - seg.p3())*(1 + smoothness) + seg.p3();
        }
        seg.setC1(lastC2);
        seg.setC2(c1);
    }
}

void CubicList::subdivide(const int sub) {
    if(sub <= 0) return;
    for(int i = 0; i < mSegments.count(); i++) {
        auto& seg = mSegments[i];
        qreal totLen = seg.length();
        qreal lenFrac = totLen/(sub + 1);
        auto remSeg = seg;
        for(int j = 1; j <= static_cast<int>(sub); j++) {
            auto divided = remSeg.dividedAtT(seg.tAtLength(lenFrac));
            auto newSeg = divided.first;
            remSeg = divided.second;
            mSegments.insert(i + j, newSeg);
        }
        mSegments.insert(i + sub + 1, remSeg);
        mSegments.removeAt(i);
        i += sub;
    }
}

void CubicList::updateTotalLength() const {
    mTotalLengthUpToDate = true;
    mTotalLength = 0;
    for(auto& seg : mSegments)
        mTotalLength += seg.length();
}

void CubicList::updateClosed() {
    if(mSegments.isEmpty()) {
        mClosed = false;
        return;
    }
    const auto& firstSeg = mSegments.first();
    const auto& lastSeg = mSegments.last();

    mClosed = pointToLen(firstSeg.p0() - lastSeg.p3()) < 0.1;
}

int cubicBezierLine(const qCubicSegment2D &seg,
                    const QLineF& line,
                    QList<QPointF>& result);

bool ascendingX(const QPointF& pt1, const QPointF& pt2) {
    return pt1.x() < pt2.x();
}

bool descendingX(const QPointF& pt1, const QPointF& pt2) {
    return pt1.x() > pt2.x();
}

bool ascendingY(const QPointF& pt1, const QPointF& pt2) {
    return pt1.y() < pt2.y();
}

bool descendingY(const QPointF& pt1, const QPointF& pt2) {
    return pt1.y() > pt2.y();
}

int CubicList::lineIntersections(const QLineF &line,
                                 QList<QPointF> &pts) const {
    if(isZero6Dec(line.length())) return 0;
    for(const auto& seg : mSegments) cubicBezierLine(seg, line, pts);
    if(isZero6Dec(line.dx())) {
        if(line.dy() > 0) {
            std::sort(pts.begin(), pts.end(), ascendingY);
        } else {
            std::sort(pts.begin(), pts.end(), descendingY);
        }
    } else {
        if(line.dx() > 0) {
            std::sort(pts.begin(), pts.end(), ascendingX);
        } else {
            std::sort(pts.begin(), pts.end(), descendingX);
        }
    }
    return pts.count();
}

int polySolveCubic(double a, double b, double c,
                   double *x0, double *x1, double *x2);

int cubicBezierLine(const qCubicSegment2D& seg,
                    const QLineF& line,
                    QList<QPointF>& result) {
    const QRectF segBB = seg.ptsBoundingRect();
    if(segBB.contains(line.p1()) || segBB.contains(line.p2())) {
    } else if(line.intersects(QLineF(segBB.topLeft(), segBB.bottomLeft()),
                              nullptr) == QLineF::BoundedIntersection) {
    } else if(line.intersects(QLineF(segBB.topLeft(), segBB.topRight()),
                              nullptr) == QLineF::BoundedIntersection) {
    } else if(line.intersects(QLineF(segBB.topRight(), segBB.bottomRight()),
                              nullptr) == QLineF::BoundedIntersection) {
    } else if(line.intersects(QLineF(segBB.bottomRight(), segBB.bottomLeft()),
                              nullptr) == QLineF::BoundedIntersection) {
    } else return 0;

    const qreal lX1 = line.x1();
    const qreal lX2 = line.x2();
    const qreal lY1 = line.y1();
    const qreal lY2 = line.y2();

//    const qreal minX = qMin(lX1, lX2);
//    const qreal minY = qMin(lY1, lY2);
//    const qreal maxX = qMax(lX1, lX2);
//    const qreal maxY = qMax(lY1, lY2);

    const qreal lA = lY2 - lY1;
    const qreal lB = lX1 - lX2;
    const qreal lC = lX1*(lY1 - lY2) + lY1*(lX2 - lX1);

    const qreal x0 = seg.p0().x();
    const qreal y0 = seg.p0().y();
    const qreal x1 = seg.c1().x();
    const qreal y1 = seg.c1().y();
    const qreal x2 = seg.c2().x();
    const qreal y2 = seg.c2().y();
    const qreal x3 = seg.p3().x();
    const qreal y3 = seg.p3().y();

    const qreal t3 = -lA*x0 + 3*lA*x1 - 3*lA*x2 + lA*x3 - lB*y0 + 3*lB*y1 - 3*lB*y2 + lB*y3;
    const qreal t2 = 3*lA*x0 - 6*lA*x1 + 3*lA*x2 + 3*lB*y0 - 6*lB*y1 + 3*lB*y2;
    const qreal t1 = -3*lA*x0 + 3*lA*x1 - 3*lB*y0 + 3*lB*y1;
    const qreal t0 = lC + lA*x0 + lB*y0;

    const qreal a = t2/t3;
    const qreal b = t1/t3;
    const qreal c = t0/t3;

    qreal sols[3];
    const int nSols = polySolveCubic(a, b, c, &sols[0], &sols[1], &sols[2]);
    int nSolsInRange = 0;
    for(int i = 0; i < nSols; i++) {
        const qreal& sol = sols[i];
        if(sol < 0 || sol > 1) continue;
        bool degenerate = false;
        for(int j = 0; j < i; j++) {
            const qreal& jSol = sols[j];
            if(isZero6Dec(jSol - sol)) {
                degenerate = true;
                break;
            }
        }
        if(degenerate) break;
        result.append(seg.posAtT(sol));
        nSolsInRange++;
    }

    return nSolsInRange;
}



#define SWAP(a,b) do { double tmp = b ; b = a ; a = tmp ; } while(0)

int polySolveCubic(double a, double b, double c,
                   double *x0, double *x1, double *x2) {
  double q = (a * a - 3 * b);
  double r = (2 * a * a * a - 9 * a * b + 27 * c);

  double Q = q / 9;
  double R = r / 54;

  double Q3 = Q * Q * Q;
  double R2 = R * R;

  double CR2 = 729 * r * r;
  double CQ3 = 2916 * q * q * q;

  if (R == 0 && Q == 0)
    {
      *x0 = - a / 3 ;
      *x1 = - a / 3 ;
      *x2 = - a / 3 ;
      return 3 ;
    }
  else if (CR2 == CQ3)
    {
      /* this test is actually R2 == Q3, written in a form suitable
         for exact computation with integers */

      /* Due to finite precision some double roots may be missed, and
         considered to be a pair of complex roots z = x +/- epsilon i
         close to the real axis. */

      double sqrtQ = sqrt (Q);

      if (R > 0)
        {
          *x0 = -2 * sqrtQ  - a / 3;
          *x1 = sqrtQ - a / 3;
          *x2 = sqrtQ - a / 3;
        }
      else
        {
          *x0 = - sqrtQ  - a / 3;
          *x1 = - sqrtQ - a / 3;
          *x2 = 2 * sqrtQ - a / 3;
        }
      return 3 ;
    }
  else if (R2 < Q3)
    {
      double sgnR = (R >= 0 ? 1 : -1);
      double ratio = sgnR * sqrt (R2 / Q3);
      double theta = acos (ratio);
      double norm = -2 * sqrt (Q);
      *x0 = norm * cos (theta / 3) - a / 3;
      *x1 = norm * cos ((theta + 2.0 * M_PI) / 3) - a / 3;
      *x2 = norm * cos ((theta - 2.0 * M_PI) / 3) - a / 3;

      /* Sort *x0, *x1, *x2 into increasing order */

      if(*x0 > *x1) SWAP(*x0, *x1) ;

      if(*x1 > *x2) {
          SWAP(*x1, *x2) ;

          if(*x0 > *x1) SWAP(*x0, *x1);
        }

      return 3;
    }
  else
    {
      double sgnR = (R >= 0 ? 1 : -1);
      double A = -sgnR * pow (fabs (R) + sqrt (R2 - Q3), 1.0/3.0);
      double B = Q / A ;
      *x0 = A + B - a / 3;
      return 1;
    }
}
