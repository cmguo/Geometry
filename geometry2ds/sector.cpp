#include "sector.h"

Sector::Sector(Resource * res)
    : Geometry2D(res)
{
}

Sector::Sector(QPointF const & pt)
    : Geometry2D(pt)
{
}

Sector::Sector(Sector const & o)
    : Geometry2D(o)
{
}

QPainterPath Sector::path()
{
    QPainterPath ph;
    if (points_.size() > 1)
    {
        QPointF pt1 = points_[0];
        QPointF pt2 = points_[1];
        ph.moveTo(pt1);
        ph.lineTo(pt2);
        QPointF d = pt2 - pt1;
        qreal r = sqrt(QPointF::dotProduct(d, d));
        QPointF pt3;
        if (points_.size() > 2) {
            pt3 = points_[2];
        } else {
            pt3 = pt1 + QPointF(r, 0);
        }
        QRectF rect(-r, -r, r * 2, r * 2);
        rect.moveCenter(points_[0]);
        qreal a1 = angle(pt2 - pt1);
        qreal a2 = angle(pt3 - pt1);
        ph.arcTo(rect, a1, a2 - a1);
        ph.closeSubpath();
    }
    return ph;
}

int Sector::hit(QPointF &pt)
{
    int elem = Geometry2D::hit(pt);
    if (elem >= 0)
        return elem;
    QPointF pt1 = points_[0];
    QPointF pt2 = points_[1];
    QPointF pt3;
    QPointF d = pt2 - pt1;
    qreal r = sqrt(QPointF::dotProduct(d, d));
    if (points_.size() == 2) {
        pt3 = pt1 + QPointF(r, 0);
        d = pt3 - pt1;
        if (QPointF::dotProduct(d, d) < HIT_DIFF_DIFF) {
            pt = pt3;
            return 2;
        }
    }
    QPointF rp;
    if (dist2PointToSegment(pt1, pt2, pt, rp) < HIT_DIFF_DIFF) {
        pt = rp;
        return 3;
    }
    if (dist2PointToSegment(pt1, pt3, pt, rp) < HIT_DIFF_DIFF) {
        pt = rp;
        return 4;
    }
    QPointF dd = pt - pt1;
    qreal rr = sqrt(QPointF::dotProduct(dd, dd));
    if (rr + HIT_DIFF > r && rr - HIT_DIFF < r)
        return 5;
    return -1;
}

bool Sector::move(int elem, const QPointF &pt)
{
    if (elem == 0) {
        if (points_.size() == 2)
            points_[0] = pt;
    } else if (elem == 1) {
        points_[1] = pt;
        if (points_.size() == 3)
            adjustToLength(points_[0], points_[2], length(points_[0] - points_[1]));
    } else {
        if (points_.size() == 2)
            points_.append(pt);
        else
            points_[2] = pt;
        adjustToLength(points_[0], points_[1], length(points_[0] - points_[2]));
    }
    return true;
}
